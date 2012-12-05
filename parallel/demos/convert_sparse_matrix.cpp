#include "CommonHeader.h"
#include "basic.h"

int main(int argc, char *argv[]) {
    TStatInt rs, cs, rr, rc;
    string sparseMatrixFileName = string(argv[1]);
    rs = lexical_cast<TStatInt>(string(argv[2]));
    cs = lexical_cast<TStatInt>(string(argv[3]));
    rr = lexical_cast<TStatInt>(string(argv[4]));
    rc = lexical_cast<TStatInt>(string(argv[5]));
    SparseMatrix sparse;
    LoadTextSparseMatrixFromFile(sparseMatrixFileName, sparse, rs, cs, rr, rc);   

    NormalMatrix normal(rr, rc);
    Sparse2Normal(sparse, normal, rs, cs);
    cout << normal.Rows() << ' ' << normal.Cols() << endl;
    cout << normal << endl;
    return 0;
}
