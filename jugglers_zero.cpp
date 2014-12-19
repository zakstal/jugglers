#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>

class CircuitInterface
{
  public:
    void add_skilz( int, int, int, int);
    int* all_skilz();
};
class JugglerInterface
{
public:
  int number;
  void add_skilz_and_choice_circuits( int*, int, CircuitInterface* );
  //add all prototype functions to here
};

const int circuit_count = 2000;
const int juggler_count = 12000;
const int max_jugglers_per_circuit = 6;
const int choice_count = 6;

class Dm
{
  public:
    std::stack<JugglerInterface> jugglers;
    CircuitInterface circuits[circuit_count];
    Dm( std::ifstream );
    void new_circuit( std::istringstream&);
    void new_juggler( std::istringstream&);
    int* get_skilz( std::istringstream& );
    CircuitInterface* get_choice_circuits( std::istringstream&  );
};

Dm::Dm( std::ifstream file ){
  std::string line;
  std::string letter;
  while (getline(file, line)) {
    std::istringstream iss(line);
    iss >> letter;
    if (letter == "C") {
      new_circuit( iss );
    } else if ( letter == "J") {
      new_juggler( iss );
    }
  }
  return;
};

void Dm::new_circuit( std::istringstream& iss ){
  std::string str;
  int circuit_num;
  iss >> str;
  circuit_num = std::atoi(str.substr(1,str.size() - 1).c_str());
  int* skilz = get_skilz( iss );
  circuits[circuit_num].add_skilz( skilz[0], skilz[1], skilz[2], circuit_num );
};

void Dm::new_juggler( std::istringstream& iss ){
  std::string str;
  int juggler_num;
  iss >> str;
  juggler_num = std::atoi(str.substr(1,str.size() - 1).c_str());
  int* skilz = get_skilz( iss );
  CircuitInterface* choice_circuits = get_choice_circuits( iss );
  JugglerInterface juggler;
  juggler.number = juggler_num;
  juggler.add_skilz_and_choice_circuits(skilz, juggler_num, choice_circuits);
  jugglers.push(juggler);
};
//
//
//
// int* Dm::get_number_and_skilz( std::string split_line ){
// [Integer( split_line[0] ),split_line[1,3].map! { |nums| Integer( nums ) }]
// };
//









//
// class Course
//
// {
//   public:
//
// }
//
//
//
// def initialize
// @time = Time.now
// @dm = Dm.new( File.readlines( "jugglefest.txt" ) )
// end
//
// def place_teams
// while !self.jugglers.empty?
// juggler = self.jugglers[-1]
// self.add_juggler_to_circuit( juggler )
//
// self.puts_jugglers_remaining_to_be_placed
// end
//
// @time = Time.now - @time
// end
//
// def jugglers( *juggler_number )
// juggler_number.empty? ? @dm.jugglers : @dm.jugglers[juggler_number.first]
// end
//
// def circuits( *circuit_number )
// circuit_number.empty? ? @dm.circuits : @dm.circuits[circuit_number.first]
// end
//
// def add_juggler_to_circuit( juggler )
// circuit_number = juggler.first_choice[0]
// circuit = circuits[circuit_number]
// circuit.add_juggler_to_circuit( juggler )
// end
//
// def puts_jugglers_remaining_to_be_placed
// clear_screen
// puts "there are #{self.jugglers.length} jugglers to be placed"
// end
//
// def get_all_circuits
// all_circuits = []
// @dm.circuits.each_with_index do |circuit, index|
// all_circuits << "C#{index} #{circuit.show_jugglers_on_circuit}"
// end
// all_circuits
// end
//
// def show_jugglers_on_circuit
// self.get_all_circuits.each do |circuit|
// puts circuit
// puts "\n\n\n\n\n"
// end
// end
//
// def get_j_value_of_1970
// self.circuits( 1970 ).jugglers.map( &:number ).inject( :+ )
// end
//









class Skilz

{
  public:
      int hand_eye;
      int pizzazz;
      int endurance;
      int number;
      Dm dm;
      Skilz( Dm );
      void add_skilz( int, int, int, int);
      int* all_skilz();

};

Skilz::Skilz( Dm md ) {
  dm = md;
  return;
};

void Skilz::add_skilz(int h, int p, int e, int n) {
  hand_eye = h;
  pizzazz = p;
  endurance = e;
  number = n;
  return;
};

int* Skilz::all_skilz() {
  int all_skills[3] = {hand_eye, pizzazz, endurance};
  return all_skills;
};

