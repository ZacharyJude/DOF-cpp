#ifndef DOF_LIB_STL_STL_LIB_H_
#define DOF_LIB_STL_STL_LIB_H_

#include "CommonHeader.h"
using namespace std;

namespace dof {
namespace lib {
namespace stl {

template<typename T>
void Append(vector<T>& v1, const vector<T>& v2) {
    for(vector<T>::const_iterator it=v2.begin();it!=v2.end();++it) {
	v1.push_back(*it);
    }
}

}; // namespace stl
}; // namespace lib
}; // namespace dof

#endif // DOF_LIB_STL_STL_LIB_H_
