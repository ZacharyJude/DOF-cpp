#include "CommonHeader.h"
#include "Util.h"
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/collectives.hpp>
using namespace boost::mpi;

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

typedef vector<TStatInt> TVecSI;

inline int sign(double number) {
    if (number < 0)
	return -1;
    else
	return 1;
}

class Compution {
private:
    TStatInt _nRow, _nCol, _nMyCol, _maxIteration, _currentIteration, _safeSize;
    string _matrixFileName;
    double **_myPartOfMatrix, _orthogonalThreshold;
    TVecSI _myMatrixIndex, _myColumnIDs, _freeColumnSapces;
    bool _isAllOrthogonal;
    communicator& _world;

    void LoadMatrixHeader(ifstream& fin) {
	string line;
	TVecS parts;
	getline(fin, line);
	if(!StringToCollection(line, parts, ' ', true)) {
	    cerr << "invalid matrix header:" << line << endl;
	    throw;
	}
	this->_nRow = lexical_cast<TStatInt>(parts[0]);
	this->_nCol = lexical_cast<TStatInt>(parts[1]);
    }

    void InnerProduct(const double *v, const double *u, TStatInt len, double &innerProduce) const {
	innerProduce = 0.0;
	for(TStatInt i=0;i<len;++i) {
	    innerProduce += v[i] * u[i];
	}
    }

    void OrthogonalizeAndMaySwitchTwoColumn(TStatInt i, TStatInt j, bool &isOrthogonal) {
	if(i>j) {
	    swap(i, j);
	}
	double *v1 = this->_myPartOfMatrix[this->_myMatrixIndex[i]], *v2 = this->_myPartOfMatrix[this->_myMatrixIndex[j]];
	double ele, ele1, ele2;
	this->InnerProduct(v1, v2, this->_nRow, ele);
	if(fabs(ele) < this->_orthogonalThreshold) { //i,j两列已经正交
	    isOrthogonal = true;
	    return;
	}
	isOrthogonal = false;
	this->InnerProduct(v1, v1, this->_nRow, ele1);
	this->InnerProduct(v2, v2, this->_nRow, ele2);
	// 根据迭代第几次判断是大到小排序，还是反过来
	bool isNeedSwap = (this->_currentIteration % 2 == 0) ? ele1 < ele2 : ele1 > ele2;
	/*只要每次旋转前都把范数大的列放在前面，就可以保证求出的奇异值是递减排序的*/
	if(isNeedSwap) {
	    swap(this->_myMatrixIndex[i], this->_myMatrixIndex[j]);
	    swap(this->_myColumnIDs[i], this->_myColumnIDs[j]);
	    swap(v1, v2);
	    swap(ele1, ele2);
	}

	double tao = (ele1 - ele2) / (2 * ele);
	double tan = sign(tao) / (fabs(tao) + sqrt(1 + pow(tao, 2)));
	double cos = 1 / sqrt(1 + pow(tan, 2));
	double sin = cos * tan;

	for(TStatInt r=0;r<this->_nRow;++r) {
	    double var1=v1[r]*cos+v2[r]*sin;
	    double var2=v2[r]*cos-v1[r]*sin;
	    v1[r] = var1;
	    v2[r] = var2;
	}
    }

    void UseColumnSpace(TStatInt logicalColumnPostion) {
	TStatInt physicalPosition = this->_freeColumnSapces.back();
	this->_freeColumnSapces.pop_back();
	this->_myMatrixIndex[logicalColumnPostion] = physicalPosition;
    }

    void DisposeColumnSpace(TStatInt logicalColumnPostion) {
	TStatInt physicalPosition = this->_myMatrixIndex[logicalColumnPostion];
	this->_freeColumnSapces.push_back(physicalPosition);
    }

public:
    Compution(const string& matrixFileName, TStatInt maxIteration, double orthogonalThreshold, communicator& world) : 
    _matrixFileName(matrixFileName),
    _world(world),
    _orthogonalThreshold(orthogonalThreshold),
    _maxIteration(maxIteration) {
	
    }

