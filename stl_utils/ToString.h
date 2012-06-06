#ifndef TO_STRING_H_
#define TO_STRING_H_

#include <string>
#include <sstream>
using namespace std;

inline int ScanNext(const string& raw, int startPos, char target) {
    int i, l=raw.length();
    for(i=startPos;i<l;++i) {
	if(raw[i] == target) {
	    return i;
	}
    }
   return i;
}

inline int ScanNextInSet(const string& raw, int startPos, const string& charSet) {
    int i, j, l=raw.length(), ls=charSet.length();
    for(i=startPos;i<l;++i) {
	char c = raw[i];
	for(j=0;j<ls;++j) {
	    if(c == charSet[j]) {
		return i;
	    }
	}
    }
    return i;
}

inline int ScanNNext(const string& raw, int startPos, char target, int n) {
    int i, l=raw.length(), p=startPos, q=p;
    for(int j=0;j<n;++j) {
	q = ScanNext(raw, p, target);
	if(q >= l) { return l; }
	p = q+1;
    }
    return q;
}

template<typename TOutputIterator>
bool OutputSeparateString(const string& s, TOutputIterator out, char sep='+') {
    int l=s.length();
    if(0 == l) {
	return true;
    }
    int p,q=0;
    p = (sep == s[0]) ? 1 : 0;
    while(q < l) {
	q = ScanNNext(s, p, sep, 1);
	if(q > l) {
	    return false;
	}
	*out = s.substr(p,q-p);
	++out;
	p = q+1;
    }
    return true;
}

template<typename TCollection>
string CollectionToString(const TCollection& target,  char sep='+', bool withLeadingSep=true) {
    stringstream ss;
    int size = target.size(), cnt = 0;

    if(withLeadingSep) {
	ss << sep;
    }

    typename TCollection::const_iterator it;
    for(it=target.begin();it!=target.end();++it) {
	++cnt;
	if(cnt == size) {
	    break;
	}
	ss << *it << sep;
    }

    ss << *it;
    return ss.str();
}

template<typename TCollection>
bool StringToCollection(const string& s, TCollection& output, char sep='+', bool isReset=true) {
    if(isReset) { output.clear(); }

    bool isSuccess = OutputSeparateString(s, inserter(output, output.end()), sep);
    if(!isSuccess) { output.clear(); }
    return isSuccess;
}

#endif // TO_STRING_H_
