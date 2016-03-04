/*
  This file is part of Vidjil <http://www.vidjil.org>
  Copyright (C) 2011, 2012, 2013, 2014, 2015 by Bonsai bioinformatics 
  at CRIStAL (UMR CNRS 9189, Université Lille) and Inria Lille
  Contributors: 
      Mathieu Giraud <mathieu.giraud@vidjil.org>
      Mikaël Salson <mikael.salson@vidjil.org>
      Marc Duez <marc.duez@vidjil.org>

  "Vidjil" is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  "Vidjil" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with "Vidjil". If not, see <http://www.gnu.org/licenses/>
*/
#include <algorithm>    // std::sort
#include <cassert>
#include "segment.h"
#include "tools.h"
#include "affectanalyser.h"
#include <sstream>
#include <cstring>
#include <string>

Segmenter::~Segmenter() {}

Sequence Segmenter::getSequence() const {
  Sequence s ;
  s.label_full = info ;
  s.label = label + " " + (reversed ? "-" : "+");
  s.sequence = revcomp(sequence, reversed);

  return s ;
}

string Segmenter::getJunction(int l) const {
  assert(isSegmented());

  int start = (getLeft() + getRight())/2 - l/2;
  
  if (start < 0 or start + l > (int)sequence.size())  // TODO: +l ou +l-1 ?
    return "" ;

  return getSequence().sequence.substr(start, l);
}

int Segmenter::getLeft() const {
  return Vend;
}
  
int Segmenter::getRight() const {
  return Jstart;
}

int Segmenter::getLeftD() const {
  return Dstart;
}
  
int Segmenter::getRightD() const {
  return Dend;
}

bool Segmenter::isReverse() const {
  return reversed;
}

bool Segmenter::isSegmented() const {
  return segmented;
}

bool Segmenter::isDSegmented() const {
  return dSegmented;
}


// E-values

void Segmenter::checkLeftRightEvaluesThreshold(double threshold, int strand)
{
  if (threshold == NO_LIMIT_VALUE)
    return ;

  if (evalue_left >= threshold && evalue_right >= threshold)
    because = UNSEG_TOO_FEW_ZERO ;
  else if ((strand == 1 ? evalue_left : evalue_right) >= threshold)
    because = UNSEG_TOO_FEW_V ;
  else if ((strand == 1 ? evalue_right : evalue_left) >= threshold)
    because = UNSEG_TOO_FEW_J ;
  else if (evalue >= threshold) // left and right are <= threshold, but their sum is > threshold
    because = UNSEG_TOO_FEW_ZERO ;
}


// Chevauchement

string Segmenter::removeChevauchement()
{
  assert(isSegmented());
  
  string chevauchement = "" ;

  if (Vend >= Jstart)
    {
      int middle = (Vend + Jstart) / 2 ;
      chevauchement = " !ov " + string_of_int (Vend - Jstart + 1);
      Vend = middle ;
      Jstart = middle+1 ;
    }

  return chevauchement ;
}

// Prettyprint


bool Segmenter::finishSegmentation() 
{
  assert(isSegmented());
  
  string seq = getSequence().sequence;
    
  seg_V = seq.substr(0, Vend+1) ;
  seg_N = seq.substr(Vend+1, Jstart-Vend-1) ;  // Twice computed for FineSegmenter, but only once in KmerSegmenter !
  seg_J = seq.substr(Jstart) ;
  Dstart=0;
  Dend=0;

  info = "VJ \t" + string_of_int(FIRST_POS) + " " + info + " " + string_of_int(seq.size() - 1 + FIRST_POS) ;
  info += "\t" + code ;

  info = (reversed ? "- " : "+ ") + info ;

  return true ;
}

bool Segmenter::finishSegmentationD() 
{
  string seq = getSequence().sequence;

  seg_V = seq.substr(0, Vend+1) ; // From pos. 0 to Vend
  seg_J = seq.substr(Jstart) ;
  seg_N = seq.substr(Vend+1, Jstart-Vend-1) ;  // Twice computed for FineSegmenter, but only once in KmerSegmenter !
  seg_D  = seq.substr(Dstart, Dend-Dstart+1) ; // From Dstart to Dend
  
  info = "VDJ \t0 " + string_of_int(Vend) +
		" " + string_of_int(Dstart) + 
		" " + string_of_int(Dend) +
		" " + string_of_int(Jstart) +
		" " + string_of_int(seq.size()-1+FIRST_POS) ;
		
  info += "\t" + code ;
  
  info = (reversed ? "- " : "+ ") + info ;

  return true ;
}

