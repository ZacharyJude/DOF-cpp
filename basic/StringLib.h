#ifndef DOF_LIB_STR_STRING_LIB_H_
#define DOF_LIB_STR_STRING_LIB_H_

#include "CommonHeader.h"
using namespace std;

namespace dof {
namespace lib {
namespace str {

template<typename TOutputIterator>
void StringToChars(const string& s, TOutputIterator out) {
    for(string::const_iterator it=s.begin();it!=s.end();++it) {
	*out = *it;
    }
}

inline void StringToCharVector(const string& s, vector<char>& out) {
    return StringToChars(s, inserter(out, out.end()));
}

inline void StringToCharSet(const string& s, set<char>& out) {
    return StringToChars(s, inserter(out, out.end()));
}

const set<char>& GetCommonBlankChars();
string Strip(const string& s, const set<char>& = GetCommonBlankChars());
string Lstrip(const string& s, const set<char>& = GetCommonBlankChars());
string Rstrip(const string& s, const set<char>& = GetCommonBlankChars());

char ToUpper(char c);
char ToLower(char c);
string ToUpperString(const string& s);
string ToLowerString(const string& s);

bool IsHex(char c);
bool IsOnlyHex(const string& s);
char Dec2Hex(int c);
int Hex2Dec(char c);
unsigned long long HexDec(const string& raw);
bool IsDigit(char c);
bool IsOnlyDigit(const string& s);

bool FindAnyChar(const string& target, const string& findingChars);

// 这个函数也是用于分割字符串，但他允许转义情况。
// 例如现在使用'+'作为分隔符，'|‘作为转义符
// 那么a|+b的分割结果就是"a+b"而不是"a"、"b"
// 那如果转义字符后面的字符并不是分隔符，那就会把转义字符一齐保留，例如a|b+c的分割结果是"a|b"、"c"
// 如果要保留在分隔符前的转义符，需要重复一次，例如：abc||+123就会分割成"abc|"、"123"
// 如果重复转义符不是用在分隔符前面，就会两个都保留下来，例如abc||123+kkk会分割为："abc||123"、"kkk"
// 这个函数设计成这样虽然有点别扭，但这特定场景下非常有用，例如refer链参数的解析，
// 该参数是用'.'来分割每个refer，而refer内部是用'='来分割信息，而且允许转义这些字符。
// 在程序处理中，会先用这个函数分割refer，再对refer内部进行分割，只要思考一下这个问题，就能知道这个函数
// 设计的权衡了。
template<typename TOutputIterator>
bool OutputSeparateStringEscape(const string& s, TOutputIterator out, char sep='+', char escaper='\\') {
    int l=s.length();
    if(0 == l) {
	return true;
    }
    int p,q=0;
    p = (sep == s[0]) ? 1 : 0;
    bool isEscaping = false, isDoubleEscaping = false;
    string temp("");
    for(int i=p;i<l;++i) {
	if(!isEscaping) {
	    if(s[i] == escaper) { //进入转义
		isEscaping = true;
		continue;
	    }
	    else if(s[i] == sep) {
		*out = temp;
		temp.clear();
		continue;
	    }
	}
	if(isEscaping) { 
	    if(isDoubleEscaping) {
		if(s[i] == sep) {
		    *out = temp;
		    temp.clear();
		}
		else {
		    temp.push_back(escaper);
		    temp.push_back(s[i]);
		}
		isDoubleEscaping = false;
		isEscaping = false;
		continue;
	    }
	    else if(s[i] == escaper) {
		// 因为被转义的不是分隔符，所以还原上一个转义符
		isDoubleEscaping = true;
		temp.push_back(s[i]);
		continue;
	    }
	    else {
		if(s[i] != sep) {
		    temp.push_back(escaper);
		}
		isEscaping = false;
		temp.push_back(s[i]);
		continue;
	    }
	}
	else {
	    temp.push_back(s[i]);
	}
    }
    *out = temp;
    return true;
}

template<typename TCollection>
bool StringToCollectionEscape(const string& s, TCollection& output, char sep='+', bool isReset=true, char escaper='\\') {
    if(isReset) { output.clear(); }

    bool isSuccess = OutputSeparateStringEscape(s, inserter(output, output.end()), sep, escaper);
    if(!isSuccess) { output.clear(); }
    return isSuccess;
}

}; // namespace string
}; // namespace lib
}; // namespace dof

#endif // DOF_LIB_STR_STRING_LIB_H_
