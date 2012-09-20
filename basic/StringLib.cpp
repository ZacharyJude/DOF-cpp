#include "StringLib.h"

const set<char>& dof::lib::str::GetCommonBlankChars() {
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

string dof::lib::str::Lstrip(const string& s, const set<char>& chars) {
    return __inside_strip(s, chars, true, false);
}

string dof::lib::str::Rstrip(const string& s, const set<char>& chars) {
    return __inside_strip(s, chars, false, true);
}

string dof::lib::str::Strip(const string& s, const set<char>& chars) {
    return __inside_strip(s, chars, true, true);
}

char dof::lib::str::ToUpper(char c) {
    if('a' <= c && c <= 'z') 
	return c ^ 32;

    return c;
}

char dof::lib::str::ToLower(char c) {
    if('A' <= c && c <= 'Z') 
	return c | 32;

    return c;
}

string dof::lib::str::ToUpperString(const string& s) {
    string d;
    d.resize(s.size());
    transform(s.begin(), s.end(), d.begin(), ToUpper);
    return d;
}

string dof::lib::str::ToLowerString(const string& s) {
    string d;
    d.resize(s.size());
    transform(s.begin(), s.end(), d.begin(), ToLower);
    return d;
}

bool dof::lib::str::IsHex(char c) {
    if( '0' <= c && c <= '9' ||
	'a' <= c && c <= 'f' ||
	'A' <= c && c <= 'F' ) {
	return true;
    }
    return false;
}

bool dof::lib::str::IsOnlyHex(const string& s) {
    const char *raw = s.c_str();
    for(int i=0,l=s.length();i<l;++i) {
	if(!IsHex(raw[i])) {
	    return false;
	}
    }
    return true;
}

char dof::lib::str::Dec2Hex(int c) {
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

int dof::lib::str::Hex2Dec(char c) {
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

unsigned long long dof::lib::str::HexDec(const string& raw) {
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

bool dof::lib::str::IsDigit(char c) {
    return '0' <= c && c <= '9';
}

bool dof::lib::str::IsOnlyDigit(const string& s) {
    const char *raw = s.c_str();
    for(int i=0,l=s.length();i<l;++i) {
	if(!IsDigit(raw[i])) {
	    return false;
	}
    }
    return true;
}

bool dof::lib::str::FindAnyChar(const string& target, const string& findingChars) {
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

int dof::lib::str::ScanNext(const string& raw, int startPos, char target) {
    int i, l=raw.length();
    for(i=startPos;i<l;++i) {
	if(raw[i] == target) {
	    return i;
	}
    }
   return i;
}

int dof::lib::str::ScanNextInSet(const string& raw, int startPos, const set<char>& charSet) {
    int i, j, l=raw.length(), ls=charSet.size();
    for(i=startPos;i<l;++i) {
	char c = raw[i];
	if(charSet.end() != charSet.find(c)) {
	    return i;
	}
    }
    return i;
}

int dof::lib::str::ScanNNextInSet(const string& raw, int startPos, const set<char>& charSet, int n) {
    int i, l=raw.length(), p=startPos, q=p;
    for(int j=0;j<n;++j) {
	q = ScanNextInSet(raw, p, charSet);
	if(q >= l) { return l; }
	p = q+1;
    }
    return q;
}

int dof::lib::str::ScanNNext(const string& raw, int startPos, char target, int n) {
    int i, l=raw.length(), p=startPos, q=p;
    for(int j=0;j<n;++j) {
	q = ScanNext(raw, p, target);
	if(q >= l) { return l; }
	p = q+1;
    }
    return q;
}

int dof::lib::str::ScanNextUtilNot(const string& raw, int startPos, char target) {
    int i, l=raw.length();
    for(i=startPos;i<l;++i) {
	if(raw[i] != target) {
	    return i;
	}
    }
    return i;
}

int dof::lib::str::ScanNextUtilNotInChars(const string& raw, int startPos, const set<char>& target) {
    int i, l=raw.length();
    for(i=startPos;i<l;++i) {
	if(target.find(raw[i]) == target.end()) {
	    return i;
	}
    }
    return i;
}

int dof::lib::str::ScanNextUtilNotInChars(const string& raw, int startPos, const string& target) {
    set<char> s;
    for(string::const_iterator it=target.begin();it!=target.end();++it) {
	s.insert(*it);
    }
    return ScanNextUtilNotInChars(raw, startPos, s);
}
