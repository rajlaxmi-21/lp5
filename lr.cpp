#include <iostream>
#include <chrono>
#include <vector>
#include <omp.h>

using namespace std;
using namespace chrono;

void seq_lr(const vector<double>&x,  const vector<double>&y, double &b0, double &b1, double&time)
{
    int n = x.size();
    double sum_x = 0, sum_y = 0, sum_xy = 0, sum_x2=0;
    auto start = high_resolution_clock::now();

    for(int i=0; i<n; i++)
    {
        sum_x += x[i];
        sum_y += y[i];
        sum_xy += x[i] * y[i];
        sum_x2 += x[i] * x[i];
    }
    b1 = (n*sum_xy - sum_x*sum_y)/ (n*sum_x2 - sum_x*sum_x);
    b0 = (sum_y - b1*sum_x)/n;
    auto end = high_resolution_clock::now();

    time = duration<double>(end - start).count();
}


void par_lr(const vector<double>&x,  const vector<double>&y, double &b0, double &b1, double&time)
{
    int n = x.size();
    double sum_x = 0, sum_y = 0, sum_xy = 0, sum_x2=0;
    auto start = high_resolution_clock::now();
    
    #pragma omp parallel for reduction (+:sum_x, sum_y, sum_xy, sum_x2)
    for(int i=0; i<n; i++)
    {
        sum_x += x[i];
        sum_y += y[i];
        sum_xy += x[i] * y[i];
        sum_x2 += x[i] * x[i];
    }
    b1 = (n*sum_xy - sum_x*sum_y)/ (n*sum_x2 - sum_x*sum_x);
    b0 = (sum_y - b1*sum_x)/n;
    auto end = high_resolution_clock::now();

    time = duration<double>(end - start).count();
}

int main()
{
    int n = 1000000;
    vector<double> x(n), y(n);

    for(int i=0; i<n; i++)
    {
        x[i] = rand() % 1000;
        y[i] = rand() % 1000;
    }

    double b0_seq, b1_seq, time_seq;
    double b0_par, b1_par, time_par;

    seq_lr(x,y, b0_seq, b1_seq, time_seq);
    par_lr(x, y, b0_par, b1_par, time_par);

    cout << "Sequential Execution:\n";
    cout << "beta0: " << b0_seq << ", beta1: " << b1_seq << ", Time: " << time_seq << "s\n";
    cout << "Equation (Sequential): y = " << b1_seq << "x + " << b0_seq << "\n\n";

    cout << "Parallel Execution:\n";
    cout << "beta0: " << b0_par << ", beta1: " << b1_par << ", Time: " << time_par << "s\n";
    cout << "Equation (Parallel): y = " << b1_par << "x + " << b0_par << "\n\n";

    double speedup = (time_par > 0) ? (time_seq / time_par) : 0;
    cout << "Speedup: " << speedup << "\n";

    return 0;
}
