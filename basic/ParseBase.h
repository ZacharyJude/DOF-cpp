#ifndef DOF_PARSE_BASE_H_
#define DOF_PARSE_BASE_H_

#include "CommonHeader.h"
using namespace std;

namespace dof {
namespace parse {

int ScanNext(const string& raw, int startPos, char target);
int ScanNextInSet(const string& raw, int startPos, const set<char>& charSet);
int ScanNNextInSet(const string& raw, int startPos, const set<char>& charSet, int n);
int ScanNNext(const string& raw, int startPos, char target, int n);
int ScanNextUtilNot(const string& raw, int startPos, char target);
int ScanNextUtilNotInChars(const string& raw, int startPos, const set<char>& target);
int ScanNextUtilNotInChars(const string& raw, int startPos, const string& target);

} // namespace parse
} // namespace dof

#endif // DOF_PARSE_BASE_H_
