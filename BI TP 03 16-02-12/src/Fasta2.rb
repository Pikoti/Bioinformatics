require './src/Sequence.rb'

class Fasta2

    attr_accessor :sequences

    def initialize(file)
        @sequences = []
        text = File.read(file)
        text = text.gsub(/\s+/, "\n")
        lines = text.split("\n")
        text = ""
        lines.each do |line|
            if line[0] == ">"
                text += "\n" + line + "\n"
            else
                text += line
            end
        end
        lines = text.split("\n")[1..-1]
        lines.each_slice(2) do |id, adn|
            @sequences.push(Sequence.new(id[1..-1], adn))
        end
    end
end
