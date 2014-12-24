#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;
const int circuit_count = 3;
const int max_jugglers_per_circuit = 4;
const int choice_count = 3;

class JugglerInterface
{
public:
  virtual void add_skilz_and_choice_circuits( int*, int, int* ) {};
  int number;
  int* choice_circuits[circuit_count];
  // virtual void remove_current_top_choice() {};
  virtual int dot_value() {};
  virtual int* first_choice() {};
  int choice;
  JugglerInterface() : choice( 0 ) {};
  //add all prototype functions to here
};


class CircuitInterface
{
public:
  JugglerInterface jugglers[ max_jugglers_per_circuit];
  virtual void add_skilz( int, int, int) {};
  virtual void add_juggler_to_circuit( JugglerInterface ) {};
  virtual int* all_skilz() {};
  int number;
};

class DmInterface
{
  public:
    CircuitInterface circuits[circuit_count];
};

class SkilzInterface{};






class Skilz: public SkilzInterface

{
public:
  int hand_eye;
  int pizzazz;
  int endurance;
  DmInterface dm;
  Skilz( DmInterface md ) : dm( md ) {};
  void add_skilz( int, int, int);
  int* all_skilz();
};

void Skilz::add_skilz(int h, int p, int e) {
  hand_eye = h;
  pizzazz = p;
  endurance = e;
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
  int choice_numbers[choice_count];
  int dot_value();
  int* first_choice();
  Juggler( DmInterface md ) : Skilz( md ), JugglerInterface() {};
  void remove_current_top_choice( DmInterface& );
  void add_choice_circuits( int*, DmInterface& );
  void add_skilz_and_choice_circuits( int*, int, int*, DmInterface& dm );
  void add_circuit_dot_p( CircuitInterface*, DmInterface& );
  int* circuit_skilz( int, DmInterface& );
  int get_dot_value( int, DmInterface& );
};

int* Juggler::first_choice() {
  cout << "choice " << choice << endl;
  cout << "choice circuits first choice in here" << choice_circuits[0][0] << " " << choice_circuits[0][1] << endl;
  return choice < 6 ? choice_circuits[choice] : other_choices[ choice - 6 ];
};

int Juggler::dot_value() {
  return first_choice()[1];
};

bool sort_other_choices( int* r1, int* r2){
  return r1[1] > r2[1];
};

void Juggler::add_circuit_dot_p( CircuitInterface*, DmInterface& dm ){
  int skipped = 0;
  int i = 0;
  while (i < circuit_count) {
    if (std::find(std::begin(choice_numbers), std::end(choice_numbers), i) != std::end(choice_numbers)){
      skipped++;
      continue;
    }
    int values[2] = { i, get_dot_value( i, dm ) };
    other_choices[i - skipped] = values;
    i++;
  }

  std::sort(other_choices, other_choices + (circuit_count - choice_count), sort_other_choices);
  return;
};



void Juggler::add_choice_circuits( int* circuit_nums, DmInterface& dm ){
  int i = 0;
  while (i < sizeof(circuit_nums)) {
    int circuit_num = circuit_nums[i];
    int dot_value = get_dot_value( circuit_num, dm );
    int value[2] = {circuit_num, dot_value};

    choice_circuits[i] = value;
    i++ ;
  }
  return;
};

void Juggler::add_skilz_and_choice_circuits( int* s, int n, int* cc, DmInterface& dm ){
  number = n;
  add_skilz( s[0], s[1], s[2]);
  add_choice_circuits( cc, dm );

  int i = 0;
  while (i < circuit_count){
    choice_numbers[i] = cc[i];
    i++;
  }
  return;
};

void Juggler::remove_current_top_choice( DmInterface& dm ){
  choice++;
  if (choice == 6){
    add_circuit_dot_p( dm.circuits, dm );
  }
  return;
};

int* Juggler::circuit_skilz( int circuit_number, DmInterface& dm ) {
  return dm.circuits[ circuit_number ].all_skilz();
};

