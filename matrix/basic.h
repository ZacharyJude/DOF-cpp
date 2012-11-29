#ifndef DOF_MATRIX_BASIC_H_
#define DOF_MATRIX_BASIC_H_

#include "CommonHeader.h"
#include "StringLib.h"

namespace dof {
namespace matrix {

typedef class SparseMatrix {
public:
    TMapLLVecPLLDB _store;
} SparseMatrix;

typedef class NormalMatrix {
public:
    double **_store;
    TLL _nRow, _nCol;

    NormalMatrix(TLL nRow, TLL nCol): _nRow(nRow), _nCol(nCol) {
	this->_store = new double*[nRow];
	for(TLL i=0;i<nRow;++i) { 
	    this->_store[i] = new double[nCol]; 
	    for(TLL j=0;j<nCol;++j) {
		this->_store[i][j] = 0.0;
	    }
	}
    }

    inline double Get(TLL r, TLL c) const {
	if(r >= this->_nRow || c >= this->_nCol) {
	    cerr << "invalid row column:" << r << '\t' << c << endl;
	    throw;
	}
	return this->_store[r][c];
    }

    inline TLL Rows() const { return this->_nRow; }
    inline TLL Cols() const { return this->_nCol; }

    ~NormalMatrix() {
	for(TLL i=0;i<this->_nRow;++i) {
	    delete[] this->_store[i];
	}
	delete[] this->_store;
    }
} NormalMatrix;

inline ostream& operator<<(ostream& os, const NormalMatrix& matrix) {
    for(TLL i=0;i<matrix.Rows();++i) {
	for(TLL j=0;j<matrix.Cols();++j) {
	    os << matrix.Get(i, j) << ' ';    
	}
	os << endl;
    }
    return os;
}

void LoadTextSparseMatrixFromFile(const string& fileName, SparseMatrix& matrix, TLL rowStart=0, TLL colStart=0, TLL readRow=-1, TLL readCol=-1) {
    ifstream fin(fileName.c_str(), ios::in);
    string line;
    TVecS parts0, parts1, parts2;
    TLL rowLimit = (readRow < 0) ? -1 : rowStart + readRow - 1;
    TLL colLimit = (readCol < 0) ? -1 : colStart + readCol - 1;
    while(!fin.eof()) {
	getline(fin, line);
	if("" == line) {
	    continue;
	} 
	if(!StringToCollection(line, parts0, '\t', true) || parts0.size() != 2) {
	    cerr << "invalid line:" << line << endl;
	    throw;
	}
	if(!StringToCollection(parts0[1], parts1, '+', true)) {
	    cerr << "invalid line:" << line << endl;
	    throw;
	}
	TLL rowID = lexical_cast<TLL>(parts0[0]);
	if( rowID < rowStart || rowID > rowLimit ) {
	    continue;
	}
	TVecPLLDB& row = matrix._store[rowID];
	for(TVecS::const_iterator it0=parts1.begin();it0!=parts1.end();++it0) {
	    if(!StringToCollection(*it0, parts2, '^', true) || parts2.size() != 2) {
		cerr << "invalid line:" << line << endl;
		throw;
	    }
	    TLL colID = lexical_cast<TLL>(parts2[0]);
	    if( colID < colStart || colID > colLimit ) {
		break;
	    }
	    double value = lexical_cast<double>(parts2[1]);
	    row.push_back(make_pair(colID, value));
	}
    }
    fin.close();
}

void Sparse2Normal(const SparseMatrix& sparse, NormalMatrix& normal, TLL rowOffset=0, TLL colOffset=0) {
    const TMapLLVecPLLDB& sparseStore = sparse._store;
    double **normalStore = normal._store;
    for(TMapLLVecPLLDB::const_iterator it0=sparseStore.begin();it0!=sparseStore.end();++it0) {
	TLL rowID = it0->first;
	TLL offsetRowID = rowID - rowOffset;
	const TVecPLLDB& sparseRow = it0->second;
	if(offsetRowID+1 > normal.Rows()) {
	    cerr << "sparse matrix row cannot fit into the normal rowID:" << rowID << endl;
	    throw;
	}
	double *normalRow = normalStore[offsetRowID];
	for(TVecPLLDB::const_iterator it1=sparseRow.begin();it1!=sparseRow.end();++it1) {
	    TLL colID = it1->first;
	    TLL offsetColID = colID - colOffset;
	    if(offsetColID+1 > normal.Cols()) {
		cerr << "sparse matrix column cannot fit into the normal colID:" << colID << endl;
		throw;
	    }
	    double value = it1->second;
	    normalRow[offsetColID] = value;
	}
    }
}

} // namespace matrix
} // namespace dof

#endif // DOF_MATRIX_BASIC_H_
