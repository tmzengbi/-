#ifndef MATRIX_H
#define MATRIX_H

#include <list>
#include <vector>

struct Matrix {
    std::vector<float> data;
    int row, col;
    Matrix();
    Matrix(const int row, const int col);
    Matrix(Matrix &&rhs);
    Matrix(const Matrix &rhs);
    float &operator () (const int i, const int j);
    const float operator () (const int i, const int j) const;
    Matrix operator + (const Matrix &rhs) const;
    Matrix operator - (const Matrix &rhs) const;
    Matrix &operator = (const Matrix &rhs);
    Matrix &operator = (Matrix &&rhs);
    void print() const;
};
Matrix normal(const Matrix &A, const Matrix &B);
// make sure matri A and B should be 2^k \times 2^k
Matrix Strassen(const Matrix &A, const Matrix &B);

struct eleInMatrix {
    int row,col;
    float val;
    eleInMatrix(int row, int col, float val);
};
struct sparseMatrix {
    std::list<eleInMatrix> data;
    void print() const;
};
sparseMatrix sparseSolution(const sparseMatrix &A, const sparseMatrix &B);

#endif