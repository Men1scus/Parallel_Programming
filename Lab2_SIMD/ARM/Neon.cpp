#include <iostream>
#include <stdio.h>
#include <arm_neon.h>
#include <stdlib.h>
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

void unalignedGaussianElimination(float** a, int n) {
    float32x4_t vt, va, vaik, vakj, vaij, vx;
    for (int k = 0; k < n; k++) {
        int j;
        vt = vmovq_n_f32(a[k][k]);
        for (j = k + 1; j + 4 <= n; j += 4) {
            va = vld1q_f32(&a[k][j]);
            va = vdivq_f32(va, vt);
            vst1q_f32(&a[k][j], va);
        }
        for (; j < n; j++) {
            a[k][j] = a[k][j] / a[k][k];
        }
        a[k][k] = 1.0;
        for (int i = k + 1; i < n; i++) {
            vaik = vdupq_n_f32(a[i][k]);
            for (j = k + 1; j + 4 <= n; j += 4) {
                vakj = vld1q_f32(&a[k][j]);
                vaij = vld1q_f32(&a[i][j]);
                vx = vmulq_f32(vakj, vaik);
                vaij = vsubq_f32(vaij, vx);
                vst1q_f32(&a[i][j], vaij);
            }
            for (; j < n; j++) {
                a[i][j] -= a[k][j] * a[i][k];
            }
            a[i][k] = 0;
        }
    }
}

void serialGaussianElimination(float** a, int n) {
    for (int k = 0; k < n; k++) {
        for (int j = k + 1; j < n; j++) {
            a[k][j] /= a[k][k];
        }
        a[k][k] = 1.0;
        for (int i = k + 1; i < n; i++) {
            for (int j = k + 1; j < n; j++) {
                a[i][j] -= a[i][k] * a[k][j];
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
    float** c = new float* [n];
    for (int i = 0; i < n; i++) {
        c[i] = new float[n];
    }
    initializeMatrix(a1, n);
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
    unalignedGaussianElimination(a1, n);
    finish2 = clock();
    double unaligned_time = (double)(finish2 - start2) * 1000 / CLOCKS_PER_SEC;
    cout << "Unaligned: " << unaligned_time << " ms" << endl << " " << endl;

    file << n << "," << serial_time << "," << unaligned_time << endl;

    delete[] a1;
    delete[] c;
}

int main() {
    ofstream file("results.csv");
    file << "n,Serial,Unaligned" << endl;

    for (int k = 100; k < 1000; k += 100) {
        runTest(k, file);
    }

    for (int k = 1000; k <= 5000; k += 500) {
        runTest(k, file);
    }

    file.close();
    return 0;
}