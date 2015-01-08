
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;
const int circuit_count = 3;
const int choice_circuit_count = 3;
const int max_jugglers_per_circuit = 4;
const int choice_count = 3;

class Skilz
{
public:
  int hand_eye;
  int pizzazz;
  int endurance;
  Skilz() {};
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
  int* all_skills;
  all_skills = new int[3];
  all_skills[0] = hand_eye;
  all_skills[1] = pizzazz;
  all_skills[2] = endurance;
  return all_skills;
};


class Juggler : public Skilz

{
public:
  int number;
  int* choice_circuits[circuit_count];
  int choice;
  int* other_choices[circuit_count - choice_count];
  int choice_numbers[choice_count];
  int dot_value();
  int* first_choice();
  Juggler();
};

Juggler::Juggler() : Skilz(){
  choice = 0;
  number = -1;
  add_skilz(0, 0, 0);
  return;
};

int* Juggler::first_choice() {
  return choice < 6 ? choice_circuits[choice] : other_choices[ choice - 6 ];
};

int Juggler::dot_value() {
  return first_choice()[1];
};

bool sort_other_choices( int* r1, int* r2){
  return r1[1] > r2[1];
};


bool sort_by_dot_value( Juggler& j1, Juggler& j2 ){
  cout << j1.number << " " << j2.number << endl;
  return j1.dot_value() > j2.dot_value();
};

class Circuit : public Skilz
{
public:
  int number;
  Juggler jugglers[ max_jugglers_per_circuit];
  Circuit();
  int max_jugglers();
  void show_jugglers_on_circuit();
};

Circuit::Circuit() : Skilz() {
  // int i = 0;
  // while (i < max_jugglers()) {
  //   jugglers[i] = dm.NILJUGGLER;
  //   i++;
  // };

  //we might have to fill the array
};

int Circuit::max_jugglers() {
  return max_jugglers_per_circuit;
};


