#ifndef DOF_COMMON_HEADER_H_
#define DOF_COMMON_HEADER_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iterator>
#include <algorithm>
typedef std::vector< int > TVecI;
typedef std::vector< long long > TVecLL;
typedef std::vector< std::string > TVecStr;
typedef std::set< std::string > TSetStr;
typedef std::pair< int, int > TPairII;
typedef std::pair< std::string, std::string> TPairStrStr;
typedef std::map< int, int > TMapII;
typedef std::map< std::string, std::string > TMapStrStr;
typedef std::map< std::string, int> TMapStrI;
typedef std::pair< std::string, std::string > TPairStrStr;
typedef std::map< std::string, int > TMapStrI;
typedef std::map< std::string, TSetStr > TMapStrSetStr;
typedef std::vector<TPairStrStr> TVecPairStrStr;
typedef std::map< std::string, long long > TMapStrLL;

// for boost.
/*
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
*/

#include "Macro.h"

#endif // DOF_COMMON_HEADER_H_
