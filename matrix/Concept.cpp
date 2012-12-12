#include "Concept.h"

namespace dof {
namespace matrix {

void InnerProduct(const double *v, const double *u, TLL len, double &innerProduce) {
    innerProduce = 0.0;
    for(TLL i=0;i<len;++i) {
	innerProduce += v[i] * u[i];
    }
}

void SymSchur2(double ele0, double ele1, double ele2, double& cos, double& sin) {
    double tao = (ele1 - ele2) / (2 * ele0);
    double tan = sign(tao) / (fabs(tao) + sqrt(1 + pow(tao, 2)));
    cos = 1 / sqrt(1 + pow(tan, 2));
    sin = cos * tan;
}

void Rotate(double *v0, double *v1, double cos, double sin, TLL len) {
    for(TLL r=0;r<len;++r) {
	double var0=v0[r]*cos+v1[r]*sin;
	double var1=v1[r]*cos-v0[r]*sin;
	v0[r] = var0;
	v1[r] = var1;
    }
}

bool IsOrthogonal(const double *v0, const double *v1, TLL len, double threshold) {
    double ele;
    InnerProduct(v0, v1, len, ele);
    if(fabs(ele) < threshold) { //i,j两列已经正交
	return true;
    }

    return false;
}

/*------------------- class SparseVector -------------------*/

SparseVector::SparseVector() { }
SparseVector::SparseVector(const string& line, char sep, char kvSep) {
    if(!CreateFromText(line, sep, kvSep, *this)) {
	cerr << "cannot create sparse vector, sep:" << sep << ", kvSep:" << kvSep << ", text:" << line << endl;
	throw;
    }
}

void SparseVector::Set(TLL nth, double value) {
    this->_store[nth] = value;
}

void SparseVector::Clear() { 
    this->_store.clear();
}

double operator* (const SparseVector& sv0, const SparseVector& sv1) {
    double ret = 0.0;
    for(TMapLLDB::const_iterator it0=sv0._store.begin();it0!=sv0._store.end();++it0) {
	TLL nth0 = it0->first;
	TMapLLDB::const_iterator find = sv1._store.find(nth0);
	if(find != sv1._store.end()) {
	    ret += (it0->second * find->second);
	}
    }
    return ret;
}


/*------------------- class SparseMatrix -------------------*/

TLL SparseMatrix::Rows() const {
    return this->_store.size();
}

void SparseMatrix::Set(TLL r, TLL c, double value) {
    this->_store[r].push_back(make_pair(c, value));
}

void SparseMatrix::Clear() {
    this->_store.clear();
}

SparseMatrix::TRow& SparseMatrix::GetRow(TLL r) {
    TMapLLVecPLLDB::const_iterator findRow = this->_store.find(r);
    if(findRow == this->_store.end()) {
	return Empty();
    }
    return this->_store[r];
}

void SparseMatrix::WriteToTextFileRowForm(const string& fileName) const {
    FILE *out = fopen(fileName.c_str(), "w");
    if(NULL == out) {
	cerr << "cannot open file:" << fileName << ", writing sparse matrix to file fails."  << endl;
	return;
    }
    for(TMapLLVecPLLDB::const_iterator it0=this->_store.begin();it0!=this->_store.end();++it0) {
	const TVecPLLDB& row = it0->second;
	TLL rowID = it0->first;
	fprintf(out, "%ld\t", rowID);
	for(TLL i=0,l=row.size();i<l;++i) {
	    const TPairLLDB& p = row[i];
	    TLL column = p.first;
	    double weight = p.second;
	    fprintf(out, "%ld^%lf", column, weight);
	    if(i+1 < l) {
		fprintf(out, "+");
	    }
	}
	fprintf(out, "\n");
    }
    fclose(out);
}

void SparseMatrix::LoadFromTextFileRowForm(const string& fileName) {
    ifstream fin(fileName.c_str(), ios::in);
    string line;
    TVecStr parts0, parts1, parts2;
    while(!fin.eof()) {
	getline(fin, line);
	if("" == line) {
	    continue;
	}
	string rowIDStr, columnsStr;
	if(!Split(line, rowIDStr, columnsStr, '\t')) {
	    cerr << "error while loading data line:" << line << endl;
	    throw;
	}
	if(!StringToCollection(columnsStr, parts1, '+', true)) {
	    cerr << "error while parsing columns:" << columnsStr << endl;
	    throw;
	}
	string columnIDStr, weightStr;
	for(TVecStr::const_iterator it0=parts1.begin();it0!=parts1.end();++it0) {
	    if(!Split(*it0, columnIDStr, weightStr, '^')) {
		cerr << "error while parsing column-weight:" << *it0 << endl;
		throw;
	    }
	    this->Set(lexical_cast<TLL>(rowIDStr), lexical_cast<TLL>(columnIDStr), lexical_cast<double>(weightStr));
	}
    }
    fin.close();
}

void SparseMatrix::LoadTextSparseMatrixFromFile(const string& fileName, TLL rowStart, TLL colStart, TLL readRow, TLL readCol, bool isReset) {
    if(isReset) {
	this->Clear();
    }

    ifstream fin(fileName.c_str(), ios::in);
    string line;
    TVecStr parts0, parts1, parts2;
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
	TVecPLLDB& row = this->_store[rowID];
	for(TVecStr::const_iterator it0=parts1.begin();it0!=parts1.end();++it0) {
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

void SparseMatrix::ToNormal(NormalMatrix& normal, TLL rowOffset, TLL colOffset) const {
    const TMapLLVecPLLDB& sparseStore = this->_store;
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

void SparseMatrix::VisitConst(FConstCellVisitor visitor) const {
    TLL rowID, colID;
    double weight;
    for(TMapLLVecPLLDB::const_iterator it0=this->_store.begin();it0!=this->_store.end();++it0) {
	const TVecPLLDB& row = it0->second;
	rowID = it0->first;
	for(TVecPLLDB::const_iterator it1=row.begin();it1!=row.end();++it1) {
	    colID = it1->first;
	    weight = it1->second;
	    visitor(rowID, colID, weight);
	}
    }
}

/*------------------- class NormalMatrix -------------------*/

NormalMatrix::NormalMatrix(TLL nRow, TLL nCol): _nRow(nRow), _nCol(nCol) {
    this->_store = new double*[nRow];
    for(TLL i=0;i<nRow;++i) { 
	this->_store[i] = new double[nCol]; 
	for(TLL j=0;j<nCol;++j) {
	    this->_store[i][j] = 0.0;
	}
    }
}

NormalMatrix::~NormalMatrix() {
    for(TLL i=0;i<this->_nRow;++i) {
	delete[] this->_store[i];
    }
    delete[] this->_store;
}   

/*------------------- class VectorGroup -------------------*/

VectorGroup::VectorGroup(TLL length, TLL capacity)
: _length(length), _nVectors(0), _capacity(capacity) 
{
    this->_store = new double*[this->_capacity];
    for(TLL i=0;i<this->_capacity;++i) {
	this->_store[i] = new double[this->_length];
	double *v = this->_store[i];
	for(TLL j=0;j<this->_length;++j) {
	    v[j] = 0.0;
	}
    }
    for(TLL i=0;i<this->_capacity;++i) {
	this->_freeStoreIndex.push_back(i);
	this->_storeIndex.push_back(-1);
	this->_vectorIDs.push_back(-1);
    }
}

VectorGroup::~VectorGroup() {
    for(TLL i=0;i<this->_capacity;++i) {
	delete[] this->_store[i];
    }
    delete[] this->_store;
}

void VectorGroup::SwapTwoVector(TLL i, TLL j) {
    swap(this->_storeIndex[i], this->_storeIndex[j]);
    swap(this->_vectorIDs[i], this->_vectorIDs[j]);
}

void VectorGroup::AddVector(TLL groupPosition, TLL vectorID) {
    if(this->_nVectors == this->_capacity) {
	cerr << "capacity error, not added." << endl;
	return;
    }
    if(this->_storeIndex[groupPosition] != -1) {
	cerr << "adding position " << groupPosition << " has vector, not added." << endl;
	return;
    }

    TLL freeIndex = this->_freeStoreIndex.back();
    this->_freeStoreIndex.pop_back();
    this->_storeIndex[groupPosition] = freeIndex;
    this->_vectorIDs[groupPosition] = vectorID;
    ++(this->_nVectors);
}

void VectorGroup::RemoveVector(TLL groupPosition) {
    if(this->_nVectors == 0) {
	cerr << "no vector, not removed." << endl;
	return;
    }
    if(this->_storeIndex[groupPosition] == -1) {
	cerr << "removing position " << groupPosition << " has no vector, not removed." << endl;
	return;
    }

    TLL index = this->_storeIndex[groupPosition];
    this->_freeStoreIndex.push_back(index);
    this->_storeIndex[groupPosition] = -1;
    this->_vectorIDs[groupPosition] = -1;
    --(this->_nVectors);
}

void VectorGroup::SetVectorComponent(TLL groupPosition, TLL nth, double value) {
    if(groupPosition < 0 || groupPosition >= this->_capacity || -1 == this->_storeIndex[groupPosition]) {
	cerr << "invalid position " << groupPosition << "." << endl;
	return;
    }
    if(nth < 0 || nth >= this->_length) {
	cerr << "invalid nth " << nth << "." << endl;
	return;
    }
    this->_store[this->_storeIndex[groupPosition]][nth] = value;
}

double VectorGroup::GetVectorComponent(TLL groupPosition, TLL nth) const {
    if(groupPosition < 0 || groupPosition >= this->_capacity || -1 == this->_storeIndex[groupPosition]) {
	cerr << "invalid position " << groupPosition << "." << endl;
	return 0.0;
    }
    if(nth < 0 || nth >= this->_length) {
	cerr << "invalid nth " << nth << "." << endl;
	return 0.0;
    }
    return this->_store[this->_storeIndex[groupPosition]][nth];
}

bool VectorGroup::HasVector(TLL groupPosition) const {
    return -1 == this->_storeIndex[groupPosition];
}

void VectorGroup::WriteToFileColumnForm(const string& fileName, bool withHeader, bool withID) const {
    FILE *out = fopen(fileName.c_str(), "w");
    if(NULL == out) {
	cerr << "cannot open file " << fileName << endl;
	return;
    }

    if(withHeader) {
	fprintf(out, "%ld %ld\n", this->_length, this->_nVectors);
    }
    if(withID) {
	for(TLL i=0;i<this->_nVectors;++i) {
	    fprintf(out, "%ld", this->_vectorIDs[i]);
	    if(i+1 < this->_nVectors) {
		fprintf(out, " ");
	    }
	}
	fprintf(out, "\n");
    }
    for(TLL i=0;i<this->_length;++i) {
	for(TLL j=0;j<this->_capacity;++j) {
	    if(this->_storeIndex[j] == -1) {
		continue;
	    }
	    fprintf(out, "%lf", this->GetVectorComponent(j, i));
	    if(j+1 < this->_nVectors) {
		fprintf(out, " ");
	    }
	}
	fprintf(out, "\n");
    }
    fclose(out);
}

void VectorGroup::WriteToFileRowForm(const string& fileName, bool withHeader, bool withID) const {
    FILE *out = fopen(fileName.c_str(), "w");
    if(NULL == out) {
	cerr << "cannot open file " << fileName << endl;
	return;
    }

    if(withHeader) {
	fprintf(out, "%ld %ld\n", this->_nVectors, this->_length);
    }
    if(withID) {
	for(TLL i=0;i<this->_nVectors;++i) {
	    fprintf(out, "%ld", this->_vectorIDs[i]);
	    if(i+1 < this->_nVectors) {
		fprintf(out, " ");
	    }
	}
	fprintf(out, "\n");
    }
    for(TLL j=0;j<this->_capacity;++j) {
	if(this->_storeIndex[j] == -1) {
	    continue;
	}
	for(TLL i=0;i<this->_length;++i) {
	    fprintf(out, "%lf", this->GetVectorComponent(j, i));
	    if(i+1 < this->_length) {
		fprintf(out, " ");
	    }
	}
	fprintf(out, "\n");
    }
    fclose(out);

}

void VectorGroup::WriteToFileVectorNorms(const string& fileName, bool withHeader, bool withID) const {
    FILE *out = fopen(fileName.c_str(), "w");
    if(NULL == out) {
	cerr << "cannot open file " << fileName << endl;
	return;
    }

    if(withHeader) {
	fprintf(out, "%ld\n", this->_nVectors);
    }

    if(withID) {
	for(TLL i=0;i<this->_nVectors;++i) {
	    fprintf(out, "%ld", this->_vectorIDs[i]);
	    if(i+1 < this->_nVectors) {
		fprintf(out, " ");
	    }
	}
	fprintf(out, "\n");
    }
    for(TLL j=0;j<this->_capacity;++j) {
	if(this->_storeIndex[j] == -1) {
	    continue;
	}
	double norm = 0.0;
	GetNorm(this->GetVector(j), this->_length, norm);
	fprintf(out, "%lf", norm);
	if(j+1 < this->_nVectors) {
	    fprintf(out, " ");
	}
    }
    fprintf(out, "\n");
    fclose(out);
}

const double* VectorGroup::GetVector(TLL groupPosition) const {
    return this->_store[this->_storeIndex[groupPosition]];
}

TLL VectorGroup::GetVectorID(TLL groupPosition) const {
    return this->_vectorIDs[groupPosition];
}

TLL VectorGroup::GetGroupSize() const {
    return this->_nVectors;
}

TLL VectorGroup::GetGroupCapacity() const {
    return this->_capacity;
}

double* VectorGroup::GetStorePointer(TLL groupPosition) {
    if(groupPosition < 0 || groupPosition >= this->_capacity || -1 == this->_storeIndex[groupPosition]) {
	cerr << "invalid position " << groupPosition << "." << endl;
	return NULL;
    }
    return this->_store[this->_storeIndex[groupPosition]];
}

} // namespace matrix
} // namespace dof
