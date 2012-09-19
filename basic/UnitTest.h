#ifndef UNIT_TEST_H_
#define UNIT_TEST_H_

#define DOF_UT_CHECK( expression ) \
    if( !(expression) ) {\
	printf("check fail at %s:%d, function:%s\n", __FILE__, __LINE__, __FUNCTION__);\
    }

#endif // UNIT_TEST_H_
