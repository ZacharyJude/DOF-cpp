#include <iostream>
#include "UnitTest.h"
#include "SeparateString.h"
using namespace std;
using namespace dof::lib::util;

void TestSeparateString() {
    SeparateString ss0("abc+1234+xxxx+yyyy", '+');
    DOF_UT_CHECK( '+' == ss0.GetSeparator() );
    DOF_UT_CHECK( "abc+1234+xxxx+yyyy" == ss0.GetString() );
    set<string> c0;
    DOF_UT_CHECK( true == ss0.ToCollection(c0) );
    DOF_UT_CHECK( c0.end() != c0.find("abc") );
    DOF_UT_CHECK( c0.end() != c0.find("1234") );
    DOF_UT_CHECK( c0.end() != c0.find("xxxx") );
    DOF_UT_CHECK( c0.end() != c0.find("yyyy") );

    SeparateString ss1("a+b+c");
    vector<string> v;
    ss1.ToCollection(v);

    DOF_UT_CHECK( 3 == v.size() );
    DOF_UT_CHECK( "a" == v[0] );
    DOF_UT_CHECK( "b" == v[1] );
    DOF_UT_CHECK( "c" == v[2] );

    SeparateString ss2("a+b+c", '|');
    ss2.ToCollection(v);

    DOF_UT_CHECK( 1 == v.size() );
    DOF_UT_CHECK( "a+b+c" == v[0] );

    SeparateString ss3;
    ss3.ToCollection(v);

    DOF_UT_CHECK( 0 == v.size() );
}

int main() {
    TestSeparateString();
    return 0;
}
