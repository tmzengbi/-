#include "matrix.h"
#include <cstdlib>
#include <cstdio>
#include <unordered_map>
#include <iostream>

Matrix normal(const Matrix &A, const Matrix&B) {
    if (A.row != B.col) throw "bad multiply";
    const int N =A.row, M =A.col, K = B.col;
    Matrix C(N, K);
    auto &x = A.data, &y = B.data;
    auto &z = C.data;
    C.row = N; C.col = K;
    #define idx (i * M + k) 
    #define idy (k * K + j)
    #define idz (i * K + j)
    for(int i = 0; i < N; ++ i) {
        for(int k = 0; k < M; ++ k) {
            float tmp = x[idx];
            for(int j = 0; j < K; ++ j) {
                // z[i][j] += x[i][k] * y[k][j];
                z[idz] += tmp * y[idy];
            }
        }
    }
    #undef idx
    #undef idy
    #undef idz
    return C;
}
Matrix::Matrix():row(0), col(0) {

}
Matrix::Matrix(const int row, const int col):row(row), col(col) {
    data.resize(row * col, 0);
}
Matrix::Matrix(Matrix &&rhs) {
    data.swap(rhs.data);
    std::swap(row,rhs.row);
    std::swap(col,rhs.col);
}
Matrix::Matrix(const Matrix &rhs)
    :row(rhs.row), col(rhs.col), data(rhs.data) {
    
}
float &Matrix::operator () (const int i, const int j) {
    return data[i * col + j];
}
const float Matrix::operator () (const int i, const int j) const {
    return data[i * col + j];
}
Matrix Matrix::operator + (const Matrix &rhs) const {
    Matrix res(row, col);
    for(int i = 0; i < row ;++ i)
        for(int j = 0; j < col ; ++ j)
            res(i,j) = this->operator()(i,j) + rhs(i,j);
    return res;
}
Matrix Matrix::operator - (const Matrix &rhs) const {
    Matrix res(row, col);
    for(int i = 0; i < row ;++ i)
        for(int j = 0; j < col ; ++ j)
            res(i,j) = this->operator()(i,j) - rhs(i,j);
    return res;
}
Matrix & Matrix::operator = (const Matrix &rhs) {
    if(this == &rhs) return *this;
    row = rhs.row;
    col = rhs.col;
    data = rhs.data;
    return *this;
}
Matrix &Matrix::operator = (Matrix &&rhs) {
    row = rhs.row;
    col = rhs.col;
    data.swap(rhs.data);
    return *this;
}
Matrix Strassen(const Matrix &A, const Matrix &B) {
    if(A.row != B.col) throw "bad multiply";
    #ifdef DEBUG
    if(A.row <= 1)
        return normal(A, B);
    #endif
    if(A.row <= 128)
        return normal(A, B);
    const int N =A.row, M =A.col , K = B.col;
    const int NN = N / 2;
    Matrix A11(NN, NN);
    Matrix B11(NN, NN);
    Matrix A12(NN, NN);
    Matrix B12(NN, NN);
    Matrix A21(NN, NN);
    Matrix B21(NN, NN);
    Matrix A22(NN, NN);
    Matrix B22(NN, NN);
    Matrix S1 (NN, NN);
    Matrix S2 (NN, NN);
    Matrix S3 (NN, NN);
    Matrix S4 (NN, NN);
    Matrix S5 (NN, NN);
    Matrix S6 (NN, NN);
    Matrix S7 (NN, NN);
    Matrix S8 (NN, NN);
    Matrix S9 (NN, NN);
    Matrix S10(NN, NN);
    Matrix P1 (NN, NN);
    Matrix P2 (NN, NN);
    Matrix P3 (NN, NN);
    Matrix P4 (NN, NN);
    Matrix P5 (NN, NN);
    Matrix P6 (NN, NN);
    Matrix P7 (NN, NN);
    Matrix C11(NN, NN);
    Matrix C12(NN, NN);
    Matrix C21(NN, NN);
    Matrix C22(NN, NN);
    for(int i=0;i<NN;++i)
        for(int j=0;j<NN;++j) {
            A11(i,j) = A(i,j);
            B11(i,j) = B(i,j);
            A12(i,j) = A(i,j + NN);
            B12(i,j) = B(i,j + NN);
            A21(i,j) = A(i + NN,j);
            B21(i,j) = B(i + NN,j);
            A22(i,j) = A(i + NN,j + NN);
            B22(i,j) = B(i + NN,j + NN);
        }
    S1 = B12 - B22;
    S2 = A11 + A12;
    S3 = A21 + A22;
    S4 = B21 - B11;
    S5 = A11 + A22;
    S6 = B11 + B22;
    S7 = A12 - A22;
    S8 = B21 + B22;
    S9 = A11 - A21;
    S10 = B11 + B12;

    P1 = Strassen(A11,S1);
    P2 = Strassen(S2 ,B22);
    P3 = Strassen(S3 ,B11);
    P4 = Strassen(A22,S4);
    P5 = Strassen(S5 ,S6);
    P6 = Strassen(S7 ,S8);
    P7 = Strassen(S9 ,S10);

    C11 = P5 + P4 - P2 + P6;
    C12 = P1 + P2;
    C21 = P3 + P4;
    C22 = P5 + P1 - P3 - P7;

    Matrix res(N, N);
    for(int i=0;i<NN;++i)
        for(int j=0;j<NN;++j) {
            res(i,j) = C11(i,j);
            res(i,j + NN) = C12(i,j);
            res(i + NN, j) = C21(i,j);
            res(i + NN, j + NN) = C22(i,j);
        }
    return res;
}
void Matrix::print() const {
    for(int i = 0; i < row; ++ i) {
        for(int j = 0; j < col ; ++ j)
            std::cout << data[i * col + j] << " ";
        std::cout<<"\n";
    }
    std::cout.flush();
}
eleInMatrix::eleInMatrix(int row, int col, float val)
        :row(row), col(col), val(val) {

        }
sparseMatrix sparseSolution(const sparseMatrix &A, const sparseMatrix &B) {
    std::unordered_map<int, std::unordered_map<int, float> > rmat, tot;
    for(auto &x : B.data)
        rmat[x.row][x.col] = x.val;
    for(auto &x: A.data) {
        int r = x.row, c = x.col;
        for(auto &p : rmat[c]) {
            tot[r][p.first] += x.val * p.second; 
        }
    }
    sparseMatrix res;
    for(auto &x : tot) {
        int r = x.first;
        for(auto &data : x.second) {
            int c = data.first;
            float siz = data.second;
            res.data.push_back(eleInMatrix(r, c, siz));
        }
    }
    return res;
}
void sparseMatrix::print() const {
    for(auto &x : data)
        std::cout<<x.row<<" "<<x.col<<" "<<x.val<<"\n";
    std::cout.flush();
}