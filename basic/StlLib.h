#ifndef DOF_LIB_STL_STL_LIB_H_
#define DOF_LIB_STL_STL_LIB_H_

#include "CommonHeader.h"
#include "basic/StringLib.h"
using namespace std;
using namespace dof::lib::str;

namespace dof {
namespace lib {
namespace stl {

template<typename T>
void Append(vector<T>& v1, const vector<T>& v2) {
    for(typename vector<T>::const_iterator it=v2.begin();it!=v2.end();++it) {
	v1.push_back(*it);
    }
}

template<typename TFirst, typename TSecond>
bool SecondMainPairCmp(const pair<TFirst, TSecond>& p1, const pair<TFirst, TSecond>& p2) {
    return p1.second == p2.second ? p1.first < p2.first : p1.second < p2.second;
}

template<typename TFirst, typename TSecond>
bool FirstMainPairCmp(const pair<TFirst, TSecond>& p1, const pair<TFirst, TSecond>& p2) {
    return p1.first == p2.first ? p1.second < p2.second : p1.first < p2.first;
}

template<typename TFirst, typename TSecond>
bool SecondOnlyPairCmp(const pair<TFirst, TSecond>& p1, const pair<TFirst, TSecond>& p2) {
    return p1.second < p2.second;
}

template<typename TFirst, typename TSecond>
bool FirstOnlyPairCmp(const pair<TFirst, TSecond>& p1, const pair<TFirst, TSecond>& p2) {
    return p1.first < p2.first;
}

template<typename TElement>
void GenSortedUniqVecFromSet(const set<TElement>& src, vector<TElement>& dest) {
    for(typename set<TElement>::const_iterator it0=src.begin();it0!=src.end();++it0) {
	dest.push_back(*it0);
    }
    sort(dest.begin(), dest.end());
}

template<typename T, typename TInserter>
void Intersect(const set<T>& s0, const set<T>& s1, TInserter out) {
    for(typename set<T>::const_iterator it0=s0.begin();it0!=s0.end();++it0) {
	typename set<T>::const_iterator find = s1.find(*it0);
	if(find != s1.end()) {
	    *out = *find;   
	}
    }
}

template<typename T, typename TInserter>
void Union(const set<T>& s0, TInserter out) {
    for(typename set<T>::const_iterator it0=s0.begin();it0!=s0.end();++it0) {
	*out = *it0;
    }
}

template<typename TElement>
void GenListToFile(const string& fileName, const vector<TElement>& list) {
    ofstream fout(fileName.c_str(), ios::out);
    for(typename vector<TElement>::const_iterator it0=list.begin();it0!=list.end();++it0) {
	fout << *it0 << endl;
    }   
    fout.close();
}

template<typename TInserter>
void LoadListFromTextLineFile(const string& fileName, TInserter out) {
    string line;
    ifstream fin(fileName.c_str(), ios::in);
    for(getline(fin, line);!fin.eof();getline(fin, line)) {
	if("" == line) { continue; }
	*out = lexical_cast<typename TInserter::container_type::value_type>(line);
    }
}

template<typename TKey, typename TValue>
void GenMapToFile(const string& fileName, const map<TKey, TValue>& m, char sep='\t') {
    ofstream fout(fileName.c_str(), ios::out);
    for(typename map<TKey, TValue>::const_iterator it0=m.begin();it0!=m.end();++it0) {
	fout << it0->first << sep << it0->second << endl;
    }
    fout.close();
}

template<typename TKey, typename TValue>
void LoadMapFromFile(const string& fileName, map<TKey, TValue>& m, char sep='\t', bool allowOverride=true) {
    ifstream fin(fileName.c_str(), ios::in);
    string key, value, line;
    while(!fin.eof()) {
	getline(fin,line);
	if("" == line) {
	    continue;
	}
	if(!Split(line, key, value, sep)) {
	    cerr << "function LoadMapFromFile, error when loading data line:" << line << endl;
	    throw;
	}

	TKey k = lexical_cast<TKey>(key);
	TValue v = lexical_cast<TValue>(value);
	if(!allowOverride && m.find(k) != m.end()) {
	    cerr << "function LoadMapFromFile, loading existing key while override is not allowed." << endl;
	    throw;
	}
	m[k] = v;
    }
    fin.close();
}

}; // namespace stl
}; // namespace lib
}; // namespace dof

#endif // DOF_LIB_STL_STL_LIB_H_