string Segmenter::getInfoLine() const
{
  string s = "" ;

  s += (segmented ? "" : "! ") + info ;
  s += " " + info_extra ;
  s += " " + segmented_germline->code ;
  s += " " + string(segmented_mesg[because]) ;

  if (evalue > NO_LIMIT_VALUE)
    s += " " + scientific_string_of_double(evalue);

  if (evalue_left > NO_LIMIT_VALUE)
    s += " " + scientific_string_of_double(evalue_left);
  if (evalue_right > NO_LIMIT_VALUE)
    s += "/" + scientific_string_of_double(evalue_right);

  return s ;
}

string KmerSegmenter::getInfoLineWithAffects() const
{
   stringstream ss;

   ss << "# "
      << right << setw(3) << score << " "
      << left << setw(30)
      << getInfoLine() ;

   if (getSegmentationStatus() != UNSEG_TOO_SHORT)
     ss << getKmerAffectAnalyser()->toString();

   return ss.str();
}


ostream &operator<<(ostream &out, const Segmenter &s)
{
  out << ">" << s.label << " " ;
  out << s.getInfoLine() << endl;

  if (s.segmented)
    {
      out << s.seg_V << endl ;
      out << s.seg_N << endl ;
      out << s.seg_J << endl ;
    }
  else
    {
      out << s.getSequence().sequence << endl ;
    }

  return out ;
}


// KmerSegmenter (Cheap)

KmerSegmenter::KmerSegmenter() { kaa = 0 ; }

KmerSegmenter::KmerSegmenter(Sequence seq, Germline *germline, double threshold, int multiplier)
{
  label = seq.label ;
  sequence = seq.sequence ;
  info = "" ;
  info_extra = "seed";
  segmented = false;
  segmented_germline = germline ;
  system = germline->code; // useful ?
  reversed = false;
  Dend=0;
  because = NOT_PROCESSED ; // Cause of unsegmentation
  score = 0 ;
  evalue = NO_LIMIT_VALUE;
  evalue_left = NO_LIMIT_VALUE;
  evalue_right = NO_LIMIT_VALUE;

  int s = (size_t)germline->index->getS() ;
  int length = sequence.length() ;

  if (length < s) 
    {
      because = UNSEG_TOO_SHORT;
      kaa = NULL;
      return ;
    }
 
  kaa = new KmerAffectAnalyser(*(germline->index), sequence);
  
  // Check strand consistency among the affectations.
  int strand;
  int nb_strand[2] = {0,0};     // In cell 0 we'll put the number of negative
                                // strand, while in cell 1 we'll put the
                                // positives
  for (int i = 0; i < kaa->count(); i++) { 
    KmerAffect it = kaa->getAffectation(i);
    if (! it.isAmbiguous() && ! it.isUnknown()) {
      strand = affect_strand(it.affect);
      nb_strand[(strand + 1) / 2] ++; // (strand+1) / 2 → 0 if strand == -1; 1 if strand == 1
    }
  }

  score = nb_strand[0] + nb_strand[1] ; // Used only for non-segmented germlines

  if ((germline->seg_method == SEG_METHOD_MAX12)
      || (germline->seg_method == SEG_METHOD_MAX1U))
    { // Pseudo-germline, MAX12 and MAX1U
      pair <KmerAffect, KmerAffect> max12 ;
      CountKmerAffectAnalyser ckaa(*(germline->index), sequence);


      set<KmerAffect> forbidden;
      forbidden.insert(KmerAffect::getAmbiguous());
      forbidden.insert(KmerAffect::getUnknown());

      if (germline->seg_method == SEG_METHOD_MAX12)
        // MAX12: two maximum k-mers (no unknown)
        {
          max12 = ckaa.max12(forbidden);

          if (max12.first.isUnknown() || max12.second.isUnknown())
            {
              because = UNSEG_TOO_FEW_ZERO ;
              return ;
            }
        }

      else
        // MAX1U: the maximum k-mers (no unknown) + unknown
        {
          CountKmerAffectAnalyser ckaa(*(germline->index), sequence);
          KmerAffect max = ckaa.max(forbidden);

          if (max.isUnknown())
            {
              because = UNSEG_TOO_FEW_ZERO ;
              return ;
            }
          max12 = make_pair(max, KmerAffect::getUnknown());
        }

      pair <KmerAffect, KmerAffect> before_after =  ckaa.sortLeftRight(max12);

      before = before_after.first ;
      after = before_after.second ;

      // This strand computation is only a heuristic, especially for chimera +/- reads
      // Anyway, it allows to gather such reads and their reverse complement into a unique window...
      // ... except when the read is quite different outside the window
      strand = nb_strand[0] > nb_strand[1] ? -1 : 1 ;
    }

  else
    { // Regular germline

  // Test on which strand we are, select the before and after KmerAffects
  if (nb_strand[0] == 0 && nb_strand[1] == 0) {
    because = UNSEG_TOO_FEW_ZERO ;
    return ;
  } else if (nb_strand[0] > RATIO_STRAND * nb_strand[1]) {
    strand = -1;
    before = KmerAffect(germline->affect_3, -1); 
    after = KmerAffect(germline->affect_5, -1);
  } else if (nb_strand[1] > RATIO_STRAND * nb_strand[0]) {
    strand = 1;
    before = KmerAffect(germline->affect_5, 1); 
    after = KmerAffect(germline->affect_3, 1);    
  } else {
    // Ambiguous information: we have positive and negative strands
    // and there is not enough difference to put them apart.
    if (nb_strand[0] + nb_strand[1] >= DETECT_THRESHOLD_STRAND)
      because = UNSEG_STRAND_NOT_CONSISTENT ;
    else
      because = UNSEG_TOO_FEW_ZERO ;
    return ;
  }

    } // endif Pseudo-germline
 
  computeSegmentation(strand, before, after, threshold, multiplier);
}

