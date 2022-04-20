#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <mkl/mkl.h>
#include <memory>
#include <map>
#include <list>
#include <cassert>
#include "util.hpp"

template<int N,int M>
class Matrix;

class solution;

template<int N, int M>
class Matrix {
    friend solution;
    static constexpr int R = N;
    static constexpr int C = M;

    typedef std::vector<float> vec;
    private:
    static std::mt19937 randData;
    std::vector<vec> data;
    /**
     * @return 返回一个[l,r]的随机数
     */ 
    static int getRand(int l,int r) {
        return randData() % (r - l + 1) + l;
    }
    public:
    Matrix(): data(std::vector<vec>(N,vec(M, 0.0))) {
        clear();
    }
    Matrix(const std::vector<std::vector<float>> &rData):data(std::vector<vec>(N,vec(M, 0.0))) {
        #define min(a,b) ((a)<(b)?(a):(b))
        int Nto = min(N, rData.size());
        for(int i=0;i<Nto;++i) {
            int Mto = min(M, rData[i].size());
            for(int j=0;j<Mto;++j)
                data[i][j] = rData[i][j];
        }
        #undef min
    }
    void clear() {
        for(int i=0;i<N;++i)
            for(int j=0;j<M;++j)
                data[i][j]=0;
    }
    /**
     * @brief 将矩阵填满随机数（未用）
     */ 
    void random() {
        for(int i=0;i<N;++i)
            for(int j=0;j<M;++j)
                data[i][j] = 1.0 * getRand(0, 10);
    }
    friend std::ostream& operator << (std::ostream &out, const Matrix &rhs) {
        for(int i=0;i<N;++i) {
            for(int j=0;j<M;++j) {
                out << rhs.data[i][j];
                if(j != M - 1) out << " ";
            }
            out << "\n";
        }
        return out;
    }
    
    Matrix operator + (const Matrix<N,M> &rhs) const {
        Matrix<N,M> res;
        for(int i=0;i<N;++i)
            for(int j=0;j<M;++j)
                res.data[i][j] = data[i][j] + rhs.data[i][j];
        return res;
    }

    Matrix operator - (const Matrix<N,M> &rhs) const {
        Matrix<N,M> res;
        for(int i=0;i<N;++i)
            for(int j=0;j<M;++j)
                res.data[i][j] = data[i][j] - rhs.data[i][j];
        return res;
    }
};

template<int N, int M> 
std::mt19937 Matrix<N, M>::randData = std::mt19937(time(nullptr));


class solution{
    public:
    /**
     * @brief 朴素矩阵乘法
     */ 
    template<int lN, int lM /* = rN*/, int rM>
    static Matrix<lN, rM> Regular_v1(const Matrix<lN, lM> &lhs, const Matrix<lM, rM> &rhs) {
        Matrix<lN, rM> res;
        for(int i=0;i<lN;++i)
            for(int j=0;j<rM;++j)
                for(int k=0;k<lM;++k) 
                    res.data[i][j] += lhs.data[i][k] * rhs.data[k][j];
        return res;
    }

