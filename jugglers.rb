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
    @jugglers.each_with_index do |juggler, juggler_number|
      juggler.choice_circuits
      puts "j#{juggler_number}"
    end
  end

  def add_juggler_to_circuit(juggler)
    @jugglers << juggler
    @jugglers.sort_by { |juggler| juggler.choice_circuits[0][1]}
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

end


class Setup

  attr_accessor :jugglers, :circuits

  def initialize(file)
    file = file
    make_arrays(file)
    create_circuts_and_jugglers(file)
  end

  def make_arrays(file)
    counts = file.map{ |line| line[0]}
    ciructs_count = counts.count("C")
    juggers_count = counts.count("J")
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
    @course = Setup.new(File.readlines("jugglefest_short.txt"))
    self.place_jugglers_to_circuits
  end

  def place_jugglers_to_circuits
    self.jugglers.each do |juggler|
      circuit_number, dot_v = juggler.choice_circuits[0]
      add_juggler_to_circuit(circuit_number, juggler)
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

end


course = Course.new
course = course.course
puts course.circuits.map { |c| c.hand_eye} == [7,2,7]
jugglers = course.jugglers
puts jugglers.first.choice_circuits.map{ |k,v| v.last } == [83, 104, 17]
