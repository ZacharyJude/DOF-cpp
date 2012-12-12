#ifndef DOF_DATA_INVERTED_INDEX_H_
#define DOF_DATA_INVERTED_INDEX_H_

#include "basic/CommonHeader.h"
#include "basic/StlLib.h"

using namespace dof;
using namespace dof::lib::stl;

namespace dof {
namespace data {

class TripleInvertedIndex {
private:
    TMapLLMapLLDB _store;
    TMapLLDB _sumWeights;
    double _allIndexesSumWeight;

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
    typedef function<void (TLL index, const TMapLLDB& bucket)> FConstVisitor;

    inline void Add(TLL index, TLL bucketNode, double weight, bool isAccumulate=true) {
	this->_store[index][bucketNode] += weight;
	this->_sumWeights[index] += weight;
	this->_allIndexesSumWeight += weight;
    }

    inline TLL GetSize() const {
	return this->_store.size();
    }

    inline TLL GetBucketSize(TLL index) const {
	TMapLLMapLLDB::const_iterator find = this->_store.find(index);
	if(find == this->_store.end()) {
	    return -1;
	}
	return find->second.size();
    }

    template<typename TInserter>
    inline void GetIndexes(TInserter out) const {
	for(TMapLLMapLLDB::const_iterator it0=this->_store.begin();it0!=this->_store.end();++it0) {
	    *out = it0->first;
	}
    }

    template<typename TInserter>
    inline void GetBucketNodes(TLL index, TInserter out) const { 
	TMapLLMapLLDB::const_iterator find = this->_store.find(index);
	if(find == this->_store.end()) { return; }

	const TMapLLDB& bucket = find->second;
	for(TMapLLDB::const_iterator it0=bucket.begin();it0!=bucket.end();++it0) {
	    *out = it0->first;
	}
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

    inline double GetAllIndexesSumWeight() const {
	return this->_allIndexesSumWeight;
    }

    void CutIndexBySumWeightThreshold(double threshold) {
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

    void LoadFromTextFile(const string& fileName, char fieldSep, TLL indexField=0, TLL bucketNodeField=1, TLL weightField=2) {
	ifstream fin(fileName.c_str(), ios::in);
	string line;
	TVecStr parts0;
	while(!fin.eof()) {
	    getline(fin, line);
	    if("" == line) { continue; }
	    
	    if(!StringToCollection(line, parts0, fieldSep, true) || parts0.size() != 3) {
		cerr << "error when loading triple inverted index line:" << line << endl;
		throw;
	    }
	    
	    this->Add(lexical_cast<TLL>(parts0[indexField]),
		      lexical_cast<TLL>(parts0[bucketNodeField]),
		      lexical_cast<double>(parts0[weightField]));
	}
	fin.close();
    }

    void VisitConst(FConstVisitor visitor) const {
	for(TMapLLMapLLDB::const_iterator it0=this->_store.begin();it0!=this->_store.end();++it0) {
	    visitor(it0->first, it0->second);
	}
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
		fprintf(out, "%ld\t%ld\t%lf\n", index, bucketNode, weight);
	    }
	}
	fclose(out);
    }
};

class TripleDataFile {
private:
    string _fileName;
    string _sortedUniqKey0FileName, _sortedUniqKey1FileName;
    string _key0InvertedIndexFileName, _key1InvertedIndexFileName;
    string _key0SumWeightsFileName, _key1SumWeightsFileName;
    TLL _key0Field, _key1Field, _weightField;
    char _fieldSep;
    TripleInvertedIndex _ii0, _ii1;
    TVecStr _sortedUniqKey0, _sortedUniqKey1;

private:
    bool CheckField(TLL field) const {
	if(!(0 <= field && field <= 2)) {
	    return false;
	}
	return true;
    }

    void DecideWeightField() {
	if( !(this->CheckField(this->_key0Field)) || 
	    !(this->CheckField(this->_key1Field)) ||
	    this->_key0Field == this->_key1Field ) {
	    cerr << "invalid field value, key0:" << this->_key0Field << ", key1:" << this->_key1Field << endl;
	    throw;
	}

	this->_weightField = (3 - this->_key0Field - this->_key1Field);
    }

