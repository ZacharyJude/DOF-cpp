#include <iostream>
#include <fstream>
#include <Eigen/Dense>
#include <Eigen/SVD>
#include <Eigen/Core>
using namespace std;
using namespace Eigen;
//using Eigen::MatrixXd;
//using Eigen::JacobiSVD;

int main() {
    ifstream fin("data.matrix", ios::out);
    string line;
    int nRow, nCol;

    //getline(fin, line);
    fin >> nRow >> nCol;
    MatrixXd m = MatrixXd(nRow, nCol);
	
    int lineCount = 0;
    while(lineCount < nRow) {
	for(int i=0;i<nCol;++i) {
	    double val;
	    fin >> val;
	    m(lineCount, i) = val;
	}
	++lineCount;
    }
    fin.close();

    cout << m << endl << endl;
    JacobiSVD<MatrixXd> svd(m, Eigen::ComputeThinU | Eigen::ComputeThinV);
    cout << svd.singularValues() << endl << endl;

    cout << svd.matrixV() << endl << endl;

    cout << svd.matrixU() << endl << endl;

    /*
    MatrixXd ms = MatrixXd(3, 3);
    ms(0, 0) = svd.singularValues()(0);
    ms(1, 1) = svd.singularValues()(1);
    ms(2, 2) = svd.singularValues()(2);

    //svd.computeU();
    //svd.computeV();
    cout << svd.matrixU() << endl << endl;
    cout << svd.matrixV() << endl << endl;
    cout << svd.matrixU() * ms * svd.matrixV().transpose() << endl;
    */
    return 0;
}
