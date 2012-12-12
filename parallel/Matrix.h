#ifndef DOF_PARALLEL_MATRIX_H_
#define DOF_PARALLEL_MATRIX_H_

#include "basic/CommonHeader.h"
#include "matrix/Concept.h"
#include "Util.h"
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>

using namespace dof::matrix;

namespace dof {
namespace parallel {

void SparseMatrixTransposedMultiplication(const string& sparseMatrixFileName, SparseMatrix& matrix, double lowLimit=1e-6);

} // namespace parallel
} // namespace dof

#endif // DOF_PARALLEL_MATRIX_H_
