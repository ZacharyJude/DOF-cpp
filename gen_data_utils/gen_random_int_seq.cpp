#include <boost/lexical_cast.hpp>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <string>
using namespace std;
using namespace boost;

int main(int argc, char *argv[]) {
    int nNumberToGen = lexical_cast<int>(string(argv[1]));
    srand(time(0));
    while(nNumberToGen--) {
	cout << rand() << endl;
    }   
    return 0;
}
