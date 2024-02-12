#ifndef NODE_H
#define NODE_H
#include <string>

class Node
{ 
public:
    Node() = default;
    virtual ~Node() = default;
    virtual Node* copy() = 0;
    virtual double evaluate() const = 0;
    virtual std::string postfix() const = 0;
    virtual std::string infix() const = 0;
    virtual std::string prefix() const = 0; 

};

class Number : public Node
{
public:
    Number() = default;
    virtual ~Number() = default;
    std::string infix() const override;
    std::string prefix() const override; 
};

class Operation : public Node
{
public:
    Operation(Operation const&) = delete;
    Operation& operator=(Operation const&) = delete;
    Operation(Node*, Node*);
    virtual ~Operation();
    std::string postfix() const override;
    std::string infix() const override;
    std::string prefix() const override; 
    virtual std::string get_symbol() const = 0;
    Node* copy() override;
    virtual Node* create_new(Node* lhs, Node* rhs) = 0;
    
protected:
    Node* operand1{};
    Node* operand2{}; 
};

class Integer : public Number
{
public:
    Integer(int);
    ~Integer() = default;
    Node* copy() override;
private:
    std::string postfix() const override;
    int value{0};
    double evaluate() const override;
};

class Real : public Number
{
public:
    Real(double);
    ~Real() = default;
    Node* copy() override;
private:
    std::string postfix() const override;
    double value{0};
    double evaluate() const override;
};


// Klass som hanterar addition
class Addition : public Operation
{
public:
    Addition(Node*, Node*);
    ~Addition() = default;
    Node* create_new(Node* lhs, Node* rhs);
private:
    std::string get_symbol() const override;
    double evaluate() const override;
};


// Klass som hanterar subtraktion
class Subtraction : public Operation
{
public:
    Subtraction(Node*, Node*);
    ~Subtraction() = default;
    Node* create_new(Node* lhs, Node* rhs);
private:
    std::string get_symbol() const override;
    double evaluate() const override;
};


// Klass som hanterar multiplikation
class Multiplication : public Operation
{
public:
    Multiplication(Node*, Node*);
    ~Multiplication() = default;
    Node* create_new(Node* lhs, Node* rhs);
private:
    std::string get_symbol() const override;
    double evaluate() const override;
};


// Klass som hanterar Division
class Division : public Operation
{
public:
    Division(Node*, Node*);
    ~Division() = default;
    Node* create_new(Node* lhs, Node* rhs);
private:
    std::string get_symbol() const override;
    double evaluate() const override;
};


// Klass som hanterar exponentiering
class Exponent : public Operation
{
public:
    Exponent(Node*, Node*);
     ~Exponent() = default;
    Node* create_new(Node* lhs, Node* rhs);
private:
    std::string get_symbol() const override;
    double evaluate() const override;
};


#endif
