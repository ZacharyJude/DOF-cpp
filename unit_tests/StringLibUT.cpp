#include <iostream>
#include <algorithm>
#include "UnitTest.h"
#include "StringLib.h"
using namespace std;
using namespace dof::lib::str;

void TestStrip() {
    string s;

    s = " \t\t  \t  123abc\t\t  \t";
    DOF_UT_CHECK( "123abc\t\t  \t" == Lstrip(s) );
    DOF_UT_CHECK( " \t\t  \t  123abc" == Rstrip(s) );
    DOF_UT_CHECK( "123abc" == Strip(s) );

    set<char> chars1;
    chars1.insert('x');
    chars1.insert('1');
    s = "x1x1xxxx111xx1zacharyxx12";
    DOF_UT_CHECK( "zacharyxx12" == Lstrip(s, chars1) );
    DOF_UT_CHECK( "x1x1xxxx111xx1zacharyxx12" == Rstrip(s, chars1) );
    DOF_UT_CHECK( "zacharyxx12" == Strip(s, chars1) );
    
    DOF_UT_CHECK( "" == Lstrip("") );
    DOF_UT_CHECK( "" == Rstrip("") );
    DOF_UT_CHECK( "" == Strip("") );
}

void TestToLowerAndToUpper() {
    DOF_UT_CHECK( 'x' == ToLower('x') );
    DOF_UT_CHECK( 'x' == ToLower('X') );
    DOF_UT_CHECK( 'Y' == ToUpper('Y') );
    DOF_UT_CHECK( 'Y' == ToUpper('y') );
    DOF_UT_CHECK( '1' == ToLower('1') );
    DOF_UT_CHECK( '1' == ToUpper('1') );

    DOF_UT_CHECK( "THE DOG AGE IS 6 NOW!!" == ToUpperString("the Dog age Is 6 nOw!!") );
    DOF_UT_CHECK( "the dog age is 6 now!!" == ToLowerString("THE Dog age Is 6 nOw!!") );
}

void TestHex2Dec() {
    DOF_UT_CHECK( 238ull == HexDec("ee") );
    DOF_UT_CHECK( 238ull == HexDec("See") );
    DOF_UT_CHECK( 0ull == HexDec("") );
    DOF_UT_CHECK( 10ull == HexDec("That") );
    DOF_UT_CHECK( 160ull == HexDec("a0") );
    DOF_UT_CHECK( 0xffffull == HexDec("FFFF") );
    DOF_UT_CHECK( 0x7733ffffull == HexDec("7733FFFF") );
}

void TestStringToCollectionEscape() {
    string s;
    set<string> setOutput;
    vector<string> vecOutput;
    bool isSuccess;

    s = "+a+abc+92";
    isSuccess = StringToCollectionEscape(s, setOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 3 == setOutput.size() );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("a") );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("abc") );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("92") );

    isSuccess = StringToCollectionEscape(s, vecOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 3 == vecOutput.size() );
    DOF_UT_CHECK( vecOutput.end() != find(vecOutput.begin(), vecOutput.end(), "a") );
    DOF_UT_CHECK( vecOutput.end() != find(vecOutput.begin(), vecOutput.end(), "abc") );
    DOF_UT_CHECK( vecOutput.end() != find(vecOutput.begin(), vecOutput.end(), "92") );

    s = "a+abc+92";
    isSuccess = StringToCollectionEscape(s, setOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 3 == setOutput.size() );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("a") );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("abc") );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("92") );

    isSuccess = StringToCollectionEscape(s, vecOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 3 == vecOutput.size() );
    DOF_UT_CHECK( vecOutput.end() != find(vecOutput.begin(), vecOutput.end(), "a") );
    DOF_UT_CHECK( vecOutput.end() != find(vecOutput.begin(), vecOutput.end(), "abc") );
    DOF_UT_CHECK( vecOutput.end() != find(vecOutput.begin(), vecOutput.end(), "92") );

    s = "aaaaaaa";
    isSuccess = StringToCollectionEscape(s, setOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 1 == setOutput.size() );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("aaaaaaa") );

    isSuccess = StringToCollectionEscape(s, vecOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 1 == vecOutput.size() );
    DOF_UT_CHECK( vecOutput.end() != find(vecOutput.begin(), vecOutput.end(), "aaaaaaa") );
    
    s = "";
    isSuccess = StringToCollectionEscape(s, setOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 0 == setOutput.size() );
    DOF_UT_CHECK( setOutput.end() == setOutput.find("") );

    isSuccess = StringToCollectionEscape(s, vecOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 0 == vecOutput.size() );
    DOF_UT_CHECK( vecOutput.end() == find(vecOutput.begin(), vecOutput.end(), "") );
    
    s = "+";
    isSuccess = StringToCollectionEscape(s, setOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 1 == setOutput.size() );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("") );

    isSuccess = StringToCollectionEscape(s, vecOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 1 == vecOutput.size() );
    DOF_UT_CHECK( vecOutput.end() != find(vecOutput.begin(), vecOutput.end(), "") );

    s = "++";
    isSuccess = StringToCollectionEscape(s, setOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 1 == setOutput.size() );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("") );

    isSuccess = StringToCollectionEscape(s, vecOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 2 == vecOutput.size() );
    DOF_UT_CHECK( vecOutput[0] == "" && vecOutput[1] == "" );

    s = "+++34343";
    isSuccess = StringToCollectionEscape(s, setOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 2 == setOutput.size() );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("") );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("34343") );

    isSuccess = StringToCollectionEscape(s, vecOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 3 == vecOutput.size() );
    DOF_UT_CHECK( vecOutput[0] == "" && vecOutput[1] == "" && vecOutput[2] == "34343" );

    s = "++++++";
    isSuccess = StringToCollectionEscape(s, setOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 1 == setOutput.size() );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("") );

    isSuccess = StringToCollectionEscape(s, vecOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 6 == vecOutput.size() );

    s = "\\+123+456+78\\+9+\\+";
    isSuccess = StringToCollectionEscape(s, setOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 4 == setOutput.size() );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("+123") );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("456") );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("78+9") );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("+") );

    isSuccess = StringToCollectionEscape(s, vecOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 4 == vecOutput.size() );
    DOF_UT_CHECK( vecOutput[0] == "+123" );
    DOF_UT_CHECK( vecOutput[1] == "456" );
    DOF_UT_CHECK( vecOutput[2] == "78+9" );
    DOF_UT_CHECK( vecOutput[3] == "+" );

    s = "\\+1\\23+456+78\\+9+\\+10\\\\+12+\\\\abc";
    isSuccess = StringToCollectionEscape(s, setOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 6 == setOutput.size() );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("+1\\23") );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("456") );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("78+9") );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("+10\\") );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("12") );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("\\\\abc") );

    isSuccess = StringToCollectionEscape(s, vecOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 6 == vecOutput.size() );
    DOF_UT_CHECK( vecOutput[0] == "+1\\23" );
    DOF_UT_CHECK( vecOutput[1] == "456" );
    DOF_UT_CHECK( vecOutput[2] == "78+9" );
    DOF_UT_CHECK( vecOutput[3] == "+10\\" );
    DOF_UT_CHECK( vecOutput[4] == "12" );
    DOF_UT_CHECK( vecOutput[5] == "\\\\abc" );
}

