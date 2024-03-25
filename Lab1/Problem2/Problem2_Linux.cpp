#include<iostream>
#include<sys/time.h>
using namespace std;
typedef long long ll;

struct timeval start_;
struct timeval end_;

const int N = 1e6 + 5;
const int M = 1;
int n;
ll sum, sum1, sum2;
ll a[N];

void init(int n) {

    for (int i = 0; i < n; i++) {
        a[i] = i;
    }
}

void print(string s) {
    cout << s << ((end_.tv_sec - start_.tv_sec) * 1000000 + (end_.tv_usec - start_.tv_usec)) * 1.0 / 1000 / M << "ms" << endl;
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

    // naive
    gettimeofday(&start_, NULL);
    for (int t = 0; t < M; t++)
        naive(n);
    gettimeofday(&end_, NULL);
    print("naive: ");

    // optimize_multi_link
    gettimeofday(&start_, NULL);
    for (int t = 0; t < M; t++)
        optimize_multi_link(n);
    gettimeofday(&end_, NULL);
    print("optimize_multi_link: ");

    // optimize_recursion_func
    gettimeofday(&start_, NULL);
    for (int t = 0; t < M; t++)
        optimize_recursion_func(n);
    gettimeofday(&end_, NULL);
    print("optimize_recursion_func: ");

    // optimize_recursion_nested_loop
    gettimeofday(&start_, NULL);
    for (int t = 0; t < M; t++)
        optimize_recursion_nested_loop(n);
    gettimeofday(&end_, NULL);
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
