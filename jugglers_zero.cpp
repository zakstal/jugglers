#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;
const int circuit_count = 2000;
const int max_jugglers_per_circuit = 6;
const int choice_count = 6;

class SkilzInterface{};

class JugglerInterface
{
  public:
    virtual void add_skilz_and_choice_circuits( int*, int, int* ) {};
    int number;
    int* choice_circuits[circuit_count];
    virtual void remove_current_top_choice() {};
    virtual int dot_value() {};
    virtual int* first_choice() {};
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

class Dm
{
  public:
    JugglerInterface NILJUGGLER;
    std::vector<JugglerInterface> jugglers;
    CircuitInterface circuits[circuit_count];
    Dm( std::ifstream& );
    void new_circuit( std::istringstream&);
    void new_juggler( std::istringstream&);
    int* get_skilz( std::istringstream& );
    int* get_choice_circuits( std::istringstream&  );
};

Dm::Dm( std::ifstream& file ) : NILJUGGLER() {
  int skilz[3] = {0,0,0};
  int circuit_choices[6] = { -1, -1, -1, -1, -1, -1 };
  NILJUGGLER.add_skilz_and_choice_circuits( skilz, -1, circuit_choices );
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
  circuits[circuit_num].number = circuit_num;
  circuits[circuit_num].add_skilz( skilz[0], skilz[1], skilz[2]);
  int i = 0;
  while (i < max_jugglers_per_circuit){
    circuits[circuit_num].jugglers[i] = NILJUGGLER;
    i++;
  }
  return;
};

void Dm::new_juggler( std::istringstream& iss ){
  std::string str;
  int juggler_num;
  iss >> str;
  juggler_num = std::atoi(str.substr(1,str.size() - 1).c_str());
  int* skilz = get_skilz( iss );
  int* choice_circuits = get_choice_circuits( iss );
  JugglerInterface juggler;
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




class Course

{
  public:
    Dm dm;
    Course( std::ifstream& file ) : dm( file ) {};
    void place_teams();
    std::vector<JugglerInterface> jugglers();
    CircuitInterface* circuits();
    void add_juggler_to_circuit( JugglerInterface );
};

//
//
// Course::Course( std::ifstream file ){
//
// };
//
void Course::place_teams(){
  JugglerInterface juggler;

  while (jugglers().size() != 0){
    juggler = jugglers()[jugglers().size() - 1];
    add_juggler_to_circuit( juggler );
  }
  return;
};

std::vector<JugglerInterface> Course::jugglers(){
  return dm.jugglers;
};

CircuitInterface* Course::circuits(){
  return dm.circuits;
};

void Course::add_juggler_to_circuit( JugglerInterface juggler ){
  int circuit_number = juggler.first_choice()[0];
  CircuitInterface circuit = circuits()[circuit_number];
  circuit.add_juggler_to_circuit( juggler );
  return;
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
// def get_j_value_of_1970
// self.circuits( 1970 ).jugglers.map( &:number ).inject( :+ )
// end
//









class Skilz: public SkilzInterface

{
  public:
      int hand_eye;
      int pizzazz;
      int endurance;
      Dm dm;
      Skilz( Dm md ) : dm( md ) {};
      void add_skilz( int, int, int);
      int* all_skilz();
};

// Skilz::Skilz( Dm md ) {
//   dm = md;
//   return;
// };

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
    void remove_current_top_choice();
    void add_choice_circuits( int* );
    void add_skilz_and_choice_circuits( int*, int, int* );
    void add_circuit_dot_p( CircuitInterface* );
    int* circuit_skilz( int );
    int get_dot_value( int );
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

void Juggler::add_circuit_dot_p( CircuitInterface* ){
  int skipped = 0;
  int i = 0;
  while (i < circuit_count) {
    if (std::find(std::begin(choice_numbers), std::end(choice_numbers), i) != std::end(choice_numbers)){
      skipped++;
      continue;
    }
    int values[2] = { i, get_dot_value( i ) };
    other_choices[i - skipped] = values;
    i++;
  }

  std::sort(std::begin(other_choices), std::end(other_choices), sort_other_choices);
  return;
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

void Juggler::add_skilz_and_choice_circuits( int* s, int n, int* cc ){
  number = n;
  add_skilz( s[0], s[1], s[2]);
  add_choice_circuits( cc );

  int i = 0;
  while (i < sizeof(choice_numbers)){
    choice_numbers[i] = cc[i];
    i++;
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
    Dm dm;
    Circuit( Dm md ) : Skilz( md ), dm( md ){};
    int max_jugglers();
    void add_juggler_to_circuit( JugglerInterface );
    bool not_nil_and_new_juggler_is_better( Juggler, int );
    void switch_worst_juggler_with_new_juggler(JugglerInterface);
    void replace_worst_juggler_if_new_juggler_is_better( JugglerInterface );
    void show_jugglers_on_circuit();

};

Circuit::Circuit( Dm md) {
  dm( md );
  int i = 0;
  while (i < max_jugglers()) {
    jugglers[i] = 0;
    i++;
  };
};

int Circuit::max_jugglers() {
  return max_jugglers_per_circuit;
};

bool sort_by_dot_value( JugglerInterface& j1, JugglerInterface& j2 ){
  return j1.dot_value() > j2.dot_value();
};

bool Circuit::not_nil_and_new_juggler_is_better( Juggler j, int i ){
  return (jugglers[i - 1].number != -1 ) && (j.dot_value() < jugglers[i - 1].dot_value());
};

void Circuit::switch_worst_juggler_with_new_juggler( JugglerInterface j ){
  jugglers[sizeof(jugglers) -1].remove_current_top_choice();
  dm.jugglers[sizeof(jugglers) -1] = jugglers[sizeof(jugglers) -1];
  jugglers[sizeof(jugglers) -1] = j;
  std::sort(std::begin(jugglers), std::end(jugglers), sort_by_dot_value);
  return;
};

void Circuit::replace_worst_juggler_if_new_juggler_is_better( JugglerInterface j ){
  if (jugglers[sizeof(jugglers) -1].dot_value() > j.dot_value()) {
    j.remove_current_top_choice();
  } else {
     switch_worst_juggler_with_new_juggler( j );
  }
  return;
};

void Circuit::add_juggler_to_circuit( JugglerInterface j ){
  if ( jugglers[sizeof(jugglers) -1].number != -1 ){
    replace_worst_juggler_if_new_juggler_is_better( j );
  } else {
    dm.jugglers.pop_back();
    //it may call the destructor function on the popped element
    jugglers[sizeof(jugglers) -1]  = j;
    sort(jugglers, jugglers + sizeof(jugglers), sort_by_dot_value);
  }
  return;
};

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
