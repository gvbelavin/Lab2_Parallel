#include <iostream>
#include <vector>
#include <thread>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <cstdlib>

using namespace std;

void init_chunk(vector<double>& a, vector<double>& b, vector<double>& c,
                int m, int n, int row_begin, int row_end)
{
    for (int i = row_begin; i < row_end; ++i) {
        for (int j = 0; j < n; ++j) {
            a[i * n + j] = i + j;
        }
        c[i] = 0.0;
    }

    int b_begin = static_cast<long long>(row_begin) * n / m;
    int b_end   = static_cast<long long>(row_end) * n / m;

    for (int j = b_begin; j < b_end; ++j) {
        b[j] = j;
    }
}

void matvec_chunk(const vector<double>& a, const vector<double>& b, vector<double>& c,
                  int n, int row_begin, int row_end)
{
    for (int i = row_begin; i < row_end; ++i) {
        double sum = 0.0;
        for (int j = 0; j < n; ++j) {
            sum += a[i * n + j] * b[j];
        }
        c[i] = sum;
    }
}

int main(int argc, char* argv[])
{
    const int m = 2000;
    const int n = 2000;

    vector<double> a(static_cast<size_t>(m) * n);
    vector<double> b(n);
    vector<double> c(m);

    unsigned int num_threads = 4;
    if (argc > 1) {
        num_threads = static_cast<unsigned int>(atoi(argv[1]));
    }
    if (num_threads == 0) {
        num_threads = 1;
    }

    vector<thread> threads;

    for (unsigned int t = 0; t < num_threads; ++t) {
        int row_begin = static_cast<long long>(t) * m / num_threads;
        int row_end   = static_cast<long long>(t + 1) * m / num_threads;

        threads.emplace_back(init_chunk,
                             ref(a), ref(b), ref(c),
                             m, n, row_begin, row_end);
    }

    for (auto& th : threads) {
        th.join();
    }

    threads.clear();

    for (unsigned int t = 0; t < num_threads; ++t) {
        int row_begin = static_cast<long long>(t) * m / num_threads;
        int row_end   = static_cast<long long>(t + 1) * m / num_threads;

        threads.emplace_back(matvec_chunk,
                             cref(a), cref(b), ref(c),
                             n, row_begin, row_end);
    }

    for (auto& th : threads) {
        th.join();
    }

    cout << fixed << setprecision(2);
    cout << "Threads: " << num_threads << '\n';
    cout << "First 10 elements of result vector c:\n";
    for (int i = 0; i < min(10, m); ++i) {
        cout << "c[" << i << "] = " << c[i] << '\n';
    }

    return 0;
}