int Juggler::get_dot_value( int circuit_number, DmInterface& dm){
  int* circuit = circuit_skilz( circuit_number, dm );
  int* skilz = all_skilz();
  //maybe make into global arrays
  int total = 0;
  int i = 0;
  while (i < 3) {
    total += ( circuit[i] * skilz[i] );
    i += 1;
  }
  return total;
};


class Dm : public DmInterface
{
  public:
    Juggler NILJUGGLER;
    std::vector<Juggler> jugglers;
    CircuitInterface circuits[circuit_count];
    Dm( std::ifstream& );
    void new_circuit( std::string&);
    void new_juggler( std::string&);
    int* get_skilz( std::istringstream& );
    int* get_choice_circuits( std::istringstream&  );
};

Dm::Dm( std::ifstream& file ) : NILJUGGLER( *this ) {
  int skilz[3] = {0,0,0};
  int circuit_choices[6] = { -1, -1, -1, -1, -1, -1 };
  NILJUGGLER.add_skilz_and_choice_circuits( skilz, -1, circuit_choices, *this );
  std::string line;
  char letter;
  while (getline(file, line)) {
    letter = line[0];
    if (letter == 'C') {
      new_circuit( line );
    } else if ( letter == 'J') {
      new_juggler( line );
    }
  }

  return;
};

void Dm::new_circuit( std::string& line ){
  std::istringstream iss(line);
  std::string str;
  int circuit_num;
  iss >> str;
  circuit_num = std::atoi(str.substr(1,str.size() - 1).c_str());
  int* skilz = get_skilz( iss );
  circuits[circuit_num].number = circuit_num;
  circuits[circuit_num].add_skilz( skilz[0], skilz[1], skilz[2]);
  int i = 0;
  while (i < max_jugglers_per_circuit){
    circuits[circuit_num].jugglers[i] = NILJUGGLER;
    i++;
  }
  return;
};

int* Dm::get_choice_circuits( std::istringstream& iss ){
  int c_circuits[choice_count];
  int num;
  int i = 0;
  cout << "iss " << iss << endl;
  while (iss >> num ){
    cout << "num " << num << endl;
    c_circuits[i] = num;
    i++;
  }
  return c_circuits;
};

void Dm::new_juggler( std::string& line ){
  std::istringstream iss(line);
  std::string str;
  int juggler_num;
  iss >> str;
  iss >> str;
  juggler_num = std::atoi(str.substr(1,str.size() - 1).c_str());
  int* skilz = get_skilz( iss );
  int* choice_circuits = get_choice_circuits( iss );
  cout << "choice circuits " << *choice_circuits << endl;
  Juggler juggler;
  juggler.number = juggler_num;
  juggler.add_skilz_and_choice_circuits(skilz, juggler_num, choice_circuits);
  jugglers.push_back(juggler);
};

int* Dm::get_skilz( std::istringstream& iss ){
  int num;
  int skilz[3];
  int i = 0;
  while (iss >> num) {
    skilz[i] = num;
    i++;
  }
  return skilz;
};


class Circuit : public Skilz, public CircuitInterface

{
  public:
    Dm dm;
    Juggler jugglers[ max_jugglers_per_circuit];
    Circuit(Dm md);
    int max_jugglers();
    void add_juggler_to_circuit( Juggler );
    bool not_nil_and_new_juggler_is_better( Juggler, int );
    void switch_worst_juggler_with_new_juggler(Juggler);
    void replace_worst_juggler_if_new_juggler_is_better( Juggler );
    void show_jugglers_on_circuit();

};

Circuit::Circuit( Dm md ) : Skilz(md), dm(md) {
  int i = 0;
  while (i < max_jugglers()) {
    jugglers[i] = dm.NILJUGGLER;
    i++;
  };
};

int Circuit::max_jugglers() {
  return max_jugglers_per_circuit;
};

bool sort_by_dot_value( Juggler& j1, Juggler& j2 ){
  return j1.dot_value() > j2.dot_value();
};

