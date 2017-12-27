#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>
#include "FHvector.h"
#include "FHsort.h"
using namespace std;


template <class Comparable>
void insertionSortGeneral(FHvector<Comparable>& vectorIn, int gap = 1) {
   int arraySize = vectorIn.size();
   for(int i = gap; i < arraySize; i++) {
      Comparable tmp = vectorIn[i];
      int j;
      for(j = i; j >= gap and tmp < vectorIn[j - gap]; j -= gap)
         vectorIn[j] = vectorIn[j - gap];
      vectorIn[j] = tmp;
   }
}


template <typename Comparable>
void shellSort2(FHvector<Comparable> vectorIn) {
   shellSort1(vectorIn);
}


template <class Comparable>
void shellSortX(FHvector<Comparable>& vectorIn, const int gapArray[], const int arraySize) {
   int index = arraySize;
   while (index--) {
      int gap = gapArray[index];
      insertionSortGeneral(vectorIn, gap);
   }
}


template <class Comparable>
void shellSortY(FHvector<Comparable> vectorIn, const int gapArray[], const int arraySize) {
   shellSortX(vectorIn, gapArray, arraySize);
}


pair<int*, size_t> sedgewickSeq(int arraySize) {
   size_t maxI    = log(arraySize) / log(4);
   size_t seqSize = maxI * 2;
   int* sequence  = new int[seqSize];
   int seqPos     = 0;
   for (int i = 1 ; i <= maxI ; i++) {
      sequence[seqPos++] = 9*(pow(4, i-1) -   pow(2, i-1)) + 1;
      sequence[seqPos++] =    pow(4, i+1) - 6*pow(2, i)    + 1;
      assert(seqPos <= seqSize);
   }
   
   return make_pair(sequence, seqSize);
}


pair<int*, size_t> tokudaSeq(int arraySize) {
   size_t maxI   = log(arraySize / 4) / log(2);
   int* sequence = new int[maxI];
   for (int i = 0 ; i < maxI ; i++)
      sequence[i] = ceil ( (9.0 * pow(9.0/4.0, i) - 4.0) / 5.0 );
   
   return make_pair(sequence, maxI);
}


#define MIN_ARRAYSIZE  10000
#define MAX_ARRAYSIZE 200000
#define NUM_ARRAYS         6
#define TEST_ARRAYSIZE pow(2, 18)

void addRandVector(vector<FHvector<int>>& FHvectors, int arraySize) {
   FHvector<int> newVector(arraySize);
   for (int i = 0 ; i < arraySize ; i++)
      newVector[i] = rand() % arraySize;
   
   FHvectors.push_back(newVector);
}