KmerSegmenter::~KmerSegmenter() {
  if (kaa)
    delete kaa;
}

KmerMultiSegmenter::KmerMultiSegmenter(Sequence seq, MultiGermline *multigermline, ostream *out_unsegmented,
                                       double threshold, int nb_reads_for_evalue)
{
  bool found_seg = false ; // Found a segmentation
  double best_evalue_seg = NO_LIMIT_VALUE ; // Best evalue, segmented sequences
  int best_score_unseg = 0 ; // Best score, unsegmented sequences
  the_kseg = NULL;
  multi_germline = multigermline;
  threshold_nb_expected = threshold;

  // E-value multiplier
  int multiplier = multi_germline->germlines.size() * nb_reads_for_evalue;
  
  // Iterate over the germlines
  for (list<Germline*>::const_iterator it = multigermline->germlines.begin(); it != multigermline->germlines.end(); ++it)
    {
      Germline *germline = *it ;

      KmerSegmenter *kseg = new KmerSegmenter(seq, germline, threshold, multiplier);
      bool keep_seg = false;

      if (out_unsegmented)
        {
          // Debug, display k-mer affectation and segmentation result for this germline
          *out_unsegmented << kseg->getInfoLineWithAffects() << endl ;
        }

      // Always remember the first kseg
      if (the_kseg == NULL)
        keep_seg = true;
      
      if (kseg->isSegmented())
        {
          // Yes, it is segmented
          // Should we keep the kseg ?
          if (!found_seg || (kseg->evalue < best_evalue_seg))
            {
              keep_seg = true;
              best_evalue_seg = kseg->evalue ;

              found_seg = true;
            }
        }
      else
        {
          // It is not segmented
          // Should we keep the kseg (with the unsegmentation cause) ?
            if (kseg->score > best_score_unseg)
            {              
              best_score_unseg = kseg->score ;
              if (!found_seg)
                keep_seg = true;
            }
        }
      
      if (keep_seg) {
        if (the_kseg)
          delete the_kseg;
        the_kseg = kseg;
      } else {
        delete kseg;
      }
    } // end for (Germlines)
}

KmerMultiSegmenter::~KmerMultiSegmenter() {
  if (the_kseg)
    delete the_kseg;
}

