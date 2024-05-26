#include <iostream>
#include <pthread.h>
#include <Windows.h>
#include<nmmintrin.h> //SSSE4.2
#include<immintrin.h> //AVX、AVX
#pragma comment(lib, "pthreadVC2.lib")
using namespace std;
const int MAX_N = 5000;
float m[MAX_N][MAX_N];
int thread_num = 4;
const int times = 10;

// 线程参数结构体
struct Thread_Param
{
	int k; //消去的轮次
	int t_id; //线程id
};

// 生成测试用例
void m_reset(int N)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < i; j++)
        {
            m[i][j] = 0;
        }
        m[i][i] = 1.0;
        for (int j = i + 1; j < N; j++)
        {
            m[i][j] = rand();
        }
    }
    for (int k = 0; k < N; k++)
    {
        for (int i = k + 1; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                m[i][j] += m[k][j];
            }
        }
    }
}

// 输出数组
void print(int N)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            cout << m[i][j] << " ";
        }
        cout << endl;
    }
}

void *threadFunc(void *param)
{
    Thread_Param *p = (Thread_Param *)param;
    int k = p->k;
    int t_id = p->t_id;
    // 水平划分（穿插）
    for (int i = k + 1 + t_id; i < MAX_N; i += thread_num)
    {
        int num = k + 1;
        for (int j = k + 1; j < MAX_N; ++j)
        {
            m[i][j] -= m[k][j] * m[i][k];
        }
        m[i][k] = 0;
    }
    pthread_exit(NULL);
    return 0;
}

void threadMain(int N)
{
    for (int k = 0; k < N; k++)
    {
        // 主线程做除法操作
        for (int j = k + 1; j < N; j++)
        {
            m[k][j] /= m[k][k];
        }
        m[k][k] = 1.0;

        // 创建子线程，进行消去操作
        pthread_t *handles = new pthread_t[thread_num];       // 创建对应句柄
        Thread_Param *param = new Thread_Param[thread_num]; // 创建对应参数

        // 分配任务
        for (int t_id = 0; t_id < thread_num; t_id++)
        {
            param[t_id].k = k;
            param[t_id].t_id = t_id;
        }

        // 创建线程
        for (int t_id = 0; t_id < thread_num; t_id++)
        {
            pthread_create(&handles[t_id], NULL, threadFunc, (void *)&param[t_id]);
        }

        // 主线程等待回收所有子线程
        for (int t_id = 0; t_id < thread_num; t_id++)
        {
            pthread_join(handles[t_id], NULL);
        }

        // 释放分配的空间
        delete[] handles;
        delete[] param;
    }
}


