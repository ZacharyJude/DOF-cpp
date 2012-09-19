#ifndef UNIT_TEST_H_
#define UNIT_TEST_H_

#include <iostream>
#include <map>

static std::map<std::string, bool> __globalSwitches;

#define DOF_UT_CHECK( expression ) \
    if( !(expression) ) {\
	printf("check fail at %s:%d, function:%s\n", __FILE__, __LINE__, __FUNCTION__);\
    }

#define DOF_UT_OUT( msg ) \
    if(!__globalSwitches[__FUNCTION__]) {\
	printf("%s output at %s\n", __FUNCTION__, __FILE__);\
	__globalSwitches[__FUNCTION__] = true;\
    }\
    std::cout << msg << std::endl;

#endif // UNIT_TEST_H_
