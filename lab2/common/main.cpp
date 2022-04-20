#include "matrix.h"
int main() {
    Matrix x(4,4);
    x.data = std::vector<float>({1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7});
    Matrix y(x);
    Matrix res = Strassen(x,y);
    res.print();
    res = normal(x,y);
    res.print();
}