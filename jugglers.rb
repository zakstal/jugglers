class Skilz

  attr_reader :hand_eye, :endurance, :pizzaz, :board

  def initialize(board)
    @board = board
    @hand_eye, @endurance, @pizzaz = 0,0,0
  end

  def add_skilz(skilz)
    @hand_eye, @endurance, @pizzaz = skilz
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

  def show_jugglers_on_circuit
    puts_jugglers = []
    @jugglers.each_with_index do |juggler, juggler_number|
      puts_jugglers <<  "j#{juggler_number}"
      juggler.choice_circuits.each do |choice, circuit|
        puts_jugglers << "C#{circuit[0]}:#{circuit[1]}"
      end
    end
    puts puts_jugglers.join(" ")
  end

  def add_juggler_to_circuit(juggler)
    @jugglers << juggler
    @jugglers.sort_by { |juggler| juggler.choice[0]}
    self.kick_out_jugglers
  end

  def kick_out_jugglers
    # puts @jugglers.length
    if @jugglers.length > max_jugglers
      juggler = @jugglers.pop
      juggler.remove_choice
      @board.jugglers << juggler
    end
  end

  def max_jugglers
    @board.max_number_of_jugglers_per_circuit
  end

end


class Juggler < Skilz

  attr_accessor :choice_circuits

  def initialize(board)
    super
    @choice_circuits = {}
  end

  def add_skilz_and_choice_circuits(skilz, choice_circuits)
    self.add_skilz(skilz)
    self.add_choice_circuits(choice_circuits)
    @choice = @choice_circuits.values
  end

  def add_choice_circuits(choice_circuits)
    choice_circuits.each_with_index do |circuit_number, index|
      dot_value = get_dot_value(circuit_number.to_i)

      @choice_circuits[index] = [circuit_number, dot_value]
    end
  end

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

  def choice
    @choice.first
  end

  def remove_choice
    puts @choice
    @choice.unshift
    puts @choice
  end
end


class Board

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
    @circuits[circuit_number].add_skilz(skilz)
  end

  def new_juggler(split_line)
    juggler_number, skilz = get_number_and_skilz(split_line)
    choice_circuits = split_line[4..-1]
    @jugglers[juggler_number].add_skilz_and_choice_circuits(skilz, choice_circuits)
  end

  private

  def get_number_and_skilz(split_line)
    [split_line[0].to_i,split_line[1,3].map(&:to_i)]
  end

  def add_to_array(array, object, count)
    count.times do
      array << object(self)
    end
  end
end

class Course

  attr_accessor :course

  def initialize
    @course = Board.new(File.readlines("jugglefest_short.txt"))
    self.place_jugglers_to_circuits
  end

  def place_jugglers_to_circuits
    puts self.jugglers.length
    self.jugglers.each do |juggler|
      circuit_number, dot_v = juggler.choice[0]
      add_juggler_to_circuit(circuit_number, juggler)
      # puts self.jugglers.length
    end
  end

  def jugglers(*juggler_number)
    juggler_number.empty? ? @course.jugglers : @course.jugglers[juggler_number.first]
  end

  def juggler_choice(juggler_number, choice)
    juggler()
  end

  def circuits(*circuit_number)
    circuit_number.empty? ? @course.circuits : @course.circuits[circuit_number.first]
  end

  def add_juggler_to_circuit(circuit_number, juggler)
    circuits_jugglers = circuits[circuit_number.to_i]

    circuits_jugglers.add_juggler_to_circuit(juggler)
  end

  def show_jugglers_on_circuit
    @course.circuits.each do |circuit|
      circuit.show_jugglers_on_circuit
    end
  end

end

#
# c = Course.new
# course = c.course
# puts course.circuits.map { |c| c.hand_eye} == [7,2,7]
# jugglers = course.jugglers
# puts jugglers.first.choice_circuits.map{ |k,v| v.last } == [83, 104, 17]
# puts c.show_jugglers_on_circuit

# C2 J6 C2:128 C1:31 C0:188,  J3 C2:120 C0:171 C1:31,   J10 C0:120 C2:86 C1:21, J0 C2:83 C0:104 C1:17
# C1 J9 C1:23 C2:86 C0:94,    J8 C1:21 C0:100 C2:80,    J7 C2:75 C1:20 C0:106,  J1 C0:119 C2:74 C1:18
# C0 J5 C0:161 C2:112 C1:26,  J11 C0:154 C1:27 C2:108,  J2 C0:128 C2:68 C1:18,  J4 C0:122 C2:106 C1:23
