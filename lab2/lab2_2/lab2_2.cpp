#include <iostream>
#include <iomanip>
#include <cmath>
#include <omp.h>
#include <ctime>

double cpuSecond()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return static_cast<double>(ts.tv_sec) + static_cast<double>(ts.tv_nsec) * 1.e-9;
}

double func(double x)
{
    return std::exp(-x * x);
}

double integrate_omp(double (*func)(double), double a, double b, int n)
{
    double h = (b - a) / n;
    double sum = 0.0;

    #pragma omp parallel
    {
        int nthreads = omp_get_num_threads();
        int threadid = omp_get_thread_num();
        int items_per_thread = n / nthreads;

        int lb = threadid * items_per_thread;
        int ub = (threadid == nthreads - 1) ? (n - 1) : (lb + items_per_thread - 1);

        double sumloc = 0.0;

        for (int i = lb; i <= ub; i++) {
            sumloc += func(a + h * (i + 0.5));
        }

        #pragma omp atomic
        sum += sumloc;
    }

    return sum * h;
}

int main()
{
    std::cout << "OpenMP threads: " << omp_get_max_threads() << '\n';

    double t = cpuSecond();
    double res = integrate_omp(func, -4.0, 4.0, 40000000);
    t = cpuSecond() - t;

    std::cout << "Elapsed time (parallel): "
              << std::fixed << std::setprecision(6)
              << t << " sec.\n";

    std::cout << "Result = "
              << std::fixed << std::setprecision(12)
              << res << '\n';

    return 0;
}