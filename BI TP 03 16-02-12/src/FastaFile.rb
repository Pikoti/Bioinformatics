require './src/FastaSequence.rb'

class FastaFile

    def initialize (file)
        file = File.read(file)
        file = clean(file)
        @sequences = parse(file)
    end

    def each
        @sequences.each do |item|
            yield item
        end    
    end

    def map
        @sequences.map do |item|
            yield item
        end    
    end

    def at (i)
        @sequences[i]
    end

    private

    def clean (file)
        clean = ""
        file = file.gsub(/\s+/, "\n")
        lines = file.split("\n")
        lines.each do |line|
            if line[0] == ">"
                clean += "\n" + line + "\n"
            else
                clean += line
            end
        end
        clean
    end

    def parse (file)
        sequences = []
        lines = file.split("\n")[1..-1]
        lines.each_slice(2) do |id, dna|
            sequences.push(FastaSequence.new(id[1..-1], dna))
        end
        sequences
    end
end
