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

void TestScanNext() {
    string raw("xx1xxx1,xxx2xxxxx3,x,x,x,");  
    int p;
    p = ScanNext(raw, 0, '1');
    DOF_UT_CHECK( 2 == p );
    p = ScanNext(raw, 2, '1');
    DOF_UT_CHECK( 2 == p );
    p = ScanNext(raw, 2, '3');
    DOF_UT_CHECK( 17 == p );
    
    p = 0;
    p = ScanNext(raw, p, ',');
    p = ScanNext(raw, p+1, ',');
    p = ScanNext(raw, p+1, ',');
    p = ScanNext(raw, p+1, ',');
    p = ScanNext(raw, p+1, ',');
    DOF_UT_CHECK( raw.length()-1 == p );
    p = ScanNext(raw, p+1, ',');
    DOF_UT_CHECK( raw.length() == p );
}

void TestScanNextUtilNot() {
    string raw("xx1xxx1,xxx2xxxxx3,x,x,x,");  
    int p;
    p = ScanNextUtilNot(raw, 0, 'x');
    DOF_UT_CHECK( 2 == p );
    p = ScanNextUtilNot(raw, 2, '1');
    DOF_UT_CHECK( 3 == p );
    p = ScanNextUtilNot(raw, 3, 'x');
    DOF_UT_CHECK( 6 == p );
    p = ScanNextUtilNot(raw, raw.length()-1, 'x');
    DOF_UT_CHECK( raw.length()-1 == p );
    p = ScanNextUtilNot(raw, raw.length()-1, ',');
    DOF_UT_CHECK( raw.length() == p );
}

void TestScanNextUtilNotInChars() {
    string raw("xx1xxx1,xxx2xxxxx3,x,x,x,");  
    int p;
    p = ScanNextUtilNotInChars(raw, 0, "x1");
    DOF_UT_CHECK( 7 == p );
    p = ScanNextUtilNotInChars(raw, 0, "x1,");
    DOF_UT_CHECK( 11 == p );
    p = ScanNextUtilNotInChars(raw, 0, "x123,");
    DOF_UT_CHECK( raw.length() == p );
    p = ScanNextUtilNotInChars(raw, 18, ",");
    DOF_UT_CHECK( 19 == p );
    p = ScanNextUtilNotInChars(raw, 18, ",x");
    DOF_UT_CHECK( raw.length() == p );
    /*
    p = ScanNextUtilNot(raw, 2, '1');
    DOF_UT_CHECK( 3 == p );
    p = ScanNextUtilNot(raw, 3, 'x');
    DOF_UT_CHECK( 6 == p );
    p = ScanNextUtilNot(raw, raw.length()-1, 'x');
    DOF_UT_CHECK( raw.length()-1 == p );
    p = ScanNextUtilNot(raw, raw.length()-1, ',');
    DOF_UT_CHECK( raw.length() == p );
    */
}

void TestStringToMap() {
    string s;
    map<string, string> output;
    bool isSuccess;

    s = "+a^123+abc^kkk+92^dfjdjfd";
    isSuccess = StringToMap<string,string>(s, output);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( "123" == output["a"] );
    DOF_UT_CHECK( "kkk" == output["abc"] );
    DOF_UT_CHECK( "dfjdjfd" == output["92"] );
    DOF_UT_CHECK( 3 == output.size() );
    
    s = "";
    isSuccess = StringToMap<string,string>(s, output);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 0 == output.size() );
    
    s = "+";
    isSuccess = StringToMap<string,string>(s, output);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 0 == output.size() );

    s = "++";
    isSuccess = StringToMap<string,string>(s, output);
    DOF_UT_CHECK( false == isSuccess );
    DOF_UT_CHECK( 0 == output.size() );

    s = "++abc^sds";
    isSuccess = StringToMap<string,string>(s, output);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 1 == output.size() );
    DOF_UT_CHECK( "sds" == output["+abc"] );

    s = "+abc^sds++";
    isSuccess = StringToMap<string,string>(s, output);
    DOF_UT_CHECK( false == isSuccess );
    DOF_UT_CHECK( 0 == output.size() );
    
    s = "+novalue^+34343^sss";
    isSuccess = StringToMap<string,string>(s, output);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( "" == output["novalue"] );
    DOF_UT_CHECK( "sss" == output["34343"] );

    s = "+novalue^+34+343^sss";
    isSuccess = StringToMap<string,string>(s, output);
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( "" == output["novalue"] );
    DOF_UT_CHECK( "sss" == output["34+343"] );

    //这个例子特点是KV分隔符出现在VALUE中
    s = "udid:42731dc4c6ff5dda59a4186986323c9bcfb2dc7a,orientation:ipad_orientation_portrait,access_token:745c428f52d401679b938239882abd7d,mac_address:70:DE:E2:C4:67:B4";
    isSuccess = StringToMap<string,string>(s, output, ',', ':');
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 4 == output.size() );
    DOF_UT_CHECK( "42731dc4c6ff5dda59a4186986323c9bcfb2dc7a" == output["udid"] );
    DOF_UT_CHECK( "ipad_orientation_portrait" == output["orientation"] );
    DOF_UT_CHECK( "745c428f52d401679b938239882abd7d" == output["access_token"] );
    DOF_UT_CHECK( "70:DE:E2:C4:67:B4" == output["mac_address"] );

    //这个例子特点是最后一个字符是分割符
    s = "udid:42731dc4c6ff5dda59a4186986323c9bcfb2dc7a,orientation:ipad_orientation_portrait,access_token:745c428f52d401679b938239882abd7d,mac_address:70:DE:E2:C4:67:B4,";
    isSuccess = StringToMap<string,string>(s, output, ',', ':');
    DOF_UT_CHECK( true == isSuccess );
    DOF_UT_CHECK( 4 == output.size() );
    DOF_UT_CHECK( "42731dc4c6ff5dda59a4186986323c9bcfb2dc7a" == output["udid"] );
    DOF_UT_CHECK( "ipad_orientation_portrait" == output["orientation"] );
    DOF_UT_CHECK( "745c428f52d401679b938239882abd7d" == output["access_token"] );
    DOF_UT_CHECK( "70:DE:E2:C4:67:B4" == output["mac_address"] );
}

int main() {
    TestStrip();   
    TestToLowerAndToUpper();
    TestHex2Dec();
    TestStringToCollectionEscape();
    TestAppendSuffix();
    TestStringToCollection();
    TestMapToString();
    TestScanNext();
    TestScanNextUtilNot();
    TestScanNextUtilNotInChars();
    TestStringToMap();
    return 0;
}
