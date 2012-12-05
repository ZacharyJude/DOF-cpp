#include <boost/mpi/communicator.hpp>
#include <boost/mpi/collectives.hpp>
#include <boost/mpi/environment.hpp>
#include "basic/CommonHeader.h"
#include "basic/StringLib.h"
#include "matrix/concept.h"
#include "parallel/ParallelUtil.h"
using namespace dof;
using namespace dof::matrix;
using namespace dof::parallel;
using namespace boost::mpi;

class ParallelSVD {
private:
    TLL _nRow, _nCol, _maxIteration, _safeSize, _nMyCol, _currentIteration;
    communicator& _world;
    double _orthogonalThreshold;
    VectorGroup *_myColumns, *_columnsV, *_columnsU;

    string _matrixFileName;

private:
    void LoadMatrixHeader(ifstream& fin) {
	string line;
	TVecStr parts;
	getline(fin, line);
	if(!StringToCollection(line, parts, ' ', true)) {
	    cerr << "invalid matrix header:" << line << endl;
	    throw;
	}
	this->_nRow = lexical_cast<TLL>(parts[0]);
	this->_nCol = lexical_cast<TLL>(parts[1]);
    }

    void OrthogonalizeMyColumns() {
	bool isDescending = this->_currentIteration % 2 == 0; 
	TVecLL columnPositions;

	this->_myColumns->GetSortedPositions(inserter(columnPositions, columnPositions.end()));
	for(TLL m=0;m<columnPositions.size();++m) {
	    for(TLL n=m+1;n<columnPositions.size();++n) {
		TLL i=columnPositions[m], j=columnPositions[n];
		if(IsOrthogonal(this->_myColumns->GetVector(i), this->_myColumns->GetVector(j), this->_nRow, this->_orthogonalThreshold)) {
		    continue;
		}
		double ele0, ele1, ele2;
		double *v0=this->_myColumns->GetStorePointer(i), *v1=this->_myColumns->GetStorePointer(j);
		double *v2=this->_columnsV->GetStorePointer(i), *v3=this->_columnsV->GetStorePointer(j);
		InnerProduct(v0, v1, this->_nRow, ele0);
		InnerProduct(v0, v0, this->_nRow, ele1);
		InnerProduct(v1, v1, this->_nRow, ele2);
		// 根据迭代第几次判断是大到小排序，还是反过来
		bool isNeedSwap = (isDescending) ? ele1 < ele2 : ele1 > ele2;
		if(isNeedSwap) {
		    this->_myColumns->SwapTwoVector(i, j); 
		    this->_columnsV->SwapTwoVector(i, j);
		    swap(v0, v1);
		    swap(v2, v3);
		    swap(ele1, ele2);
		}
		double cos, sin;
		SymSchur2(ele0, ele1, ele2, cos, sin);
		Rotate(v0, v1, cos, sin, this->_nRow);
		Rotate(v2, v3, cos, sin, this->_nRow);
	    }
	}
    }

    void DoDataTransport(TLL iter) {
	TLL myRank = this->_world.rank();
	if(myRank+1 == this->_world.size()) { //最右边的节点
	    return;
	}
	TLL nNextNodeCol = -1;
	this->_world.recv(myRank+1, 0, nNextNodeCol); //接收对方有多少列
	TLL newColPosition = this->_nMyCol;
	for(TLL i=0;i<nNextNodeCol;++i,++newColPosition) {
	    TLL colID = -1;
	    this->_world.recv(myRank+1, 1, colID); //接收列号
	    this->_myColumns->AddVector(newColPosition, colID);
	    this->_world.recv(myRank+1, 2, this->_myColumns->GetStorePointer(newColPosition), this->_nRow); 

	    this->_world.recv(myRank+1, 3, colID);
	    this->_columnsV->AddVector(newColPosition, colID);
	    this->_world.recv(myRank+1, 4, this->_columnsV->GetStorePointer(newColPosition), this->_nRow);
	}
	this->_nMyCol = newColPosition;
	// 到这个时候，本节点已经拥有两组列了

	// 是时候对本节点拥有的列做证交化了
	this->OrthogonalizeMyColumns();

	// 到这个时候，本节点的两组列已经做好正交化处理，并且按模长从大到小排序
	nNextNodeCol = (this->_nMyCol / 2) + (this->_nMyCol % 2);
	this->_world.send(myRank+1, 0, nNextNodeCol);
	for(TLL i=(this->_nMyCol / 2),j=0;j<nNextNodeCol;++i,++j) {
	    this->_world.send(myRank+1, 1, this->_myColumns->GetVectorID(i));
	    this->_world.send(myRank+1, 2, this->_myColumns->GetVector(i), this->_nRow);
	    this->_myColumns->RemoveVector(i);

	    this->_world.send(myRank+1, 3, this->_columnsV->GetVectorID(i));
	    this->_world.send(myRank+1, 4, this->_columnsV->GetVector(i), this->_nRow);
	    this->_columnsV->RemoveVector(i);
	}
	this->_nMyCol -= nNextNodeCol; //这一步确保本节点处理的只是自己那一半的列了
    }

