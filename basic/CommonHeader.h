#ifndef DOF_COMMON_HEADER_H_
#define DOF_COMMON_HEADER_H_

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <iterator>
#include <algorithm>
using namespace std;

// for boost.
#include <boost/regex.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
using namespace boost;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::filesystem;
using namespace boost::archive;

namespace dof {

typedef long long int TLL;
typedef vector< int > TVecI;
typedef vector< TLL > TVecLL;
typedef vector< string > TVecStr;
typedef set< string > TSetStr;
typedef set< TLL > TSetLL;
typedef pair< int, int > TPairII;
typedef pair< string, string> TPairStrStr;
typedef pair<TLL, double> TPairLLDB;
typedef map< int, int > TMapII;
typedef map< TLL, TLL> TMapLLLL;
typedef map< string, string > TMapStrStr;
typedef map< string, int> TMapStrI;
typedef map< string, TLL > TMapStrLL;
typedef map< TLL, double > TMapLLDB;
typedef map< string, int > TMapStrI;
typedef map< string, TSetStr > TMapStrSetStr;
typedef vector<TPairStrStr> TVecPairStrStr;
typedef vector<TPairLLDB> TVecPLLDB;
typedef map<TLL, TMapLLDB> TMapLLMapLLDB;
typedef map<TLL, TVecPLLDB> TMapLLVecPLLDB;
typedef map<TLL, TMapLLLL> TMapLLMapLLLL;

} // namespace dof

#include "Macro.h"

#endif // DOF_COMMON_HEADER_H_
