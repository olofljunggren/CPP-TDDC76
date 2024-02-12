#include <iostream>
#include <sstream>
#include <cctype>
#include <stack>
#include <string>
#include "expression.h"
#include "node.h"
#include "postfix.h"
#include <limits>
#include <memory>
#include <algorithm>


using namespace std;

// Konstruktorer
Expression::Expression()
{
    expression = new Integer{0};
}

Expression::Expression(string& line) : expression{make_expression_tree(line)}
{
    
}


// Destruktor
Expression::~Expression()
{
    delete expression;
    expression = nullptr; 
}


// Flyttkonstruktor
Expression::Expression(Expression && e2) : expression{e2.expression}
{
    e2.expression = nullptr;
}


// Kopieringskonstruktor
Expression::Expression(Expression const& e2) : expression{e2.expression->copy()}
{
}


// Operatoröverlagringar
Expression& Expression::operator = (Expression const& e2)
{
    delete expression;
    expression = e2.expression->copy();
    return *this;
}

Expression& Expression::operator = (Expression && rhs)
{
    Node* temp{expression};
    expression = rhs.expression;
    rhs.expression = temp;
    return *this;
}


// Returnerar svaret på en beräkning
double Expression::evaluate() const
{ 
    return expression->evaluate();
}

// Returnerar det inskrivna som infix
string Expression::to_infix() const
{
    return expression->infix();
}

// Returnerar det inskrivna som postfix
string Expression::to_postfix() const
{
    return expression->postfix();
}

// Returnerar det inskrivna som prefix
string Expression::to_prefix() const
{
    return expression->prefix();
}

// Returnerar basen av en trädstruktur med noder.
Node* Expression::make_expression_tree(string & line)
{
    string word{};
    stack<unique_ptr<Node>> node_stack{};
    Postfix postfix{line};
    string postfix_line = postfix.to_string();
    istringstream ss{postfix_line};
    Node* operand1{nullptr};
    Node* operand2{nullptr};

    while(ss >> word)
    {
	if(word == "inf")
	{
	    // Vi tolkar talet som mycket stort..
	    node_stack.push(move( make_unique<Integer>(numeric_limits<int>::max()) ));
	}
	else if ( all_of( begin(word), end(word), ::isdigit ) )
	{	  
	    // Vi har hittat ett heltal
	    node_stack.push(move( make_unique<Integer>(stoi(word)) ));
	}
	else if ( isdigit(word.at(0)) )
	{
	    // Vi räknar ordet som flyttal
	    node_stack.push(move( make_unique<Real>(stod(word)) ));
	}
	else if ( isalpha(word.at(0)) )
	{
	    // Vi räknar ordet som variabelnamn
	    double variable{0};
	    cout << "Submit the value of the variable " << word << ": ";
	    if(cin >> variable)
	    {	
		node_stack.push(move( make_unique<Real>(variable) ));		
	    }
	    else
	    {
		cin.clear();
		throw logic_error("Input is not a number.");
	    }
	    cin.ignore(1000,'\n');
			
	}
	else
	{
	    // Vi räknar ordet som en operator
	    operand2 = node_stack.top().get()->copy();
	    node_stack.pop();

	    operand1 = node_stack.top().get()->copy();
	    node_stack.pop();
	    
	    if(word.length() == 1)
	    {
		switch(word.at(0))
		{
		case '+':
		    node_stack.push(move( make_unique<Addition>(operand1,operand2) ));
		    break;

		case '-':
		    node_stack.push(move( make_unique<Subtraction>(operand1,operand2) ));
		    break;

		case '*':
		    node_stack.push(move( make_unique<Multiplication>(operand1,operand2) ));
		    break; 

		case '/':
		    node_stack.push(move( make_unique<Division>(operand1,operand2) ));
		    break;

		case '^':
		    node_stack.push(move( make_unique<Exponent>(operand1,operand2) ));
		    break;
		
		default:
		    throw logic_error("Unimplemented operator.");   		
		}
	    }
	}
    }
    operand1 = node_stack.top().get()->copy();
    node_stack.pop();
 
    return operand1;
    
}
