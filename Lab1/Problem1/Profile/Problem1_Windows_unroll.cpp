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

void unroll(int n) {
    // Reference: https://github.com/NKULYX/NKU-COSC0025-Parallel-Programming/blob/main/Lab1/problem1/problem1_Windows.cpp

    for (int i = 0; i < n; i++)
        sum[i] = 0;
    for (int j = 0; j < n; j += 10)
    {
        int tmp0 = 0, tmp1 = 0, tmp2 = 0, tmp3 = 0, tmp4 = 0, tmp5 = 0, tmp6 = 0, tmp7 = 0, tmp8 = 0, tmp9 = 0;
        for (int i = 0; i < n; i++)
        {
            tmp0 += a[j + 0] * b[j + 0][i];
            tmp1 += a[j + 1] * b[j + 1][i];
            tmp2 += a[j + 2] * b[j + 2][i];
            tmp3 += a[j + 3] * b[j + 3][i];
            tmp4 += a[j + 4] * b[j + 4][i];
            tmp5 += a[j + 5] * b[j + 5][i];
            tmp6 += a[j + 6] * b[j + 6][i];
            tmp6 += a[j + 6] * b[j + 6][i];
            tmp7 += a[j + 7] * b[j + 7][i];
            tmp8 += a[j + 8] * b[j + 8][i];
            tmp9 += a[j + 9] * b[j + 9][i];
        }
        sum[j + 0] = tmp0;
        sum[j + 1] = tmp1;
        sum[j + 2] = tmp2;
        sum[j + 3] = tmp3;
        sum[j + 4] = tmp4;
        sum[j + 5] = tmp5;
        sum[j + 6] = tmp6;
        sum[j + 7] = tmp7;
        sum[j + 8] = tmp8;
        sum[j + 9] = tmp9;
    }
}

void experiment(int n) {
    cout << "n = " << n << endl;
    init(n);
    // similar to CLOCKS_PER_SEC
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);

    // unroll
    QueryPerformanceCounter((LARGE_INTEGER*)&head);
    for (int t = 0; t < M; t++)
        unroll(n);
    QueryPerformanceCounter((LARGE_INTEGER*)&tail);
    print("unroll: ");
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
