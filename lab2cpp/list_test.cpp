#include "catch.hpp"
#include "list.h"
#include <iostream>

using namespace std;

TEST_CASE ("Lista")
{


  SECTION("Skapa och kolla första och sista")
    {
      List list1{};
      CHECK( list1.is_empty() );
     
      list1.add_front(120);
      CHECK( !(list1.is_empty()) );
      list1.add_front(50);
      CHECK( list1.front() == 50 );
      list1.add_front(60); 
      CHECK( list1.front() == 60 );
   
      list1.add_back(213);
      CHECK( list1.back() == 213 );
      list1.add_back(21);
      CHECK( list1.back() == 21 );

    }
  SECTION("Listlängd och kolla valfri position")
    {
      List list1{};
      list1.add_front(21);
      list1.add_front(213);
      list1.add_front(120);
      list1.add_front(50);
      list1.add_front(60);
      
      CHECK( list1.get(1) == 60 );
      CHECK( list1.get(2) == 50 );
      CHECK( list1.get(3) == 120 );
      CHECK( list1.get(4) == 213 );
      CHECK( list1.get(5) == 21 );
      
      CHECK( list1.list_len() == 5 );

      list1.add_front(5);
      CHECK( list1.get(1) == 5 );
      
    }

  SECTION("To string")
    {
      List list1{};
      list1.add_front(21);
      list1.add_front(213);
      list1.add_front(120);
      list1.add_front(50);
      list1.add_front(60);
      list1.add_front(5);
      List list2{};
      
      CHECK( list1.to_string()== "[5, 60, 50, 120, 213, 21]");
      list1.add_back(7);
      CHECK( list1.to_string()== "[5, 60, 50, 120, 213, 21, 7]");
      CHECK( list2.to_string() == "[]");
    }
   
  SECTION("Radera element")
    {
      List list2{};
      list2.add_front(7);
      list2.del_back();
      CHECK( list2.is_empty() );
      list2.add_front(7);
      list2.del_front();
      CHECK( list2.is_empty() );
      
      List list1{};
      list1.add_front(7);
      list1.add_front(21);
      list1.add_front(213);
      list1.add_front(120);
      list1.add_front(50);
      list1.add_front(60);
      list1.add_front(5);
      
      list1.del_back();
      CHECK( list1.back() == 21 );
      list1.del_back();
      CHECK( list1.front() == 5 );
      CHECK( !(list1.is_empty()) );
      CHECK( list1.back() == 213 );
      list1.del_front();
      CHECK( list1.front() == 60 );
      list1.del_front();
      CHECK( list1.front() == 50 );
      CHECK( !(list1.is_empty()) );
    }
 
  SECTION("Kopiering")
    {
      List list1{};
      list1.add_back(213);
      list1.add_front(120);
      list1.add_front(50);
      list1.add_front(45);
      
      List list2{list1};
      CHECK( list2.get(1) == 45 );
      CHECK( list2.get(2) == 50 );
      CHECK( list2.get(3) == 120 );
      CHECK( list2.get(4) == 213 );

      List list3{};
      List list4{list3};
      CHECK( list4.is_empty() );
      CHECK_THROWS( list4.get(1) == 45 );

      list3.add_front(20);
      list3.add_back(89);
      list3 = list2;

      CHECK( list3.get(1) == 45 );
      CHECK( list3.get(2) == 50 );
      CHECK( list3.get(3) == 120 );
      CHECK( list3.get(4) == 213 );
      CHECK( list3.to_string()== "[45, 50, 120, 213]");
    }
  SECTION("Radera allt + throws")
    {
      List list1{};
      list1.add_front(213);
      list1.add_front(120);
      list1.add_front(50);
      list1.add_front(45);
      
      list1.del_all();
      list1.del_all();
      CHECK( list1.is_empty() );
      CHECK( list1.list_len() == 0 );

      CHECK_THROWS( list1.front() );
      CHECK_THROWS( list1.back() );
      CHECK_THROWS( list1.del_front() );
      CHECK_THROWS( list1.del_back() );
      CHECK_THROWS( list1.get(1) );
      CHECK_THROWS( list1.get(-5) );
   }
}



TEST_CASE ("Sortera")
{


  SECTION("Minsta till största")
    {
      List list1{};
      list1.add_front(213);
      list1.add_front(120);
      list1.add_front(50);
      list1.add_front(45);
      list1.sort();
      CHECK( list1.to_string() == "[45, 50, 120, 213]" );


      List list2{};
      list2.add_front(1);
      list2.add_front(2);
      list2.add_front(3);
      list2.add_front(4);
      list2.sort();
      CHECK( list2.to_string() == "[1, 2, 3, 4]" );


      List list3{};
      list3.add_front(1);
      list3.sort();

      List list4{};
      list2.sort();

      List list5{};
      list5.add_front(6);
      list5.add_front(2);
      list5.add_front(1);
      list5.add_front(8);
      list5.add_front(3);
      list5.add_front(5);
      list5.add_front(2);
      list5.add_front(4);
      list5.sort();
      CHECK( list5.to_string() == "[1, 2, 2, 3, 4, 5, 6, 8]" );

      List list6{};
      list6.add_front(1);
      list6.add_front(2);
      list6.add_front(3);
      list6.add_front(4);
      list6.add_front(5);
      list6.add_front(6);
      list6.add_front(37);
      list6.add_front(8);
      list6.add_front(9);
      list6.add_front(10);
      list6.add_front(11);
      list6.add_front(12);
      list6.add_front(13);
      list6.add_front(14);
      list6.add_front(15);
      list6.add_front(16);
      list6.add_front(17);
      list6.add_front(18);
      list6.add_front(19);
      list6.add_front(2);
      list6.add_front(21);
      list6.add_front(22);
      list6.add_front(23);
      list6.add_front(2);
      list6.add_front(25);
      list6.add_front(26);
      list6.add_front(27);
      list6.add_front(28);
      list6.add_front(2);
      list6.add_front(30);
      list6.add_front(31);
      list6.add_front(32);
      list6.add_front(33);
      list6.add_front(34);
      list6.add_front(35);
      list6.add_front(1);
      list6.add_front(37);
      list6.add_front(38);
      list6.add_front(39);
      list6.add_front(49);
      list6.sort();
      CHECK( list6.to_string() == "[1, 1, 2, 2, 2, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 28, 30, 31, 32, 33, 34, 35, 37, 37, 38, 39, 49]" );
      cout << list6.to_string() << endl;
    }

}

