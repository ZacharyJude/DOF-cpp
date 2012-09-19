#include "ParseBase.h"

int dof::parse::ScanNext(const string& raw, int startPos, char target) {
    int i, l=raw.length();
    for(i=startPos;i<l;++i) {
	if(raw[i] == target) {
	    return i;
	}
    }
   return i;
}

int dof::parse::ScanNextInSet(const string& raw, int startPos, const set<char>& charSet) {
    int i, j, l=raw.length(), ls=charSet.size();
    for(i=startPos;i<l;++i) {
	char c = raw[i];
	if(charSet.end() != charSet.find(c)) {
	    return i;
	}
    }
    return i;
}

int dof::parse::ScanNNextInSet(const string& raw, int startPos, const set<char>& charSet, int n) {
    int i, l=raw.length(), p=startPos, q=p;
    for(int j=0;j<n;++j) {
	q = ScanNextInSet(raw, p, charSet);
	if(q >= l) { return l; }
	p = q+1;
    }
    return q;
}

int dof::parse::ScanNNext(const string& raw, int startPos, char target, int n) {
    int i, l=raw.length(), p=startPos, q=p;
    for(int j=0;j<n;++j) {
	q = ScanNext(raw, p, target);
	if(q >= l) { return l; }
	p = q+1;
    }
    return q;
}

int dof::parse::ScanNextUtilNot(const string& raw, int startPos, char target) {
    int i, l=raw.length();
    for(i=startPos;i<l;++i) {
	if(raw[i] != target) {
	    return i;
	}
    }
    return i;
}

int dof::parse::ScanNextUtilNotInChars(const string& raw, int startPos, const set<char>& target) {
    int i, l=raw.length();
    for(i=startPos;i<l;++i) {
	if(target.find(raw[i]) == target.end()) {
	    return i;
	}
    }
    return i;
}

int dof::parse::ScanNextUtilNotInChars(const string& raw, int startPos, const string& target) {
    set<char> s;
    for(string::const_iterator it=target.begin();it!=target.end();++it) {
	s.insert(*it);
    }
    return ScanNextUtilNotInChars(raw, startPos, s);
}
