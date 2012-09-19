#ifndef DOF_LIB_UTIL_SEPARATE_STRING_H_
#define DOF_LIB_UTIL_SEPARATE_STRING_H_

#include "CommonHeader.h"
#include "StringLib.h"
using namespace std;
using namespace dof::lib::str;

namespace dof {
namespace lib {
namespace util {

class SeparateString {
private:
    string _innerString;
    char _sep;
public:
    SeparateString():_innerString(""), _sep('+') { }
    SeparateString(const char *str):_innerString(str), _sep('+') { }
    SeparateString(const string& str):_innerString(str), _sep('+') { }
    SeparateString(const string& str, char sep):_innerString(str), _sep(sep) { }

    inline char GetSeparator() const { return this->_sep; }
    inline string GetString() const { return this->_innerString; }

    template<typename TCollection>
    inline bool ToCollection(TCollection& output, bool isReset=true) const {
	return StringToCollection(this->_innerString, output, this->_sep, isReset);
    }

    //注意在比较的时候是忽略了sep信息，也就是两个内部string一样，但分隔符不一样的
    //SeparateString也被认为是相等的。因此当要比较（例如在容器里存放）拥有不同分隔符的SeparateString时
    //要格外小心
    bool operator<(const SeparateString& another) {
	if(this == &another) {
	    return false;
	}

	return this->_innerString < another._innerString;
    }
};

} // namespace util
} // namespace lib
} // namespace dof

#endif // DOF_LIB_UTIL_SEPARATE_STRING_H_