void KmerSegmenter::computeSegmentation(int strand, KmerAffect before, KmerAffect after,
                                        double threshold, int multiplier) {
  // Try to segment, computing 'Vend' and 'Jstart'
  // If not segmented, put the cause of unsegmentation in 'because'

  affect_infos max;
  max = kaa->getMaximum(before, after);

  // We did not find a good segmentation point
  if (!max.max_found) {
    // We labeled it detected if there were both enough affect_5 and enough affect_3
    bool detected = (max.nb_before_left + max.nb_before_right >= DETECT_THRESHOLD)
      && (max.nb_after_left + max.nb_after_right >= DETECT_THRESHOLD);

    if (max.nb_before_left + max.nb_before_right + max.nb_after_left + max.nb_after_right == 0)
      because = UNSEG_TOO_FEW_ZERO ;
    else if ((strand == 1 && max.nb_before_left == 0) || (strand == -1 && max.nb_after_right == 0))
      because = detected ? UNSEG_AMBIGUOUS : UNSEG_TOO_FEW_V ;
    else if ((strand == 1 && max.nb_after_right == 0)|| (strand == -1 && max.nb_before_left == 0))
      because = detected ? UNSEG_AMBIGUOUS : UNSEG_TOO_FEW_J ;
    else
      because = UNSEG_AMBIGUOUS;

    return ;
  }


  // E-values
  pair <double, double> pvalues = kaa->getLeftRightProbabilityAtLeastOrAbove();
  evalue_left = pvalues.first * multiplier ;
  evalue_right = pvalues.second * multiplier ;
  evalue = evalue_left + evalue_right ;

  checkLeftRightEvaluesThreshold(threshold, strand);

  if (because != NOT_PROCESSED)
    return ;

   // There was a good segmentation point

   Vend = max.first_pos_max;
   Jstart = max.last_pos_max + 1;
   if (strand == -1) {
     int tmp = sequence.size() - Vend - 1;
     Vend = sequence.size() - Jstart - 1;
     Jstart = tmp;
   }

  // Yes, it is segmented
  segmented = true;
  reversed = (strand == -1);
  because = reversed ? SEG_MINUS : SEG_PLUS ;

  info = string_of_int(Vend + FIRST_POS) + " " + string_of_int(Jstart + FIRST_POS)  ;

  // removeChevauchement is called once info was already computed: it is only to output info_extra
  info_extra += removeChevauchement();
  finishSegmentation();

  return ;
}

KmerAffectAnalyser *KmerSegmenter::getKmerAffectAnalyser() const {
  return kaa;
}

int Segmenter::getSegmentationStatus() const {
  return because;
}

void Segmenter::setSegmentationStatus(int status) {
  because = status;
  segmented = (status == SEG_PLUS || status == SEG_MINUS);
}

// FineSegmenter


void best_overlap_split(int overlap, string seq_left, string seq_right,
                        string ref_left, string ref_right,
                        int *pos_seq_left, int *pos_seq_right,
                        int *trim_ref_left, int *trim_ref_right, Cost segment_cost)
{
      int score_r[overlap+1];
      int score_l[overlap+1];
      
      //LEFT
      DynProg dp_l = DynProg(seq_left, ref_left,
			   DynProg::Local, segment_cost);
      score_l[0] = dp_l.compute();

      
      //RIGHT
      // reverse right sequence
      ref_right=string(ref_right.rbegin(), ref_right.rend());
      seq_right=string(seq_right.rbegin(), seq_right.rend());


      DynProg dp_r = DynProg(seq_right, ref_right,
			   DynProg::Local, segment_cost);
      score_r[0] = dp_r.compute();



      int trim_l[overlap+1];
      int trim_r[overlap+1];

      for(int i=0; i<=overlap; i++) {
        score_l[i] = i < seq_left.size()  ? dp_l.best_score_on_i(seq_left.size()  - i, trim_l + i) : MINUS_INF ;
        score_r[i] = i < seq_right.size() ? dp_r.best_score_on_i(seq_right.size() - i, trim_r + i) : MINUS_INF ;
      }


// #define DEBUG_OVERLAP
#ifdef DEBUG_OVERLAP
      cout << dp_l ;
      cout << dp_r ;

      cout << "seq:" << seq_left << "\t\t" << seq_right << endl;
      cout << "ref:" << ref_left << "\t\t" << ref_right << endl;
      for(int i=0; i<=overlap; i++)
        cout << i << "  left: " << score_l[i] << "/" << trim_l[i] << "     right: " << score_r[i] << "/" << trim_r[i] << endl;
#endif

      int score = MINUS_INF;
      int best_i = 0 ;
      int best_j = 0 ;


      // Find (i, j), with i+j >= overlap,
      // maximizing score_l[j] + score_r[i]
      for (int i=0; i<=overlap; i++){
	for (int j=overlap-i; j<=overlap; j++){
          int score_ij = score_l[i] + score_r[j];

	  if (score_ij > score) {
            best_i = i ;
            best_j = j ;
            *trim_ref_left  = ref_left.size() - trim_l[i];
	    *trim_ref_right = ref_right.size() - trim_r[j];
	    score = score_ij;
	  }
	}
      }

      *pos_seq_left -= best_i ;
      *pos_seq_right += best_j ;

#ifdef DEBUG_OVERLAP
      cout << "overlap: " << overlap << ", " << "best_overlap_split: " << score
           << "    left: " << best_i << "-" << *trim_ref_left << " @" << *pos_seq_right
           << "    right:" << best_j << "-" << *trim_ref_right << " @" << *pos_seq_right
           << endl;
#endif
}

