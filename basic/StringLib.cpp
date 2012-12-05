#include "StringLib.h"

namespace dof {
namespace lib {
namespace str {

const set<char>& GetCommonBlankChars() {
    static set<char> __chars;
    static bool __isInit = false;
    if(!__isInit) {
	StringToCharSet(" \t\r\n", __chars);
    }
    return __chars;
}

string __inside_strip(const string& s, const set<char>& chars, bool isLstrip, bool isRstrip) {
    int l=s.length(),i=0,j=l-1;
    if(isLstrip) {
	for(;i<l;++i) {
	    if(chars.end() == chars.find(s[i])) {
		break;
	    }
	}
    }

    if(isRstrip) {
	for(;j>=0;--j) {
	    if(chars.end() == chars.find(s[j])) {
		break;
	    }
	}
    }

    return s.substr(i, j-i+1);
}

string Lstrip(const string& s, const set<char>& chars) {
    return __inside_strip(s, chars, true, false);
}

string Rstrip(const string& s, const set<char>& chars) {
    return __inside_strip(s, chars, false, true);
}

string Strip(const string& s, const set<char>& chars) {
    return __inside_strip(s, chars, true, true);
}

char ToUpper(char c) {
    if('a' <= c && c <= 'z') 
	return c ^ 32;

    return c;
}

char ToLower(char c) {
    if('A' <= c && c <= 'Z') 
	return c | 32;

    return c;
}

string ToUpperString(const string& s) {
    string d;
    d.resize(s.size());
    transform(s.begin(), s.end(), d.begin(), ToUpper);
    return d;
}

string ToLowerString(const string& s) {
    string d;
    d.resize(s.size());
    transform(s.begin(), s.end(), d.begin(), ToLower);
    return d;
}

bool IsHex(char c) {
    if( '0' <= c && c <= '9' ||
	'a' <= c && c <= 'f' ||
	'A' <= c && c <= 'F' ) {
	return true;
    }
    return false;
}

bool IsOnlyHex(const string& s) {
    const char *raw = s.c_str();
    for(int i=0,l=s.length();i<l;++i) {
	if(!IsHex(raw[i])) {
	    return false;
	}
    }
    return true;
}

char Dec2Hex(int c) {
    if(0 <= c && c <= 9) {
	return c + '0';
    }
    else if(10 <= c <= 15) {
	return 'A' + c - 10;
    }
    else {
	return -1;
    }
}

int Hex2Dec(char c) {
    if('0' <= c && c <= '9') {
	return c - '0';
    }
    else if('a' <= c && c <= 'f') {
	return c - 'a' + 10;
    }
    else if('A' <= c && c <= 'F') {
	return c - 'A' + 10;
    }
    else {
	return -1;
    }
}

unsigned long long HexDec(const string& raw) {
    int offset = 0, l = raw.length();
    char c;
    unsigned long long ret = 0;
    for(int i=l-1;i>=0;--i) {
	c = raw[i];
	if('0' <= c && c <= '9') {
	    ret += (c - '0') * (1 << offset);
	    offset += 4;
	}
	else if('a' <= c && c <= 'f') {
	    ret += (c - 'a' + 10) * (1 << offset);
	    offset += 4;
	}
	else if('A' <= c && c <= 'F') {
	    ret += (c - 'A' + 10) * (1 << offset);
	    offset += 4;
	}
    }
    return ret;
}

bool IsDigit(char c) {
    return '0' <= c && c <= '9';
}

bool IsOnlyDigit(const string& s) {
    const char *raw = s.c_str();
    for(int i=0,l=s.length();i<l;++i) {
	if(!IsDigit(raw[i])) {
	    return false;
	}
    }
    return true;
}

bool FindAnyChar(const string& target, const string& findingChars) {
    size_t p;
    char c;
    for(int i=0,l=target.length();i<l;++i) {
	c = target[i];
	p = findingChars.find_first_of(c, 0);
	if(p != findingChars.npos) {
	    return true;
	}
    }
    return false;
}

int ScanNext(const string& raw, int startPos, char target) {
    int i, l=raw.length();
    for(i=startPos;i<l;++i) {
	if(raw[i] == target) {
	    return i;
	}
    }
   return i;
}

int ScanNextInSet(const string& raw, int startPos, const set<char>& charSet) {
    int i, j, l=raw.length(), ls=charSet.size();
    for(i=startPos;i<l;++i) {
	char c = raw[i];
	if(charSet.end() != charSet.find(c)) {
	    return i;
	}
    }
    return i;
}

int ScanNNextInSet(const string& raw, int startPos, const set<char>& charSet, int n) {
    int i, l=raw.length(), p=startPos, q=p;
    for(int j=0;j<n;++j) {
	q = ScanNextInSet(raw, p, charSet);
	if(q >= l) { return l; }
	p = q+1;
    }
    return q;
}

int ScanNNext(const string& raw, int startPos, char target, int n) {
    int i, l=raw.length(), p=startPos, q=p;
    for(int j=0;j<n;++j) {
	q = ScanNext(raw, p, target);
	if(q >= l) { return l; }
	p = q+1;
    }
    return q;
}

int ScanNextUtilNot(const string& raw, int startPos, char target) {
    int i, l=raw.length();
    for(i=startPos;i<l;++i) {
	if(raw[i] != target) {
	    return i;
	}
    }
    return i;
}

int ScanNextUtilNotInChars(const string& raw, int startPos, const set<char>& target) {
    int i, l=raw.length();
    for(i=startPos;i<l;++i) {
	if(target.find(raw[i]) == target.end()) {
	    return i;
	}
    }
    return i;
}

int ScanNextUtilNotInChars(const string& raw, int startPos, const string& target) {
    set<char> s;
    for(string::const_iterator it=target.begin();it!=target.end();++it) {
	s.insert(*it);
    }
    return ScanNextUtilNotInChars(raw, startPos, s);
}

template<>
bool StringToMap<string, string>(const string& s, map<string, string>& output, char sep, char kvSep, bool isReset) {
    if(isReset) {
	output.clear();
    }

    int l=s.length();
    if(0 == l) {
	return true;
    }

    int p,q=0;
    p = (sep == s[0]) ? 1 : 0;
    int kStart, kLength, vStart, vLength;
    set<char> charSet;
    charSet.insert(kvSep);
    charSet.insert(sep);
    //如果最后一个字符是sep，那么也接受，而且结束解析
    //每一次循环开始,p表示key的第一个字符位置，q表示上一轮中sep的位置
    while(q < l-1) { 
	//这一步正确预期是碰到kvSep，中间所有的sep都会被当作key的一部分
	q = ScanNext(s, p, kvSep);
	if(q >= l || s[q] == sep) { 
	    output.clear(); //注意这里一旦出错，会把集合中原有的数据也清空
	    return false;
	}
	kStart = p;
	kLength = q-p;

	p = q+1;
	//这一步正确预期是碰到sep，中间所有的kvSep都当作value的一部分
	q = ScanNext(s, p, sep);
	if(q > l) { // 这里就允许了最后的key对应空的value
	    return false;
	}
	vStart = p;
	vLength = q-p;

	output[s.substr(kStart, kLength)] = s.substr(vStart, vLength);
	p = q+1;
    }
    return true;
}

inline bool Split(const string& s, string& first, string& second, char sep) {
    static TVecStr __parts;
    if(!StringToCollection(s, __parts, sep, true) || __parts.size() != 2) {
	return false;
    }
    first = __parts[0];
    second = __parts[1];
    return true;
}

inline bool Split(const string& s, string& first, string& second, string& third, char sep) {
    static TVecStr __parts;
    if(!StringToCollection(s, __parts, sep, true) || __parts.size() != 3) {
	return false;
    }
    first = __parts[0];
    second = __parts[1];
    third = __parts[2];
    return true;
}

}; // namespace str
}; // namespace lib
}; // namespace dof
