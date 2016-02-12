require './src/ADN.rb'

class Sequence

    attr_accessor :id, :adn

    def initialize (id, adn)
        @id = id
        @adn = ADN.new(adn)
    end

end
