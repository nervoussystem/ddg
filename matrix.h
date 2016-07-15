#pragma once
#include <Eigen/Dense>
#include <Eigen/Sparse>

typedef double Real;
typedef Eigen::SparseMatrix<Real, Eigen::RowMajor> SparseMatrix;
typedef Eigen::DiagonalMatrix<Real, Eigen::Dynamic> DiagonalMatrix;
typedef Eigen::Triplet<Real> Triplet;
typedef Eigen::ConjugateGradient<SparseMatrix> SparseSolver;
typedef Eigen::VectorXd Vector;
typedef Eigen::Vector3d Vector3;