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

void optimize_multi_link(int n) {
    // 多链路式
    sum1 = 0; sum2 = 0;
    for (int i = 0; i < n; i += 2) {
        sum1 += a[i];
        sum2 += a[i + 1];  
    }
    sum = sum1 + sum2;
}

void optimize_recursion_func(int n) {
    //实现方式1：递归函数，优点是简单，缺点是递归函数调用开销较大
    if (n == 1)
        return;
    else {
        for (int i = 0; i < n / 2; i++)
            a[i] += a[n - i - 1];
        n = n / 2;
        optimize_recursion_func(n);
    }
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

    // naive
    QueryPerformanceCounter((LARGE_INTEGER*)&head);
    for (int t = 0; t < M; t++)
        naive(n);
    QueryPerformanceCounter((LARGE_INTEGER*)&tail);
    print("naive: ");

    // optimize_multilink
    QueryPerformanceCounter((LARGE_INTEGER*)&head);
    for (int t = 0; t < M; t++)
        optimize_multi_link(n);
    QueryPerformanceCounter((LARGE_INTEGER*)&tail);
    print("optimize_multi_link: ");

    // optimize_recursion_func
    QueryPerformanceCounter((LARGE_INTEGER*)&head);
    for (int t = 0; t < M; t++)
        optimize_recursion_func(n);
    QueryPerformanceCounter((LARGE_INTEGER*)&tail);
    print("optimize_recursion_func: ");

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
    for (int i = 1e4; i < 1e5; i += 1000) {
        experiment(i);
    }
    for (int i = 1e5; i <= 1e6; i += 1e4) {
        experiment(i);
    }

    return 0;
}
