#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include "expression.h"

using namespace std;
// Deklaration av en erase-funktion
void erase(vector<Expression>&, int);

// Huvudprogrammet!
int main()
{
    cout << "Du har startat miniräknaren och kan nu beräkna!" << endl;
    vector<Expression> data{};
    string line{};
    Expression active{};
    while (true)
    {
	getline(cin, line);
	if(line == "")
	{
	    continue;
	}
	if(line.at(0) == ':')
	{

	    if(line == ":quit" || line == ":exit" || line == ":Quit" || line == ":Exit")
	    {
		return 0;
	    }

	    else if(line == ":prefix")
	    {
		cout << active.to_prefix() << endl;
	    }

	    else if(line == ":postfix")
	    {
		cout << active.to_postfix() << endl;
	    }

	    else if(line == ":infix")
	    {
		cout << active.to_infix() << endl;
	    }

	    else if(line == ":calc")
	    {
		try
		{
		    cout << active.evaluate() << endl;
		}
	        catch(logic_error const& e)
		{
		    cout << e.what() << endl;
		}
		
	    }
	    else if(line == ":save")
	    {    	
		data.push_back(active);
	    }
	    else if(line == ":list")
	    {
		unsigned long len{data.size()};
	        for( int counter{0} ; counter<int(len) ; counter++)
		{
		    cout << data.at(counter).to_infix() << endl;
		}
	    }
	    
	    else if(line.substr(0,9) == ":activate")
	    {
		int n{0};
		string str{};
		
		line.erase(0,9);
		stringstream ss1{line};
		stringstream ss2{line};
		ss1 >> str;
		if( isalpha(str.at(0)) )
		{
		    cout << "Incorrect index " + str + "." << endl;
		    continue;
		}      
		
		try
		{
		    ss2 >> n;
		    active = data.at(n);
		    
		}
		catch (const out_of_range& e)
		{
		    ss2.clear();
		    cout << "Index out of range, index " +
			to_string(n) + " is not saved." << endl;			
		}	       
	    }
	    
	    else if(line.substr(0,6) == ":erase")
	    {
		int n{0};
		string str{};
		
		line.erase(0,6);
		stringstream ss1{line};
		stringstream ss2{line};
		ss1 >> str;
		if( isalpha(str.at(0)) )
		{
		    cout << "Incorrect index " + str + "." << endl;
		    continue;
		}
		
		try
		{
		    ss2 >> n;
		    erase(data, n);
		}
        	catch (logic_error const& e)
		{
		    ss2.clear();     
		    cout << "Index out of range, index " +
			to_string(n) + " is not saved." << endl;		   		   
		}
	    }
	    
	    else if(line == ":delete")
	    {
		data.clear();
	    }
	    
	    continue;
	}
	try
	{
	    Expression e{line};
	    active = move(e);
	}
	catch(logic_error const& e)
	{
	    cout << e.what() << endl;
	}

    }
}

// Funktion för att ta bort element med index n
void erase(vector<Expression>& data, int n)
{
    vector<Expression> v{};
    int len{int(data.size())};
    if(n > len-1 || n < 0)
    {
	throw logic_error("Index error");
    }
    
    for(int i{0} ; i<len ; i++)
    {
	if(i != n)
	{
	    v.push_back(data.at(i));
	}
    }
    data = v;
}

