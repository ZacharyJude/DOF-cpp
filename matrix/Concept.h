#ifndef DOF_MATRIX_CONCEPT_H_
#define DOF_MATRIX_CONCEPT_H_

#include "basic/CommonHeader.h"
#include "basic/StringLib.h"

using namespace dof::lib::str;

namespace dof {
namespace matrix {

class SparseVector;
class SparseMatrix;
class NormalMatrix;

typedef class SparseVector {
private:
    TMapLLDB _store;

public:
    static bool CreateFromText(const string& line, char sep, char kvSep, SparseVector& sv) {
	static TVecStr __parts0;
	if(!StringToCollection(line, __parts0, sep, true)) {
	    //cerr << "cannot create sparse vector from text:" << line << endl;
	    return false;
	}
	string nth, value;
	for(TVecStr::const_iterator it0=__parts0.begin();it0!=__parts0.end();++it0) {
	    if(!Split(*it0, nth, value, kvSep)) {
		//cerr << "cannot split:" << *it0 << " using sep:" << kvSep << endl;
		sv.Clear();
		return false;
	    }
	    sv.Set(lexical_cast<TLL>(nth), lexical_cast<double>(value));
	}

	return true;
    }

    friend double operator* (const SparseVector&, const SparseVector&);

public:
    SparseVector();
    SparseVector(const string& line, char sep, char kvSep);

    void Set(TLL nth, double value);
    void Clear();

} SparseVector;

double operator* (const SparseVector& sv0, const SparseVector& sv1);

typedef class SparseMatrix {
private:
    TMapLLVecPLLDB _store;

public:
    typedef vector< pair<TLL, double> > TRow;
    typedef function<void (TLL row, TLL col, double weight)> FConstCellVisitor;
    static TRow& Empty() {
	static TRow __empty;
	return __empty;
    }

    TLL Rows() const;
    void Set(TLL r, TLL c, double value);
    void Clear();
    TRow& GetRow(TLL r);
    void WriteToTextFileRowForm(const string& fileName) const;
    void LoadFromTextFileRowForm(const string& fileName);
    void LoadTextSparseMatrixFromFile(const string& fileName, TLL rowStart=0, TLL colStart=0, TLL readRow=-1, TLL readCol=-1, bool isReset=true);
    void ToNormal(NormalMatrix& normal, TLL rowOffset=0, TLL colOffset=0) const;
    void VisitConst(FConstCellVisitor visitor) const;

} SparseMatrix;

typedef class NormalMatrix {
public:
    double **_store;
    TLL _nRow, _nCol;

    NormalMatrix(TLL nRow, TLL nCol);
    inline double Get(TLL r, TLL c) const {
	if(r >= this->_nRow || c >= this->_nCol) {
	    cerr << "invalid row column:" << r << '\t' << c << endl;
	    throw;
	}
	return this->_store[r][c];
    }
    inline TLL Rows() const { return this->_nRow; }
    inline TLL Cols() const { return this->_nCol; }

    ~NormalMatrix();
} NormalMatrix;

inline int sign(double number) {
    if (number < 0)
	return -1;
    else
	return 1;
}

void InnerProduct(const double *v, const double *u, TLL len, double &innerProduce);
void SymSchur2(double ele0, double ele1, double ele2, double& cos, double& sin);
void Rotate(double *v0, double *v1, double cos, double sin, TLL len);
bool IsOrthogonal(const double *v0, const double *v1, TLL len, double threshold);
inline void GetNorm(const double *v, TLL len, double &norm) {
    InnerProduct(v, v, len, norm);
    norm = sqrt(norm);
}

typedef class VectorGroup {
private:
    double **_store;
    TVecLL _storeIndex, _vectorIDs, _freeStoreIndex;
    TLL _length, _nVectors, _capacity;
public:

    VectorGroup(TLL length, TLL capacity);
    ~VectorGroup();

    void SwapTwoVector(TLL i, TLL j);
    void AddVector(TLL groupPosition, TLL vectorID = -1);
    void RemoveVector(TLL groupPosition);
    template<typename TOutputIterator>
    void GetSortedPositions(TOutputIterator out) const {
	for(TLL i=0;i<this->_capacity;++i) {
	    if(this->_storeIndex[i] == -1) {
		continue;
	    }
	    *out = i;
	}
    }

    void SetVectorComponent(TLL groupPosition, TLL nth, double value);
    double GetVectorComponent(TLL groupPosition, TLL nth) const;
    bool HasVector(TLL groupPosition) const;
    void WriteToFileColumnForm(const string& fileName, bool withHeader=true, bool withID=false) const;
    void WriteToFileRowForm(const string& fileName, bool withHeader=true, bool withID=false) const;
    void WriteToFileVectorNorms(const string& fileName, bool withHeader=true, bool withID=false) const;
    const double* GetVector(TLL groupPosition) const;
    TLL GetVectorID(TLL groupPosition) const;
    TLL GetGroupSize() const;
    TLL GetGroupCapacity() const;
    double* GetStorePointer(TLL groupPosition);

} VectorGroup;

inline ostream& operator<<(ostream& os, const NormalMatrix& matrix) {
    for(TLL i=0;i<matrix.Rows();++i) {
	for(TLL j=0;j<matrix.Cols();++j) {
	    os << matrix.Get(i, j);
	    if(j+1 < matrix.Cols()) {
		os << ' ';    
	    }
	}
	os << endl;
    }
    return os;
}

} // namespace matrix
} // namespace dof

#endif // DOF_MATRIX_CONCEPT_H_
