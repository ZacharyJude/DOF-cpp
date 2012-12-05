#ifndef DOF_DATA_INVERTED_INDEX_H_
#define DOF_DATA_INVERTED_INDEX_H_

#include "basic/StlLib.h"

using namespace dof::lib::stl;

namespace dof {
namespace data {

class TripleInvertedIndex {
private:
    TMapLLMapLLDB _store;
    TMapLLDB _sumWeights;

private:

    inline TMapLLDB::const_iterator FindSumWeight(TLL index) const {
	TMapLLDB::const_iterator it0 = this->_sumWeights.find(index);
	if(it0 == this->_sumWeights.end()) {
	    cerr << "no index:" << index << " at FindSumWeight." << endl;
	    throw;
	}
	return it0;
    }

public:
    inline void Add(TLL index, TLL bucketNode, double weight, bool isAccumulate=true) {
	this->_store[index][bucketNode] += weight;
	this->_sumWeights[index] += weight;
    }

    inline double GetWeight(TLL index, TLL bucketNode, bool& canFind) const {
	canFind = false;
	TMapLLMapLLDB::const_iterator findIndex = this->_store.find(index);
	if(findIndex == this->_store.end()) {
	    return 0.0;
	}
	TMapLLDB::const_iterator findBucketNode = findIndex->second.find(bucketNode);
	if(findBucketNode == findIndex->second.end()) {
	    return 0.0;
	}
	canFind = true;
	return findBucketNode->second;
    }

    inline double GetSumWeight(TLL index, bool& canFind) const {
	canFind = false;
	TMapLLDB::const_iterator findIndex = this->_sumWeights.find(index);
	if(findIndex == this->_sumWeights.end()) {
	    return 0.0;
	}
	canFind = true;
	return findIndex->second;
    }

    void CutIndexByThreshold(double threshold) {
	TVecLL cuttingIndexes;
	for(TMapLLMapLLDB::const_iterator it0=this->_store.begin();
	    it0!=this->_store.end();
	    ++it0) {
	    if(this->_sumWeights[it0->first] < threshold) {
		cuttingIndexes.push_back(it0->first);
	    }
	}
	for(TVecLL::const_iterator it0=cuttingIndexes.begin();it0!=cuttingIndexes.end();++it0) {
	    this->_store.erase(*it0);
	    this->_sumWeights.erase(*it0);
	}
    }

    const TMapLLDB& GetSumWeights() const {
	return this->_sumWeights;
    }

    void WriteToTextFile(const string& fileName) const {
	FILE *out = fopen(fileName.c_str(), "w");
	if(NULL == out) {
	    cerr << "cannot open file:" << fileName << ", write inverted index to file fails." << endl;
	    return ;
	}
	for(TMapLLMapLLDB::const_iterator it0=this->_store.begin();it0!=this->_store.end();++it0) {
	    const TMapLLDB& bucket = it0->second;
	    TLL index = it0->first;
	    for(TMapLLDB::const_iterator it1=bucket.begin();it1!=bucket.end();++it1) {
		TLL bucketNode = it1->first;
		double weight = it1->second;
		fprintf(out, "%ld %ld %lf\n", index, bucketNode, weight);
	    }
	}
	fclose(out);
    }
};

class TripleDataFile {
private:
    string _fileName;
    string _sortedUniqIndexesFileName, _sortedUniqBucketNodesFileName, _invertedIndexFileName;
    TLL _indexField, _bucketNodeField, _weightField;
    char _fieldSep;
    TripleInvertedIndex _ii;
    TVecStr _sortedUniqIndexes, _sortedUniqBucketNodes;

private:
    bool CheckField(TLL field) const {
	if(!(0 <= field && field <= 2)) {
	    return false;
	}
	return true;
    }

    void DecideWeightField() {
	if( !(this->CheckField(this->_indexField)) || 
	    !(this->CheckField(this->_bucketNodeField)) ||
	    this->_indexField == this->_bucketNodeField ) {
	    cerr << "invalid field value, index:" << this->_indexField << ", bucket node:" << this->_bucketNodeField << endl;
	    throw;
	}

	this->_weightField = (3 - this->_indexField - this->_bucketNodeField);
    }

