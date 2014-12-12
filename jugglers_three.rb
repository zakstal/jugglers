module Helpers

  def space(num)
    space = ""
    num.times do
      space << " "
    end
    space
  end

  def sc(words,justify)
    puts space(justify) + words.to_s
  end

  def op(words,option, justify)
    puts space(justify) + options(words, option)
  end

  def options(words, option)
    space = 35 - (words.length + option.length)
    words + space(space) + option
  end

  def clear_screen
    puts "\e[H\e[2J"
    puts "\n\n\n\n"
  end

  def start_screen
    clear_screen
    space = 10
    sc("Welcome to Jugglefest!",space)
    puts ""
    op("Place teams","p",space)
    op("View placed teams","v",space)
    op("See a circuit","c",space)
    op("Get circuit 1970's J value", "j",space)
    op("exit","e",space)
    puts ""
    sc("Enter option >>",0)
  end

  def puts_option(words)
    clear_screen
    sc(words,10)
    puts ""
    sc("Press Enter to return",0)
    gets
    start_screen
  end

  def get_circuit(course)
    clear_screen
    sc("Enter circuit number",10)
    puts ""
    number = gets.chomp
    puts course.circuits(number).show_jugglers_on_circuit
    puts_options("")
  end

  def choices(course)
    choice = ""
    start_screen
    until choice == "e"
      choice = gets.chomp
      case choice
      when "p"
        course.place_teams
        puts_option("Teams have been placed")
      when "v"
        puts_option(course.show_jugglers_on_circuit)
      when "c"
        get_circuit(course)
      when "j"
        puts_option(course.get_j_value_of_1970)
      end
    end
  end

end

class Skilz

  attr_reader :hand_eye, :endurance, :pizzaz, :board, :number

  def initialize(board)
    @board = board
    @hand_eye, @endurance, @pizzaz = 0,0,0
  end

  def add_skilz(skilz, number)
    @hand_eye, @endurance, @pizzaz = skilz
    @number = number
  end

  def all_skilz
    [self.hand_eye, self.endurance, self.pizzaz]
  end

end

class Circuit < Skilz

  attr_accessor :jugglers

  def initialize(board)
    super
    @jugglers = []
  end

  def add_juggler_to_circuit(juggler)
    @jugglers << juggler
    @jugglers = @jugglers.sort_by { |juggler| -juggler.choice[1] }
  end

  def max_jugglers
    @board.max_number_of_jugglers_per_circuit
  end

  def show_jugglers_on_circuit
    puts_jugglers = []
    @jugglers.each do |juggler|
      puts_jugglers <<  "j#{juggler.number}"
      juggler.choice_circuits.each do |choice, circuit|
        puts_jugglers << "C#{circuit[0]}:#{circuit[1]}"
      end
    end
    puts_jugglers.join(" ")
  end

end


class Juggler < Skilz

  attr_accessor :choice_circuits, :choice, :other_choices

  def initialize(board)
    super
    @choice_circuits = {}
    @other_choices = []
    @choice = []
  end

  def choice
    @choice.first
  end

  def add_skilz_and_choice_circuits(skilz, number, choice_circuits)
    self.add_skilz(skilz, number)
    self.add_choice_circuits(choice_circuits)
  end

  def add_choice_circuits(choice_circuits)
    choice_circuits.each_with_index do |circuit_number, index|
      dot_value = get_dot_value(circuit_number.to_i)
      value = [circuit_number, dot_value]

      @choice << value
      @choice_circuits[index] = value
    end
  end

  def add_circuit_dot_p(circuits)
    circuits.each do |c|
      @other_choices << [c.number, get_dot_value(c.number)]
    end
    @other_choices = @other_choices.sort_by { |choice| -choice[1] }
  end

  def remove_choice
    choice = @choice.shift
    @other_choices -= [choice]
    @choice = self.add_circuit_dot_p(@board.circuits) if @choice.empty?
  end

  private

  def get_dot_value(circuit_number)
    circuit = circuit_skilz(circuit_number)
    skilz = self.all_skilz

    total = 0
    i = 0
    while i < circuit.length
      total += (circuit[i] * skilz[i])
      i += 1
    end
    total
  end

  def circuit_skilz(circuit_number)
    self.board.circuits[circuit_number].all_skilz
  end
