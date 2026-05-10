#include <iostream>
#include <chrono>
#include <vector>
#include <queue>
#include <stack>
#include <omp.h>

using namespace std;
using namespace chrono;

typedef high_resolution_clock hrc;

class Graph
{
    public:
       int vertices;
       vector<vector<int>> G;
       vector<bool> visited;

       Graph(int v)
       {
         vertices = v;
         G.resize(v);
         visited.resize(v, false);
       }

       void addedge(int u, int v)
       {
         G[u].push_back(v);
         G[v].push_back(u);
       }

       void resetvisited()
       {
         for (int i = 0; i < vertices; i++) 
                {
                visited[i] = false;
                }
       }

       void dfs(int start)
       {
          resetvisited();
          stack <int> st;
          st.push(start);
          visited[start] = true;

          while(!st.empty())
          {
            int curr = st.top();
            st.pop();
            cout<<curr<<" ";
            for(int i=0; i<G[curr].size(); i++)
            {
                int neigh = G[curr][i];
                if(!visited[neigh])
                {
                    st.push(neigh);
                    visited[neigh]= true;
                }
            }
          }
       }

       void parallel_dfs(int start)
       {
         resetvisited();
         #pragma omp parallel
         {
            #pragma omp single
            dfs_task(start);
         }
       }

        void dfs_task(int start)
       {
          stack <int> st;
          st.push(start);
          visited[start] = true;

          while(!st.empty())
          {
            int curr = st.top();
            st.pop();
            cout<<curr<<" ";

            #pragma omp parallel for
            for(int i=0; i<G[curr].size(); i++)
            {
                int neigh = G[curr][i];
                if(!visited[neigh])
                {
                    #pragma omp critical
                    {
                   if(!visited[neigh])
                   {
                       st.push(neigh);
                       visited[neigh]= true;
                   }
                }
                }
            }
          }
       }

       void bfs(int start)
       {
          resetvisited();
          queue <int> q;
          q.push(start);
          visited[start] = true;

          while(!q.empty())
          {
            int curr = q.front();
            q.pop();
            cout<<curr<<" ";
            for(int i=0; i<G[curr].size(); i++)
            {
                int neigh = G[curr][i];
                if(!visited[neigh])
                {
                    q.push(neigh);
                    visited[neigh]= true;
                }
            }
          }
       }

       void parallel_bfs(int start)
       {
        resetvisited();
        #pragma omp parallel
        {
            #pragma omp single
            bfs_task(start);
        }
       }

       void bfs_task(int start)
       {
          queue <int> q;
          q.push(start);
          visited[start] = true;

          while(!q.empty())
          {
            int curr = q.front();
            q.pop();
            cout<<curr<<" ";
            #pragma omp parallel for
            for(int i=0; i<G[curr].size(); i++)
            {
                int neigh = G[curr][i];
                if(!visited[neigh])
                {
                    #pragma omp critical
                    {
                   if(!visited[neigh])
                   {
                       q.push(neigh);
                       visited[neigh]= true;
                   }
                }
                }
            }
          }
       }
};

int main()
{
    int V = 50;
    int E = 1000;
    Graph g(V);

    for(int i=0; i<E; i++)
    {
        int a = rand()%V;
        int b = rand()%V;
        if(a!=b)
        {
            g.addedge(a, b);
        }
    }

    cout<<"SEQ DFS: "<<endl;
    auto start = hrc::now();
    g.dfs(0);
    auto end = hrc::now();
    double time1 = duration<double>(end-start).count();
    cout<<time1<<" seconds"<<endl;

    cout << "\nParallel DFS:\n";
    start = high_resolution_clock::now();
    g.parallel_dfs(0);
    end = high_resolution_clock::now();
    double time2 = duration<double>(end - start).count();
    cout << "\nTime: " << time2 << " seconds\n";
    cout << "Speedup (DFS): " << time1 / time2 << "\n";

    cout << "\nSequential BFS:\n";
    start = high_resolution_clock::now();
    g.bfs(0);
    end = high_resolution_clock::now();
    double time3 = duration<double>(end - start).count();
    cout << "\nTime: " << time3 << " seconds\n";

    cout << "\nParallel BFS:\n";
    start = high_resolution_clock::now();
    g.parallel_bfs(0);
    end = high_resolution_clock::now();
    double time4 = duration<double>(end - start).count();
    cout << "\nTime: " << time4 << " seconds\n";
    cout << "Speedup (BFS): " << time3 / time4 << "\n";

    return 0;

}