    void LoadFileGenSortedUniqs() {
	string line, index, bucketNode;
	TSetStr uniqIndexes, uniqBucketNodes;
	TVecStr parts0;
	ifstream fin(this->_fileName.c_str(), ios::in);
	while(!fin.eof()) {
	    getline(fin, line);
	    if("" == line) {
		continue;
	    }
	    if(!StringToCollection(line, parts0, this->_fieldSep, true) || parts0.size() != 3) {
		cerr << "error while loading data line:" << line << endl;
		throw;
	    }
	    index = parts0[this->_indexField];
	    bucketNode = parts0[this->_bucketNodeField];

	    uniqIndexes.insert(index);
	    uniqBucketNodes.insert(bucketNode);
	}
	fin.close();

	GenSortedUniqVecFromSet(uniqIndexes, this->_sortedUniqIndexes);
	GenSortedUniqVecFromSet(uniqBucketNodes, this->_sortedUniqBucketNodes);
    }

    void GenOffsetInvertedIndex() {
	string line, index, bucketNode;
	double weight;
	TSetStr uniqIndexes, uniqBucketNodes;
	TVecStr parts0;
	ifstream fin(this->_fileName.c_str(), ios::in);
	while(!fin.eof()) {
	    getline(fin, line);
	    if("" == line) {
		continue;
	    }
	    if(!StringToCollection(line, parts0, this->_fieldSep, true) || parts0.size() != 3) {
		cerr << "error while loading data line:" << line << endl;
		throw;
	    }
	    index = parts0[this->_indexField];
	    bucketNode = parts0[this->_bucketNodeField];
	    weight = lexical_cast<double>(parts0[this->_weightField]);

	    TVecStr::const_iterator findIndex = lower_bound(this->_sortedUniqIndexes.begin(), this->_sortedUniqIndexes.end(), index);
	    TVecStr::const_iterator findBucketNode = lower_bound(this->_sortedUniqBucketNodes.begin(), this->_sortedUniqBucketNodes.end(), bucketNode);
	    if(*findIndex != index || *findBucketNode != bucketNode) {
		cerr << "error when finding index or bucket node offset, index:" << index << ", bucket node:" << bucketNode << endl;
		throw;
	    }

	    TLL indexOffset = findIndex - this->_sortedUniqIndexes.begin();
	    TLL bucketNodeOffset = findBucketNode - this->_sortedUniqBucketNodes.begin();

	    this->_ii.Add(indexOffset, bucketNodeOffset, weight);

	    uniqIndexes.insert(index);
	    uniqBucketNodes.insert(bucketNode);
	}
	fin.close();
    
    }

public:
    TripleDataFile(const string& fileName):
    _fileName(fileName),
    _sortedUniqIndexesFileName("indexes.sorted.uniq"),
    _sortedUniqBucketNodesFileName("bucket_nodes.sorted.uniq"),
    _invertedIndexFileName("inverted_index"),
    _fieldSep('\t'),
    _indexField(0),
    _bucketNodeField(1),
    _weightField(2) { }

    SETTER_GETTER_RETURN_VALUE(IndexesFileName,string,_sortedUniqIndexesFileName)
    SETTER_GETTER_RETURN_VALUE(BucketNodesFileName,string,_sortedUniqBucketNodesFileName)
    SETTER_GETTER_RETURN_VALUE(InvertedIndexFileName,string,_invertedIndexFileName)
    SETTER_GETTER_RETURN_VALUE(IndexField,TLL,_indexField)
    SETTER_GETTER_RETURN_VALUE(BucketNodeField,TLL,_bucketNodeField)
    SETTER_GETTER_RETURN_VALUE(FieldSep,char,_fieldSep)

    void GenOffsetDataSuite() {
	this->DecideWeightField();
	this->LoadFileGenSortedUniqs();
	this->GenOffsetInvertedIndex();
    }

    void WriteOffsetDataSuiteToTextFile() const {
	GenListToFile(this->_sortedUniqIndexesFileName, this->_sortedUniqIndexes);
	GenListToFile(this->_sortedUniqBucketNodesFileName, this->_sortedUniqBucketNodes);
	this->_ii.WriteToTextFile(this->_invertedIndexFileName);
    }
};

} // namespace dof
} // namespace data

#endif 