    void WriteMyPartOfMatrixToFile() const {
	stringstream ss;
	ss << "matrix." << this->_world.rank();
	FILE *out = fopen(ss.str().c_str(), "w");
	fprintf(out, "%ld %ld\n", this->_nRow, this->_nMyCol);

	/*
	for(TStatInt i=0;i<this->_nMyCol;++i) {
	    fprintf(out, "%ld", this->_myColumnIDs[i]);
	    if(i+1<this->_nMyCol) 
		fprintf(out, "\t");
	}
	fprintf(out, "\n");
	for(TStatInt i=0;i<this->_nMyCol;++i) {
	    const double *v = this->_myPartOfMatrix[this->_myMatrixIndex[i]];
	    double ele = -1.0;
	    this->InnerProduct(v, v, this->_nRow, ele);
	    fprintf(out, "%lf", ele);
	    if(i+1<this->_nMyCol) 
		fprintf(out, " ");
	}
	fprintf(out, "\n");
	*/
	for(TStatInt j=0;j<this->_nRow;++j) {
	    for(TStatInt i=0;i<this->_nMyCol;++i) {
		fprintf(out, "%lf", this->_myPartOfMatrix[this->_myMatrixIndex[i]][j]);
		if(i+1<this->_nMyCol) {
		    fprintf(out, " ");
		}
	    }
	    fprintf(out, "\n");
	}
	fclose(out);
    }

    void WriteMyPartOfSqrtNorms() {
	stringstream ss;
	ss << "S." << this->_world.rank();
	FILE *out = fopen(ss.str().c_str(), "w");
	for(TStatInt i=0;i<this->_nMyCol;++i) {
	    double eigen = 0.0;
	    const double *v = this->_myPartOfMatrix[this->_myMatrixIndex[i]];
	    for(TStatInt j=0;j<this->_nRow;++j) {
		eigen += (v[j] * v[j]);
		//cout << "rank:" << this->_world.rank() << '\t' << eigen << endl;
	    }
	    fprintf(out, "%lf", sqrt(eigen));
	    if(i+1<this->_nMyCol) {
		fprintf(out, " ");
	    }
	}
	fprintf(out, "\n");
	fclose(out);
    }

    void InitMatrixSpace() {
	this->_safeSize = 2 * ((this->_nCol / this->_world.size()) + 1);
	this->_myPartOfMatrix = new double*[this->_safeSize];
	this->_myMatrixIndex.resize(this->_safeSize);
	this->_myColumnIDs.resize(this->_safeSize);

	for(TStatInt i=0;i<this->_safeSize;++i) { 
	    this->_myPartOfMatrix[i] = new double[this->_nRow]; 
	}

	for(TStatInt i=0;i<this->_safeSize;++i) {
	    this->_freeColumnSapces.push_back(i);
	}
    }

    void LoadMatrix() {
	ifstream fin(this->_matrixFileName.c_str(), ios::in);
	TVecS parts;
	string line;
	
	this->LoadMatrixHeader(fin);
	TStatInt myStart=-1;
	GetPartionArgs<TStatInt>(this->_nCol, this->_world.rank(), this->_world.size(), myStart, this->_nMyCol);

	this->InitMatrixSpace();
	for(TStatInt i=0,j=myStart;i<this->_nMyCol;++i,++j) {
	    this->UseColumnSpace(i);
	    this->_myColumnIDs[i] = j;
	}
	for(TStatInt currentRow=0;!fin.eof() && currentRow<this->_nRow;++currentRow) {
	    getline(fin, line);
	    if("" == line) {
		continue;
	    }
	    if(!StringToCollection(line, parts, ' ', true) || parts.size() != this->_nCol) {
		cerr << "error when load row " << currentRow << endl;
		throw;
	    }
	    for(TStatInt i=myStart,j=0;j<this->_nMyCol;++i,++j) {
		TStatInt physicalPosition = this->_myMatrixIndex[j];
		this->_myPartOfMatrix[physicalPosition][currentRow] = lexical_cast<double>(parts[i]);
	    }
	}
	fin.close();
    }

