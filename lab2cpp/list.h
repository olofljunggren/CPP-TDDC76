#ifndef LIST_H
#define LIST_H
#include <string>


//VALGRIND-kontroll?


class List
{ 
public:

  List(); // KLAR
  ~List(); // KLAR
  List(const List &list); // KLAR
  void add_front(int); //KLAR
  void add_back(int); // KLAR
  void del_front(); // KLAR Ska vi throw error om tom?
  void del_back(); //KLAR Ska vi throw error om tom?
  void sort(); 
  bool is_empty() const; // KLAR
  std::string to_string() const; // KLAR
  int front() const; // KLAR
  int back() const; // KLAR
  int get(int index) const; // KLAR
  void del_all(); // KLAR
  int list_len() const;  // KLAR
  List& operator = (List const & rhs); // KLAR 

private:

  class Element
  {
  public:
    Element(int val, Element* ptr);
    //  public:
    int value{};
    Element* next{};
  };
  void under_sort(Element*& current, Element*& temp, Element*& previous, int streak);
  // Datamedlemmar
  Element* head_ptr{};
  Element* back_ptr{};
};


#endif
