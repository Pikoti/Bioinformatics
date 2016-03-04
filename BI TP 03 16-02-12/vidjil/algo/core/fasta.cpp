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

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include "fasta.h"

#include "../lib/gzstream.h"


// http://stackoverflow.com/a/5840160/4475279
unsigned long long filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

void Fasta::init(int extract_field, string extract_separator)
{
  this -> extract_field = extract_field ;
  this -> extract_separator = extract_separator ; 
  total_size = 0;
}

Fasta::Fasta(int extract_field, string extract_separator)
{
  init(extract_field, extract_separator);
}

Fasta::Fasta(const string &input, 
	     int extract_field, string extract_separator,
             bool verbose) 
{
  init(extract_field, extract_separator);

  if (!input.size()) // Do not open empty filenames (D germline if not segmentD)
    return ;

  add(input, verbose);  
}

void Fasta::add(istream &in, bool verbose) {
  in >> *this;

  if (verbose)
  cout << "\t" << setw(6) << total_size << " bp in " << setw(3) << size() << " sequences" << endl ;
}

void Fasta::add(const string &filename, bool verbose) {
  igzstream is(filename.c_str());
  if (is.fail())
    {
      throw invalid_argument(" !! Error in opening file: "+ filename);
    }

  if (verbose)
  cout << " <== " << filename ;
  add(is, verbose);
  is.close();
}

void Fasta::add(Sequence seq) {
  reads.push_back(seq);
  total_size += seq.sequence.size();
}

int Fasta::size() const{ return (int)reads.size(); }
int Fasta::totalSize() const { return total_size ; }

list<Sequence> Fasta::getAll() const {
  list<Sequence> reads;

  for (int i=0; i < size(); i++) {
    reads.push_back(read(i));
  }

  return reads;
}
const string& Fasta::label(int index) const{ return reads[index].label; }
const string& Fasta::label_full(int index) const{ return reads[index].label_full; }
const Sequence& Fasta::read(int index) const {return reads[index];}
const string& Fasta::sequence(int index) const{ return reads[index].sequence; }

// OnlineFasta

OnlineFasta::OnlineFasta(int extract_field, string extract_separator):
  input(NULL), extract_field(extract_field), extract_separator(extract_separator){}

OnlineFasta::OnlineFasta(const string &input, 
                         int extract_field, string extract_separator)
  :input(new igzstream(input.c_str())),
  extract_field(extract_field), 
  extract_separator(extract_separator)
{
  if (this->input->fail()) {
    delete this->input;
    throw invalid_argument("!! Error in opening file "+input);
  }

  cout << "  <== " << input << endl ;
  input_allocated = true;
  init();
}

OnlineFasta::OnlineFasta(istream &input, 
                         int extract_field, string extract_separator)
  :input(&input), extract_field(extract_field),
  extract_separator(extract_separator)
{
  input_allocated = false;
  init();
}

OnlineFasta::~OnlineFasta() {
  if (input_allocated)
    delete input;
  if (current.seq)
    delete [] current.seq;
}

void OnlineFasta::init() {
  char_nb = 0;
  line_nb = 0;
  line = getInterestingLine();
  current.seq = NULL;
}

unsigned long long OnlineFasta::getPos() {
  return char_nb;
}

size_t OnlineFasta::getLineNb() {
  return line_nb;
}

Sequence OnlineFasta::getSequence() {
  return current;
}

bool OnlineFasta::hasNext() {
  return (! input->eof()) || line.length() > 0;
}

