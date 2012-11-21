#include <boost/lexical_cast.hpp>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <string>
using namespace std;
using namespace boost;

int main(int argc, char *argv[]) {
    int nRow = lexical_cast<int>(string(argv[1]));
    int nCol = lexical_cast<int>(string(argv[2]));
    srand(time(0));
    for(int i=0;i<nRow;++i) {
	for(int j=0;j<nCol;++j) {
	    double val = (double)(rand() % 1000001) / (double)1000000;
	    printf("%lf", val);
	    if(j+1<nCol) {
		printf(" ");
	    }   
	}   
	printf("\n");
    }   
    return 0;
}