bool comp_pair (pair<int,int> i,pair<int,int> j)
{
  return ( i.first > j.first);
}

int align_against_collection(string &read, Fasta &rep, bool reverse_both, bool local, int *tag, 
			     int *del, int *del2, int *begin, int *length, vector<pair<int, int> > *score
			    , Cost segment_cost)
{
  
  int best_score = MINUS_INF ;
  int best_r = MINUS_INF ;
  int best_best_i = (int) string::npos ;
  int best_best_j = (int) string::npos ;
  int best_first_i = (int) string::npos ;
  int best_first_j = (int) string::npos ;
  string best_label = "" ;
  vector<pair<int, int> > score_r;

  DynProg::DynProgMode dpMode = DynProg::LocalEndWithSomeDeletions;
  if (local==true) dpMode = DynProg::Local;
  
  for (int r = 0 ; r < rep.size() ; r++)
    {
      DynProg dp = DynProg(read, rep.sequence(r),
			   dpMode, // DynProg::SemiGlobalTrans, 
			   segment_cost, // DNA
			   reverse_both, reverse_both);
      int score = dp.compute();
      
      if (local==true){ 
	dp.backtrack();
      }
      
      if (score > best_score)
	{
	  best_score = score ;
	  best_best_i = dp.best_i ;
	  best_best_j = dp.best_j ;
	  best_first_i = dp.first_i ;
	  best_first_j = dp.first_j ;
	  best_r = r ;
	  best_label = rep.label(r) ;
	}
	
	score_r.push_back(make_pair(score, r));

	// #define DEBUG_SEGMENT      

#ifdef DEBUG_SEGMENT	
	cout << rep.label(r) << " " << score << " " << dp.best_i << endl ;
#endif

    }
    sort(score_r.begin(),score_r.end(),comp_pair);

  *del = reverse_both ? best_best_j : rep.sequence(best_r).size() - best_best_j - 1;
  *del2 = best_first_j;
  *begin = best_first_i;
  *tag = best_r ; 
  
  *length -= *del ;
  
  *score=score_r;

#ifdef DEBUG_SEGMENT	
  cout << "best: " << best_labels << " " << best_score ;
  cout << "del/del2/begin:" << (*del) << "/" << (*del2) << "/" << (*begin) << endl;
  cout << endl;
#endif
  
  return best_best_i ;
}

string format_del(int deletions)
{
  return deletions ? *"(" + string_of_int(deletions) + " del)" : "" ;
}