void* Thread_Func_SSE(void* param) {
	Thread_Param* p = (Thread_Param*)param;
	int k = p->k;
	int t_id = p->t_id;
	for (int i = k + 1 + t_id;i < MAX_N;i += thread_num) {
		//SSE
		float tmp[4] = { m[i][k] ,m[i][k] ,m[i][k] ,m[i][k] };
		__m128 tmp_ik = _mm_loadu_ps(tmp);
		int num = k + 1;
		for (int j = k + 1;j+4 <= MAX_N;j+=4,num=j) {
			__m128 tmp_ij = _mm_loadu_ps(m[i] + j);
			__m128 tmp_kj = _mm_loadu_ps(m[k] + j);
			tmp_kj = _mm_mul_ps(tmp_kj, tmp_ik);
			tmp_ij = _mm_sub_ps(tmp_ij, tmp_kj);
			_mm_storeu_ps(m[i] + j, tmp_ij);
		}
		for (int j = num;j < MAX_N;j++) {
			m[i][j] -= (m[i][k] * m[k][j]);
		}
		m[i][k] = 0;
	}
	pthread_exit(NULL);
	return 0;
}
void Thread_Main_SSE(int N) {
	for (int k = 0;k < N;k++) {
		//主线程做除法操作
		for (int j = k + 1;j < N;j++) {
			m[k][j] /= m[k][k];
		}
		m[k][k] = 1.0;
		//创建子线程，进行消去操作
		pthread_t* handles = new pthread_t[thread_num]; //创建对应句柄
		Thread_Param* param = new Thread_Param[thread_num]; //创建对应参数
		//分配任务
		for (int t_id = 0;t_id < thread_num;t_id++) {
			param[t_id].k = k;
			param[t_id].t_id = t_id;
		}
		//创建线程
		for (int t_id = 0;t_id < thread_num;t_id++) {
			pthread_create(&handles[t_id], NULL, Thread_Func_SSE, (void*)&param[t_id]);
		}
		//主线程等待回收所有子线程
		for (int t_id = 0;t_id < thread_num;t_id++) {
			pthread_join(handles[t_id], NULL);
		}
		//释放分配的空间
		delete[]handles;
		delete[]param;
	}
}
void* Thread_Func_AVX(void* param) {
	Thread_Param* p = (Thread_Param*)param;
	int k = p->k;
	int t_id = p->t_id;
	for (int i = k + 1 + t_id;i < MAX_N;i += thread_num) {
		//AVX
		float tmp[8] = { m[i][k] , m[i][k] ,m[i][k] ,m[i][k] , m[i][k] , m[i][k] , m[i][k] , m[i][k] };
		__m256 tmp_ik = _mm256_loadu_ps(tmp);
		int num = k + 1;
		for (int j = k + 1;j + 8 <= MAX_N;j += 8, num = j) {
			__m256 tmp_ij = _mm256_loadu_ps(m[i] + j);
			__m256 tmp_kj = _mm256_loadu_ps(m[k] + j);
			tmp_kj = _mm256_mul_ps(tmp_kj, tmp_ik);
			tmp_ij = _mm256_sub_ps(tmp_ij, tmp_kj);
			_mm256_storeu_ps(m[i] + j, tmp_ij);
		}
		for (int j = num;j < MAX_N;j++) {
			m[i][j] -= (m[i][k] * m[k][j]);
		}
		m[i][k] = 0;
	}
	pthread_exit(NULL);
	return 0;
}
void Thread_Main_AVX(int N) {
	for (int k = 0;k < N;k++) {
		//主线程做除法操作
		for (int j = k + 1;j < N;j++) {
			m[k][j] /= m[k][k];
		}
		m[k][k] = 1.0;
		//创建子线程，进行消去操作
		pthread_t* handles = new pthread_t[thread_num]; //创建对应句柄
		Thread_Param* param = new Thread_Param[thread_num]; //创建对应参数
		//分配任务
		for (int t_id = 0;t_id < thread_num;t_id++) {
			param[t_id].k = k;
			param[t_id].t_id = t_id;
		}
		//创建线程
		for (int t_id = 0;t_id < thread_num;t_id++) {
			pthread_create(&handles[t_id], NULL, Thread_Func_AVX, (void*)&param[t_id]);
		}
		//主线程等待回收所有子线程
		for (int t_id = 0;t_id < thread_num;t_id++) {
			pthread_join(handles[t_id], NULL);
		}
		//释放分配的空间
		delete[]handles;
		delete[]param;
	}
}

void measure_performance(int N)
{
    long long begin, end, freq;
    double timeuse1 = 0, timeuse2 = 0, timeuse3 = 0;
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq);

    for (int i = 0; i < times; i++)
    {
        m_reset(N);
        QueryPerformanceCounter((LARGE_INTEGER *)&begin);
        threadMain(N);
        QueryPerformanceCounter((LARGE_INTEGER *)&end);
        timeuse1 += (end - begin) * 1000.0 / freq;
    }
    cout << "n=" << N << " pthread1:  " << timeuse1 / times << "ms" << endl;

    for (int i = 0; i < times; i++)
    {
        m_reset(N);
        QueryPerformanceCounter((LARGE_INTEGER *)&begin);
        Thread_Main_SSE(N);
        QueryPerformanceCounter((LARGE_INTEGER *)&end);
        timeuse2 += (end - begin) * 1000.0 / freq;
    }
    cout << "n=" << N << " pthread1_SSE:  " << timeuse2 / times << "ms" << endl;

    for (int i = 0; i < times; i++)
    {
        m_reset(N);
        QueryPerformanceCounter((LARGE_INTEGER *)&begin);
        Thread_Main_AVX(N);
        QueryPerformanceCounter((LARGE_INTEGER *)&end);
        timeuse3 += (end - begin) * 1000.0 / freq;
    }
    cout << "n=" << N << " pthread1_AVX:  " << timeuse3 / times << "ms" << endl;
}

int main()
{
    for (int N = 100; N <= 1000; N += 100)
    {
        measure_performance(N);
    }

    for (int N = 1000; N <= 5000; N += 500)
    {
        measure_performance(N);
    }

    return 0;
}
