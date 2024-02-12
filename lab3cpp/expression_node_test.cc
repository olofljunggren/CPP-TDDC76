#include <limits>
#include <cmath> // abs
#include <memory>
#include "node.h"
#include "catch.hpp"
#include "expression.h"
#include <string>

using namespace std;

// If catch report 0.42 != 0.42 you are likely to have a small
// rounding error in some invisible decimal place. In such case you
// can compare if the difference is small enough to consider two
// doubles equal anyway.
bool compare_equal(double a, double b)
{
  return std::abs(a - b) <= 0.001; // epsilon fungerade ej... :(
}

TEST_CASE("operand real")
{
  Number* pi{ new Real{3.14} };

  CHECK( compare_equal( pi->evaluate(), 3.14 ) );
  CHECK( pi->postfix() == "3.140" );
}


TEST_CASE("addition")
{
  Node* pi = new Real{3.14};
  Node* e = new Real{2.72};
  Node* pluslr = new Addition{pi, e};
  Node* plusrl = new Addition{e, pi};

  CHECK(  compare_equal(pluslr->evaluate(), 5.86 ));
  CHECK(  compare_equal(plusrl->evaluate(), 5.86 ));
  CHECK(pluslr->postfix() == "3.140 2.720 +" );
  CHECK(plusrl->postfix() == "2.720 3.140 +" );
}


TEST_CASE("test case ignoring rounding errors")
{
  Node* a = new Real{0.01};
  Node* b = new Real{0.09};
  Node* plus = new Addition{a, b};

  // CHECK(compare_equal(abs(-0.100),  0.1) ); // make sure we use correct "abs"
  //CHECK( plus->evaluate() == 0.1 ); // Rounding error!!
  CHECK( compare_equal(plus->evaluate(), 0.1) );
}


TEST_CASE("operand integer")
{
  Number* i = new Integer{7};
  
  CHECK( i->evaluate() == 7 );
  CHECK( i->postfix() == "7" );

}

TEST_CASE("Subtraction")
{
    Number* i = new Integer{7};
    Number* j = new Real{3.1415};
    Node* iminusj = new Subtraction{i, j};
    Node* jminusi = new Subtraction{j, i};
    CHECK( compare_equal(iminusj->evaluate(), 3.8585) );
    CHECK( compare_equal(jminusi->evaluate(), -3.8585) );
    CHECK(iminusj->postfix() == "7 3.142 -" );
}

TEST_CASE("Multiplication")
{
    Number* i = new Integer{7};
    Number* j = new Real{3.1415};
    Number* zero = new Real{0};
    
    Node* first = new Multiplication{i, j};
    Node* second = new Multiplication{zero, i};

    CHECK( compare_equal(first->evaluate(), 3.1415*7) );
    CHECK( compare_equal(second->evaluate(), 0) );
    CHECK(first->postfix() == "7 3.142 *" );
}

TEST_CASE("Division")
{
    Number* i = new Integer{7};
    Number* j = new Real{3.1415};
    Number* zero = new Integer{0};
    
    Node* first = new Division{i, j};
    Node* second = new Division{i, zero};
    Node* third = new Division{j, i};

    CHECK( compare_equal(first->evaluate(), 7/3.1415) );
    CHECK_THROWS( second->evaluate() );
    CHECK( compare_equal(third->evaluate(), 3.1415/7) );
    CHECK(third->postfix() == "3.142 7 /" );
}

TEST_CASE("Exponent")
{
    Number* i = new Integer{7};
    Number* j = new Real{3.1415};
    Number* zero = new Real{0};
    Number* neg = new Integer{-2};
    Number* neg2 = new Real{-3.2};
    
    Node* first = new Exponent{i, j};
    Node* second = new Exponent{zero, j};
    Node* third = new Exponent{zero, zero};
    Node* fourth = new Exponent{neg2, i};
    Node* fifth = new Exponent{neg, j};
    Node* sixth = new Exponent{zero, neg};

    CHECK( compare_equal(first->evaluate(), pow(7,3.1415)) );
    CHECK( compare_equal(second->evaluate(), 0 ));
    CHECK( compare_equal( fourth->evaluate(), pow(-3.2,7)) );
    CHECK_THROWS( third->evaluate() );
    CHECK_THROWS( fifth->evaluate() );
    CHECK_THROWS( sixth->evaluate() );
    CHECK(first->postfix() == "7 3.142 ^" );
}

TEST_CASE("Infix and prefix")
{
    Number* i = new Integer{7};
    Number* j = new Real{3.1415};
    Node* first = new Exponent{i, j};
    CHECK(first->postfix() == "7 3.142 ^" );
    CHECK(first->prefix() == "^ 7 3.142" );
    CHECK(first->infix() == "( 7 ^ 3.142 )" );
}

  

TEST_CASE("conversion to string")
{
  Node* a = new Addition{ new Subtraction{ new Real{1.0},
					   new Integer{1} },
			  new Multiplication{ new Integer{2},
					      new Integer{5} }
  };
  SECTION("prefix")
    {
      CHECK( a->prefix() == "+ - 1.000 1 * 2 5" );
    }
  SECTION("infix")
    {
      CHECK( a->infix() == "( ( 1.000 - 1 ) + ( 2 * 5 ) )" );
    }
  SECTION("postfix")
    {
      CHECK( a->postfix() == "1.000 1 - 2 5 * +" );
    }
}



TEST_CASE("Flytt Expression")
{
    string str1{"( 1 + 3 ) * ( 3 - 4 ) "};
    string str2{"( 1 + 3 ^ 3 - 4 ) "};
    
    Expression* e = new Expression{str1};
    CHECK(e->to_postfix() == "1 3 + 3 4 - *" );
    CHECK(e->to_prefix() == "* + 1 3 - 3 4" );
    CHECK(e->to_infix() == "( ( 1 + 3 ) * ( 3 - 4 ) )" );
    CHECK(e->evaluate() == -4 );

    Expression e1{str1};
    Expression e2{str2};
    CHECK(e1.to_postfix() == "1 3 + 3 4 - *" );
    CHECK(e2.to_postfix() == "1 3 3 ^ + 4 -" );

    e1 = move(e2);
    CHECK(e2.to_postfix() == "1 3 + 3 4 - *" );
    CHECK(e1.to_postfix() == "1 3 3 ^ + 4 -" );
    Expression e3{move(e1)};
    CHECK(e3.to_postfix() == "1 3 3 ^ + 4 -" );
}

TEST_CASE("Kopiering Expression")
{
    string str1{"( 1 + 3 ) * ( 3 - 4 ) "};
    string str2{"( 1 + 3 ^ 3 - 4 ) "};

    Expression e1{str1};
    Expression e2{e1};
    CHECK(e1.to_postfix() == "1 3 + 3 4 - *" );
    CHECK(e2.to_postfix() == "1 3 + 3 4 - *" );
    Expression e3{};
    e2 = e3;
    CHECK(e2.to_postfix() == "0" );
    e3 = e1;
    CHECK(e3.to_postfix() == "1 3 + 3 4 - *" );

}

