#!/usr/bin/ruby

require './src/Fasta2.rb'

fa = Fasta.new("fasta-tp3/TRGV.fasta")
print fa.at(ARGV[0].to_i).adn.length
