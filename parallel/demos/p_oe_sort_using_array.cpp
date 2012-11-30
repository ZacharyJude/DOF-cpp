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
void LoadDataFromFileArray(const string& fileName, lli myStart, lli myLength, lli totalNumbers, TNumber *data, lli& dataSize) {
    ifstream fin(fileName.c_str(), ios::in);
    string line;
    lli myLimit = myStart + myLength;
    dataSize = 0;
    for(lli i=0;i<totalNumbers && i< myLimit;++i) {
	getline(fin, line);
	if(i < myStart) {
	    continue;
	}
	data[dataSize++] = lexical_cast<TNumber>(line);
    }
    fin.close();
}

template<typename TNumber>
void UpdateMyDataFromPreNodeArray(lli myRank, TNumber *myData, lli& dataSize, communicator& world) {
    if(myRank-1 >= 0) {
	world.send(myRank-1, 0, dataSize);
	world.send(myRank-1, 1, myData, dataSize); //myRank-1 提取数据
	world.recv(myRank-1, 0, dataSize);
	world.recv(myRank-1, 1, myData, dataSize); //myRank-1 处理完之后把数据发回来
    }
}

template<typename TNumber>
void MergeSortDataForMeAndNextNodeArray(lli myRank, TNumber *myData, lli& dataSize, communicator& world) {
    static TNumber *nextNodeData = new TNumber[dataSize*2];
    static TNumber *mergeData = new TNumber[dataSize*3];
    lli nextNodeDataSize=0, mergeDataSize=0;
    if(myRank+1 < world.size()) { //有右边节点
	nextNodeDataSize = 0;
	world.recv(myRank+1, 0, nextNodeDataSize);
	world.recv(myRank+1, 1, nextNodeData, nextNodeDataSize);

	merge(myData, myData+dataSize, nextNodeData, nextNodeData+nextNodeDataSize, mergeData);
	mergeDataSize = dataSize + nextNodeDataSize;
	dataSize = 0;
	for(lli i=0,limit=(mergeDataSize/2);i<limit;++i) {
	    myData[dataSize++] = mergeData[i];
	}
	nextNodeDataSize = 0;
	for(lli i=(mergeDataSize/2),limit=mergeDataSize;i<limit;++i) {
	    nextNodeData[nextNodeDataSize++] = mergeData[i];
	}
	world.send(myRank+1, 0, nextNodeDataSize);
	world.send(myRank+1, 1, nextNodeData, nextNodeDataSize);
    }
}

template<typename TNumber>
void ParallelOddEvenSortArray(lli myRank, lli worldSize, TNumber *myData, lli& dataSize, communicator& world) {
    if(myRank % 2 == 1) {
	UpdateMyDataFromPreNodeArray(myRank, myData, dataSize, world);
    }
    for(lli i=(myRank % 2);i<world.size()-1;i+=2) {
	MergeSortDataForMeAndNextNodeArray(myRank, myData, dataSize, world);
	//条件判断当前的奇（偶）轮迭代是不是最后一轮迭代，不是的话还得把自己的数据给pre node更新
	if((i+1) == world.size() - 1) { break; }
	UpdateMyDataFromPreNodeArray(myRank, myData, dataSize, world);
    }
}
	
int main(int argc, char *argv[]) {
    COUNT_TIME_START(global);
    environment env(argc, argv);
    communicator world;
    lli totalNumbers = lexical_cast<lli>(string(argv[1]));
    lli myRank = world.rank();
    lli worldSize = world.size();
    lli myStart=-1, myLength=-1;

    GetPartionArgs<lli>(totalNumbers, myRank, worldSize, myStart, myLength);
    lli *myData = new lli[myLength+1]; //安全长度
    lli myDataSize = 0;
    COUNT_TIME_START(load_file);
    LoadDataFromFileArray("number_to_sort", myStart, myLength, totalNumbers, myData, myDataSize);
    COUNT_TIME_END(load_file);
    COUNT_TIME_START(parallel_sorting);
    sort(myData, myData+myDataSize);
    ParallelOddEvenSortArray(myRank, worldSize, myData, myDataSize, world);
    COUNT_TIME_END(parallel_sorting);

    COUNT_TIME_START(write_result);
    stringstream ss;
    ss << "number_sorted_part." << myRank;
    FILE *out = fopen(ss.str().c_str(), "w");
    for(lli i=0;i<myDataSize;++i) {
	fprintf(out, "%ld\n", myData[i]);
    }
    fclose(out);
    out = NULL;
    COUNT_TIME_END(write_result);
    COUNT_TIME_END(global);

    return 0;
}
