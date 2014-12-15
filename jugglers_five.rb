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

    if @teams_placed
      op("View placed teams","v",space)
      op("See a circuit","c",space)
      op("Get circuit 1970's J value", "j",space)
    end

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
    puts_option(course.circuits(number.to_i).show_jugglers_on_circuit)
  end

  def choices(course)
    choice = ""
    @teams_placed = false
    start_screen
    until choice == "e"
      choice = gets.chomp
      case choice
      when "p"
        course.place_teams
        @teams_placed = true
        puts_option("Teams have been placed. it took #{course.time}")
      when "v"
        begin
          course.show_jugglers_on_circuit
        rescue => e
          puts "thats not a circuit"
        end
        puts_option("")
      when "c"
        get_circuit(course)
      when "j"
        puts_option(course.get_j_value_of_1970)
      end
    end
  end

end

class Skilz

  attr_reader :hand_eye, :endurance, :pizzazz, :board, :number

  def initialize(board)
    @board = board
    @hand_eye, @endurance, @pizzazz = 0,0,0
  end

  def add_skilz(skilz, number)
    @hand_eye, @endurance, @pizzazz = skilz
    @number = number
  end

  def all_skilz
    [self.hand_eye, self.endurance, self.pizzazz]
  end

end

class Circuit < Skilz

  attr_accessor :jugglers

  def initialize( board )
    super
    @jugglers = Array.new( 6, nil )
  end

  def add_juggler_to_circuit(juggler)
    if !@jugglers[5].nil?
      self.replace_shittiest_if_not_shittier(juggler)
    else
      @board.jugglers.pop
      self.add_juggler_to_appropriate_location(juggler)
    end
  end

  def max_jugglers
    @board.max_number_of_jugglers_per_circuit
  end

  def replace_shittiest_if_not_shittier(juggler)
    if @jugglers.last.dot_value > juggler.dot_value
      juggler.remove_choice
    else
      @jugglers[5].remove_choice
      @board.jugglers[-1] = @jugglers[5]
      self.add_juggler_to_appropriate_location(juggler)
    end
  end

  def add_juggler_to_appropriate_location(juggler)
    i = @jugglers.length - 1
    placed = false
    while i > 0
      if !@jugglers[i - 1].nil? && juggler.dot_value < @jugglers[i - 1].dot_value
        @jugglers[i] = juggler
        placed = true
        break
      else
        @jugglers[i] = @jugglers[i - 1]
      end
      i -= 1
    end
    if !placed
      if !@jugglers[0].nil? && juggler.dot_value < @jugglers[0].dot_value
          @jugglers[1] = juggler
      else
        @jugglers[0], @jugglers[1] = juggler, @jugglers[0]

      end
    end
  end

  def show_jugglers_on_circuit
    puts_jugglers = []
    @jugglers.each_with_index do |juggler, i|
      puts_jugglers <<  "j#{juggler.number}"

      juggler.choice_circuits.each do |choice, circuit|
        puts_jugglers << "C#{circuit[0]}:#{circuit[1]}"
      end

      puts_jugglers << " -- " if i != @jugglers.length - 1
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
    @choice = 0
  end

  def first_choice
    @choice < 6 ? @choice_circuits[choice] : @other_choices[ choice - 6 ]
  end

  def dot_value
    first_choice[1]
  end

  def add_skilz_and_choice_circuits(skilz, number, choice_circuits)
    self.add_skilz(skilz, number)
    self.add_choice_circuits(choice_circuits)
  end

  def add_choice_circuits(choice_circuits)
    choice_circuits.each_with_index do |circuit_number, index|
      circuit_num = Integer(circuit_number)
      dot_value = get_dot_value(circuit_num)
      value = [circuit_num, dot_value]

      @choice_circuits[index] = value
    end
  end

  def add_circuit_dot_p(circuits)
    preferred_circuit_numbers = @choice_circuits.values.map!{ |arr| arr[0] }
    circuits.each do |c|
      next if preferred_circuit_numbers.include?(c.number)
      @other_choices << [c.number, get_dot_value(c.number)]
    end
    @other_choices = @other_choices.sort_by { |choice| -choice[1] }
  end

  def remove_choice
    @choice += 1
    self.add_circuit_dot_p(@board.circuits) if @choice == 6
    # @board.jugglers << self
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
      # split_line = line.scan(/\d+(?=,)|\d+(?=\ )|\d+(?=$)/)
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
    [Integer(split_line[0]),split_line[1,3].map! { |nums| Integer(nums) }]
  end
end

class Course
  include Helpers
  attr_accessor :Dm, :time

  def initialize
    @time = Time.now
    @dm = Dm.new(File.readlines("jugglefest.txt"))
  end

  def place_teams
    while !self.jugglers.empty?
      juggler = self.jugglers[-1]
      self.add_juggler_to_circuit(juggler)

      self.puts_jugglers_remaining_to_be_placed
    end

    @time = Time.now - @time
  end

  def jugglers(*juggler_number)
    juggler_number.empty? ? @dm.jugglers : @dm.jugglers[juggler_number.first]
  end

  def circuits(*circuit_number)
    circuit_number.empty? ? @dm.circuits : @dm.circuits[circuit_number.first]
  end

  def add_juggler_to_circuit(juggler)
    circuit_number = juggler.first_choice[0]
    circuits_jugglers = circuits[circuit_number]
    circuits_jugglers.add_juggler_to_circuit(juggler)
  end

  def puts_jugglers_remaining_to_be_placed
    clear_screen
    puts "there are #{self.jugglers.length} jugglers to be placed"
  end

  def get_all_circuits
    all_circuits = []
    @dm.circuits.each_with_index do |circuit, index|
      all_circuits << "C#{index} #{circuit.show_jugglers_on_circuit}"
    end
    all_circuits
  end

  def show_jugglers_on_circuit
    self.get_all_circuits.each do |circuit|
      puts circuit
      puts "\n\n\n\n\n"
    end
  end

  def get_j_value_of_1970
    self.circuits(1970).jugglers.map(&:number).inject(:+)
  end

end

include Helpers
c = Course.new

choices(c)