FineSegmenter::FineSegmenter(Sequence seq, Germline *germline, Cost segment_c,  double threshold, int multiplier)
{
  segmented = false;
  dSegmented = false;
  because = NOT_PROCESSED ;
  segmented_germline = germline ;
  info_extra = "" ;
  code_short = "" ;
  label = seq.label ;
  sequence = seq.sequence ;
  Dend=0;
  segment_cost=segment_c;
  evalue = NO_LIMIT_VALUE;
  evalue_left = NO_LIMIT_VALUE;
  evalue_right = NO_LIMIT_VALUE;

  CDR3start = -1;
  CDR3end = -1;
  
  if (!germline->rep_5.size() || !germline->rep_3.size())
    {
      // We check whether this sequence is segmented with MAX12 or MAX1U (with default e-value parameters)
      KmerSegmenter *kseg = new KmerSegmenter(seq, germline, THRESHOLD_NB_EXPECTED, 1);
      if (kseg->isSegmented() && ((germline->seg_method == SEG_METHOD_MAX12)
                                  || (germline->seg_method == SEG_METHOD_MAX1U)))
        {
          reversed = kseg->isReverse();

          KmerAffect left = reversed ? KmerAffect(kseg->after, true) : kseg->before ;
          KmerAffect right = reversed ? KmerAffect(kseg->before, true) : kseg->after ;

          code_short = "Unexpected ";

          code_short += left.toStringSigns() + germline->index->getLabel(left);
          code_short += "/";
          code_short += right.toStringSigns() + germline->index->getLabel(right);
          info_extra += " " + left.toString() + "/" + right.toString() + " (" + code_short + ")";
        }
      return ;
    }

  // TODO: factoriser tout cela, peut-etre en lancant deux segmenteurs, un +, un -, puis un qui chapote
  
  // Strand +
  
  int plus_score = 0 ;
  int tag_plus_V, tag_plus_J;
  int plus_length = 0 ;
  int del_plus_V, del_plus_J ;
  int del2=0;
  int beg=0;
  
  vector<pair<int, int> > score_plus_V;
  vector<pair<int, int> > score_plus_J;
  
  int plus_left = align_against_collection(sequence, germline->rep_5, false, false, &tag_plus_V, &del_plus_V, &del2, &beg, 
					   &plus_length, &score_plus_V
					   , segment_cost);
  int plus_right = align_against_collection(sequence, germline->rep_3, true, false, &tag_plus_J, &del_plus_J, &del2, &beg,
					    &plus_length, &score_plus_J
					    , segment_cost);
  plus_length += plus_right - plus_left ;

  plus_score=score_plus_V[0].first + score_plus_J[0].first ;
  
  // Strand -
  string rc = revcomp(sequence) ;
  int minus_score = 0 ;
  int tag_minus_V, tag_minus_J;
  int minus_length = 0 ;
  int del_minus_V, del_minus_J ;
  
  vector<pair<int, int> > score_minus_V;
  vector<pair<int, int> > score_minus_J;
  
  int minus_left = align_against_collection(rc, germline->rep_5, false, false, &tag_minus_V, &del_minus_V, &del2, &beg,
					    &minus_length, &score_minus_V
					    ,  segment_cost);
  int minus_right = align_against_collection(rc, germline->rep_3, true, false, &tag_minus_J, &del_minus_J, &del2, &beg,
					     &minus_length, &score_minus_J
					     , segment_cost);
  minus_length += minus_right - minus_left ;

  minus_score=score_minus_V[0].first + score_minus_J[0].first ;
  
  reversed = (minus_score > plus_score) ;

  if (!reversed)
    {
      Vend = plus_left ;
      Jstart = plus_right ;
      best_V = tag_plus_V ;
      best_J = tag_plus_J ;
      del_V = del_plus_V ;
      del_J = del_plus_J ;
      score_V=score_plus_V;
      score_J=score_plus_J;
    }
  else
    {
      Vend = minus_left ;
      Jstart = minus_right ;
      best_V = tag_minus_V ;
      best_J = tag_minus_J ;
      del_V = del_minus_V ;
      del_J = del_minus_J ;
      score_V=score_minus_V;
      score_J=score_minus_J;
    }

  /* E-values */
  evalue_left  = multiplier * sequence.size() * germline->rep_5.totalSize() * segment_cost.toPValue(score_V[0].first);
  evalue_right = multiplier * sequence.size() * germline->rep_3.totalSize() * segment_cost.toPValue(score_J[0].first);
  evalue = evalue_left + evalue_right ;

  /* Unsegmentation causes */
  if (Vend == (int) string::npos)
    {
      evalue_left = BAD_EVALUE ;
    }
      
  if (Jstart == (int) string::npos)
    {
      evalue_right = BAD_EVALUE ;
    }

  checkLeftRightEvaluesThreshold(threshold, reversed ? -1 : 1);

  if (because != NOT_PROCESSED)
    {
      segmented = false;
      info = " @" + string_of_int (Vend + FIRST_POS) + "  @" + string_of_int(Jstart + FIRST_POS) ;
      return ;
    }

  /* The sequence is segmented */
  segmented = true ;
  because = reversed ? SEG_MINUS : SEG_PLUS ;

  string sequence_or_rc = getSequence().sequence; // segmented sequence, possibly rev-comped

    //overlap VJ
    if(Jstart-Vend <=0){
      int overlap=Vend-Jstart+1;

      string seq_left = sequence_or_rc.substr(0, Vend+1);
      string seq_right = sequence_or_rc.substr(Jstart);

      best_overlap_split(overlap, seq_left, seq_right,
                         germline->rep_5.sequence(best_V), germline->rep_3.sequence(best_J),
                         &Vend, &Jstart, &del_V, &del_J, segment_cost);

      if (Jstart>=(int) sequence.length())
	  Jstart=sequence.length()-1;
    }

    // string chevauchement = removeChevauchement();

    /// used only below, then recomputed in finishSegmentation() ;
    seg_N = sequence_or_rc.substr(Vend+1, Jstart-Vend-1);

  code = germline->rep_5.label(best_V) +
    " "+ string_of_int(del_V) + 
    "/" + seg_N + 
    // chevauchement +
    "/" + string_of_int(del_J) +
    " " + germline->rep_3.label(best_J); 

    stringstream code_s;
   code_s<< germline->rep_5.label(best_V) <<
    " -" << string_of_int(del_V) << "/" 
    << seg_N.size()
    // chevauchement +
    << "/-" << string_of_int(del_J)
    <<" " << germline->rep_3.label(best_J);
    code_short=code_s.str();
    
  code_light = germline->rep_5.label(best_V) +
    "/ " + germline->rep_3.label(best_J); 

 
  info = string_of_int(Vend + FIRST_POS) + " " + string_of_int(Jstart + FIRST_POS) ;
  finishSegmentation();
}