    void UpdateMyDataFromPreNode() {
	TStatInt myRank = this->_world.rank();
	if(myRank-1 < 0) {
	    return;
	}

	this->_world.send(myRank-1, 0, this->_nMyCol); //发送我有多少个列
	for(TStatInt i=0;i<this->_nMyCol;++i) {
	    TStatInt colID = this->_myColumnIDs[i];
	    this->_world.send(myRank-1, 1, colID); //发送列号
	    TStatInt physicalPosition = this->_myMatrixIndex[i];
	    this->_world.send(myRank-1, 2, this->_myPartOfMatrix[physicalPosition], this->_nRow); //发送列数据
	    this->DisposeColumnSpace(i);
	}
	this->_world.recv(myRank-1, 0, this->_nMyCol); //接收有多少个列要发过来
	for(TStatInt i=0;i<this->_nMyCol;++i) {
	    this->_world.recv(myRank-1, 1, this->_myColumnIDs[i]); //接收列号
	    this->UseColumnSpace(i);
	    TStatInt physicalPosition = this->_myMatrixIndex[i];
	    this->_world.recv(myRank-1, 2, this->_myPartOfMatrix[physicalPosition], this->_nRow); //接收列数据
	}
    }
    
    void OrthogonalizeMyColumns() {
	bool isOneOrthogonal;
	TStatInt sweepDirection = 0;
	for(TStatInt i=0,l=this->_nMyCol-1;i<l;++i) {
	    for(TStatInt j=i+1;j<this->_nMyCol;++j) {
		this->OrthogonalizeAndMaySwitchTwoColumn(i, j, isOneOrthogonal);
		//this->_isAllOrthogonal = (this->_isAllOrthogonal && isOneOrthogonal);
	    }
	}
    }

    void CompareTwoColumnByNorm(TStatInt i, TStatInt j) {
	if(i>j) {
	    swap(i, j);
	}
	double *v1 = this->_myPartOfMatrix[this->_myMatrixIndex[i]], *v2 = this->_myPartOfMatrix[this->_myMatrixIndex[j]];
	double ele1, ele2;
	this->InnerProduct(v1, v1, this->_nRow, ele1);
	this->InnerProduct(v2, v2, this->_nRow, ele2);
	if(ele1 < ele2) {
	    swap(this->_myMatrixIndex[i], this->_myMatrixIndex[j]);
	    swap(this->_myColumnIDs[i], this->_myColumnIDs[j]);
	}
    }

    void SortMyColumnsByNorm() {
	for(TStatInt i=0,l=this->_nMyCol-1;i<l;++i) {
	    for(TStatInt j=i+1;j<this->_nMyCol;++j) {
		this->CompareTwoColumnByNorm(i, j);
		//this->_isAllOrthogonal = (this->_isAllOrthogonal && isOneOrthogonal);
	    }
	}
    }

    void PrintMyColumnInfos(TStatInt iter) {
	stringstream ss;
	ss << "oe iter:" << iter << '\t' << "rank:" << this->_world.rank() << '\t' ;
	for(TStatInt j=0;j<this->_nMyCol;++j) {
	    ss << this->_myColumnIDs[j] << ':';
	}
	ss << endl;
	for(TStatInt i=0;i<this->_nRow;++i) {
	    for(TStatInt j=0;j<this->_nMyCol;++j) {
		const double *v = this->_myPartOfMatrix[this->_myMatrixIndex[j]];
		ss << v[i] << ' ';
	    }
	    ss << endl;
	}
	ss << endl;
	cout << ss.str();
    }