    /**
     * @brief gemm 优化矩阵乘法
     */ 
    template<int lN, int lM /* = rN*/, int rM>
    static Matrix<lN, rM> Regular_v2(const Matrix<lN, lM> &lhs, const Matrix<lM, rM> &rhs) {
        Matrix<lN, rM> res;
        for(int i=0;i<lN;i+=8)
            for(int j=0;j<rM;j+=8) 
                for(int mi=0;mi<2;++mi)
                    for(int ni=0;ni<2;++ni) {
                        int m=i+mi*4;
                        int n=j+ni*4;
                        for(int k=0;k<lM;k+=4) {
                            res.data[m + 0][n + 0] += lhs.data[m + 0][k + 0] * rhs.data[k + 0][n + 0];
                            res.data[m + 0][n + 0] += lhs.data[m + 0][k + 1] * rhs.data[k + 1][n + 0];
                            res.data[m + 0][n + 0] += lhs.data[m + 0][k + 2] * rhs.data[k + 2][n + 0];
                            res.data[m + 0][n + 0] += lhs.data[m + 0][k + 3] * rhs.data[k + 3][n + 0];

                            res.data[m + 1][n + 0] += lhs.data[m + 1][k + 0] * rhs.data[k + 0][n + 0];
                            res.data[m + 1][n + 0] += lhs.data[m + 1][k + 1] * rhs.data[k + 1][n + 0];
                            res.data[m + 1][n + 0] += lhs.data[m + 1][k + 2] * rhs.data[k + 2][n + 0];
                            res.data[m + 1][n + 0] += lhs.data[m + 1][k + 3] * rhs.data[k + 3][n + 0];

                            res.data[m + 2][n + 0] += lhs.data[m + 2][k + 0] * rhs.data[k + 0][n + 0];
                            res.data[m + 2][n + 0] += lhs.data[m + 2][k + 1] * rhs.data[k + 1][n + 0];
                            res.data[m + 2][n + 0] += lhs.data[m + 2][k + 2] * rhs.data[k + 2][n + 0];
                            res.data[m + 2][n + 0] += lhs.data[m + 2][k + 3] * rhs.data[k + 3][n + 0];

                            res.data[m + 3][n + 0] += lhs.data[m + 3][k + 0] * rhs.data[k + 0][n + 0];
                            res.data[m + 3][n + 0] += lhs.data[m + 3][k + 1] * rhs.data[k + 1][n + 0];
                            res.data[m + 3][n + 0] += lhs.data[m + 3][k + 2] * rhs.data[k + 2][n + 0];
                            res.data[m + 3][n + 0] += lhs.data[m + 3][k + 3] * rhs.data[k + 3][n + 0];

                            res.data[m + 0][n + 1] += lhs.data[m + 0][k + 0] * rhs.data[k + 0][n + 1];
                            res.data[m + 0][n + 1] += lhs.data[m + 0][k + 1] * rhs.data[k + 1][n + 1];
                            res.data[m + 0][n + 1] += lhs.data[m + 0][k + 2] * rhs.data[k + 2][n + 1];
                            res.data[m + 0][n + 1] += lhs.data[m + 0][k + 3] * rhs.data[k + 3][n + 1];

                            res.data[m + 1][n + 1] += lhs.data[m + 1][k + 0] * rhs.data[k + 0][n + 1];
                            res.data[m + 1][n + 1] += lhs.data[m + 1][k + 1] * rhs.data[k + 1][n + 1];
                            res.data[m + 1][n + 1] += lhs.data[m + 1][k + 2] * rhs.data[k + 2][n + 1];
                            res.data[m + 1][n + 1] += lhs.data[m + 1][k + 3] * rhs.data[k + 3][n + 1];

                            res.data[m + 2][n + 1] += lhs.data[m + 2][k + 0] * rhs.data[k + 0][n + 1];
                            res.data[m + 2][n + 1] += lhs.data[m + 2][k + 1] * rhs.data[k + 1][n + 1];
                            res.data[m + 2][n + 1] += lhs.data[m + 2][k + 2] * rhs.data[k + 2][n + 1];
                            res.data[m + 2][n + 1] += lhs.data[m + 2][k + 3] * rhs.data[k + 3][n + 1];

                            res.data[m + 3][n + 1] += lhs.data[m + 3][k + 0] * rhs.data[k + 0][n + 1];
                            res.data[m + 3][n + 1] += lhs.data[m + 3][k + 1] * rhs.data[k + 1][n + 1];
                            res.data[m + 3][n + 1] += lhs.data[m + 3][k + 2] * rhs.data[k + 2][n + 1];
                            res.data[m + 3][n + 1] += lhs.data[m + 3][k + 3] * rhs.data[k + 3][n + 1];

                            res.data[m + 0][n + 2] += lhs.data[m + 0][k + 0] * rhs.data[k + 0][n + 2];
                            res.data[m + 0][n + 2] += lhs.data[m + 0][k + 1] * rhs.data[k + 1][n + 2];
                            res.data[m + 0][n + 2] += lhs.data[m + 0][k + 2] * rhs.data[k + 2][n + 2];
                            res.data[m + 0][n + 2] += lhs.data[m + 0][k + 3] * rhs.data[k + 3][n + 2];

                            res.data[m + 1][n + 2] += lhs.data[m + 1][k + 0] * rhs.data[k + 0][n + 2];
                            res.data[m + 1][n + 2] += lhs.data[m + 1][k + 1] * rhs.data[k + 1][n + 2];
                            res.data[m + 1][n + 2] += lhs.data[m + 1][k + 2] * rhs.data[k + 2][n + 2];
                            res.data[m + 1][n + 2] += lhs.data[m + 1][k + 3] * rhs.data[k + 3][n + 2];

                            res.data[m + 2][n + 2] += lhs.data[m + 2][k + 0] * rhs.data[k + 0][n + 2];
                            res.data[m + 2][n + 2] += lhs.data[m + 2][k + 1] * rhs.data[k + 1][n + 2];
                            res.data[m + 2][n + 2] += lhs.data[m + 2][k + 2] * rhs.data[k + 2][n + 2];
                            res.data[m + 2][n + 2] += lhs.data[m + 2][k + 3] * rhs.data[k + 3][n + 2];

                            res.data[m + 3][n + 2] += lhs.data[m + 3][k + 0] * rhs.data[k + 0][n + 2];
                            res.data[m + 3][n + 2] += lhs.data[m + 3][k + 1] * rhs.data[k + 1][n + 2];
                            res.data[m + 3][n + 2] += lhs.data[m + 3][k + 2] * rhs.data[k + 2][n + 2];
                            res.data[m + 3][n + 2] += lhs.data[m + 3][k + 3] * rhs.data[k + 3][n + 2];

                            res.data[m + 0][n + 3] += lhs.data[m + 0][k + 0] * rhs.data[k + 0][n + 3];
                            res.data[m + 0][n + 3] += lhs.data[m + 0][k + 1] * rhs.data[k + 1][n + 3];
                            res.data[m + 0][n + 3] += lhs.data[m + 0][k + 2] * rhs.data[k + 2][n + 3];
                            res.data[m + 0][n + 3] += lhs.data[m + 0][k + 3] * rhs.data[k + 3][n + 3];

                            res.data[m + 1][n + 3] += lhs.data[m + 1][k + 0] * rhs.data[k + 0][n + 3];
                            res.data[m + 1][n + 3] += lhs.data[m + 1][k + 1] * rhs.data[k + 1][n + 3];
                            res.data[m + 1][n + 3] += lhs.data[m + 1][k + 2] * rhs.data[k + 2][n + 3];
                            res.data[m + 1][n + 3] += lhs.data[m + 1][k + 3] * rhs.data[k + 3][n + 3];

                            res.data[m + 2][n + 3] += lhs.data[m + 2][k + 0] * rhs.data[k + 0][n + 3];
                            res.data[m + 2][n + 3] += lhs.data[m + 2][k + 1] * rhs.data[k + 1][n + 3];
                            res.data[m + 2][n + 3] += lhs.data[m + 2][k + 2] * rhs.data[k + 2][n + 3];
                            res.data[m + 2][n + 3] += lhs.data[m + 2][k + 3] * rhs.data[k + 3][n + 3];

                            res.data[m + 3][n + 3] += lhs.data[m + 3][k + 0] * rhs.data[k + 0][n + 3];
                            res.data[m + 3][n + 3] += lhs.data[m + 3][k + 1] * rhs.data[k + 1][n + 3];
                            res.data[m + 3][n + 3] += lhs.data[m + 3][k + 2] * rhs.data[k + 2][n + 3];
                            res.data[m + 3][n + 3] += lhs.data[m + 3][k + 3] * rhs.data[k + 3][n + 3];
                        }
                    }
        return res;
    }