void OnlineFasta::next() {
  fasta_state state = FASTX_UNINIT;

  // Reinit the Sequence object
  current.label_full.erase();
  current.label.erase();
  current.sequence.erase();
  current.quality.erase();
  if (current.seq) {
    delete [] current.seq;
    current.seq = NULL;
  }

  if  (hasNext()) {
    switch(line[0]) {
    case '>': state=FASTX_FASTA; break;
    case '@': state=FASTX_FASTQ_ID; break;
    default: 
      throw invalid_argument("The file seems to be malformed!");
    }
    
    // Identifier line
    current.label_full = line.substr(1);
    current.label = extract_from_label(current.label_full, extract_field, extract_separator);

    line = getInterestingLine();
    while (hasNext() && ((state != FASTX_FASTA || line[0] != '>')
                         && (state != FASTX_FASTQ_QUAL || line[0] != '@'))) {

      if (hasNext()) {
        switch(state) {
        case FASTX_FASTA: case FASTX_FASTQ_ID:
          // Sequence
          current.sequence += line;
          break;
        case FASTX_FASTQ_SEQ:
          // FASTQ separator between sequence and quality
          if (line[0] != '+')
            throw invalid_argument("Expected line starting with + in FASTQ file");
          break;
        case FASTX_FASTQ_SEP:
          // Reading quality
          current.quality = line;
          if (current.quality.length() != current.sequence.length())
            throw invalid_argument("Quality and sequence don't have the same length ");
          break;
        default:
          throw invalid_argument("Unexpected state after reading identifiers line");
        }
        if (state >= FASTX_FASTQ_ID && state <= FASTX_FASTQ_SEP)
          state = (fasta_state)(((int)state) + 1);
      } else {
        unexpectedEOF();
      }
      line = getInterestingLine(state);
    }

    if (state >= FASTX_FASTQ_ID && state < FASTX_FASTQ_QUAL) 
      unexpectedEOF();

    // Sequence in uppercase
    transform(current.sequence.begin(), current.sequence.end(), current.sequence.begin(), (int (*)(int))toupper);

    // Compute seq
    current.seq = new int[current.sequence.length()];
    for (unsigned int i=0; i< current.sequence.length(); i++)
      {
	current.seq[i] = nuc_to_int(current.sequence[i]) ;
      }

  } else
    unexpectedEOF();
}

string OnlineFasta::getInterestingLine(int state) {
  string line;
  while (line.length() == 0 && hasNext() && getline(*input, line)) {
    line_nb++;
    char_nb += line.length() + 1;
    remove_trailing_whitespaces(line);

    if (line.length() && line[0] == '#' && state != FASTX_FASTQ_SEP)
      line = "" ;
  }
  return line;
}

void OnlineFasta::unexpectedEOF() {
  throw invalid_argument("Unexpected EOF while reading FASTA/FASTQ file");
}

// Operators

istream& operator>>(istream& in, Fasta& fasta){
	string line;
	Sequence read;
        OnlineFasta of(in, fasta.extract_field, fasta.extract_separator);

        while (of.hasNext()) {
          of.next();
          fasta.add(of.getSequence());
        }
	return in;
}

ostream& operator<<(ostream& out, Fasta& fasta){
	for(int i = 0 ; i < fasta.size() ; i++){
          out << fasta.read(i);
	}
	return out;
}

ostream &operator<<(ostream &out, const Sequence &seq) {
  bool is_fastq=false;
  if (seq.quality.length() > 0) {
    is_fastq = true;
    out << "@";
  } else
    out << ">";
  out << seq.label << endl;
  out << seq.sequence << endl;
  if (is_fastq) {
    out << "+" << endl << seq.quality << endl;
  }
  return out;
}

int nb_sequences_in_fasta(string f, bool approx)
{
  if (approx)
    return approx_nb_sequences_in_fasta(f);

  OnlineFasta *sequences = new OnlineFasta(f, 1, " ");
  int nb_sequences = 0 ;

  while (sequences->hasNext())
    {
      sequences->next();
      nb_sequences++ ;
    }

  cout << "  ==> " << nb_sequences << " sequences" << endl;

  delete sequences ;
  return nb_sequences ;
}


#define SAMPLE_APPROX_NB_SEQUENCES 200

int approx_nb_sequences_in_fasta(string f)
{
  OnlineFasta *sequences = new OnlineFasta(f, 1, " ");
  int nb_sequences = 0 ;

  while (nb_sequences < SAMPLE_APPROX_NB_SEQUENCES && sequences->hasNext())
    {
      sequences->next();
      nb_sequences++ ;
    }

  cout << "  ==> " ;

  if (sequences->hasNext())
    {
      cout << "approx. " ;
      float ratio = (float) filesize(f.c_str()) / (float) sequences->getPos();
      nb_sequences = (int) (ratio * nb_sequences);
    }

  cout << nb_sequences << " sequences" << endl;

  delete sequences ;
  return nb_sequences ;
}
