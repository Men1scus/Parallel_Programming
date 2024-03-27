#include<iostream>
#include<windows.h>
using namespace std;
typedef long long ll;
ll head, tail, freq; // timers

const int N = 1e4 + 5;
const int M = 1e4;
int n;
ll sum, sum1, sum2;
ll a[N];

void init(int n) {

    for (int i = 0; i < n; i++) {
        a[i] = i;
    }
}

void print(string s) {
    cout << s << (tail - head) * 1000.0 / freq / M << "ms" << endl;
}

void optimize_multi_link(int n) {
    // 多链路式
    sum1 = 0; sum2 = 0;
    for (int i = 0; i < n; i += 2) {
        sum1 += a[i];
        sum2 += a[i + 1];  
    }
    sum = sum1 + sum2;
}

void experiment(int n) {
    cout << "n = " << n << endl;
    init(n);
    // similar to CLOCKS_PER_SEC
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq); // &freq 取freq的地址，并且转换成"指向LARGE_INTEGER的指针"这种类型。


    // optimize_multilink
    QueryPerformanceCounter((LARGE_INTEGER*)&head);
    for (int t = 0; t < M; t++)
        optimize_multi_link(n);
    QueryPerformanceCounter((LARGE_INTEGER*)&tail);
    print("optimize_multi_link: ");

    cout << endl;
}

int main() {
    cout << "M = " << M << endl;
    
    for (int i = 10; i < 100; i += 10) {
        experiment(i);
    }
    for (int i = 100; i < 1e4; i += 100) {
        experiment(i);
    }


    return 0;
}
