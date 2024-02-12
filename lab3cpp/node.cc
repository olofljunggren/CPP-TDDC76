#include "node.h"
#include <string>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <cmath>

using namespace std;

// Konstruktorer
Operation::~Operation()
{
    delete operand1;
    delete operand2;
}


Operation::Operation(Node* num, Node* num2) : operand1{num}, operand2{num2}
{
}

Integer::Integer(int num) : value{num}
{
}

Real::Real(double num) : value{num}
{
}

Addition::Addition(Node* num, Node* num2)
  : Operation{num, num2}
{
}

Subtraction::Subtraction(Node* num, Node* num2)
  : Operation{num, num2}
{ 
}

Multiplication::Multiplication(Node* num, Node* num2)
  : Operation{num, num2}
{
}

Division::Division(Node* num, Node* num2)
  : Operation{num, num2}
{ 
}

Exponent::Exponent(Node* num, Node* num2)
  : Operation{num, num2}
{ 
}

// Evaluerar värdet av en nod
double Integer::evaluate() const
{
  return value;
}

double Real::evaluate() const
{
  return value;
}


double Addition::evaluate() const
{
  return operand1->evaluate() + operand2->evaluate();
}

double Subtraction::evaluate() const
{
  return operand1->evaluate() - operand2->evaluate();
}

double Multiplication::evaluate() const
{
  return operand1->evaluate() * operand2->evaluate();
}

double Division::evaluate() const
{
    if(operand2->evaluate() == 0)
    {
	throw logic_error("Division is not defined!");
    }
    return operand1->evaluate() / operand2->evaluate();
}

double Exponent::evaluate() const
{
    if(operand1->evaluate() == 0 && operand2->evaluate() <= 0)
    {
	throw logic_error("Exponentiation is not defined!");
    }
    
    if(operand1->evaluate() < 0 && operand2->evaluate()-floor(operand2->evaluate()) != 0)
    {
	throw logic_error("Exponentiation is not defined!");
    }

    return pow(operand1->evaluate(), operand2->evaluate());
}




// Postfix
string Operation::postfix() const
{
    return operand1->postfix() + " " + operand2->postfix() + " " + get_symbol();
}

string Real::postfix() const
{
  stringstream ss{};
  ss << setprecision(3) << fixed << value;
  return ss.str();
}

string Integer::postfix() const
{
  return to_string(value);
}


// Prefix
string Operation::prefix() const
{
    return  get_symbol() + " " + operand1->prefix() + " " + operand2->prefix();  
}

string Number::prefix() const
{
    return postfix();
}


// Infix
string Operation::infix() const
{
    return "( " + operand1->infix() + " " + get_symbol() + " " + operand2->infix() + " )";
}

string Number::infix() const
{
    return postfix();
}


// Get_symbol för operatorerna
string Subtraction::get_symbol() const
{
    return "-";
}
string Addition::get_symbol() const
{
  return "+";
}
string Multiplication::get_symbol() const
{
    return "*";
}
string Division::get_symbol() const
{
  return "/";
}
string Exponent::get_symbol() const
{
    return "^";
}


// Skapar djup kopia av ett Number
Node* Integer::copy()
{
    Node* new_copy = new Integer(value);
    return new_copy;
}

Node* Real::copy()
{
    Node* new_copy = new Real(value);
    return new_copy;
}

Node* Operation::copy()
{
    return this->create_new(operand1->copy(), operand2->copy());
}


// Skapar djup kopia av en Operation
Node* Addition::create_new(Node* lhs, Node* rhs)
{
    Node* new_op = new Addition(lhs,rhs);
    return new_op;
}
Node* Subtraction::create_new(Node* lhs, Node* rhs)
{
    Node* new_op = new Subtraction(lhs,rhs);
    return new_op;
}
Node* Multiplication::create_new(Node* lhs, Node* rhs)
{
    Node* new_op = new Multiplication(lhs,rhs);
    return new_op;
}
Node* Division::create_new(Node* lhs, Node* rhs)
{
    Node* new_op = new Division(lhs,rhs);
    return new_op;
}
Node* Exponent::create_new(Node* lhs, Node* rhs)
{
    Node* new_op = new Exponent(lhs,rhs);
    return new_op;
}