int main() {
   
      // Generate Randomized FHvectors
   srand(static_cast<unsigned int>(time(NULL)));
   
   vector<FHvector<int>> FHvectors;
   size_t increment = (MAX_ARRAYSIZE - MIN_ARRAYSIZE) / (NUM_ARRAYS-1);
   for (int i = 0, arraySize = MIN_ARRAYSIZE ;
        i < NUM_ARRAYS ;
        i++, arraySize += increment)
   {
      addRandVector(FHvectors, arraySize);
   }
   
      // Add array of size 2 ^ 18
   addRandVector(FHvectors, TEST_ARRAYSIZE);
   
   
      // Explicit Shell Sequence
   int shellSequence[21] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024,
      2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288,
      1048576 };
   int shellSize = sizeof(shellSequence) / sizeof(*shellSequence);
   
   
      // Sedgewick Sequence
   pair<int*, int> sedgewick = sedgewickSeq(MAX_ARRAYSIZE);
   int* sedgewickSequence    = sedgewick.first;
   int sedgewickSize         = sedgewick.second;
   cout << "size of Sedgewick sequence " << endl
   << sedgewickSize << endl
   << "printing Sedgewick sequence..." << endl;
   for (int i = 0 ; i < sedgewickSize ; i++)
      cout << sedgewickSequence[i] << " ";
   cout << "\n\n";
   
   
      // Mysequence (Tokuda Sequence)
   pair <int*, int> tokuda = tokudaSeq(MAX_ARRAYSIZE);
   int* tokudaSequence     = tokuda.first;
   int tokudaSize          = tokuda.second;
   cout << "size of Tokuda sequence " << endl
   << tokudaSize << endl
   << "printing Tokuda sequence..." << endl;
   for (int i = 0 ; i < tokudaSize ; i++)
      cout << tokudaSequence[i] << " ";
   cout << "\n\n";
   
   
      // Print Table Headings
   printf("%-11s|%-22s|%-22s|%-22s|%-22s \n",
          "size",
          "impliedShellSorted(s)",
          "explicitShellSorted(s)",
          "sedgewickSorted(s)",
          "tokudaSorted(s)");
   
   
   for (FHvector<int> randVector : FHvectors) {
         // using modified insertion sorts so each sort recieves unsorted array
         // shellSortY passes to shellSortX by value
      
      clock_t start = clock();
      
      function<double()> elapsedTime = [&start] {
         double elapsed = (double) (clock() - start) / CLOCKS_PER_SEC;
         start = clock();
         
         return elapsed;
      };
      
      shellSort2(randVector);
      double impliedShellSorted = elapsedTime();
      
      shellSortY(randVector, shellSequence, shellSize);
      double explicitShellSorted = elapsedTime();
      
      shellSortY(randVector, sedgewickSequence, sedgewickSize);
      double sedgewickSorted = elapsedTime();
      
      shellSortY(randVector, tokudaSequence, tokudaSize);
      double tokudaSorted = elapsedTime();
      
      
      printf("%-11d|%-22f|%-22f|%-22f|%-22f \n",
             randVector.size(),
             impliedShellSorted,
             explicitShellSorted,
             sedgewickSorted,
             tokudaSorted);
   }
   
   cout << "\n\n";
   
   return 0;
}


/*
 * Run Results
 *
 
 size of Sedgewick sequence
 16
 printing Sedgewick sequence...
 1 5 19 41 109 209 505 929 2161 3905 8929 16001 36289 64769 146305 260609
 
 size of Tokuda sequence
 15
 printing Tokuda sequence...
 1 4 9 20 46 103 233 525 1182 2660 5985 13467 30301 68178 153401
 
 size       |impliedShellSorted(s) |explicitShellSorted(s)|sedgewickSorted(s)    |tokudaSorted(s)
 10000      |0.005154              |0.011617              |0.004269              |0.004931
 48000      |0.032041              |0.085048              |0.022065              |0.023318
 86000      |0.063595              |0.259896              |0.050500              |0.052534
 124000     |0.106739              |0.517916              |0.073418              |0.074315
 162000     |0.141944              |0.575631              |0.096830              |0.105640
 200000     |0.188569              |0.673566              |0.109279              |0.110588
 262144     |1.248412              |1.262668              |0.172970              |0.176926
 
 
 Program ended with exit code: 0
 
 
 *
 * Why does Shell's gap sequence implied by shellSort1() give a different timing result than the
 * explicit array described above and passed to shellSortX()?  Which is faster and why?
 *
 
 hk   := current gap size used for insertionSort
 hk+1 := previously executed gap size
 hk+2 := gap size executed before hk+1
 n    := input size
 
 the explicit gap array results in much slower run times - this is because every gap in the
 explicit sequence (except for the last gap: 1) is even.
 
 Before the array is hk sorted, it has already been h(k+1), h(k+2), h(k+3)... h(t) sorted,
 where h(k+1), h(k+2), h(k+3)... h(t) are all even. This means that a[p-i] < a[p]
 iff i can be expressed as a linear combination of h(k+1), h(k+2), h(k+3)... h(t).
 There is no linear combination of even numbers that will result in an odd number, so
 a[p-i] < a[p] cannot be guaranteed to be when i is odd. When we get to the gap 1, we are
 not guaranteed order at any point along the iteration - this results in the inner loop,
 - which is O(n/hk) - to continue to be executed. This is especially expensive when
 hk is small (like when it =1).
 
 This is less of a problem for the generated gap array because if array size n != 2^m for all
 non-zero ints m, we will sort with some odd gaps before we get to the gap 1. This means that
 when we start getting to small enough gap sizes where we can produce odd linear combinations
 of previously executed gap sizes that are smaller than the array size: we are guaranteed
 a[p-i] < a[p] for those comparisons. This saves us some expensive swapping operations.
 
 
 */