    void UpdateMyDataFromPreNode() {
	TLL myRank = this->_world.rank();
	if(myRank-1 < 0) {
	    return;
	}

	this->_world.send(myRank-1, 0, this->_nMyCol); //发送我有多少个列
	for(TLL i=0;i<this->_nMyCol;++i) {
	    this->_world.send(myRank-1, 1, this->_myColumns->GetVectorID(i)); //发送列号
	    this->_world.send(myRank-1, 2, this->_myColumns->GetVector(i), this->_nRow); //发送列数据
	    this->_myColumns->RemoveVector(i);
	    this->_world.send(myRank-1, 3, this->_columnsV->GetVectorID(i));
	    this->_world.send(myRank-1, 4, this->_columnsV->GetVector(i), this->_nRow);
	    this->_columnsV->RemoveVector(i);
	}
	this->_world.recv(myRank-1, 0, this->_nMyCol); //接收有多少个列要发过来
	for(TLL i=0;i<this->_nMyCol;++i) {
	    TLL colID = -1;
	    this->_world.recv(myRank-1, 1, colID); //接收列号
	    this->_myColumns->AddVector(i, colID);
	    this->_world.recv(myRank-1, 2, this->_myColumns->GetStorePointer(i), this->_nRow); //接收列数据
	    this->_world.recv(myRank-1, 3, colID);
	    this->_columnsV->AddVector(i, colID);
	    this->_world.recv(myRank-1, 4, this->_columnsV->GetStorePointer(i), this->_nRow);
	}
    }

    void DoParallelOneSideJacobi() {
	TLL myRank = this->_world.rank();
	if(myRank % 2 == 1) {
	    this->UpdateMyDataFromPreNode();
	}
	for(TLL i=(myRank % 2);i<this->_world.size()-1;i+=2) {
	    this->DoDataTransport(i);
	    //条件判断当前的奇（偶）轮迭代是不是最后一轮迭代，不是的话还得把自己的数据给pre node更新
	    if((i+1) == this->_world.size() - 1) { break; }
	    this->UpdateMyDataFromPreNode();
	    //cout << "rank " << this->_world.rank() << '\t';
	    //cout << "iteration " << this->_currentIteration << '\t';
	    //cout << "oe transport round:" << i << endl;
	}
    }

    /*
    void CompareTwoColumnByNorm(TLL i, TLL j) {
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
	for(TLL i=0,l=this->_nMyCol-1;i<l;++i) {
	    for(TLL j=i+1;j<this->_nMyCol;++j) {
		this->CompareTwoColumnByNorm(i, j);
		//this->_isAllOrthogonal = (this->_isAllOrthogonal && isOneOrthogonal);
	    }
	}
    }
    */

public:
    ParallelSVD(const string& matrixFileName, TLL maxIteration, double orthogonalThreshold, communicator& world) : 
    _matrixFileName(matrixFileName), 
    _maxIteration(maxIteration), 
    _orthogonalThreshold(orthogonalThreshold),
    _world(world),
    _currentIteration(0),
    _myColumns(NULL)
    {
    }