    void LoadFileGenSortedUniqs() {
	string line, key0, key1;
	TSetStr uniqKey0, uniqKey1;
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
	    key0 = parts0[this->_key0Field];
	    key1 = parts0[this->_key1Field];

	    uniqKey0.insert(key0);
	    uniqKey1.insert(key1);
	}
	fin.close();

	GenSortedUniqVecFromSet(uniqKey0, this->_sortedUniqKey0);
	GenSortedUniqVecFromSet(uniqKey1, this->_sortedUniqKey1);
    }

    void GenOffsetInvertedIndex() {
	string line, key0, key1;
	double weight;
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
	    key0 = parts0[this->_key0Field];
	    key1 = parts0[this->_key1Field];
	    weight = lexical_cast<double>(parts0[this->_weightField]);

	    TVecStr::const_iterator findKey0 = lower_bound(this->_sortedUniqKey0.begin(), this->_sortedUniqKey0.end(), key0);
	    TVecStr::const_iterator findKey1 = lower_bound(this->_sortedUniqKey1.begin(), this->_sortedUniqKey1.end(), key1);
	    if(*findKey0 != key0 || *findKey1 != key1) {
		cerr << "error when finding key0 or key1 offset, key0:" << key0 << ", key1:" << key1 << endl;
		throw;
	    }

	    TLL key0Offset = findKey0 - this->_sortedUniqKey0.begin();
	    TLL key1Offset = findKey1 - this->_sortedUniqKey1.begin();

	    this->_ii0.Add(key0Offset, key1Offset, weight);
	    this->_ii1.Add(key1Offset, key0Offset, weight);
	}
	fin.close();
    
    }

public:
    TripleDataFile(const string& fileName):
    _fileName(fileName),
    _sortedUniqKey0FileName("key0.sorted.uniq"),
    _sortedUniqKey1FileName("key1.sorted.uniq"),
    _key0InvertedIndexFileName("key0.ii"),
    _key1InvertedIndexFileName("key1.ii"),
    _key0SumWeightsFileName("key0.sum.weights"),
    _key1SumWeightsFileName("key1.sum.weights"),
    _fieldSep('\t'),
    _key0Field(0),
    _key1Field(1),
    _weightField(2) { }

    SETTER_GETTER_RETURN_VALUE(SortedUniqKey0FileName,string,_sortedUniqKey0FileName)
    SETTER_GETTER_RETURN_VALUE(SortedUniqKey1FileName,string,_sortedUniqKey1FileName)
    SETTER_GETTER_RETURN_VALUE(Key0InvertedIndexFileName,string,_key0InvertedIndexFileName)
    SETTER_GETTER_RETURN_VALUE(Key1InvertedIndexFileName,string,_key1InvertedIndexFileName)
    SETTER_GETTER_RETURN_VALUE(Key0SumWeightsFileName,string,_key0SumWeightsFileName)
    SETTER_GETTER_RETURN_VALUE(Key1SumWeightsFileName,string,_key1SumWeightsFileName)
    SETTER_GETTER_RETURN_VALUE(Key0Field,TLL,_key0Field)
    SETTER_GETTER_RETURN_VALUE(Key1Field,TLL,_key1Field)
    SETTER_GETTER_RETURN_VALUE(FieldSep,char,_fieldSep)

    void GenOffsetDataSuite() {
	this->DecideWeightField();
	this->LoadFileGenSortedUniqs();
	this->GenOffsetInvertedIndex();
    }

    void WriteOffsetDataSuiteToTextFile() const {
	GenListToFile(this->_sortedUniqKey0FileName, this->_sortedUniqKey0);
	GenListToFile(this->_sortedUniqKey1FileName, this->_sortedUniqKey1);
	GenMapToFile(this->_key0SumWeightsFileName, this->_ii0.GetSumWeights(), '\t');
	GenMapToFile(this->_key1SumWeightsFileName, this->_ii1.GetSumWeights(), '\t');
	this->_ii0.WriteToTextFile(this->_key0InvertedIndexFileName);
	this->_ii1.WriteToTextFile(this->_key1InvertedIndexFileName);
    }
};

} // namespace dof
} // namespace data

#endif 
