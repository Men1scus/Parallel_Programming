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

void naive(int n) {
    // 逐列访问矩阵元素: 一步外层循环（内存循环一次完整执行）计算出一个内积结果
    for (int i = 0; i < n; i++)  
        sum[i] = 0;

    for(int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            sum[i] += b[j][i] * a[j];
    }
}

void experiment(int n) {
    cout << "n = " << n << endl;
    init(n);
    // similar to CLOCKS_PER_SEC
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);

    // naive
    QueryPerformanceCounter((LARGE_INTEGER*)&head);
    for (int t = 0; t < M; t++)
        naive(n);
    QueryPerformanceCounter((LARGE_INTEGER*)&tail);
    print("naive: ");
}
    
int main(){
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