void TestAppendSuffix() {
    DOF_UT_CHECK( "abc" == AppendSuffix<4096>("ab", "c") );
    DOF_UT_CHECK( "xxxxxxxdddddd" == AppendSuffix<14>("xxxxxxx", "dddddd") );
    DOF_UT_CHECK( "" == AppendSuffix<13>("xxxxxxx", "dddddd") );
}

void TestStringToCollection() {
    string s;
    set<string> setOutput;
    vector<string> vecOutput;
    bool isSuccess;

    s = "+a+abc+92";
    isSuccess = StringToCollection(s, setOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 3 == setOutput.size() );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("a") );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("abc") );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("92") );

    isSuccess = StringToCollection(s, vecOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 3 == vecOutput.size() );
    DOF_UT_CHECK( vecOutput.end() != find(vecOutput.begin(), vecOutput.end(), "a") );
    DOF_UT_CHECK( vecOutput.end() != find(vecOutput.begin(), vecOutput.end(), "abc") );
    DOF_UT_CHECK( vecOutput.end() != find(vecOutput.begin(), vecOutput.end(), "92") );

    s = "a+abc+92";
    isSuccess = StringToCollection(s, setOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 3 == setOutput.size() );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("a") );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("abc") );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("92") );

    isSuccess = StringToCollection(s, vecOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 3 == vecOutput.size() );
    DOF_UT_CHECK( vecOutput.end() != find(vecOutput.begin(), vecOutput.end(), "a") );
    DOF_UT_CHECK( vecOutput.end() != find(vecOutput.begin(), vecOutput.end(), "abc") );
    DOF_UT_CHECK( vecOutput.end() != find(vecOutput.begin(), vecOutput.end(), "92") );

    s = "aaaaaaa";
    isSuccess = StringToCollection(s, setOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 1 == setOutput.size() );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("aaaaaaa") );

    isSuccess = StringToCollection(s, vecOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 1 == vecOutput.size() );
    DOF_UT_CHECK( vecOutput.end() != find(vecOutput.begin(), vecOutput.end(), "aaaaaaa") );
    
    s = "";
    isSuccess = StringToCollection(s, setOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 0 == setOutput.size() );
    DOF_UT_CHECK( setOutput.end() == setOutput.find("") );

    isSuccess = StringToCollection(s, vecOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 0 == vecOutput.size() );
    DOF_UT_CHECK( vecOutput.end() == find(vecOutput.begin(), vecOutput.end(), "") );
    
    s = "+";
    isSuccess = StringToCollection(s, setOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 1 == setOutput.size() );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("") );

    isSuccess = StringToCollection(s, vecOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 1 == vecOutput.size() );
    DOF_UT_CHECK( vecOutput.end() != find(vecOutput.begin(), vecOutput.end(), "") );

    s = "++";
    isSuccess = StringToCollection(s, setOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 1 == setOutput.size() );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("") );

    isSuccess = StringToCollection(s, vecOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 2 == vecOutput.size() );
    DOF_UT_CHECK( vecOutput[0] == "" && vecOutput[1] == "" );

    s = "+++34343";
    isSuccess = StringToCollection(s, setOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 2 == setOutput.size() );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("") );
    DOF_UT_CHECK( setOutput.end() != setOutput.find("34343") );

    isSuccess = StringToCollection(s, vecOutput);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 3 == vecOutput.size() );
    DOF_UT_CHECK( vecOutput[0] == "" && vecOutput[1] == "" && vecOutput[2] == "34343" );
}

void TestMapToString() {
    map<string, int> m;
    m["abc"] = 123;
    m["xxxxxx"] = 868;
    m[""] = 1000;
    m["negative"] = -123;
    
    DOF_UT_OUT( MapToString(m) );
}

int main() {
    TestStrip();   
    TestToLowerAndToUpper();
    TestHex2Dec();
    TestStringToCollectionEscape();
    TestAppendSuffix();
    TestStringToCollection();
    TestMapToString();
    return 0;
}