void Circuit::show_jugglers_on_circuit(){
  int i = 0;
  int j = 0;
  while (i < max_jugglers_per_circuit) {
    std::cout << "j" << jugglers[i].number << " ";
    while (j < choice_circuit_count) {
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


class Dm
{
  public:
    Juggler NILJUGGLER;
    std::vector<Juggler> jugglers;
    Circuit circuits[circuit_count];
    Dm( std::ifstream& );
    void remove_current_top_choice( Juggler& );
    void add_circuit_dot_p( Circuit*, Juggler& );
    int get_dot_value( int, Juggler& );
    void add_choice_circuits( int*, Juggler& );
    void add_skilz_and_choice_circuits( int*, int, int*, Juggler&  );
    void add_juggler_to_circuit( Circuit&, Juggler& );
    void switch_worst_juggler_with_new_juggler( Circuit&, Juggler& );
    void replace_worst_juggler_if_new_juggler_is_better( Circuit&, Juggler& );
    void clear_junk( std::string& );
    void new_circuit( std::string&);
    void new_juggler( std::string&);
    int* get_skilz( std::istringstream& );
    int* get_choice_circuits( std::istringstream&  );
};


Dm::Dm( std::ifstream& file ) : NILJUGGLER() {
  int skilz[3] = {0,0,0};
  int circuit_choices[6] = { -1, -1, -1, -1, -1, -1 };
  add_skilz_and_choice_circuits( skilz, -1, circuit_choices, NILJUGGLER );
  std::string line;
  char letter;
  while (getline(file, line)) {
    letter = line[0];
    if (letter == 'C') {
      new_circuit( line );
    } else if ( letter == 'J') {
      cout << "yes we can" << endl;
      new_juggler( line );
    }
  }

  return;
};


void Dm::replace_worst_juggler_if_new_juggler_is_better( Circuit& circuit, Juggler& j ){
  if (circuit.jugglers[max_jugglers_per_circuit - 1].dot_value() > j.dot_value()) {
    cout << circuit.jugglers[max_jugglers_per_circuit - 1].number << " is better than " << j.number << endl;
    remove_current_top_choice( j );
    cout << j.choice << " choice" << endl;
  } else {
    cout << circuit.jugglers[max_jugglers_per_circuit -1].number << " is worse than " << j.number << endl;
    switch_worst_juggler_with_new_juggler( circuit, j );
  }
  return;
};

void Dm::switch_worst_juggler_with_new_juggler( Circuit& circuit, Juggler& j ){
  remove_current_top_choice( circuit.jugglers[max_jugglers_per_circuit - 1]);
  jugglers[jugglers.size() -1] = circuit.jugglers[max_jugglers_per_circuit - 1];
  circuit.jugglers[max_jugglers_per_circuit -1] = j;
  std::sort(std::begin(circuit.jugglers), std::end(circuit.jugglers), sort_by_dot_value);
  return;
};

void Dm::add_juggler_to_circuit( Circuit& circuit, Juggler& j ){
  if ( circuit.jugglers[max_jugglers_per_circuit - 1].number != -1 ){
    replace_worst_juggler_if_new_juggler_is_better( circuit, j );
    cout << j.choice << " in add juggler to circuit" << endl;

  } else {
    //it may call the destructor on the popped element
    circuit.jugglers[max_jugglers_per_circuit - 1]  = j;
    cout << "dat new juggla numba " << j.number << endl;
    jugglers.pop_back();
    cout << "dat new juggla numba " << j.number << endl;
    std::sort(begin(circuit.jugglers), end(circuit.jugglers), sort_by_dot_value);
    cout << "captain of the circuit " << circuit.jugglers[0].dot_value() << endl;
    cout << "--next up " << circuit.jugglers[1].dot_value() << endl;
    //KC says: We might have to do jugglers + max_jugglers_per_circuit there.
  }
  return;
};

void Dm::add_skilz_and_choice_circuits( int* s, int n, int* choice_circuits, Juggler& juggler ){
  juggler.number = n;
  juggler.add_skilz( s[0], s[1], s[2]);
  cout << *choice_circuits << " *choice circuits" << endl;
  add_choice_circuits( choice_circuits, juggler );
  int i = 0;
  while (i < circuit_count){
    juggler.choice_numbers[i] = choice_circuits[i];
    i++;
  }
  cout << "we made it!" << endl;
  return;
};

void Dm::add_choice_circuits( int* circuit_nums, Juggler& juggler ){
  int i = 0;
  while (i < choice_circuit_count) {
    int circuit_num = circuit_nums[i];
  cout << circuit_num << " circuit num, " << i << " this is i" << endl;
    int dot_value = get_dot_value( circuit_num, juggler );
    int* value;
    value = new int[2];
    value[0] = circuit_num;
    value[1] = dot_value;

    juggler.choice_circuits[i] = value;
    i++ ;
  }
  return;
};


int Dm::get_dot_value( int circuit_number, Juggler& juggler){
  int* circuit = circuits[ circuit_number ].all_skilz();
  int* skilz = juggler.all_skilz();
  //maybe make into global arrays
  int total = 0;
  int i = 0;
  while (i < 3) {
    total += ( circuit[i] * skilz[i] );
    i += 1;
  }
  return total;
};



void Dm::remove_current_top_choice( Juggler& juggler ){
  cout << juggler.choice << " remove current top ONE" << endl;
  juggler.choice++;
  cout << juggler.choice << " remove current top" << endl;
  if (juggler.choice == 6){
    add_circuit_dot_p( circuits, juggler );
  }
  return;
};

void Dm::add_circuit_dot_p( Circuit* circuits, Juggler& juggler ){
  int skipped = 0;
  int i = 0;
  while (i < circuit_count) {
    if (std::find(std::begin(juggler.choice_numbers), std::end(juggler.choice_numbers), i) != std::end(juggler.choice_numbers)){
      skipped++;
      continue;
    }
    int values[2] = { i, get_dot_value( i, juggler ) };
    juggler.other_choices[i - skipped] = values;
    i++;
  }

  std::sort(juggler.other_choices, juggler.other_choices + (circuit_count - choice_count), sort_other_choices);
  return;
};


void Dm::new_circuit( std::string& line ){
  clear_junk( line );
  std::istringstream iss(line);
  int circuit_num;
  // circuit_num = std::atoi(str.substr(1,str.size() - 1).c_str());
  iss >> circuit_num;
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
  int* c_circuits;
  c_circuits = new int[choice_count];
  int num;
  int i = 0;
  while (iss >> num ){
    c_circuits[i] = num;
    i++;
  }
  return c_circuits;
};

void Dm::clear_junk( std::string& line) {
  std::replace(line.begin(), line.end(), 'C', ' ');
  std::replace(line.begin(), line.end(), 'J', ' ');
  std::replace(line.begin(), line.end(), 'H', ' ');
  std::replace(line.begin(), line.end(), 'P', ' ');
  std::replace(line.begin(), line.end(), 'E', ' ');
  std::replace(line.begin(), line.end(), ',', ' ');
  std::replace(line.begin(), line.end(), ':', ' ');
  return;
}

void Dm::new_juggler( std::string& line ){
  clear_junk( line );
  std::istringstream iss(line);
  int juggler_num;
  // iss >> str;
  // iss >> str;
  // juggler_num = std::atoi(str.substr(1,str.size() - 1).c_str());
  iss >> juggler_num;
  int* skilz = get_skilz( iss );
  int* choice_circuits = get_choice_circuits( iss );
  cout << choice_circuits[0] << " choice circuits" << endl;
  Juggler juggler;
  juggler.number = juggler_num;

  add_skilz_and_choice_circuits(skilz, juggler_num, choice_circuits, juggler);
  cout << "we made it thrice!" << endl;
  jugglers.push_back(juggler);
};

int* Dm::get_skilz( std::istringstream& iss ){
  int* skilz;
  skilz = new int[3];
  int i = 0;
  while (i < 3 ) {
    iss >> skilz[i];
    i++;
  }
  return skilz;
};


class Course

{
public:
  Dm dm;
  Course( std::ifstream& file ) : dm( file ) {};
  void place_teams();
  std::vector<Juggler> jugglers();
  Circuit* circuits();
  void add_juggler_to_circuit( Juggler& );
  int get_j_value_of_1970();
};

void Course::place_teams(){
  while (jugglers().size() != 0){
    cout << "SIZE OF JUGGLERS!! " << jugglers().size() << endl;
    cout << "last juggler first choice " << jugglers()[jugglers().size() - 1].choice_circuits[0][0] << endl;
    // *juggler = jugglers()[jugglers().size() - 1];
    add_juggler_to_circuit( jugglers()[jugglers().size() - 1] );
    cout << "juggler choice ok:: " << jugglers()[jugglers().size() - 1].choice << endl;
    cout << "jugladdress ok:: " << &(jugglers()[jugglers().size() - 1]) << endl;
  }
  return;
};

std::vector<Juggler> Course::jugglers(){
  return dm.jugglers;
};

Circuit* Course::circuits(){
  return dm.circuits;
};

void Course::add_juggler_to_circuit( Juggler& juggler ){
  cout << "juggladress:: " << &juggler << endl;
  cout << "juggler choice " << juggler.choice << endl;
  int circuit_number = juggler.first_choice()[0];
  cout << "circuit number " << circuit_number << endl;
  // Circuit circuit = circuits()[circuit_number];
  dm.add_juggler_to_circuit( circuits()[circuit_number], juggler );
  cout << "juggladress again:: " << &juggler << endl;
  circuits()[circuit_number].show_jugglers_on_circuit();
  // cout << &circuit << " " << &(circuits()[circuit_number]) << endl;
  cout << juggler.choice << " choice of juggler" << endl;
  return;
};

int Course::get_j_value_of_1970(){
  Circuit circuit1970 = dm.circuits[ 1 ];
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
  c.dm.circuits[0].show_jugglers_on_circuit();
  return 0;
};