bool Circuit::not_nil_and_new_juggler_is_better( Juggler j, int i ){
  return (jugglers[i - 1].number != -1 ) && (j.dot_value() < jugglers[i - 1].dot_value());
};

void Circuit::switch_worst_juggler_with_new_juggler( Juggler j ){
  jugglers[max_jugglers_per_circuit -1].remove_current_top_choice( dm );
  dm.jugglers[dm.jugglers.size() -1] = jugglers[max_jugglers_per_circuit -1];
  jugglers[max_jugglers_per_circuit -1] = j;
  std::sort(std::begin(jugglers), std::end(jugglers), sort_by_dot_value);
  return;
};

void Circuit::replace_worst_juggler_if_new_juggler_is_better( Juggler j ){
  if (jugglers[max_jugglers_per_circuit -1].dot_value() > j.dot_value()) {
    j.remove_current_top_choice( dm );
  } else {
     switch_worst_juggler_with_new_juggler( j );
  }
  return;
};

void Circuit::add_juggler_to_circuit( Juggler j ){
  if ( jugglers[max_jugglers_per_circuit -1].number != -1 ){
    replace_worst_juggler_if_new_juggler_is_better( j );
  } else {
    dm.jugglers.pop_back();
    //it may call the destructor function on the popped element
    jugglers[max_jugglers_per_circuit -1]  = j;
    sort(begin(jugglers), end(jugglers), sort_by_dot_value);
    //KC says: We might have to do jugglers + max_jugglers_per_circuit there.
  }
  return;
};

void Circuit::show_jugglers_on_circuit(){
  int i = 0;
  int j = 0;
  while (i < max_jugglers_per_circuit) {
    std::cout << "j" << jugglers[i].number << " ";
    while (j < sizeof(jugglers[i].choice_circuits)) {
      //KC says: The above line probably won't work. Not sure though or what to replace it with.
      //         Also is this the only display function we have so far?
      std::cout << "C" << jugglers[i].choice_circuits[j][0] << ":" << jugglers[i].choice_circuits[j][1] << " ";
      j++;
    };
    if (i < max_jugglers_per_circuit - 1) {
      std::cout << " -- ";
    }
    i++;
    j = 0;
  }
  std::cout << std::endl;
};

class Course

{
public:
  Dm dm;
  Course( std::ifstream& file ) : dm( file ) {};
  void place_teams();
  std::vector<Juggler> jugglers();
  CircuitInterface* circuits();
  void add_juggler_to_circuit( Juggler );
  int get_j_value_of_1970();
};

//
//
// Course::Course( std::ifstream file ){
//
// };
//
void Course::place_teams(){
  while (jugglers().size() != 0){
    cout << "SIZE OF JUGGLERS!! " << jugglers().size() << endl;
    // *juggler = jugglers()[jugglers().size() - 1];
    add_juggler_to_circuit( jugglers()[jugglers().size() - 1] );
  }
  return;
};

std::vector<Juggler> Course::jugglers(){
  return dm.jugglers;
};

CircuitInterface* Course::circuits(){
  return dm.circuits;
};

void Course::add_juggler_to_circuit( Juggler juggler ){
  cout << "juggler choice " << juggler.choice << endl;
  int circuit_number = juggler.first_choice()[0];
  cout << "circuit number " << circuit_number << endl;
  CircuitInterface circuit = circuits()[circuit_number];
  circuit.add_juggler_to_circuit( juggler );
  return;
};

int Course::get_j_value_of_1970(){
  CircuitInterface circuit1970 = dm.circuits[ 1 ];
  int total = 0;
  int i = 0;
  while ( i < max_jugglers_per_circuit) {
    total += circuit1970.jugglers[i].number;
    i++;
  }
  return total;
};

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

//


int main() {
  ifstream stream("jugglefest_short.txt");
  Course c(stream);
  c.place_teams();
  cout << "not here" << endl;
  cout << c.get_j_value_of_1970() << endl;
  return 0;
};