void FineSegmenter::FineSegmentD(Germline *germline, double evalue_threshold, int multiplier){
  
  if (segmented){
    
    int end = (int) string::npos ;
    int tag_D;
    int length = 0 ;
    int begin = 0;
    
    // Create a zone where to look for D, adding at most EXTEND_D_ZONE nucleotides at each side
    int l = Vend - EXTEND_D_ZONE;
    if (l<0) 
      l=0 ;

    int r = Jstart + EXTEND_D_ZONE;

    string seq = getSequence().sequence; // segmented sequence, possibly rev-comped

    if (r > (int) seq.length())
      r = seq.length();
      
    string str = seq.substr(l, r-l);

    // Align
    end = align_against_collection(str, germline->rep_4, false, true, &tag_D, &del_D_right, &del_D_left, &begin,
				&length, &score_D, segment_cost);
    
    best_D = tag_D;
    
    Dstart = l + begin;
    Dend = l + end;
	
    float evalue_D = multiplier * (r-l) * germline->rep_4.totalSize() * segment_cost.toPValue(score_D[0].first);



    if (evalue_D > evalue_threshold)
      return ;

    dSegmented=true;
    
    //overlap VD
    if(Dstart-Vend <=0){
      int overlap=Vend-Dstart+1;
      string seq_left = seq.substr(0, Vend+1);
      string seq_right = seq.substr(Dstart, Dend-Dstart+1);

      best_overlap_split(overlap, seq_left, seq_right,
                         germline->rep_5.sequence(best_V), germline->rep_4.sequence(best_D),
                         &Vend, &Dstart, &del_V, &del_D_left, segment_cost);
    }

    seg_N1 = seq.substr(Vend+1, Dstart-Vend-1) ; // From Vend+1 to Dstart-1
    
    //overlap DJ
    if(Jstart-Dend <=0){
      int overlap=Dend-Jstart+1;
      string seq_left = seq.substr(Dstart, Dend-Dstart+1);
      string seq_right = seq.substr(Jstart, seq.length()-Jstart);

      best_overlap_split(overlap, seq_left, seq_right,
                         germline->rep_4.sequence(best_D), germline->rep_3.sequence(best_J),
                         &Dend, &Jstart, &del_D_right, &del_J, segment_cost);
    }

    seg_N2 = seq.substr(Dend+1, Jstart-Dend-1) ; // From Dend+1 to right-1
    code = germline->rep_5.label(best_V) +
    " "+ string_of_int(del_V) + 
    "/" + seg_N1 + 
    
    "/" + string_of_int(del_D_left) +
    " " + germline->rep_4.label(best_D) +
    " " + string_of_int(del_D_right) +
    
    "/" + seg_N2 +
    "/" + string_of_int(del_J) +
    " " + germline->rep_3.label(best_J); 

    stringstream code_s;
    code_s << germline->rep_5.label(best_V) 
    << " -" << string_of_int(del_V) << "/" 
    << seg_N1.size()
    
    << "/-" << string_of_int(del_D_left) 
    << " " << germline->rep_4.label(best_D) 
    << " -" << string_of_int(del_D_right) << "/"
    
    << seg_N2.size()
    << "/-" << string_of_int(del_J) 
    << " " << germline->rep_3.label(best_J);
    code_short=code_s.str();
    
    
    code_light = germline->rep_5.label(best_V) +
    "/ " + germline->rep_4.label(best_D) +
    "/ " + germline->rep_3.label(best_J); 
    
    finishSegmentationD();
  }
}

