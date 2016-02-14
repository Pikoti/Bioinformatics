class DNA

    attr_accessor :dna

    def initialize (dna)
        @dna = dna.upcase
    end

    def clone ()
        DNA.new(@dna)
    end

    def size ()
        @dna.length
    end

    def window (i, length)
        DNA.new(@dna[i..(i + length - 1)])
    end

    def mutate (n)
        n.times do 
            self.changeOneLetter()
        end
    end

    def toKmers (size)
        str = ""
        kmers = []
        @dna.each_char do |c|
            str += c
            if str.length == size
                kmers.push(str[0..(size - 1)])
                str = str[1..(size - 1)]
            end
        end
        kmers
    end

    def toSpacedKmers (seed)
        kmers = self.toKmers(seed.length)
        kmers.map do |kmer|
            newKmer = ""
            seed.length.times do |i|
                if seed[i] == '#'
                    newKmer += kmer[i]
                end
            end
            newKmer
        end
    end

    def changeOneLetter ()
        i = rand(self.size())
        alphabet = ["A", "C", "G", "T"]
        alphabet = alphabet - [@dna[i]]
        alphabet = alphabet.shuffle
        @dna[i] = alphabet[0]
    end

end
