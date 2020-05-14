#include <iostream>
#include <map>
#include <set>
#include "M_tools.hpp"

using namespace std;

void test()
{
    Pair<string, int> p("min shuai", 26);
    Pair<string, int> p1 = _make_pair("valerio", 25);
    cout << p._first << " " << p._second << endl;
    cout << p1._first << " " << p1._second << endl;
}

void test01()
{
    map<int,string> m{
        {2,"valerio"}
    };

    map<int,string>::iterator it = m.begin();
    cout << it->first << "-" << it->second << endl;
    m.insert({1,"minshuai"});
    m.insert({3,"minshuai"});

    it = m.begin();
    cout << it->first << "-" << it->second << endl;
    map<int,string>::iterator eit = --m.end();
    cout << eit->first << "-" << eit->second << endl;
}

void test02()
{
    RBTree<int> rb;
    cout << "OK" << endl;
    rb.insert(2);
    rb.insert(1);
    rb.insert(14);
    rb.insert(5);
    rb.insert(9);
    rb.insert(7);
    rb.insert(11);
    rb.insert(21);
    rb.insert(22);
    rb.insert(6);
    rb.insert(8);
    rb.insert(20);
    rb.insert(18);
    rb.insert(15);
    rb.insert(13);
    rb.insert(10);
    rb.insert(17);
    rb.insert(19);
    rb.insert(4);

    rb.print();
    cout << "*******************" << endl;
    rb.remove(9);
    rb.print();
}
int main()
{
    /* test(); */
    /* test01(); */
    /* test02(); */
    return 0;
}