end


class Dm

  attr_accessor :jugglers, :circuits
  attr_reader :max_number_of_jugglers_per_circuit

  def initialize(file)
    file = file
    make_arrays(file)
    create_circuts_and_jugglers(file)
  end

  def make_arrays(file)
    counts = file.map{ |line| line[0]}
    ciructs_count = counts.count("C")
    juggers_count = counts.count("J")

    @max_number_of_jugglers_per_circuit = juggers_count/ciructs_count
    @circuits = Array.new(ciructs_count){ Circuit.new(self) }
    @jugglers = Array.new(juggers_count){ Juggler.new(self) }
  end

  def create_circuts_and_jugglers(file)
    file.each do |line|
      split_line = line.scan(/\d+/)
      new_circuit(split_line) if line[0] == "C"
      new_juggler(split_line) if line[0] == "J"
    end
  end

  def new_circuit(split_line)
    circuit_number, skilz = get_number_and_skilz(split_line)
    @circuits[circuit_number].add_skilz(skilz, circuit_number)
  end

  def new_juggler(split_line)
    juggler_number, skilz = get_number_and_skilz(split_line)
    choice_circuits = split_line[4..-1]
    @jugglers[juggler_number].add_skilz_and_choice_circuits(skilz, juggler_number, choice_circuits)
  end

  private

  def get_number_and_skilz(split_line)
    [split_line[0].to_i,split_line[1,3].map(&:to_i)]
  end
end

class Course
  include Helpers
  attr_accessor :Dm

  def initialize
    @dm = Dm.new(File.readlines("jugglefest.txt"))
  end

  def place_teams
    self.place_jugglers_to_circuits while !self.jugglers.empty?
  end

  def jugglers(*juggler_number)
    juggler_number.empty? ? @dm.jugglers : @dm.jugglers[juggler_number.first]
  end

  def circuits(*circuit_number)
    circuit_number.empty? ? @dm.circuits : @dm.circuits[circuit_number.first]
  end

  def add_juggler_to_circuit(juggler)
    circuit_number, dot_v = juggler.choice[0]
    circuits_jugglers = circuits[circuit_number.to_i]
    circuits_jugglers.add_juggler_to_circuit(juggler)
  end

  def place_jugglers_to_circuits
    until self.jugglers.empty?
      juggler = self.jugglers.shift
      self.add_juggler_to_circuit(juggler)
    end

    self.remove_excess_jugglers_from_all_circuits
    self.puts_jugglers_remaining_to_be_placed
  end

  def add_jugglers_greater_than_max_back_to_dmjugglers( circuit, max )
    @dm.jugglers += circuit.jugglers[max..-1]
    circuit.jugglers = circuit.jugglers[0...max]
  end

  def promote_jugglers_next_prefered_choice
    @dm.jugglers.each do |juggler|
      juggler.remove_choice
    end
  end

  def remove_excess_jugglers_from_all_circuits
    max = @dm.max_number_of_jugglers_per_circuit
    self.circuits.each do |circuit|
       self.add_jugglers_greater_than_max_back_to_dmjugglers( circuit, max ) if circuit.jugglers.length > max
    end

    self.promote_jugglers_next_prefered_choice
  end

  def puts_jugglers_remaining_to_be_placed
    clear_screen
    puts "there are #{self.jugglers.length} jugglers to be placed"
  end

  def show_jugglers_on_circuit
    all_circuits = []
    @dm.circuits.each_with_index do |circuit, index|
      all_circuits << "C#{index} #{circuit.show_jugglers_on_circuit}"
    end
    all_circuits.join(" ")
  end

  def get_j_value_of_1970
    self.circuits(1970).jugglers.map(&:number).inject(:+)
  end

end

include Helpers
c = Course.new

choices(c)
