class Genom

    attr_accessor :id, :genom

    def initialize (id, genom)
        @id = id
        @genom = genom
    end

    def clone ()
        Genom.new(@id, @genom)
    end

    def size ()
        @genom.length
    end

    def stats ()
        "#{@id} #{self.size()}"
    end

    def fasta
        ">#{@id}\n#{@genom}"
    end

    def to_s
        @genom
    end

    def mutate (n)
        n.times do self.changeOneLetter() end
        self
    end

    def toKmers (size)
        str = ""
        kmers = []
        @genom.each_char do |c|
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
        alphabet = alphabet - [@genom[i]]
        alphabet = alphabet.shuffle
        @genom[i] = alphabet[0]
    end

    def window(i, length)
        @genom[i..i + length]
    end    

end