    /**
     * @brief 基于 cache 优化矩阵乘法
     */ 

    template<int lN, int lM /* = rN*/, int rM>
    static Matrix<lN, rM> Regular_v3(const Matrix<lN, lM> &lhs, const Matrix<lM, rM> &rhs) {
        Matrix<lN, rM> res;
        for(int i=0;i<lN;i++) 
            for(int k=0;k<lM;k+=2) {
                float x1 = lhs.data[i + 0][k + 0];
                float x2 = lhs.data[i + 0][k + 1];
                for(int j=0;j<rM;j+=4) {
                    res.data[i + 0][j + 0] += x1 * rhs.data[k + 0][j + 0];
                    res.data[i + 0][j + 1] += x1 * rhs.data[k + 0][j + 1];
                    res.data[i + 0][j + 2] += x1 * rhs.data[k + 0][j + 2];
                    res.data[i + 0][j + 3] += x1 * rhs.data[k + 0][j + 3];

                    res.data[i + 0][j + 0] += x2 * rhs.data[k + 1][j + 0];
                    res.data[i + 0][j + 1] += x2 * rhs.data[k + 1][j + 1];
                    res.data[i + 0][j + 2] += x2 * rhs.data[k + 1][j + 2];
                    res.data[i + 0][j + 3] += x2 * rhs.data[k + 1][j + 3];
                }
            }
        return res;
    }


