#define CATCH_CONFIG_RUNNER
#include "list.h"
#include <string>
#include <stdexcept>
#include <sstream>
#include <iostream>

using namespace std;

List::List(): head_ptr{nullptr}, back_ptr{nullptr}
{
  
}

List::Element::Element(int val, Element* ptr): value{val}, next{ptr} 
{
}

List::~List()
{
  del_all();
}


List::List(const List &list2): head_ptr{nullptr}, back_ptr{nullptr}
{
  Element* temp{list2.head_ptr};
  int index{1};
  while(temp != nullptr)
    {
      add_back(list2.get(index));
      temp = temp->next;
      ++index;
    }
}


void List::add_front(int num)
{
  if(is_empty())
    {
      back_ptr = new Element{num, nullptr};
      head_ptr = back_ptr;
    }
  else
    {
      head_ptr = new Element{num, head_ptr};
    }
} 

void List::add_back(int num)
{
  Element* temp{back_ptr};
  if(is_empty())
    {
      head_ptr = new Element{num, nullptr};
      back_ptr = head_ptr;
    }
  else
    {
      back_ptr = new Element{num, nullptr};
      temp->next = back_ptr;
    }
}

void List::del_front()
{
   if(is_empty())
    {
      throw logic_error("Det finns inga element att ta bort!");
    }
  if(head_ptr == back_ptr)
    {
      delete head_ptr;
      head_ptr = nullptr;
      back_ptr = nullptr;
      return;
    }
  Element* temp{};
  temp = head_ptr;
  head_ptr = head_ptr->next;
  delete temp;
}

void List::del_back()
{
  if(is_empty())
    {
      throw logic_error("Det finns inga element att ta bort!");
    }
  
  if(head_ptr == back_ptr)
    {
      delete back_ptr;
      head_ptr = nullptr;
      back_ptr = nullptr;
      return;
    }
  
  Element* temp{};
  temp = head_ptr;
  
  while(temp->next != back_ptr)
    {
      temp = temp->next;
    }
 
  delete back_ptr;
  back_ptr = temp;
  back_ptr->next = nullptr;

}

bool List::is_empty() const
{
  return head_ptr == nullptr;
}

string List::to_string() const
{
  Element* temp{head_ptr};
  stringstream ss{};
  string text{};
  
  ss << "[";

  while(temp != nullptr)
    {
      ss << temp->value;
      if(temp->next != nullptr)
	{
	  ss << ", ";
	}
      temp = temp->next;
    }
  ss << "]";
  return ss.str();
}

int List::front() const
{
  if(is_empty())
    {
      throw logic_error("Värdet finns inte. Listan är tom.");
    }
  return head_ptr->value;
}

int List::back() const
{
  if(is_empty())
    {
      throw logic_error("Värdet finns inte. Listan är tom.");
    }
  return back_ptr->value; 
}

int List::get(int index) const
{
  if(index <= 0)
    {
      throw logic_error("Ange ett positivt heltal."); 
    }
  if(index > list_len())
    {
      throw logic_error("Indexet finns inte.");
    }
  Element* temp{};
  temp = head_ptr;
  for(int i{1} ; i<index ; i++)
    {      
      temp = temp->next;
    }
  return temp->value;
}

void List::del_all()
{
  while(head_ptr != nullptr)
    {
      del_front();
    }
}

int List::list_len() const
{
  Element* temp{head_ptr};
  int length{0};
    while(temp != nullptr)
    {
      temp = temp->next;
      ++length;
    }
    return length;
}


List& List::operator = (List const & rhs)
{
  List list3{rhs};
  Element* temp{};
  temp = head_ptr;
  head_ptr = list3.head_ptr;
  list3.head_ptr = temp;
  return *this;
}


void List::sort()
{
  int len{list_len()};
  if(len == 0 || len == 1)
    {
      return;
    }
  
  Element* current{head_ptr};
  Element* previous{};
  Element* temp{};     
  int streak{0};
  bool sorted{false};
  while(!sorted)
    {
      for(int i{0}; i<len; i++)
	{
	  //	  cout << "Lista: " << to_string() << endl;
      
	  if(current->next == nullptr)
	    {
	      temp->next = current;
	      back_ptr = current;
	    }
	  
	  if(streak == 0)
	    {
	      current = head_ptr;
	      previous = head_ptr->next;	 	  
	    }
	  temp = current->next;

	 
	  if(current->value <= temp->value)
	    {
	      streak++;
	      if(streak == len-1)
		{		  
		  sorted = true;
		  break;
		}
	      previous = current;
	      current = current->next;
	      continue;
	    }

	  if(streak == 0)
	    {
	      head_ptr = previous;
	    }
	  
	  under_sort(current, temp, previous, streak);
	  streak = 0;
	  
	}
    }
  
}

void List::under_sort(Element*& current, Element*& temp, Element*& previous, int streak)
{
  
  //Om streak=0: current är 1, temp och previous är 2
  //Om streak=n: current är n, temp är n+1 och previous är n-1
  
  int counter{0};
  if(streak > 0)
    {
      counter++;
    }
  while(current->value > temp->value)
    {
      if(temp->next == nullptr)
	{
	  temp->next = current;
	  current->next = nullptr;
	  back_ptr = current;
	  return;
	}

      if(counter == 1)
	{
	  previous->next = temp;
	}
      
      if(counter >= 1)
	{
	  previous = previous->next;
	}
      
      temp = temp->next;
      counter++;
      
    }
  previous->next = current;
  current->next = temp;
  return;
}


