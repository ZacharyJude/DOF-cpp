#ifndef DOF_LIB_STL_STL_LIB_H_
#define DOF_LIB_STL_STL_LIB_H_

#include "CommonHeader.h"
using namespace std;

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
    return p1.first == p2.first ? p1.second < p2.second : p1.first < p2.first;
}

template<typename TFirst, typename TSecond>
bool FirstMainPairCmp(const pair<TFirst, TSecond>& p1, const pair<TFirst, TSecond>& p2) {
    return p1.second == p2.second ? p1.first < p2.first : p1.second < p2.second;
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

template<typename TElement>
void GenListToFile(const string& fileName, const vector<TElement>& list) {
    ofstream fout(fileName.c_str(), ios::out);
    for(typename vector<TElement>::const_iterator it0=list.begin();it0!=list.end();++it0) {
	fout << *it0 << endl;
    }   
    fout.close();
}

}; // namespace stl
}; // namespace lib
}; // namespace dof

#endif // DOF_LIB_STL_STL_LIB_H_