    /**
     * @brief strassen 算法的执行部分
     */ 
    private:
    template<int NN>
    static Matrix<NN,NN> Strassen_solve(const Matrix<NN ,NN> &lhs, const Matrix<NN ,NN> &rhs) {
        #ifdef _CHECK
        if constexpr (NN == 1) [
            return Matrix<1,1>(vector<vector<float>>({{lhs.data[0][0] * rhs.data[0][0]}}));
        ]
        #endif
        if constexpr (NN <= 64) {
            return Regular_v3(lhs,rhs);
        }
        const int MM = NN / 2;
        Matrix<MM,MM> A11, A12, A21, A22;
        Matrix<MM,MM> B11, B12, B21, B22;
        for(int i=0;i<MM;++i)
            for(int j=0;j<MM;++j) {
                A11.data[i][j] = lhs.data[i][j];
                B11.data[i][j] = rhs.data[i][j];
                A12.data[i][j] = lhs.data[i][j + MM];
                B12.data[i][j] = rhs.data[i][j + MM];
                A21.data[i][j] = lhs.data[i + MM][j];
                B21.data[i][j] = rhs.data[i + MM][j];
                A22.data[i][j] = lhs.data[i + MM][j + MM];
                B22.data[i][j] = rhs.data[i + MM][j + MM];
            }
        Matrix<MM,MM> S1 = B12 - B22;
        Matrix<MM,MM> S2 = A11 + A12;
        Matrix<MM,MM> S3 = A21 + A22;
        Matrix<MM,MM> S4 = B21 - B11;
        Matrix<MM,MM> S5 = A11 + A22;
        Matrix<MM,MM> S6 = B11 + B22;
        Matrix<MM,MM> S7 = A12 - A22;
        Matrix<MM,MM> S8 = B21 + B22;
        Matrix<MM,MM> S9 = A11 - A21;
        Matrix<MM,MM> S10 = B11 + B12;

        Matrix<MM,MM> P1 = Strassen_solve(A11, S1);
        Matrix<MM,MM> P2 = Strassen_solve(S2, B22);
        Matrix<MM,MM> P3 = Strassen_solve(S3, B11);
        Matrix<MM,MM> P4 = Strassen_solve(A22, S4);
        Matrix<MM,MM> P5 = Strassen_solve(S5,S6);
        Matrix<MM,MM> P6 = Strassen_solve(S7,S8);
        Matrix<MM,MM> P7 = Strassen_solve(S9,S10);

        Matrix<MM,MM> C11 = P5 + P4 - P2 + P6;
        Matrix<MM,MM> C12 = P1 + P2;
        Matrix<MM,MM> C21 = P3 + P4;
        Matrix<MM,MM> C22 = P5 + P1 - P3 - P7;

        Matrix<NN,NN> res;
        for(int i=0;i<MM;++i)
            for(int j=0;j<MM;++j) {
                res.data[i][j] = C11.data[i][j];
                res.data[i][j + MM] = C12.data[i][j];
                res.data[i + MM][j] = C21.data[i][j];
                res.data[i + MM][j + MM] = C22.data[i][j];
            }
        return res;
    }