class Juggler : public Skilz, public JugglerInterface

{
  public:
    int choice;
    int* other_choices[circuit_count - choice_count];
    int* choice_circuits[circuit_count];
    int* first_choice();
    int dot_value();
    void add_choice_circuits( int* );
    void add_skilz_and_choice_circuits( int*, int, int** );
    int** add_circuit_dot_p( CircuitInterface* );
    void remove_current_top_choice();
    int* circuit_skilz( int );
    int get_dot_value( int );
};

int* Juggler::first_choice() {
  return choice < 6 ? choice_circuits[choice] : other_choices[ choice - 6 ];
};

int Juggler::dot_value() {
  return first_choice()[1];
};


void Juggler::add_choice_circuits( int* circuit_nums ){
  int i = 0;
  while (i < sizeof(circuit_nums)) {
    int circuit_num = circuit_nums[i];
    int dot_value = get_dot_value( circuit_num );
    int value[2] = {circuit_num, dot_value};

    choice_circuits[i] = value;
    i++ ;
  }
  return;
};

void Juggler::remove_current_top_choice(){
  choice++;
  if (choice == 6){
    add_circuit_dot_p( dm.circuits );
  }
  return;
};

int* Juggler::circuit_skilz( int circuit_number ) {
  return dm.circuits[ circuit_number ].all_skilz();
};

int Juggler::get_dot_value( int circuit_number){
  int* circuit = circuit_skilz( circuit_number );
  int* skilz = all_skilz();
  //maybe make into global arrays
  int total = 0;
  int i = 0;
  while (i < sizeof(circuit)) {
    total += ( circuit[i] * skilz[i] );
    i += 1;
  }
  return total;
};

class Circuit : public Skilz, public CircuitInterface

{
  public:
    Juggler jugglers[ max_jugglers_per_circuit];
    Dm dm;
    int max_jugglers();
    bool not_nil_and_new_juggler_is_better( Juggler, int );
    void switch_worst_juggler_with_new_juggler(Juggler);
    void replace_worst_juggler_if_new_juggler_is_better( Juggler );
    void add_juggler_to_circuit( Juggler );
    void show_jugglers_on_circuit();
};

// Circuit::Circuit( Dm md) {
//   dm = md;
//   int i = 0;
//   while (i < max_jugglers()) {
//     jugglers[i] = 0;
//     i++;
//   };
// };

int Circuit::max_jugglers() {
  return max_jugglers_per_circuit;
};

//constant in Dm for max jugglers

bool Circuit::not_nil_and_new_juggler_is_better( Juggler j, int i ){
  return (jugglers[i - 1] != 0) && (j.dot_value() < jugglers[i - 1].dot_value());
};

void Circuit::switch_worst_juggler_with_new_juggler( Juggler j ){
  jugglers[sizeof(jugglers) -1].remove_current_top_choice();
  dm.jugglers[sizeof(jugglers) -1] = jugglers[sizeof(jugglers) -1];
  jugglers[sizeof(jugglers) -1] = j;
  sort(jugglers, jugglers + sizeof(jugglers), sortfunction);
  //sort needs to have a nil juggler
  return;
};

void Circuit::replace_worst_juggler_if_new_juggler_is_better( Juggler j ){
  if (jugglers[sizeof(jugglers) -1].dot_value() > j.dot_value()) {
    j.remove_current_top_choice();
  } else {
     switch_worst_juggler_with_new_juggler( j );
  }
  return;
};

void Circuit::add_juggler_to_circuit( Juggler j ){
  if ( jugglers[sizeof(jugglers) -1] != 0 ){
    replace_worst_juggler_if_new_juggler_is_better( j );
  } else {
    dm.jugglers.pop();
    jugglers[sizeof(jugglers) -1]  = j;
    sort(jugglers, jugglers + sizeof(jugglers), sortfunction);
    //sort needs to have a nil juggler
  }
  return;
};

//dm jugglers as stack
void Circuit::show_jugglers_on_circuit(){
  int i = 0;
  int j = 0;
  while (i < sizeof(jugglers)) {
    std::cout << "j" << jugglers[i].number << " ";
    while (j < sizeof(jugglers[i].choice_circuits)) {
      std::cout << "C" << jugglers[i].choice_circuits[j][0] << ":" << jugglers[i].choice_circuits[j][1] << " ";
      j++;
    };
    if (i < sizeof(jugglers) - 1) {
      std::cout << " -- ";
    }
    i++;
    j = 0;
  }
  std::cout << std::endl;
};