    ~ParallelSVD() {
	if(NULL != this->_myColumns) {
	    delete this->_myColumns;
	}
	if(NULL != this->_columnsV) {
	    delete this->_columnsV;
	}
	if(NULL != this->_columnsU) {
	    delete this->_columnsU;
	}
    }

    void LoadMatrix() {
	ifstream fin(this->_matrixFileName.c_str(), ios::in);
	TVecStr parts;
	string line;
	
	this->LoadMatrixHeader(fin);
	TLL myStart=-1;
	GetPartionArgs<TLL>(this->_nCol, this->_world.rank(), this->_world.size(), myStart, this->_nMyCol);
	this->_safeSize = 2 * ((this->_nCol / this->_world.size()) + 1);
	this->_myColumns = new VectorGroup(this->_nRow, this->_safeSize);
	this->_columnsV = new VectorGroup(this->_nRow, this->_safeSize);
	this->_columnsU = new VectorGroup(this->_nRow, this->_safeSize);
	for(TLL i=0,j=myStart;i<this->_nMyCol;++i,++j) {
	    this->_myColumns->AddVector(i, j);
	    this->_columnsV->AddVector(i, j);
	}
	for(TLL currentRow=0;!fin.eof() && currentRow<this->_nRow;++currentRow) {
	    getline(fin, line);
	    if("" == line) {
		continue;
	    }
	    if(!StringToCollection(line, parts, ' ', true) || parts.size() != this->_nCol) {
		cerr << "error when load row " << currentRow << endl;
		throw;
	    }
	    for(TLL i=myStart,j=0;j<this->_nMyCol;++i,++j) {
		this->_myColumns->GetStorePointer(j)[currentRow] = lexical_cast<double>(parts[i]);
		if(i == currentRow) {
		    this->_columnsV->SetVectorComponent(j, currentRow, 1.0);
		}
	    }
	}
	fin.close();
    }

    void Compute() {
	for(this->_currentIteration=0;this->_currentIteration<this->_maxIteration;++this->_currentIteration) {
	    this->DoParallelOneSideJacobi();
	}

	for(TLL i=0;i<this->_nMyCol;++i) {
	    this->_columnsU->AddVector(i, i);
	}
	TVecLL columnPositions;
	this->_myColumns->GetSortedPositions(inserter(columnPositions, columnPositions.end()));
	for(TLL m=0;m<columnPositions.size();++m) {
	    TLL i=columnPositions[m];
	    double norm = 0.0;
	    GetNorm(this->_myColumns->GetVector(i), this->_nRow, norm);
	    for(TLL j=0;j<this->_nRow;++j) {
		double u = (this->_myColumns->GetVectorComponent(i, j) / norm);
		this->_columnsU->SetVectorComponent(m, j, u);
	    }
	}
    }

    void WriteMyPartOfMatrixToFile() const {
	stringstream ss;
	ss << "matrix." << this->_world.rank();
	this->_myColumns->WriteToFileColumnForm(ss.str());

	ss.str("");
	ss << "V." << this->_world.rank();
	this->_columnsV->WriteToFileColumnForm(ss.str());

	ss.str("");
	ss << "U." << this->_world.rank();
	this->_columnsU->WriteToFileColumnForm(ss.str());
    }

    void WriteMyPartOfSqrtNorms() {
	stringstream ss;
	ss << "S." << this->_world.rank();
	this->_myColumns->WriteToFileVectorNorms(ss.str());
    }
};

int main(int argc, char *argv[]) {
    COUNT_TIME_START(global);
    environment env(argc, argv);
    communicator world;
    string matrixFileName = string(argv[1]);      
    TLL maxInteration = lexical_cast<TLL>(string(argv[2]));
    double orthogonalThreshold = lexical_cast<double>(string(argv[3]));
    //if(world.rank() != 5) { return 0;}
    ParallelSVD svd(matrixFileName, maxInteration, orthogonalThreshold, world);
    svd.LoadMatrix();
    svd.Compute();
    svd.WriteMyPartOfMatrixToFile();
    svd.WriteMyPartOfSqrtNorms();

    COUNT_TIME_END(global);
    return 0;
}