    void DoDataTransport(TStatInt iter) {
	TStatInt myRank = this->_world.rank();
	if(myRank+1 == this->_world.size()) { //最右边的节点
	    if(this->_world.rank() == 0) {
		this->PrintMyColumnInfos(iter);
	    }
	    return;
	}
	TStatInt nNextNodeCol = -1;
	this->_world.recv(myRank+1, 0, nNextNodeCol); //接收对方有多少列
	TStatInt newColPosition = this->_nMyCol, newColPhysicalPosition=0;
	for(TStatInt i=0;i<nNextNodeCol;++i,++newColPosition) {
	    TStatInt colID = -1;
	    this->_world.recv(myRank+1, 1, colID); //接收列号
	    this->_myColumnIDs[newColPosition] = colID;
	    this->UseColumnSpace(newColPosition);
	    TStatInt physicalPosition = this->_myMatrixIndex[newColPosition];
	    this->_world.recv(myRank+1, 2, this->_myPartOfMatrix[physicalPosition], this->_nRow); //接收列数据
	}
	this->_nMyCol = newColPosition;
	// 到这个时候，本节点已经拥有两组列了

	// 是时候对本节点拥有的列做证交化了
	this->OrthogonalizeMyColumns();
	/*
	if(this->_world.rank() == 0) {
	    this->PrintMyColumnInfos(iter);
	}
	this->SortMyColumnsByNorm();
	if(this->_world.rank() == 0) {
	    this->PrintMyColumnInfos(iter);
	}
	*/

	// 到这个时候，本节点的两组列已经做好正交化处理，并且按模长从大到小排序
	nNextNodeCol = (this->_nMyCol / 2) + (this->_nMyCol % 2);
	this->_world.send(myRank+1, 0, nNextNodeCol);
	for(TStatInt i=(this->_nMyCol / 2),j=0;j<nNextNodeCol;++i,++j) {
	    this->_world.send(myRank+1, 1, this->_myColumnIDs[i]);
	    this->_world.send(myRank+1, 2, this->_myPartOfMatrix[this->_myMatrixIndex[i]], this->_nRow);
	    this->DisposeColumnSpace(i);
	}
	this->_nMyCol -= nNextNodeCol; //这一步确保本节点处理的只是自己那一半的列了
    }

    void DoParallelOneSideJacobi() {
	TStatInt myRank = this->_world.rank();
	if(myRank % 2 == 1) {
	    this->UpdateMyDataFromPreNode();
	}
	for(TStatInt i=(myRank % 2);i<this->_world.size()-1;i+=2) {
	    this->DoDataTransport(i);
	    cout << "rank " << this->_world.rank() << '\t';
	    cout << "iteration " << this->_currentIteration << '\t';
	    cout << "oe transport round:" << i << endl;
	    //条件判断当前的奇（偶）轮迭代是不是最后一轮迭代，不是的话还得把自己的数据给pre node更新
	    if((i+1) == this->_world.size() - 1) { break; }
	    this->UpdateMyDataFromPreNode();
	}
    }

    void DoSVD() {
	for(this->_currentIteration=0;this->_currentIteration<this->_maxIteration;++this->_currentIteration) {
	    this->DoParallelOneSideJacobi();
	    //cout << "iteration:" << i << endl;
	}
    }
};

int main(int argc, char *argv[]) {
    COUNT_TIME_START(global);
    environment env(argc, argv);
    communicator world;
    string matrixFileName = string(argv[1]);      
    TStatInt maxInteration = lexical_cast<TStatInt>(string(argv[2]));
    double orthogonalThreshold = lexical_cast<double>(string(argv[3]));
    //if(world.rank() != 5) { return 0;}
    Compution compu(matrixFileName, maxInteration, orthogonalThreshold, world);
    compu.LoadMatrix();
    compu.DoSVD();
    compu.WriteMyPartOfMatrixToFile();
    compu.WriteMyPartOfSqrtNorms();

    COUNT_TIME_END(global);
    return 0;
}
