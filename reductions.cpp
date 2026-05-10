#include <iostream>
#include <omp.h>
#include <chrono>
#include <math.h>
#include <vector>

using namespace std;
using namespace chrono;
using hrc = high_resolution_clock;

const int n =1000;


//Serial sum
int serial_sum(vector<int> arr)
{
    int sum=0;
    for(int x: arr)
        sum+=x;
    return sum;
}

int parallel_sum(vector<int>&arr)
{
    int sum=0;
    #pragma omp parallel for reduction(+ : sum)
    for(int i = 0; i < arr.size(); i++)
{
    sum += arr[i];
}
    return sum;
}

double serial_avg(vector<int>& arr)
{
    int sum = serial_sum(arr);

    return (double)sum / arr.size();
}

// PARALLEL AVERAGE
double parallel_avg(vector<int>& arr)
{
    int sum = parallel_sum(arr);

    return (double)sum / arr.size();
}
int serial_min(vector<int>& arr)
{
    int mn = arr[0];

    for(int i = 1; i < arr.size(); i++)
    {
        if(arr[i] < mn)
        {
            mn = arr[i];
        }
    }

    return mn;
}
int parallel_min(vector<int>&arr)
{
   int mn = arr[0];
   #pragma omp parallel for reduction(min: mn)
   
    for(int i = 1; i < arr.size(); i++)
    {
        if(arr[i] < mn)
        {
            mn = arr[i];
        }
    }

    return mn;
   
}

int serial_max(vector<int>& arr)
{
    int mx = arr[0];

    for(int i = 1; i < arr.size(); i++)
    {
        if(arr[i] > mx)
        {
            mx = arr[i];
        }
    }

    return mx;
}

// PARALLEL MAX
int parallel_max(vector<int>& arr)
{
    int mx = arr[0];

    #pragma omp parallel for reduction(max : mx)
    for(int i = 1; i < arr.size(); i++)
    {
        if(arr[i] > mx)
        {
            mx = arr[i];
        }
    }

    return mx;
}

int main()
{
    int n;
    cout<<"Enter size of array: "<<endl;
    cin>>n;

    vector<int>arr(n);

    for(int i=0; i<n; i++)
    {
        arr[i]=rand()%100;
    }
    auto start = hrc::now();
    int sum_s = serial_sum(arr);
    auto end = hrc::now();
    double serial_sum_time = duration<double>(end - start).count();

    start = hrc::now();
    int sum_p = parallel_sum(arr);
    end = hrc::now();

    double parallel_sum_time =
        duration<double>(end - start).count();

    cout << "\nSUM\n";
    cout << "Serial Sum: " << sum_s << endl;
    cout << "Parallel Sum: " << sum_p << endl;

    cout << "Serial Time: "
         << serial_sum_time << " sec\n";

    cout << "Parallel Time: "
         << parallel_sum_time << " sec\n";

    cout << "Speedup: "
         << serial_sum_time / parallel_sum_time
         << endl;

    // AVERAGE
    start = hrc::now();
    double avg_s = serial_avg(arr);
    end = hrc::now();

    double serial_avg_time = duration<double>(end - start).count();

    start = hrc::now();
    double avg_p = parallel_avg(arr);
    end = hrc::now();

    double parallel_avg_time =duration<double>(end - start).count();

    cout << "\nAVERAGE\n";
    cout << "Serial Average: " << avg_s << endl;
    cout << "Parallel Average: " << avg_p << endl;

    cout << "Serial Time: "
         << serial_avg_time << " sec\n";

    cout << "Parallel Time: "
         << parallel_avg_time << " sec\n";

    cout << "Speedup: "
         << serial_avg_time / parallel_avg_time
         << endl;

    // MIN
    start = hrc::now();
    int min_s = serial_min(arr);
    end = hrc::now();

    double serial_min_time =duration<double>(end - start).count();

    start = hrc::now();
    int min_p = parallel_min(arr);
    end = hrc::now();

    double parallel_min_time =duration<double>(end - start).count();

    cout << "\nMINIMUM\n";
    cout << "Serial Min: " << min_s << endl;
    cout << "Parallel Min: " << min_p << endl;

    cout << "Serial Time: "
         << serial_min_time << " sec\n";

    cout << "Parallel Time: "
         << parallel_min_time << " sec\n";

    cout << "Speedup: "
         << serial_min_time / parallel_min_time
         << endl;

    // MAX
    start = hrc::now();
    int max_s = serial_max(arr);
    end = hrc::now();

    double serial_max_time = duration<double>(end - start).count();

    start = hrc::now();
    int max_p = parallel_max(arr);
    end = hrc::now();

    double parallel_max_time = duration<double>(end - start).count();

    cout << "\nMAXIMUM\n";
    cout << "Serial Max: " << max_s << endl;
    cout << "Parallel Max: " << max_p << endl;

    cout << "Serial Time: "
         << serial_max_time << " sec\n";

    cout << "Parallel Time: "
         << parallel_max_time << " sec\n";

    cout << "Speedup: "
         << serial_max_time / parallel_max_time
         << endl;

    return 0;


}
