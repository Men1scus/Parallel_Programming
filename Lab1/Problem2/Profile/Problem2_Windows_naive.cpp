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

void naive(int n) {
    // 链式：将给定元素依次累加到结果变量即可 
    for (int i = 0; i < n; i++)
        sum += a[i];
}

void experiment(int n) {
    cout << "n = " << n << endl;
    init(n);
    // similar to CLOCKS_PER_SEC
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq); // &freq 取freq的地址，并且转换成"指向LARGE_INTEGER的指针"这种类型。

    // naive
    QueryPerformanceCounter((LARGE_INTEGER*)&head);
    for (int t = 0; t < M; t++)
        naive(n);
    QueryPerformanceCounter((LARGE_INTEGER*)&tail);
    print("naive: ");

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
