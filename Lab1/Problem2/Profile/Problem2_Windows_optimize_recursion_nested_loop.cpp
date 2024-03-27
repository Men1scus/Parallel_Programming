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



void optimize_recursion_nested_loop(int n) {
    //实现方式2：二重循环
    for (int m = n; m > 1; m /= 2) // log(n)个步骤
        for (int i = 0; i < m / 2; i++)
            a[i] = a[i * 2] + a[i * 2 + 1]; // 相邻元素相加连续存储到数组最前面
    // a[0]为最终结果
}


void experiment(int n) {
    cout << "n = " << n << endl;
    init(n);
    // similar to CLOCKS_PER_SEC
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq); // &freq 取freq的地址，并且转换成"指向LARGE_INTEGER的指针"这种类型。

    // optimize_recursion_nested_loop
    QueryPerformanceCounter((LARGE_INTEGER*)&head);
    for (int t = 0; t < M; t++)
        optimize_recursion_nested_loop(n);
    QueryPerformanceCounter((LARGE_INTEGER*)&tail);
    print("optimize_recursion_nested_loop: ");

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
