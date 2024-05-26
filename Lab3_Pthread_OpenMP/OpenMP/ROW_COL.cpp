#include <iostream>
#include <Windows.h>
#include <omp.h>

using namespace std;

const int MAX_N = 3000;
float **m;
const int NUM_THREADS = 6;
const int times = 10;

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

// 普通高斯消元算法
void serial_LU(int N)
{
    for (int k = 0; k < N; k++)
    {
        for (int j = k + 1; j < N; j++)
        {
            m[k][j] /= m[k][k];
        }
        m[k][k] = 1.0;
        for (int i = k + 1; i < N; i++)
        {
            for (int j = k + 1; j < N; j++)
            {
                m[i][j] -= m[k][j] * m[i][k];
            }
            m[i][k] = 0;
        }
    }
}

// OpenMP 列划分
void Openmp_Col(int N)
{
    int i, j, k;
    float tmp;
#pragma omp parallel num_threads(NUM_THREADS) private(i, j, k, tmp)
    for (k = 0; k < N; k++)
    {
#pragma omp single
        {
            tmp = m[k][k];
            for (j = k + 1; j < N; j++)
            {
                m[k][j] /= tmp;
            }
            m[k][k] = 1.0;
        }
#pragma omp for
        for (j = k + 1; j < N; j++)
        {
            tmp = m[k][j];
            for (i = k + 1; i < N; i++)
            {
                m[i][j] -= (m[i][k] * tmp);
            }
        }
#pragma omp single
        {
            for (i = k + 1; i < N; i++)
            {
                m[i][k] = 0;
            }
        }
    }
}

// OpenMP 行划分
void Openmp_Row(int N)
{
    int i, j, k;
    float tmp;
#pragma omp parallel num_threads(NUM_THREADS) private(i, j, k, tmp)
    for (k = 0; k < N; k++)
    {
#pragma omp single
        {
            tmp = m[k][k];
            for (j = k + 1; j < N; j++)
            {
                m[k][j] /= tmp;
            }
            m[k][k] = 1.0;
        }
#pragma omp for
        for (i = k + 1; i < N; i++)
        {
            tmp = m[i][k];
            for (int j = k + 1; j < N; j++)
            {
                m[i][j] -= (tmp * m[k][j]);
            }
            m[i][k] = 0;
        }
    }
}

void allocate_memory(int N)
{
    m = new float *[N];
    for (int i = 0; i < N; ++i)
    {
        m[i] = new float[N];
    }
}

void free_memory(int N)
{
    for (int i = 0; i < N; ++i)
    {
        delete[] m[i];
    }
    delete[] m;
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
        serial_LU(N);
        QueryPerformanceCounter((LARGE_INTEGER *)&end);
        timeuse1 += (end - begin) * 1000.0 / freq;
    }
    cout << "n=" << N << " Serial:  " << timeuse1 / times << "ms" << endl;

    for (int i = 0; i < times; i++)
    {
        m_reset(N);
        QueryPerformanceCounter((LARGE_INTEGER *)&begin);
        Openmp_Col(N);
        QueryPerformanceCounter((LARGE_INTEGER *)&end);
        timeuse2 += (end - begin) * 1000.0 / freq;
    }
    cout << "n=" << N << " OpenMP_Col:  " << timeuse2 / times << "ms" << endl;

    for (int i = 0; i < times; i++)
    {
        m_reset(N);
        QueryPerformanceCounter((LARGE_INTEGER *)&begin);
        Openmp_Row(N);
        QueryPerformanceCounter((LARGE_INTEGER *)&end);
        timeuse3 += (end - begin) * 1000.0 / freq;
    }
    cout << "n=" << N << " OpenMP_Row:  " << timeuse3 / times << "ms" << endl;
}

int main()
{
    for (int N = 100; N < 1000; N += 100)
    {
        allocate_memory(N);
        measure_performance(N);
        free_memory(N);
    }

    for (int N = 1000; N <= 3000; N += 500)
    {
        allocate_memory(N);
        measure_performance(N);
        free_memory(N);
    }

    return 0;
}
