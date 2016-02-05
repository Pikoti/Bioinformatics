require './src/Genom.rb'

class Fasta

    def self.toGenom (file)
        fasta = File.read(file).split("\n", 2)
        Genom.new(fasta[0][1..-1], fasta[1].delete("\n"))
    end
end
