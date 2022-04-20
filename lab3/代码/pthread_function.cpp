#include <pthread.h>
#include <boost/lexical_cast.hpp>
#include <iostream>
using namespace std;

int counter = 0;
pthread_mutex_t mutex;
pthread_cond_t cond_var;
double delta;
double a,b,c;
pair<double, double> res;
int ok = 0;
const double eps = 1e-10;
void *calcDelta(void *argv) {
    delta = b*b-4.0*a*c;
    while(counter < 2);
    if(fabs(delta) < eps) delta = 0;
    pthread_cond_broadcast(&cond_var);
    return NULL;
}
void *getx1(void *argv) {
    res.first = -b / 2.0 / a;
    pthread_mutex_lock(&mutex);
    ++ counter;
    while(pthread_cond_wait(&cond_var, &mutex) != 0);
    pthread_mutex_unlock(&mutex);
    if(delta < eps) return NULL;
    res.first += sqrt(delta) / 2.0 / a;
    return &res.first;
}
void *getx2(void *argv) {
    res.second = -b / 2.0 / a;
    pthread_mutex_lock(&mutex);
    ++ counter;
    while(pthread_cond_wait(&cond_var, &mutex) != 0);
    pthread_mutex_unlock(&mutex);
    if(delta < -eps) return NULL;
    res.second += -sqrt(delta) / 2.0 / a;
    return &res.second;
}
int main(int argc, char *argv[]) {
    cin >> a >> b >> c;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_var, NULL);
    pthread_t thread[3];
    pthread_create(&thread[0], NULL, calcDelta, NULL);
    pthread_create(&thread[1], NULL, getx1, NULL);
    pthread_create(&thread[2], NULL, getx2, NULL);
    for(int i = 0; i < 3; ++ i)
        pthread_join(thread[i], NULL);
    if(delta < -eps) {
        cout << "no result because \\delta < 0" << endl;
    } else {
        cout << res.first <<" " <<res.second <<endl;
    }
}