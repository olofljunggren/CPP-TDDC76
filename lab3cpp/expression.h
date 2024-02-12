#ifndef EXPRESSION_H
#define EXPRESSION_H
#include <string>
#include "node.h"

// Klass med ett matematiskt uttryck i ett träd.
// Datamedlemmen expression pekar på basnoden.
class Expression
{ 
public:
    Expression();
    Expression(std::string&);
    ~Expression();
    Expression(Expression const&);
    Expression& operator = (Expression const&);
    Expression(Expression && list);
    Expression& operator = (Expression && rhs);
    
    double evaluate() const;
    std::string to_infix() const;
    std::string to_postfix() const;
    std::string to_prefix() const;

private:
    Node* expression{};
    Node* make_expression_tree(std::string &);

};

#endif
