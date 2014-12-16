#include <iostream>
#include <fstream>
#include <sstream>

class Dm
{

};

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

class Juggler : public Skilz

{
public:
  int choice;
  int** other_choices;
  int** choice_circuit;
  int* first_choice();
  int dot_value();
  void add_choice_circuits( int** );
};


class Circuit : public Skilz

{
  public:
    Juggler jugglers[Dm::max_number_jugglers_per_circuit];
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
  return Dm::max_number_of_jugglers_per_circuit;
};

//constant in Dm for max jugglers

bool Circuit::not_nil_and_new_juggler_is_better( Juggler j, int i ){
  return (jugglers[i - 1] != 0) && (j.dot_value() < jugglers[i - 1].dot_value());
};

void Circuit::switch_worst_juggler_with_new_juggler( Juggler j ){
  jugglers[jugglers.size() -1].remove_current_top_choice();
  dm.jugglers[jugglers.size() -1] = jugglers[jugglers.size() -1];
  jugglers[jugglers.size() -1] = j;
  sort(jugglers, jugglers + jugglers.size(), sortfunction);
  //sort needs to have a nil juggler
  return;
};

void Circuit::replace_worst_juggler_if_new_juggler_is_better( Juggler j ){
  if (jugglers[jugglers.size() -1].dot_value() > j.dot_value()) {
    j.remove_current_top_choice();
  } else {
     switch_worst_juggler_with_new_juggler( j );
  }
  return;
};

void Circuit::add_juggler_to_circuit( Juggler j ){
  if ( jugglers[jugglers.size() -1] != 0 ){
    replace_worst_juggler_if_new_juggler_is_better( j );
  } else {
    dm.jugglers.pop();
    jugglers[jugglers.size() -1]  = j;
    sort(jugglers, jugglers + jugglers.size(), sortfunction);
    //sort needs to have a nil juggler
  }
  return;
};

//dm jugglers as stack
void Circuit::show_jugglers_on_circuit(){
  int i = 0;
  int j = 0;
  while (i < jugglers.size()) {
    std::cout << "j" << jugglers[i].number << " ";
    while (j < jugglers[i].choice_circuits.size()) {
      std::cout << "C" << jugglers[i].choice_circuits[j][0] << ":" << jugglers[i].choice_circuits[j][1] << " ";
      j++;
    };
    if (i < jugglers.size() - 1) {
      std::cout << " -- ";
    }
    i++;
    j = 0;
  }
  std::cout << std::endl;
};
