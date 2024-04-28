
#include <iostream>
#include <xmmintrin.h>
#include <stdlib.h>
#include <immintrin.h>
#include <fstream>
using namespace std;

void initializeMatrix(float** a, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < i; j++)
            a[i][j] = 0;
        a[i][i] = 1.0;
        for (int j = i + 1; j < n; j++)
            a[i][j] = rand();
    }
    for (int k = 0; k < n; k++) {
        for (int i = k + 1; i < n; i++) {
            for (int j = 0; j < n; j++) {
                a[i][j] += a[k][j];
            }
        }
    }
}

void* aligned_malloc(size_t size) {
    return _mm_malloc(size, 16);
}

void serialGaussianElimination(float** a, int n) {
    for (int k = 0; k < n; k++) {
        for (int j = k + 1; j < n; j++) {
            a[k][j] /= a[k][k];
        }
        a[k][k] = 1.0f;
        for (int i = k + 1; i < n; i++) {
            for (int j = k + 1; j < n; j++) {
                a[i][j] -= a[i][k] * a[k][j];
            }
            a[i][k] = 0;
        }
    }
}

void sseAlignedGaussianElimination(float** a, int n) {
    __m128 vt, va, vaik, vakj, vaij, vx;
    for (int k = 0; k < n; k++) {
        vt = _mm_set1_ps(a[k][k]);
        for (int j = k + 1; j + 3 < n; j += 4) {
            va = _mm_load_ps(&a[k][j]);
            va = _mm_div_ps(va, vt);
            _mm_store_ps(&a[k][j], va);
        }
        for (int j = k + 1 + (n & ~3); j < n; j++) {
            a[k][j] = a[k][j] / a[k][k];
        }
        a[k][k] = 1.0f;
        for (int i = k + 1; i < n; i++) {
            vaik = _mm_set1_ps(a[i][k]);
            for (int j = k + 1; j + 3 < n; j += 4) {
                vakj = _mm_load_ps(&a[k][j]);
                vaij = _mm_load_ps(&a[i][j]);
                vx = _mm_mul_ps(vakj, vaik);
                vaij = _mm_sub_ps(vaij, vx);
                _mm_store_ps(&a[i][j], vaij);
            }
            for (int j = k + 1 + (n & ~3); j < n; j++) {
                a[i][j] -= a[k][j] * a[i][k];
            }
            a[i][k] = 0;
        }
    }
}

void sseUnalignedGaussianElimination(float** a, int n) {
    __m128 vt, va, vaik, vakj, vaij, vx;
    for (int k = 0; k < n; k++) {
        int j;
        vt = _mm_set_ps1(a[k][k]);
        for (j = k + 1; j + 4 <= n; j += 4) {
            va = _mm_loadu_ps(&a[k][j]);
            va = _mm_div_ps(va, vt);
            _mm_storeu_ps(&a[k][j], va);
        }
        for (; j < n; j++) {
            a[k][j] = a[k][j] / a[k][k];
        }
        a[k][k] = 1.0;
        for (int i = k + 1; i < n; i++) {
            vaik = _mm_set_ps1(a[i][k]);
            for (j = k + 1; j + 4 <= n; j += 4) {
                vakj = _mm_loadu_ps(&a[k][j]);
                vaij = _mm_loadu_ps(&a[i][j]);
                vx = _mm_mul_ps(vakj, vaik);
                vaij = _mm_sub_ps(vaij, vx);
                _mm_storeu_ps(&a[i][j], vaij);
            }
            for (; j < n; j++) {
                a[i][j] -= a[k][j] * a[i][k];
            }
            a[i][k] = 0;
        }
    }
}

void avxAlignedGaussianElimination(float** a, int n) {
    __m256 vt, va, vaik, vakj, vaij, vx;
    for (int k = 0; k < n; k++) {
        vt = _mm256_set1_ps(a[k][k]);
        for (int j = k + 1; j + 7 < n; j += 8) {
            va = _mm256_load_ps(&a[k][j]);
            va = _mm256_div_ps(va, vt);
            _mm256_store_ps(&a[k][j], va);
        }
        for (int j = k + 1 + (n & ~7); j < n; j++) {
            a[k][j] = a[k][j] / a[k][k];
        }
        a[k][k] = 1.0;
        for (int i = k + 1; i < n; i++) {
            vaik = _mm256_set1_ps(a[i][k]);
            for (int j = k + 1; j + 7 < n; j += 8) {
                vakj = _mm256_load_ps(&a[k][j]);
                vaij = _mm256_load_ps(&a[i][j]);
                vx = _mm256_mul_ps(vakj, vaik);
                vaij = _mm256_sub_ps(vaij, vx);
                _mm256_store_ps(&a[i][j], vaij);
            }
            for (int j = k + 1 + (n & ~7); j < n; j++) {
                a[i][j] -= a[k][j] * a[i][k];
            }
            a[i][k] = 0;
        }
    }
}

