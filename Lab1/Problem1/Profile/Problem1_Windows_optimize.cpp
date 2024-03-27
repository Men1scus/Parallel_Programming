#include<iostream>
#include<windows.h>
using namespace std;
typedef long long ll;
ll head, tail, freq; // timers

const int N = 1e4 + 5;
const int M = 1e1;
int n;
int sum[N];
int a[N];
int b[N][N];

void init(int n) {

    for (int i = 0; i < n; i++) {
        a[i] = i;
        for (int j = 0; j < n; j++)
            b[i][j] = i + j;
    }
}

void print(string s) {
    cout << s << (tail - head) * 1000.0 / freq / M << "ms" << endl;
}

void optimize(int n) {
    // 改为逐行访问矩阵元素: 一步外层循环计算不出任何一个内积, 只是向每个内积累加一个乘法结果

    for (int i = 0; i < n; i++)
        sum[i] = 0;
    for (int j = 0; j < n; j++)
        for (int i = 0; i < n; i++)
            sum[i] += b[j][i] * a[j];

}


void experiment(int n) {
    cout << "n = " << n << endl;
    init(n);
    // similar to CLOCKS_PER_SEC
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);


    // optimize
    QueryPerformanceCounter((LARGE_INTEGER*)&head);
    for (int t = 0; t < M; t++)
        optimize(n);
    QueryPerformanceCounter((LARGE_INTEGER*)&tail);
    print("optimize: ");

    cout << endl;
}

int main() {
    cout << "M = " << M << endl;
    //n = 1e4;
    for (int i = 10; i <= 100; i += 10) {
        experiment(i);
    }
    for (int i = 200; i <= 1e4; i += 100) {
        experiment(i);
    }

    return 0;
}
