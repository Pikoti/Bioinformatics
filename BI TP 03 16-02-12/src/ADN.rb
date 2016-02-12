class ADN

    attr_accessor :adn

    def initialize (adn)
        @adn = adn.upcase
    end

    def clone ()
        Genom.new(@adn)
    end

    def size ()
        @genom.length
    end

    def to_s
        @adn
    end

    def mutate (n)
        n.times do self.changeOneLetter() end
        self
    end

    def toKmers (size)
        str = ""
        kmers = []
        @adn.each_char do |c|
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
        alphabet = alphabet - [@adn[i]]
        alphabet = alphabet.shuffle
        @adn[i] = alphabet[0]
    end

end
