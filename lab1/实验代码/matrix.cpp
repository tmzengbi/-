#include <bits/stdc++.h>
#include "matrix.hpp"
#include "util.hpp"
using namespace std;

#define cur clock()
mt19937 rnd(time(nullptr));


template<int size>
void check() {
    #define _CHECK
    vector<vector<float>> lData(size,vector<float>(size)), rData(lData);
    for(int i=0;i<size;++i) {
        for(int j=0;j<size;++j) {
            lData[i][j] = rnd()%11;
            rData[i][j] = rnd()%11;
        }
    }
    Matrix<size,size> aLeft(lData), aRight(rData);
    MKL_Matrix<size,size> bLeft(lData), bRight(rData);

    cout<<"start normal algorithm"<<endl;
    cout<<solution::Regular_v1(aLeft,aRight)<<endl;

    cout<<"start Strassen algorithm"<<endl;
    cout<<solution::Strassen(aLeft,aRight)<<endl;

    cout<<"start gemm algorithm"<<endl;
    cout<<solution::Regular_v2(aLeft,aRight)<<endl;

    cout<<"start My algorithm based on cache optimization"<<endl;
    cout<<solution::Regular_v3(aLeft,aRight)<<endl;

    cout<<"start intel mkl algorithm"<<endl;
    cout<<MKL_solution::solver(bLeft,bRight)<<endl;
    #undef _CHECK
}

void checkSparse() {
    eleInMatrix a(1,1,2);
    eleInMatrix b(2,2,3);
    eleInMatrix c(1,2,4);
    eleInMatrix d(4,1,6);
    eleInMatrix e(3,3,2);
    sparseMatrix xx({a,b,c,d,e});
    Matrix<4,4> xxx({{2,4,0,0},{0,3,0,0},{0,0,2,0},{6,0,0,0}});
    cout<<solution::Regular_v3(xxx,xxx)<<endl;
    cout<<sparse_solution::solve(xx,xx)<<endl;
}


template<int size>
void test() {
    vector<vector<float>> lData(size,vector<float>(size)), rData(lData);
    for(int i=0;i<size;++i) {
        for(int j=0;j<size;++j) {
            lData[i][j] = rnd()%11;
            rData[i][j] = rnd()%11;
        }
    }
    Matrix<size,size> aLeft(lData), aRight(rData);
    MKL_Matrix<size,size> bLeft(lData), bRight(rData);

    auto x = clock();

    // cout<<"start intel mkl algorithm"<<endl;
    x=cur;
    MKL_solution::solver(bLeft,bRight);
    cout<<1.0*(cur - x) / CLOCKS_PER_SEC <<endl;
    
    // cout<<"start My algorithm based on cache optimize"<<endl;
    x=cur;
    solution::Regular_v3(aLeft,aRight);
    cout<<1.0*(cur - x) / CLOCKS_PER_SEC <<endl;

    // cout<<"start Strassen algorithm"<<endl;
    x=cur;
    solution::Strassen(aLeft,aRight);
    cout<<1.0*(cur - x) / CLOCKS_PER_SEC <<endl;
    
    // cout<<"start gemm algorithm"<<endl;
    x=cur;
    solution::Regular_v2(aLeft,aRight);
    cout<<1.0*(cur - x) / CLOCKS_PER_SEC <<endl;
    
    // cout<<"start normal algorithm"<<endl;
    // x=cur;
    // solution::Regular_v1(aLeft,aRight);
    // cout<<1.0*(cur - x) / CLOCKS_PER_SEC <<endl;
}


int main() { 
    // check<8>();
    // checkSparse();
    // for(int i=0;i<5;++i) {
    //     test<4096>();
    //     cout<<endl;
    // }
    Matrix<2,3> a();

}