void avxUnalignedGaussianElimination(float** a, int n) {
    __m256 vt, va, vaik, vakj, vaij, vx;
    for (int k = 0; k < n; k++) {
        vt = _mm256_set1_ps(a[k][k]);
        for (int j = k + 1; j + 7 < n; j += 8) {
            va = _mm256_loadu_ps(&a[k][j]);
            va = _mm256_div_ps(va, vt);
            _mm256_storeu_ps(&a[k][j], va);
        }
        for (int j = k + 1 + (n & ~7); j < n; j++) {
            a[k][j] = a[k][j] / a[k][k];
        }
        a[k][k] = 1.0;
        for (int i = k + 1; i < n; i++) {
            vaik = _mm256_set1_ps(a[i][k]);
            for (int j = k + 1; j + 7 < n; j += 8) {
                vakj = _mm256_loadu_ps(&a[k][j]);
                vaij = _mm256_loadu_ps(&a[i][j]);
                vx = _mm256_mul_ps(vakj, vaik);
                vaij = _mm256_sub_ps(vaij, vx);
                _mm256_storeu_ps(&a[i][j], vaij);
            }
            for (int j = k + 1 + (n & ~7); j < n; j++) {
                a[i][j] -= a[k][j] * a[i][k];
            }
            a[i][k] = 0;
        }
    }
}


void runTest(int n, ofstream& file) {
    float** a1 = new float* [n];
    for (int i = 0; i < n; i++) {
        a1[i] = new float[n];
    }
    float** b1 = static_cast<float**>(aligned_malloc(n * sizeof(float*)));
    for (int i = 0; i < n; i++) {
        b1[i] = static_cast<float*>(aligned_malloc(n * sizeof(float)));
    }
    float** a2 = new float* [n];
    for (int i = 0; i < n; i++) {
        a2[i] = new float[n];
    }
    float** b2 = static_cast<float**>(aligned_malloc(n * sizeof(float*)));
    for (int i = 0; i < n; i++) {
        b2[i] = static_cast<float*>(aligned_malloc(n * sizeof(float)));
    }
    float** c = new float* [n];
    for (int i = 0; i < n; i++) {
        c[i] = new float[n];
    }
    initializeMatrix(a1, n);
    initializeMatrix(b1, n);
    initializeMatrix(a2, n);
    initializeMatrix(b2, n);
    initializeMatrix(c, n);
    cout << "Data size: " << n << "x" << n << endl;
    clock_t start1, finish1;
    start1 = clock();
    serialGaussianElimination(c, n);
    finish1 = clock();
    double serial_time = (double)(finish1 - start1) * 1000 / CLOCKS_PER_SEC;
    cout << "Serial: " << serial_time << " ms" << endl << " " << endl;
    clock_t start2, finish2;
    start2 = clock();
    sseUnalignedGaussianElimination(a1, n);
    finish2 = clock();
    double sse_unaligned_time = (double)(finish2 - start2) * 1000 / CLOCKS_PER_SEC;
    cout << "SSE Unaligned: " << sse_unaligned_time << " ms" << endl << " " << endl;
    clock_t start3, finish3;
    start3 = clock();
    sseAlignedGaussianElimination(b1, n);
    finish3 = clock();
    double sse_aligned_time = (double)(finish3 - start3) * 1000 / CLOCKS_PER_SEC;
    cout << "SSE Aligned: " << sse_aligned_time << " ms" << endl << " " << endl;
    clock_t start4, finish4;
    start4 = clock();
    avxUnalignedGaussianElimination(a2, n);
    finish4 = clock();
    double avx_unaligned_time = (double)(finish4 - start4) * 1000 / CLOCKS_PER_SEC;
    cout << "AVX Unaligned: " << avx_unaligned_time << " ms" << endl << " " << endl;
    clock_t start5, finish5;
    start5 = clock();
    avxAlignedGaussianElimination(b2, n);
    finish5 = clock();
    double avx_aligned_time = (double)(finish5 - start5) * 1000 / CLOCKS_PER_SEC;
    cout << "AVX Aligned: " << avx_aligned_time << " ms" << endl << " " << endl;

    file << n << "," << serial_time << "," << sse_unaligned_time << "," << sse_aligned_time << "," << avx_unaligned_time << "," << avx_aligned_time << endl;

    delete[] a1;
    //delete[] b1;
    delete[] a2;
    //delete[] b2;
    delete[] c;
}

int main() {
    ofstream file("results1.csv");
    file << "n,Serial,SSE Unaligned,SSE Aligned,AVX Unaligned,AVX Aligned" << endl;

   /* for (int k = 100; k < 1000; k += 100) {
        runTest(k, file);
    }

    for (int k = 1000; k <= 3000; k += 500) {
        runTest(k, file);
    }*/

    for (int k = 3500; k <= 5000; k += 500) {
        runTest(k, file);
    }

    file.close();
    return 0;
}


