#include <iostream>
#include <Windows.h>
#include <omp.h>
#include <nmmintrin.h> //SSSE4.2
#include <immintrin.h> //AVX、AVX

using namespace std;
float** m;
const int NUM_THREADS = 6;
const int times = 10;

void allocate_memory(int N) {
    m = new float*[N];
    for (int i = 0; i < N; ++i) {
        m[i] = new float[N];
    }
}

void free_memory(int N) {
    for (int i = 0; i < N; ++i) {
        delete[] m[i];
    }
    delete[] m;
}

void print(int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout << m[i][j] << " ";
        }
        cout << endl;
    }
}

void m_reset(int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < i; j++) {
            m[i][j] = 0;
        }
        m[i][i] = 1.0;
        for (int j = i + 1; j < N; j++) {
            m[i][j] = rand();
        }
    }
    for (int k = 0; k < N; k++) {
        for (int i = k + 1; i < N; i++) {
            for (int j = 0; j < N; j++) {
                m[i][j] += m[k][j];
            }
        }
    }
}

void Openmp_SSE(int N) {
    int i, j, k;
    float tmp;
#pragma omp parallel num_threads(NUM_THREADS), private(i, j, k, tmp)
    for (k = 0; k < N; k++) {
#pragma omp single 
        {
            tmp = m[k][k];
            for (j = k + 1; j < N; j++) {
                m[k][j] /= tmp;
            }
            m[k][k] = 1.0;
        }
#pragma omp for
        for (i = k + 1; i < N; i++) {
            float temp[4] = { m[i][k], m[i][k], m[i][k], m[i][k] };
            __m128 tmp_ik = _mm_loadu_ps(temp);
            int num = k + 1;
            for (j = k + 1; j + 4 <= N; j += 4, num = j) {
                __m128 tmp_ij = _mm_loadu_ps(m[i] + j);
                __m128 tmp_kj = _mm_loadu_ps(m[k] + j);
                tmp_kj = _mm_mul_ps(tmp_kj, tmp_ik);
                tmp_ij = _mm_sub_ps(tmp_ij, tmp_kj);
                _mm_storeu_ps(m[i] + j, tmp_ij);
            }
            for (j = num; j < N; j++) {
                m[i][j] -= (m[i][k] * m[k][j]);
            }
            m[i][k] = 0;
        }
    }
}

void Openmp_AVX(int N) {
    int i, j, k;
    float tmp;
#pragma omp parallel num_threads(NUM_THREADS), private(i, j, k, tmp)
    for (k = 0; k < N; k++) {
#pragma omp single 
        {
            tmp = m[k][k];
            for (j = k + 1; j < N; j++) {
                m[k][j] /= tmp;
            }
            m[k][k] = 1.0;
        }
#pragma omp for
        for (i = k + 1; i < N; i++) {
            float temp[8] = { m[i][k], m[i][k], m[i][k], m[i][k], m[i][k], m[i][k], m[i][k], m[i][k] };
            __m256 tmp_ik = _mm256_loadu_ps(temp);
            int num = k + 1;
            for (j = k + 1; j + 8 <= N; j += 8, num = j) {
                __m256 tmp_ij = _mm256_loadu_ps(m[i] + j);
                __m256 tmp_kj = _mm256_loadu_ps(m[k] + j);
                tmp_kj = _mm256_mul_ps(tmp_kj, tmp_ik);
                tmp_ij = _mm256_sub_ps(tmp_ij, tmp_kj);
                _mm256_storeu_ps(m[i] + j, tmp_ij);
            }
            for (j = num; j < N; j++) {
                m[i][j] -= (m[i][k] * m[k][j]);
            }
            m[i][k] = 0;
        }
    }
}

void measure_performance(int N) {
    long long begin, end, freq;
    double timeuse1 = 0, timeuse2 = 0;
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);

    for (int i = 0; i < times; i++) {
        m_reset(N);
        QueryPerformanceCounter((LARGE_INTEGER*)&begin);
        Openmp_SSE(N);
        QueryPerformanceCounter((LARGE_INTEGER*)&end);
        timeuse1 += (end - begin) * 1000.0 / freq;
    }
    cout << "n=" << N << " OpenMP_SSE:  " << timeuse1 / times << "ms" << endl;

    for (int i = 0; i < times; i++) {
        m_reset(N);
        QueryPerformanceCounter((LARGE_INTEGER*)&begin);
        Openmp_AVX(N);
        QueryPerformanceCounter((LARGE_INTEGER*)&end);
        timeuse2 += (end - begin) * 1000.0 / freq;
    }
    cout << "n=" << N << " OpenMP_AVX:  " << timeuse2 / times << "ms" << endl;
}

int main() {
    for (int N = 100; N < 1000; N += 100) {
        allocate_memory(N);
        measure_performance(N);
        free_memory(N);
    }

    for (int N = 1000; N <= 3000; N += 500) {
        allocate_memory(N);
        measure_performance(N);
        free_memory(N);
    }

    return 0;
}
