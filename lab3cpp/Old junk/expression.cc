#include <iostream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <stack>
#include <string>
#include "expression.h"
#include "calculator.h"
#include "uppgift12/postfix.h"
#include <limits>
#include <vector>

using namespace std;

Expression::Expression()
{
    expression = new Integer{0};
}


Expression::Expression(string& line) : expression{make_expression_tree(line)}
{
    
}

Expression::~Expression()
{
    if( expression != nullptr)
    {
	delete expression;
	expression = nullptr;
    }  
}

Expression::Expression(Expression && e2) : expression{e2.expression}
{
    e2.expression = nullptr;
}

Expression::Expression(Expression const& e2) : expression{e2.expression->copy()}
{

}

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


double Expression::evaluate() const
{ 
    return expression->evaluate();
}

string Expression::to_infix() const
{
    return expression->infix();
}

string Expression::to_postfix() const
{
    return expression->postfix();
}

string Expression::to_prefix() const
{
    return expression->prefix();
}


Node* Expression::make_expression_tree(string & line)
{
    int counter{0};
    string word{};
    std::stack<Node*> stack{};
    Postfix postfix{line};
    string postfix_line = postfix.to_string();
    istringstream ss{postfix_line};
    Node* operand1{nullptr};
    Node* operand2{nullptr};
    Node* ptr{nullptr};
    while(ss >> word)
    {
	if(word == "inf")
	{
	    ptr = new Integer{numeric_limits<int>::max()};
	    stack.push(ptr);
	}
	else if ( std::all_of( begin(word), end(word), ::isdigit ) )
	{	  
	    // Vi har hittat ett heltal
	    ptr = new Integer{stoi(word)};
	    stack.push(ptr);
	}
	else if ( isdigit(word.at(0)) )
	{
	    // Vi räknar ordet som flyttal
	    ptr = new Real{stod(word)};
	    stack.push(ptr);
	}
	else if ( isalpha(word.at(0)) )
	{
	    // Vi räknar ordet som variabelnamn
	    double variable{0};
	    cout << "Skriv in värdet för " << word << ": ";
	    if(cin >> variable)
	    {
		ptr = new Variable{variable};
		stack.push(ptr);
	    }
	    else
	    {
		throw logic_error("Du matade inte in ett giltigt värde.");
	    }
	}
	else
	{
	    // Vi räknar ordet som en operator
	    
	    if(stack.empty())
	    {
		throw logic_error("Hittar inte tillräckligt med operander.");
	    }
	    operand2 = stack.top();	    
	    stack.pop();
	    
	    if(stack.empty())
	    {
		throw logic_error("Finns inte tillräckligt med operander.");
	    }
	    operand1 = stack.top(); 
	    stack.pop();
	    
	    if(word.length() == 1)
	    {
		switch(word.at(0))
		{
		case '+':
		    ptr = new Addition{operand1, operand2};
		    break;

		case '-':
		    ptr = new Subtraction{operand1, operand2};
		    break;

		case '*':
		    ptr = new Multiplication{operand1, operand2};
		    break;

		case '/':
		    ptr = new Division{operand1, operand2};
		    break;

		case '^':
		    ptr = new Exponent{operand1, operand2};
		    break;
		
		default:
		    throw logic_error("Felaktig operator.");   		
		}
	    }
	    else
	    {
		throw logic_error("Felaktig operator.");
	    }
	    
	    stack.push(ptr);
	}
	counter++;
    }
    operand1 = stack.top();
    stack.pop();
    
    if( !stack.empty() )
    {
	throw logic_error("Saknas operatorer.");
    }

    return operand1;
    
}