void FineSegmenter::findCDR3(){
    string str = getSequence().sequence;
    
    list<string> codon_start;
    codon_start.push_back("TGT");
    codon_start.push_back("TGC");
    
    list<string> codon_end;
    codon_end.push_back("TTT");
    codon_end.push_back("TTC");
    codon_end.push_back("TGG");
    
    list<int> p_start;
    list<int> p_end;

    size_t loc;
    std::list<string>::const_iterator it;
    for (it = codon_start.begin(); it != codon_start.end(); ++it) {//filter 1 : start codon must be in V
        loc = 0;
        while ( loc != string::npos && loc < (size_t)Vend){
            loc = str.find(*it, loc+3);
            if (loc != string::npos && loc < (size_t)Vend) {
                p_start.push_front(loc);
            }
        }
    }

    for (it = codon_end.begin(); it != codon_end.end(); ++it) {//filter 2 : end codon must be in J
        loc = Jstart;
        while ( loc != string::npos){
            loc = str.find(*it, loc+3);
            if (loc != string::npos) {
                p_end.push_back(loc);
            }
        }
    }

    CDR3start = -1;
    CDR3end = -1;
    
    std::list<int>::const_iterator it1;
    for (it1 = p_start.begin(); it1 != p_start.end(); ++it1) {
        
        std::list<int>::const_iterator it2;
        for (it2 = p_end.begin(); it2 != p_end.end(); ++it2) {
            
            if ( (*it2-*it1)%3 == 0){       //filter 3 : start/stop codon must be seprated by a multiple of 3
                
                if ( fabs((*it2-*it1)-36 ) < fabs((CDR3end-CDR3start)-36) ){ //filter 4 : cdr3 length must be close to 12 AA
                    CDR3start = *it1;
                    CDR3end = *it2;
                }
            }
        }
    }
    
}

json FineSegmenter::toJson(){
  json seg;
    
  if (isSegmented()) {
    seg["5"] = segmented_germline->rep_5.label(best_V);
    seg["5start"] = 0;
    seg["5end"] = Vend;
    seg["5del"] = del_V;
    
    if (isDSegmented()) {
      seg["4"] = segmented_germline->rep_4.label(best_D);
      seg["4start"] = Dstart;
      seg["4end"] = Dend;
      seg["4delLeft"] = del_D_left;
      seg["4delRight"] = del_D_right;

      seg["N1"] = seg_N1.size();
      seg["N2"] = seg_N2.size();
    }
    else {
      seg["N"] = seg_N.size();
    }
    
    seg["3"] = segmented_germline->rep_3.label(best_J);
    seg["3start"] = Jstart;
    seg["3del"] = del_J;

    if (CDR3start >= 0) {
        seg["cdr3"] = {
            {"start", CDR3start},
            {"stop", CDR3end}
        };
    }
  }
  
  return seg;
}

json KmerSegmenter::toJson() {
    json seg;
    int sequenceSize = sequence.size();

    if (evalue > NO_LIMIT_VALUE)
        seg["_evalue"] = scientific_string_of_double(evalue);
    if (evalue_left > NO_LIMIT_VALUE)
      seg["_evalue_left"] = scientific_string_of_double(evalue_left);
    if (evalue_right > NO_LIMIT_VALUE)
      seg["_evalue_right"] = scientific_string_of_double(evalue_right);

    seg["affectValues"] = {
        {"start", 0},
        {"stop", sequenceSize},
        {"seq", getKmerAffectAnalyser()->toStringValues()}
    };
    
    seg["affectSigns"] = {
        {"start", 0},
        {"stop", sequenceSize},
        {"seq", getKmerAffectAnalyser()->toStringSigns()}
    };
    return seg;
}


