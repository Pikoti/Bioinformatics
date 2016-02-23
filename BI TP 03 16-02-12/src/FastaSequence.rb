require './src/DNA.rb'

class FastaSequence

    attr_accessor :id, :dna

    def initialize (id, dna)
        @id = id
        @dna = DNA.new(dna)
    end

end
