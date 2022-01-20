#include "testlib.h"
#include <string>

using namespace std;

void swap(string & a, string & b)
{ string temp=a;
    a=b;
    b=temp; }

void bubblesort(string A[], int N)         
{ for (int i=N; i>1; i-=1)
    { for (int j=0; j<i-1; j+=1)
    { if (A[j]>A[j+1])
        { swap(A[j], A[j+1]); } } } }           

int main(int argc, char * argv[])
{ string * data;
    tester T(argc, argv, data);
    int length = T.get_number();

    bubblesort(data, length);

    T.finish(); }