    public:
    /**
     * @brief strassen 算法主体
     * @warning need to keep lN == lM == rN == rM == 2^k
     */ 
    template<int lN, int lM /* = rN*/, int rM>
    static Matrix<lN, rM> Strassen(const Matrix<lN, lM> &lhs, const Matrix<lM, rM> &rhs) {
        constexpr int NN = upperBound2<lN, lM, rM>::value;
        Matrix<NN, NN> lv(lhs.data), rv(rhs.data);
        return Matrix<lN, rM>(Strassen_solve(lv,rv).data);
    }

    template<int lN, int lM /* = rN*/, int rM>
    static Matrix<lN, rM> Coppersmith_Winograd(const Matrix<lN, lM> &lhs, const Matrix<lM, rM> &rhs);
};

/**
 * @warning need to link libblas64.so which is in .../lib/mkl/
 */ 
class MKL_solution;

/**
 * @brief 封装了 float 指针，便于处理 MKL 矩阵乘法。
 */ 
template<int N, int M>
class MKL_Matrix {
    friend MKL_solution;
    private:
    float *data;
    public:
    static constexpr int R = N;
    static constexpr int C = M;

    MKL_Matrix():data(new float[N * M]) {}

    MKL_Matrix(const std::vector<std::vector<float>> &rData):data(new float[N * M]) {
        for(int i=0;i<N;++i)
            for(int j=0;j<M;++j)
                data[i*M+j] = rData[i][j];
    }
    MKL_Matrix(const MKL_Matrix &rhs):data(new float[N * M]) {
        memcpy(data, rhs.data, N * M * sizeof(float));
    }
    ~MKL_Matrix() {
        delete[] data;
    }
    friend std::ostream & operator << (std::ostream &os, const MKL_Matrix &rhs) {
        for(int i=0;i<rhs.R;++i) {
            for(int j=0;j<rhs.C;++j)
                os<<rhs.data[i*rhs.C + j]<<" ";
            os<<"\n";
        }
        return os;
    }
};

class MKL_solution {
    public:
    template<int N, int M, int K>
    static MKL_Matrix<N, K> solver(const MKL_Matrix<N, M> &lhs, const MKL_Matrix<M, K> &rhs) {
        MKL_Matrix<N, K> res;
        cblas_sgemm(CblasRowMajor, CblasNoTrans , CblasNoTrans, M, N, K, 1, lhs.data, K, rhs.data, N, 0, res.data, N);
        return res;
    }
};

struct eleInMatrix {
    int row,col;
    float val;
    eleInMatrix(int row, int col, float val): row(row), col(col), val(val) {}
};

class sparse_solution;
/**
 * @brief 稀疏矩阵只存非零元素
 */ 
class sparseMatrix {
    friend sparse_solution;
    private:
    std::list<eleInMatrix> data;
    public:
    void clear() {
        data.clear();
    }
    explicit sparseMatrix(const std::list<eleInMatrix> &rhs): data(rhs) {}
    sparseMatrix() {}
    friend std::ostream& operator << (std::ostream &out, const sparseMatrix &rhs) {
        for(auto &x : rhs.data)
            out<<x.row<<" "<<x.col<<" "<<x.val<<std::endl;
        return out;
    }
};

class sparse_solution{
    public:
    static sparseMatrix solve(const sparseMatrix &lhs, const sparseMatrix &rhs) {
        std::unordered_map<int, std::unordered_map<int, float> > rmat, tot;
        for(auto &x : rhs.data)
            rmat[x.row][x.col] = x.val;
        for(auto &x: lhs.data) {
            int r = x.row, c = x.col;
            for(auto &p : rmat[c]) {
                tot[r][p.first] += x.val * p.second; 
            }
        }
        std::list<eleInMatrix> res;
        for(auto &x : tot) {
            int r = x.first;
            for(auto &data : x.second) {
                int c = data.first;
                float siz = data.second;
                res.push_back(eleInMatrix(r, c, siz));
            }
        }
        return sparseMatrix(res);
    }
};
