#include <iostream>
#include "UnitTest.h"
#include "ParseBase.h"
using namespace dof::parse;

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

int main() {
    TestScanNext();
    TestScanNextUtilNot();
    TestScanNextUtilNotInChars();
    return 0;
}
