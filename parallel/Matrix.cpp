#include "Matrix.h"

namespace dof {
namespace parallel {

void __LoadMyVectors(	const string& sparseMatrixFileName, 
			TLL myRank, 
			TLL wholeSize, 
			map<TLL, SparseVector>& vectors) {

    ifstream fin(sparseMatrixFileName.c_str(), ios::in);
    string line, rowIDStr, sparseColumnsStr;
    for(TLL readLineCount=0;!fin.eof();++readLineCount) {
	getline(fin, line);
	if("" == line) { continue; }
	if(readLineCount % wholeSize != myRank) { continue; }

	if(!Split(line, rowIDStr, sparseColumnsStr, '\t')) {
	    cerr << "cannot parse sparse row:" << line << endl;
	    throw;
	}
	SparseVector& newSparseVector = vectors[lexical_cast<TLL>(rowIDStr)];
	newSparseVector.Clear();
	SparseVector::CreateFromText(sparseColumnsStr, '+', '^', newSparseVector); 
    }
    fin.close();
}

void __DoMultiply(const TVecStr& lines, const map<TLL, SparseVector>& vectors, SparseMatrix& matrix, double lowLimit) {
    string rowIDStr, sparseColumnsStr;
    for(TVecStr::const_iterator it0=lines.begin();it0!=lines.end();++it0) {
	if(!Split(*it0, rowIDStr, sparseColumnsStr, '\t')) {
	    cerr << "cannot parse sparse row:" << *it0 << endl;
	    throw;
	}
	TLL colID = lexical_cast<TLL>(rowIDStr); // 行号转置后成为列号
	SparseVector sv0(sparseColumnsStr, '+', '^');
	for(map<TLL, SparseVector>::const_iterator it0=vectors.begin();it0!=vectors.end();++it0) {
	    TLL rowID = it0->first;
	    const SparseVector& sv1 = it0->second;
	    double dotProduct = sv0 * sv1;
	    if(dotProduct > lowLimit) {
		matrix.Set(rowID, colID, sv0 * sv1);
	    }
	}
    }
}

void __SparseVectorsMultiplySparseMatrixInFile(	const string& sparseMatrixFileName, 
						const map<TLL, SparseVector>& vectors, 
						SparseMatrix& matrix,
						double lowLimit) {
    
    ifstream fin(sparseMatrixFileName.c_str(), ios::in);
    string line, rowIDStr, sparseColumnsStr;
    boost::mpi::communicator world;
    TVecStr buffer;
    for(TLL readLineCount=0;!fin.eof();++readLineCount) {
	getline(fin, line);
	if("" == line) { continue; }
	buffer.push_back(line);
	if(readLineCount % 1000 == 0) {
	    __DoMultiply(buffer, vectors, matrix, lowLimit);
	    buffer.clear();
	    cout << "read line:" << readLineCount << ", rank:" << world.rank() << endl;
	}
    }
    fin.close();
    __DoMultiply(buffer, vectors, matrix, lowLimit);
    buffer.clear();
}

void SparseMatrixTransposedMultiplication(const string& sparseMatrixFileName, SparseMatrix& matrix, double lowLimit) {
    matrix.Clear();
    boost::mpi::communicator world;
    map<TLL, SparseVector> myVectors;
    __LoadMyVectors(sparseMatrixFileName, world.rank(), world.size(), myVectors);
    __SparseVectorsMultiplySparseMatrixInFile(sparseMatrixFileName, myVectors, matrix, lowLimit);
}

} // namespace parallel
} // namespace dof
