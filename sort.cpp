#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>


using namespace std;
using namespace chrono;

typedef high_resolution_clock hrc;

void print_array(vector<int>&arr)
{
    cout<<"Array: [";
    for(int x: arr)
    {
        cout<<x<<" ";
    }
    cout<<" ]"<<endl;
}

double seq_bubble_sort(vector<int> arr)
{
    auto start= hrc::now();
    int n= arr.size();
    
    for(int i=0; i<n-1; i++) //passes
    {
        for(int j=0; j<n-i-1; j++) 
        {
            if(arr[j]>arr[j+1])
            {
                swap(arr[j], arr[j+1]);
            }
        }
    }
    auto end = hrc::now();
    cout<<"Seq BS: ";
    print_array(arr);
    return duration<double>(end-start).count();
}

double parallel_bs(vector<int> arr)
{
    auto start= hrc::now();
    int n= arr.size();

    for(int phase =0; phase<n; phase++)
    {
        // EVEN PHASE
        if(phase % 2 == 0)
        {
            #pragma omp parallel for
            for(int i = 0; i < n - 1; i += 2)
            {
                if(arr[i] > arr[i + 1])
                {
                    swap(arr[i], arr[i + 1]);
                }
            }
        }

        // ODD PHASE
        else
        {
            #pragma omp parallel for
            for(int i = 1; i < n - 1; i += 2)
            {
                if(arr[i] > arr[i + 1])
                {
                    swap(arr[i], arr[i + 1]);
                }
            }
        }

    }
    auto end = hrc::now();
    cout<<"Parallel BS: ";
    print_array(arr);
    return duration<double>(end-start).count();
}

void merge(vector<int>&arr, int left, int mid, int right)
{
    vector<int> temp;
    int i=left;
    int j=mid+1;

    while(i<=mid && j<=right)
    {
        if(arr[i] < arr[j])
        {
            temp.push_back(arr[i++]);
        }
        else
        {
            temp.push_back(arr[j++]);
        }
    }
    while(i<=mid)
    {
        temp.push_back(arr[i++]);
    }
    while(j<=right)
    {
        temp.push_back(arr[j++]);
    }
    for(int k = 0; k < temp.size(); k++)
    {
        arr[left + k] = temp[k];
    }
}

void seq_merge_sort(vector<int>&arr, int left, int right)
{
    if(left<right)
    {
        int mid= (left+right)/2;
        seq_merge_sort(arr, left, mid);
        seq_merge_sort(arr, mid+1, right);

        merge(arr, left, mid, right);
    }
}

void parallel_merge_sort(vector<int>&arr, int left, int right)
{
    if(left<right)
    {
        int mid= (left+right)/2;
        #pragma omp parallel sections
        {
          #pragma omp section
          parallel_merge_sort(arr, left, mid);
          #pragma omp section
          parallel_merge_sort(arr, mid+1, right);
        }
        merge(arr, left, mid, right);
    }
}
int main()
{
    int n;

    cout << "Enter array size: ";
    cin >> n;

    vector<int> arr(n);

    for(int i = 0; i < n; i++)
    {
        arr[i] = rand() % 100;
    }

    cout << "\nOriginal Array:\n";
  

    // BUBBLE SORT
    double seqBubbleTime = seq_bubble_sort(arr);
    double parBubbleTime = parallel_bs(arr);

    cout << "\nSequential Bubble Time: "
         << seqBubbleTime << " sec\n";

    cout << "Parallel Bubble Time: "
         << parBubbleTime << " sec\n";

    cout << "Speedup: "
         << seqBubbleTime / parBubbleTime << endl;

    // MERGE SORT
    vector<int> arr1 = arr;
    vector<int> arr2 = arr;

    auto start = high_resolution_clock::now();
    seq_merge_sort(arr1, 0, n - 1);
    auto end = high_resolution_clock::now();

    double seqMergeTime =
        duration<double>(end - start).count();

    start = high_resolution_clock::now();
    parallel_merge_sort(arr2, 0, n - 1);
    end = high_resolution_clock::now();

    double parMergeTime =
        duration<double>(end - start).count();

    cout << "\nSequential Merge Sort:\n";
    //print_array(arr1);

    cout << "\nParallel Merge Sort:\n";
    //print_array(arr2);

    cout << "\nSequential Merge Time: "
         << seqMergeTime << " sec\n";

    cout << "Parallel Merge Time: "
         << parMergeTime << " sec\n";

    cout << "Speedup: "
         << seqMergeTime / parMergeTime << endl;

    return 0;
}
