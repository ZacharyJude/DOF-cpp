#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <boost/serialization/vector.hpp>
using namespace std;
using namespace boost;
using namespace boost::mpi;
using namespace boost::posix_time;

typedef long long int lli;

#define COUNT_TIME_START(id) \
	ptime start##id = second_clock::local_time();

#define COUNT_TIME_END(id) \
	ptime end##id = second_clock::local_time();\
	time_duration td##id = end##id - start##id;\
	cout << #id << " time use:" << to_simple_string(td##id) << endl;

template<typename TInt>
void GetPartionArgs(TInt totalLength, TInt myRank, TInt partsCount, TInt& myStart, TInt& myLength) {
    TInt groupSize = (TInt)(totalLength / partsCount);
    TInt remain = (TInt)(totalLength % partsCount);
    myStart = (myRank < remain) ? (groupSize * myRank + myRank) : (groupSize * myRank + remain);
    myLength = (myRank < remain) ? groupSize + 1 : groupSize;
}

template<typename TNumber>
void LoadDataFromFile(const string& fileName, lli myStart, lli myLength, lli totalNumbers, vector<TNumber>& data) {
    ifstream fin(fileName.c_str(), ios::in);
    string line;
    lli myLimit = myStart + myLength;
    for(lli i=0;i<totalNumbers && i< myLimit;++i) {
	getline(fin, line);
	if(i < myStart) {
	    continue;
	}
	data.push_back(lexical_cast<TNumber>(line));
    }
    fin.close();
}

template<typename TNumber>
void UpdateMyDataFromPreNode(lli myRank, vector<TNumber>& myData, communicator& world) {
    if(myRank-1 >= 0) {
	world.send(myRank-1, 0, myData); //myRank-1 提取数据
	myData.clear();
	world.recv(myRank-1, 0, myData); //myRank-1 处理完之后把数据发回来
    }
}

template<typename TNumber>
void MergeSortDataForMeAndNextNode(lli myRank, vector<TNumber>& myData, communicator& world) {
    static vector<TNumber> nextNodeData, mergeData;
    if(myRank+1 < world.size()) { //有右边节点
	nextNodeData.clear();
	world.recv(myRank+1, 0, nextNodeData);
	mergeData.clear();
	merge(myData.begin(), myData.end(), nextNodeData.begin(), nextNodeData.end(), inserter(mergeData, mergeData.end()));
	myData.clear();
	for(lli i=0,limit=(mergeData.size()/2);i<limit;++i) {
	    myData.push_back(mergeData[i]);
	}
	nextNodeData.clear();
	for(lli i=(mergeData.size()/2),limit=mergeData.size();i<limit;++i) {
	    nextNodeData.push_back(mergeData[i]);
	}
	world.send(myRank+1, 0, nextNodeData);
    }
}

template<typename TNumber>
void ParallelOddEvenSort(lli myRank, lli worldSize, vector<TNumber>& initNumbers, communicator& world) {
    vector<TNumber> myData = initNumbers;

    if(myRank % 2 == 1) {
	UpdateMyDataFromPreNode(myRank, myData, world);
    }
    for(lli i=(myRank % 2);i<world.size()-1;i+=2) {
	MergeSortDataForMeAndNextNode(myRank, myData, world);
	//条件判断当前的奇（偶）轮迭代是不是最后一轮迭代，不是的话还得把自己的数据给pre node更新
	if((i+1) == world.size() - 1) { break; }
	UpdateMyDataFromPreNode(myRank, myData, world);
    }
    initNumbers = myData;
}

int main(int argc, char *argv[]) {
    COUNT_TIME_START(global);
    environment env(argc, argv);
    communicator world;
    lli totalNumbers = lexical_cast<lli>(string(argv[1]));
    lli myRank = world.rank();
    lli worldSize = world.size();
    lli myStart=-1, myLength=-1;

    vector<lli> initNumbers;
    GetPartionArgs<lli>(totalNumbers, myRank, worldSize, myStart, myLength);
    COUNT_TIME_START(load_file);
    LoadDataFromFile("number_to_sort", myStart, myLength, totalNumbers, initNumbers);
    COUNT_TIME_END(load_file);
    COUNT_TIME_START(parallel_sorting);
    sort(initNumbers.begin(), initNumbers.end());
    ParallelOddEvenSort(myRank, worldSize, initNumbers, world);
    COUNT_TIME_END(parallel_sorting);

    COUNT_TIME_START(write_result);
    stringstream ss;
    ss << "number_sorted_part." << myRank;
    FILE *out = fopen(ss.str().c_str(), "w");
    for(vector<lli>::const_iterator it0=initNumbers.begin();it0!=initNumbers.end();++it0) {
	fprintf(out, "%ld\n", *it0);
    }
    fclose(out);
    out = NULL;
    COUNT_TIME_END(write_result);
    COUNT_TIME_END(global);

    return 0;
}
