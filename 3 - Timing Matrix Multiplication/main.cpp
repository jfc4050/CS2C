   //
   // Created by Justin on 10/11/17.
   //

/***********************
 *
 * PART A
 *
 ***********************/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <utility>

using namespace std;

#define PRINT_WIDTH 8
#define RAND_RANGE 10


template <class type>
class DynMat {
   
   const int    size;
   type** const spine;
   
public:
   
   mutable bool populated;
   
   explicit DynMat(int sizeIn)
   : size(sizeIn), spine(new type*[sizeIn]), populated(false) {}
   
   ~DynMat() {
      if (populated) {
         type** temp = spine;
         type** end = spine + size;
         for ( ; temp != end ; temp++) {
            delete[] *temp; *temp = nullptr;
         }
      }
   }
   
   type** getSpine() const { return spine; }
   
   const int getSize() const { return size; }
   
   const type* getRow(int row) const { return spine[row]; }
   
   type* getCol(int col) const {
      auto* colOut = new type[size];
      for (int i = 0 ; i < size ; i++) { colOut[i] = spine[i][col]; }
      return colOut;    // points to dynamically allocated array
   }
   
   type& operator()(const int row, const int col) const {
      return spine[row][col];
   }
   
   void populate(float percentPopulated) {
      for (int row = 0 ; row < size ; row++) {
         spine[row] = new type[size];
         for (int col = 0 ; col < size ; col++) {
            spine[row][col] = type();
         }
      }
      try {
         if (percentPopulated < 0 or percentPopulated > 100) {
            throw InvalidPercentageException();
         }
         auto fieldsToFill = (int) ( (percentPopulated / 100) * (size * size) );
         while (fieldsToFill--) {
            auto row  = (int)  ( ( (float)rand()/(float)RAND_MAX ) * size );
            auto col  = (int)  ( ( (float)rand()/(float)RAND_MAX ) * size );
            auto val = (type) ( ( (float)rand()/(float)RAND_MAX ) * RAND_RANGE );
            spine[row][col] = val;
         }
      } catch (InvalidPercentageException&) {
         cout << "ERROR: Invalid Percentage Input" << endl;
         return;
      }
      populated = true;
   }
   
   class InvalidPercentageException{};
   
};

class UnpopulatedMatrixException{};
class OutOfBoundsAccessException{};
class NullPtrAccessException{};

template <class type>
void showDynMat(DynMat<type>& matIn, int start, int stop) {
   try {
      if (!matIn.populated) {
         throw UnpopulatedMatrixException();
      }
      if (start < 0) {
         throw OutOfBoundsAccessException();
      }
   } catch (UnpopulatedMatrixException) {
      cout << "ERROR: Matrix is not populated" << endl;
      return;
   } catch (OutOfBoundsAccessException) {
      cout << "ERROR: Out of bounds access" << endl;
      return;
   }
   
   int max = matIn.getSize();
   if (start+stop > max) stop = max;
   
   cout << "\n";
   type** const spine = matIn.getSpine();
   try {
      for (int row = start ; row < stop ; row++) {
         if (spine[row] == nullptr) {
            throw NullPtrAccessException();
         }
         for (int col = 0 ; col < stop ; col++) {
            cout << setw(PRINT_WIDTH) << setprecision(2)
            << spine[row][col];
         } cout << endl;
      }
   } catch (NullPtrAccessException) {
      cout << "ERROR: Attempted to access nullptr" << endl;
      return;
   }
   
   
}

template<class type>
type multiplyArrays(const type arrA[], const type arrB[], int size) {
   type total = type();
   for (int ind = 0 ; ind < size ; ind++) {
      total += ( arrA[ind] * arrB[ind] );
   }
   return total;
}

template <class type>
DynMat<type> multiplyDynMat(const DynMat<type>& matA, 
                            const DynMat<type>& matB, 
                            const int size)
{
   DynMat<type> matOut(size);
   matOut.populate(0);
   
   for (int col = 0 ; col < size ; col++) {
      type* curCol = matB.getCol(col);
      for (int row = 0 ; row < size ; row++) {
         matOut(row, col) = multiplyArrays(matA.getRow(row), curCol, size);
      }
      delete[] curCol; curCol = nullptr;
   }
   return matOut;
}

class CSVWriter {
   string filePath;
   string delimiter;
   int lineNum;
   
public:
   explicit CSVWriter(string pathIn, string delim = ",") :
   filePath(std::move(pathIn)),
   delimiter(std::move(delim)),
   lineNum(0)
   { writeToRow("matrix size", "elapsed time"); }
   
   template<class col1, class col2>
   void writeToRow(col1 size, col2 time) {
      fstream csvFile;
      csvFile.open(filePath, ios::out | ( lineNum ? ios::app : ios::trunc) );
      csvFile << size << delimiter << time
      << "\n";
      lineNum++;
      csvFile.close();
   }
};


#define TYPE float
#define PERCENT_POPULATED 1
#define START 0
#define STOP 15

void sanityChecking() {
   
   cout << "\n--------------------------\n\n"
   << "START OF SANITY CHECKS"
   << "\n\n--------------------------\n\n\n";
   
   int size = 5;
   
   DynMat<TYPE> testMat(5);
   
      // should cause "Matrix is not populated" error
   showDynMat(testMat, 0, 5);
   
   testMat.populate(0);
      // should cause "Out of bounds access" error
   showDynMat(testMat, -1, 4);
   
   auto* matPtr = new DynMat<int>(size);
      // works with ints
      // Should cause "Matrix is not populated" error
   showDynMat(*matPtr, 0, 5);
   matPtr->populate(100);
      // should be 100% populated with ints
   showDynMat(*matPtr, 0, 5);
   
   delete matPtr;
      // should cause "attempted to access nullptr" error
   showDynMat(*matPtr, 0, 5);
   
      // should automatically correct to 5x5 matrix without any problems
   showDynMat(testMat, 0, 6);
   
      // error free output
   showDynMat(testMat, 0, 5);
   
      // should both throw "Invalid percentage input" error
   testMat.populate(150);
   testMat.populate(-1);
   
   cout << "\n\n\n--------------------------\n\n"
   << "END OF SANITY CHECKS"
   << "\n\n--------------------------\n\n\n";
}


void multiplicationTiming(bool printingMatrices = false) {
   
   string filePath = "/Users/justin/Desktop/partA-CSV.csv";
   CSVWriter writer(filePath);
   
   cout << "\n--------------------------\n\n"
   << "START OF MULTIPLICATION TIMING"
   << "\n\n--------------------------\n\n\n";
   
   for (int size = 0 ; size < 3201 ; size += 100) {
      DynMat<TYPE> DynMatA(size);
      DynMatA.populate(PERCENT_POPULATED);
      
      if (printingMatrices) {
         cout << "Printing Input DynMat..." << endl;
         showDynMat(DynMatA, START, STOP);
      }
      
      clock_t startTime = clock();
      DynMat<TYPE> matB( multiplyDynMat(DynMatA, DynMatA, size) );
      clock_t stopTime = clock();
      
      double elapsedTime = (double)(stopTime - startTime)/(double)CLOCKS_PER_SEC;
      
      writer.writeToRow(size, elapsedTime);
      
      if (printingMatrices) {
         cout << "Printing Output DynMat..." << endl;
         showDynMat(matB, START, STOP);
      }
      
      cout << "size " << size << " matrix finished in " << elapsedTime << " seconds\n\n";
      
   }
   
   cout << "\n\n\n--------------------------\n\n"
   << "END OF MULTIPLICATION TIMING"
   << "\n\n--------------------------\n\n\n";
}


int main() {
   
   sanityChecking();
   multiplicationTiming(true);
   
   return 0;
   
}

/* ------------------------------
 *
 * WRITTEN PORTION (PART A)
 *
 * ------------------------------
 
 ****************************
 *
 * TIME COMPLEXITY ANALYSIS
 *
 ****************************
 
 const type* getRow(int row) const
 {
 return spine[row];                                    T(n) <= c*1
 }
 
 type* getCol(int col) const
 {
 type* colOut = new type[size];
 for (int i = 0 ; i < size ; i++) {                    T(n) <= c*n
 colOut[i] = spine[i][col];                            T(n) <= c*1
 }
 return colOut;
 }
 
 type& operator()(const int row, const int col) const
 {
 return spine[row][col];                               T(n) <= c*1
 }
 
 template<class type>
 type multiplyArrays(const type arrA[],
 const type arrB[],
 int size)
 {
 type total = type();
 for (int ind = 0 ; ind < size ; ind++) {              T(n) <= c*n
 total += ( arrA[ind] * arrB[ind] );                   T(n) <= c*1
 }
 return total;
 }
 
 template <class type>
 DynMat<type> multiplyDynMat(const DynMat<type>& matA,
 const DynMat<type>& matB,
 const int size)
 {
 DynMat<type> matOut(size);
 matOut.populate(0);
 
 for (int col = 0 ; col < size ; col++) {            T(n) <= n(n(c*n) + c*n)
 type* curCol = matB.getCol(col);                   T(n) <=   c*n
 for (int row = 0 ; row < size ; row++) {           T(n) <= n(c*n)
 matOut(row, col) =
 multiplyArrays(matA.getRow(row), curCol, size);    T(n) <= c*n
 }
 delete[] curCol; curCol = nullptr;                    T(n) <= c*1
 }
 return matOut;
 }
 
 In best and worst cases (best and worst cases are identical)
 
 Worst case:
 T(n) <= n( n(c * n) + (c * n) ) >= c(n^3)
 T(n)  = O(n^3)
 
 Best case (identical to worst case):
 T(n)  = omega(n^3)
 
 T(n)  = theta(n^3)
 
 **********************************
 *
 * QUESTIONS
 *
 **********************************
 
 
 1) Unsure of the meaning of "non-zero" run time:
 
 Input size of 1x1:  2 x 10^-6 seconds
 31x31:  0.0001 seconds
 549x549:  0.507 seconds
 685x685:  1.02 seconds
 
 
 2) starting n    (n=100)
 double n    (n=200), running time increases by factor of 7.49
 triple n    (n=300), running time increases by factor of 26.5
 quadruple n (n=400), running time increases by factor of 60.73
 
 input size      runtime(sec)
 0         1.40E-05
 100         0.003259
 200         0.024439
 300         0.08635
 400         0.197903
 500         0.399974
 600         0.831392
 700         1.16752
 800         1.93123
 900         2.52494
 1000         3.26175
 
 
 3) Using input sizes above 5100x5100 (finished in 407 seconds) caused my
 computer to begin heating up a little more than I was comfortable with
 so I decided to stop the testing there.
 
 
 4) When running time is graphed against input size and compared to the lines:
 T(n) = c*(n^3) and T(n) = c*(n^2),
 
 running time appears to be bounded above by c*(n^3)
 when c = (1/320000000)
 for all n > 1400 --> O(n^3)
 
 running time appears to be bounded below by c*(n^3)
 when c = (1/330000000)
 for all n > 3800 --> omega(n^3)
 
 running time: theta(n^3)
 running times are consistent with initial estimate
 
 
 *---------------------------------
 *
 * SAMPLE RUN
 *
 * --------------------------------
 
 "/Users/justin/Documents/Documents/Projects/CLion Projects/CS2C/cmake-build-debug/Assignment3A"
 
 --------------------------
 
 START OF SANITY CHECKS
 
 --------------------------
 
 
 ERROR: Matrix is not populated
 ERROR: Out of bounds access
 ERROR: Matrix is not populated
 
 7       0       3       8       0
 9       9       7       0       5
 0       2       2       0       0
 0       0       7       6       0
 0       5       5       2       0
 
 ERROR: Attempted to access nullptr
 
 0       0       0       0       0
 0       0       0       0       0
 0       0       0       0       0
 0       0       0       0       0
 0       0       0       0       0
 
 0       0       0       0       0
 0       0       0       0       0
 0       0       0       0       0
 0       0       0       0       0
 0       0       0       0       0
 ERROR: Invalid Percentage Input
 ERROR: Invalid Percentage Input
 
 
 
 --------------------------
 
 END OF SANITY CHECKS
 
 --------------------------
 
 
 
 --------------------------
 
 START OF MULTIPLICATION TIMING
 
 --------------------------
 
 
 Printing Input DynMat...
 
 Printing Output DynMat...
 
 size 0 matrix finished in 3e-06 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0     6.9       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0     8.5       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0    0.11       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 size 100 matrix finished in 0.0031 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0     8.9       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0     1.5       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 size 200 matrix finished in 0.024 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0     1.8       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0    0.58
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0     3.3       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 size 300 matrix finished in 0.087 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0     9.4       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0    0.89       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0     7.8       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0      35       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0      18
 0       0       0       0       0       0       0       0       0       0       0       0      28       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0      36       0       0
 2.4       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0     3.5       0       0       0       0      42       0       0       0       0       0       0      61       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 size 400 matrix finished in 0.23 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0     1.2       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       7       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0     5.8       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 0      14       0       0       0       0       0       0       0       0       0       0       0       0       0
 0      24       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0     8.5       0       0      35       0
 0       0       0       0      43       0       0       0       0     3.5       0       0       0       0     3.5
 41       0       0       0       0       0       0       0       0       0       0       0     2.4       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0      33       0       0       0       0       0       0     1.4
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0      11
 5       0       0       0      14       0       0       0       0       0      40       0       0       0       0
 1      14       0   0.081       0       0       0       0       0       0       0       0       0       0       0
 size 500 matrix finished in 0.43 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0     9.5       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0      66       0       0      53       0       0
 0       0      51       0       0       0       0      22       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0      44       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0     3.8       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0     8.9       0       0      27       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0     6.8       0       0       0       0       0
 0       0       0       0       0      51       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0      34       0      58      27       0       0
 0       0       0       0       0       0       0       0       0       0       0       0      21       0       0
 size 600 matrix finished in 0.72 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0     5.3       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       3
 0       0       0     9.8       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 0       0       0       0      35       0      27       0       0       0       0       0       0       0       0
 0       0       0       0       0      27       0     3.3       0       0       0       0       0       0       0
 0       0       0       0       0       0       0     5.4       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0      51       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 36      61       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0      34
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0      15       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 size 700 matrix finished in 1.1 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       3       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 0       0       0       0       0       0       0      34       0       0       0       0       0       0       0
 0       0      26       0       0       0       0       0       0       0       0       0       0       0       0
 23       0       0       0       0      16       0       0      59       0      14       0       0       0       0
 0      27       0       0       0       0       0       0      23       0       0       0       0       0       0
 0       0      47     5.8       0      15       0       0      74       0       0       0       0       0       0
 0       0       0       0       0       0       0       0     6.2       0       0       0       0       0       0
 0       0       0       0       0       0     3.4       0       0       0       0       0    0.33       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0      67       0       0      32       0       0       0       0       0      67
 0       0       0      35       0      44       0       0       0      32       0       0       0       0       0
 0       0       0       0       0       0       0       0      43       0       0       0      47       0       0
 0       0       0       0       0       0       0       0       0       0       0       0      23       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0     3.7       0       0
 0       0       0       0       0       0       0       0       0       0       0       0      16       0       0
 size 800 matrix finished in 1.6 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0     8.9       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0     9.8       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0      56
 0.22       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0      43       0       0       0       0       0       0       0       0      31       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0      30       0       0       0       0       0       0      64
 0       0       0       0     5.9       0      14       0       0       0       0      37       0       0      42
 14       0       0      28       0       0       0       0       0       0       0      20       0       0       0
 0       0       0       0       0      45     3.6       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 21       0      88       0       0       0       0       0       0       0       0      31       0       9       0
 0       0       0     5.6       0      20       0       0       0       0      20       0       0       0       0
 0       0       0       0       0       0       0       0       0     8.8      63       0     9.8       0       0
 0       0    0.88       0       0       0       0       0       0       0       0       0       0       0       0
 size 900 matrix finished in 2.4 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0     4.7       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0     9.7       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       2       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0     3.9       0       0       0       0       0       0     9.4       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 0       0       0       0       0     9.2       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       2
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0      14       0       0      41       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0      19       0       0       0      65      20       0       0       0       0       0       0      18       0
 0       0       0       0       0       0       0     1.4      72       0       0       0      61       0       0
 0     7.2       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0      38       0       0       0      37      42       0      32       0       0       0      88       0       0
 0       0       0       0       0       0       0       0       0       0      46       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 size 1000 matrix finished in 3.2 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0    0.28       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0     8.3       0       0       0       0     6.5       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0      76
 0       0       0      27       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0      70       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0      23       0       0       0      26       0       0      13       0
 0       0       0       0       0       0       0     6.5       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0      12       0       0       0       0       0       0
 0      62       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0     2.6       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0     3.3       0       0       0       0       0
 0       0       0       0       0      52       0       0       0     4.9       0       0       0       0       0
 size 1100 matrix finished in 4.2 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0     4.7       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       7       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0     4.3       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0     5.5       0       0       0       0       0       0
 Printing Output DynMat...
 
 0       0      29       0      67       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0     3.4       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0     2.8       0
 0       0       0       0       0       0       0       0       0      55       0       0       0       0       0
 0       0      19       0       0       0       0       0       0       0       0      14       0       0       0
 0       0      52       0       0      37       0       0       0       0       0       0       0     5.4       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0     3.6       0       0       0     1.5       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0     1.4       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0      68       0       0       0       0
 0       0       0       0       0       0       0       0      18       0       0       0       0       0       0
 0       0       0      31       0       0       0       0       0       0       0       0       0     5.1       0
 size 1200 matrix finished in 5.8 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0     7.6       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 7.3       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 0       0      12       0     3.1       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0      16       0       0       0       0       0       0       0       0       0      30       0
 0       0       0       0       0     0.2       0       0       0       0       0       0       0       0       0
 43       0       0       0     8.9       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0      43       0       0      66       0      37       0       0       0       0
 0       0      48       0       0       0       0       0       0       0       0       0       0       0       0
 0      78       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0     8.1      82       3
 0       0       0      14       0       0      37       0       0       0       0       0       0      74       0
 0       0       0       0       0       0       0       0       0       0      36       0       0       0       0
 0       0     6.7       0       0       0       0       0       0       0       0       0       0      14       0
 0       0       0      49       0       0       0       0       0       0       0       0       0       0       0
 0       0       0     3.6       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0      33       0       0       0      49
 size 1300 matrix finished in 6.3 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0     6.9       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0     3.4       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 33       0       0       0       0     8.5      22      23       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0      65       0       0       0
 0     9.2       0       0       0       0       0       0       0     1.9       0       0       0       0       0
 0      13       0       0       0       0       0       0       0       0       0       0       0       0       0
 2.4      28       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0      44       0       0       0       0       0       0       0
 0       0       0       0      29       0       0       0       0       0       0       0      20       0       0
 0       0       0       0       0       0       0       0      34       0       0       0       0       0       0
 18       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0      17       0       0       0       0       0       0       0       0      29       0       0
 0       0       0       0       0       0     9.7       0       0       0       0       0      22       0      13
 21     7.2       0       0       0       0       0       0       0       0       0     6.3       0       0       0
 0     9.1       0       0       0      16       0       0       0       0       0       0       0       0       0
 0      39       0       0       0       0       0       0      22       0       0      30      12       0       0
 0       0       0     6.6       0      11       0       0       0       0       0       0      31       0       0
 size 1400 matrix finished in 8.5 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 0       0       0       0       0       0       0       0       0      28       0       0       0     2.9       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0      68
 0.81       0       0       0       0       0       0       0       0       0      27       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0      27       0
 0       0       0       0       0     8.3       0       0       0       0       0       0       0       0       0
 0      27       0       0      32       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0      17       0       0      31       0       0       0       0
 0       0       0      69       0       0       6       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 51       0       0     3.8       0       0       0       0      48       0      11       0       0       0       0
 0   1e+02       0       0       0      70       0       0       0    0.77       0       0       0       0       0
 0       0     2.2       0       0       0       0      48       0       0       0      80       0       0       0
 0       0       0       0       0       0       0       0       0       0     2.3       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0     2.8       0       0
 0       0       0      27       0       0       0       0       0      48      59       0       0       0      76
 size 1500 matrix finished in 10 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 0      20       0       0       0      53       0       0       0       0      11       0       0      35       0
 0       0       0       0       0       0       0      86       0      65      41       0       0       0       0
 0       0       0       0       0     0.8       0       0       0       0       0       0       0       0       0
 0       0       0       0   1e+02       0       0       0       0      33       0       0       0       0       0
 0       0       0      44       0       0       0       0       0      12       0       0       0       0       0
 0      76       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0     5.7       0       0       0       0       0       0       0       0       0       0       0       0
 0       0      79       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0      20       0       0       0       0
 0      58       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0      38       0       0       0       0       0
 0       0     5.2       0      25       0       0       0      57       0       0      44       0       0       0
 0       0       0       0       0       0      31       0       0       0       0      25       0       0      37
 0       0       0       0       0      18       0       0      49       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 size 1600 matrix finished in 12 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0     7.1       0       0     2.7       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0     6.5       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0     1.2       0
 Printing Output DynMat...
 
 23       0       0       0       0       0      26      15      38       0       0       0       0       0       0
 88      68       0       0       0       0       0     4.2      18       0       0       0      40       0       0
 0       0      28       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0      18      12      22       0       0       0       0       0       0       0       0       0
 0       0       0       0       0      18      24       0       0       0       0       0       0       0       0
 0       0      48       0       0       0      15       0       0       0       0       0      17       0       0
 0       0      15       0       0       0       0       0       0       0       0       0    0.36      46    0.29
 0      25       0      39       0       0       0      69       0       0       0       0       0       0       0
 0       0      22       0     4.2       0       0       0       0       0       0       0       0       0       0
 0     4.7       0       0      66       0       0       0       0      75     3.3       0       0    0.97       0
 52       0       0       0       0       0       0       0       0       0       0      19       0       0       0
 0       0       0      49       0       0       0       0      26       0      75       0       0       0       0
 0       0       0       0      80       0       0       0      12       0       0      60       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0     3.5     4.8       0       0       0       0
 size 1700 matrix finished in 15 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0     7.4       0       0       0       0       0     5.3       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 0     1.2     2.6      45       0       0       0      25       0       0       0     5.9       0       0      16
 29       0    0.21      36       0       0       0       0       0       1      18       0       0       0       0
 0       0       0       0      54       0       0       0       0       0       0       0       0       0      21
 0       0       0       0       0      67       0       0       0       0       0       0       0       0       0
 0       0      13       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0      13      23       0       0      58       0      50     3.6       0      19       0
 0       0       0       0       0      10       0       0       0       0      24    0.25       0       0       0
 0       0      14       0       0       0       0       0       0       0       0       0       0       0       0
 0      43       0       0      54       0       0       0       0       0       6       0      17       0       0
 0       0       0       0       0       0       0       0       0       0      24       0       0       0       0
 0       0       0       0       0       0      13       0       0       0       0       0       0       0       0
 0       0      47       0       0       0      80       0      44       0       0       0       0      14       0
 0       0       0       0       0       0       0       0      12       0       0       0       0      31       0
 0       0       0       0       0       0       0       0       0       0      48      30       0       0       0
 0     4.1      27       0       0       0       0       0      17      32      17       0       0       0      25
 size 1800 matrix finished in 18 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0     6.1       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0     2.2       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0      31       0       0       0
 0       0     8.3      11       0       0       0       0      41       0       0     1.5       0      69       0
 0       0       0       0       0       0       0      50       0       0       0      43       0      54       0
 0       0       0      44       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0      26       0    0.42       0      58       0       0       0       0
 0       0       0       0       0       0       0       0       0     9.9      26       0       0       0       0
 0       0       0       0       0       0      31       0       0      18       0     5.5       0      12       0
 0       0       0       0       0       0     1.5     0.4       0    0.71       0       0       0       0       0
 0       0       0     7.9       0       0       0       0      33      63    0.19       0       0      40       0
 0     4.2       0     2.8       0       0       0       0       0       9       0     4.1       0       0      15
 23       0       0      52       0       0       0       0       0       0       0       0       0       0       0
 26       0       0       0    0.25       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       5       0       0      47       0
 0       0       0       0       0      55       0       0       0      78       0       0       0       0       0
 0       0       0       0       0       0      75       0       0      35       0      25       0       0       0
 size 1900 matrix finished in 21 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       8       0
 0       0       0       0       0       0       0       0       0       0       0       0    0.78       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0     6.8       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 4.3       0       0       0       0       0       0       0       0       0       0       0       0       0    0.85
 1.2e+02       0       0       0       0       0       0       0      45     8.6     1.7       0       0       0       0
 82       0       0       0       0       0       0       0       0       0       0       0      50       0       0
 0       0       0       0       0       0       0       0      21       0     5.3       0       0       0      24
 0       0       0       0       0       0       0       0       0      34       0       0      11     4.2       0
 0       0     6.8       0       0       0       0       0       0     7.8       0      48       0       0       0
 0       0       0       0       0       0       0      11       0       0       0     1.9       0       0       0
 0      38       0      18       0      32       0       0       0       0     8.8       0       0       0       0
 26       0       0       0       0       0       0       0       0       0       0      39       0       0       0
 84       0       0       0       0       0       0      49       0     5.4       0      83       0      23       0
 0      54       0       0       0       0       0      46       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0     8.5       0       0       0       0
 0       0       0       0       0       0       0       0       0      30       0       0     5.5       0       0
 0       0       0      46       0       0      15      31       0       0       0       0       0       0      68
 0      43       0       0       0       0       0       0       0       0       0      33     5.6       0       0
 size 2000 matrix finished in 25 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0     3.2       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0     3.4       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 3.8      35       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0      93       0       0       0       0       0       0       0     2.3       0       0
 0       0       0       0       0      19       0       0       0       0      76       0       0      34       0
 0       0       0       0       0       0       0       0      20       0       0      18       0       0       0
 0       0      60       0       0      97       0       0      27       0       0       0      25       0       0
 0      81       0       0       0      34       0       0       0       0       0      12       0       0       0
 24       0       0       2       0      11       0      46       0      26       0       0       0       0       0
 0       0       0       0       0       0      40       0       0     1.3       0       0       0       0      34
 0       0       0       0       0       0       0       0      87      21     9.3       0       0       0       0
 0       0       0       0       0      34       0       0       0       0       0       0       0       0       0
 0       0       0       0       0      79       0       0       0       0      11       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0      20       0       0      13       0       0       0     6.7
 24       0       0       0     4.2       0       0       0       0     2.3     4.3      11       0       0       0
 0       0       0     3.1       0       0       0       0      61      84       0       0       0       0       0
 size 2100 matrix finished in 28 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 8.1       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       9       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 1.8       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 0      72       0       0       0       0       0       0       0       0       0       0      59    0.33       0
 5.8       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0      17       0      33       0      61      30       0       0       0       0
 16       0       0       0       0       0       0       0       0       0       0       0       0     1.1     7.4
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0      22       0       0       0       0       0      24
 0       0       0       0       0      42       0       0     5.7       0      24       0      62       0       0
 0       0      22       0       0       0       0    0.35       0       0       0      10       0       0       0
 0       0       0       0       0       0      44       0       0      23       0       0     8.8    0.26       0
 0       0       0       0       0       0       0 1.3e+02       0       0       0       0       0       0       0
 34      43       0       0       0       0       0       0       0       0       0       0       0       0      14
 0     9.3       0       0       0       0       0      17       0      22     4.6       0       0      61       0
 0      48       0       0       0       0       0       0       0       0       0      13       0       0       8
 0       0       0       0       0       0       0       0       0       0       0       0       0     7.1       0
 24      54       0       0       0     1.1       0       0       0       0       0       0       0       0       0
 size 2200 matrix finished in 32 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0     1.8       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0     7.4       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0     2.5       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 0       0      26       0      12       0       0       0       0       0       0       0       0       0      11
 0       0       0       0       0      31       0       0       0       0       0      46     6.9       0       0
 0      11       0      39       0     7.9       0       0       0       0       0       0       0       0       0
 0       0       0       0       0      66       0       0       0       0      55       0      46       0       0
 0       0      45       0       0      17       0       0       0       0       0       0      12       0       0
 0     8.7       0      53       0       0       0       0       0       0       0       0       0      33       0
 3.8       0       0       0       0      16       0       0     4.6       0       0       0       0       0       0
 0      42       0      31       0      13       0      49       0       0      48       0       0       0       0
 0       0       0       0       0       0       0       0     3.4       0       0       0       0       0       0
 0      20       0      97      81       0       0       0       0     3.1       0       0       0       0       0
 0       0       0       0      31      89       0      38       0       0       0      55      36       0       0
 27       0       0    0.78       0       0       0     7.2       0       0      74       0       0       0       0
 0       0       0       0       0       0     5.9       0       0      23       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0     6.6     7.4       0       0     5.6
 0       0       0      28     8.9       0       0       0       0       0       0       0       0      95       0
 size 2300 matrix finished in 37 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0     9.6       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0     5.8       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0     4.9       0       0       0       0       0       0       0       0       0       0
 0       0       0       0    0.31       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 0       0       0     7.6       0       0      28       0     1.3       0       0       0       0       0       0
 13       0       0       0       0       0       0       0       0       0       0      43       0       0       0
 0       0      33       0       0       0       0      62       0      57       0       0      64       0      39
 0       0       0       0       0       0     5.5       0       0    0.52       0      61       0       0       0
 0       0       0       0       0       0       0       0       0       0       0      57       0       0       0
 0       0       0     7.3      20  0.0012       0       0       0       0       0       0       0       0       0
 0     5.4       0       0       0       0      17       0      12       1       0       0       0       0       0
 0       0       0       0       0       0     4.7       0       0      54       0       0       0      51       0
 23       0       0       0       0       0       0       0      17       0       0       0      83       0       0
 0       0       0       0      57      15       0       0      48       0       0       0       0       0       0
 0       0       0       0      23       0       0      53       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0      39       0       0       0       0       0       0
 0       0     3.5       0       0       0     8.1     6.8       0       0       0     4.1       0       0       0
 0       0       0       0       0       0       0       0       0     2.1       0      41       0       0       0
 0       0       0       0       0     2.2       5       0       0       0       0       0       0       0       0
 size 2400 matrix finished in 44 seconds
 
 Printing Input DynMat...
 
 0     4.5       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0    0.52       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0    0.66       0
 Printing Output DynMat...
 
 33       0      67       0       0      16       0      46       0       0      29       0       0     0.9       0
 0      27       0      16       0       0       0       0       0       0       0       0       0       0       7
 0      31       0       0       0       0      17       0       0       0       0       0      26       0       0
 0      54       0       0       0       0      16      25       0       0      37       0       0       0       0
 0       0       0       0       0       0       0       0       0       0      25       0       0       0       0
 0       0     9.1       0       0       0       0       0     9.8      95       0       0       0       0       0
 0       0      12     9.1       0       0       0       0      15       0       0      25       0      51     8.1
 0       0       0       0      31       0       0     3.6      19       0       0       0       0       0       0
 0      46       0      53       0      30      27       0       0      26      32       0       0       0       0
 0      32       0       0       0       0       0      62       0       0       0      49       0      77       0
 0       0       0       0      44       0       0       0       0       0       0      40       0       0       0
 0      51      19       0       0      23      73       0       0     2.7      26      32       0      11      18
 0      21       0       0       0     9.9       0       0     1.7       0       0       0       0       0      73
 0       0       0       0       0       0       0       0       0       0      32      26       0      54     0.2
 0       0       0     4.5       0      91      79       0       0       0       0     4.3       0       0       0
 size 2500 matrix finished in 49 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 3.4       0       0     3.3       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0     2.8       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 0      84       0      12       0       0       0       0       0     3.8       0       0       0       0       0
 54       0       0       0       0       0       0       0       5      44       0       0       0       0       0
 0       0       0       0       0       0      77      44      64       0       0       0 2.2e+02       0      50
 19       0       0      35      39       0       0       0       0       0       0       0       0       0       0
 0      25       0       0       0      17       0      14       0       0      58       0       0       0       0
 0     9.5       0       0       0       0      13       0      17       0       0       0       0       0       0
 0       0       0       0     1.1       0       0       0       0       0       0       0       0       0       0
 0      43      11       0       0       0       0       0       0       0       0       0      28       0       0
 0       0      19       0       0      10       0     3.6    0.16      32       0       0       0       0       0
 0       0       0     9.9       0       0      57       0       0     2.4       0       0       0       0       0
 0       0      40     2.2     9.7       0       0     2.4       0       0       0      48       0       0      53
 0      13       0       0       0       0       0       0       0       0       0       0       0     6.7      20
 0       0       0       0       0     2.6       0      18    0.36       0       0       0       0       0       0
 0       0       0       0      31       0       0       0      17       0       0      14       0      57       0
 0       0       0      38       0       0       0       0       0       0       0       0       0       0       0
 size 2600 matrix finished in 52 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0     4.1
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0     6.1       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 0       0      27       0       0      60       0       0       0       0       0       0       0      10       0
 0       0       0       0      26       0       0      42      12     9.2      10     9.4       0       0       0
 0       0       0      56       0       0       0       0     4.1       0       0       0       0       0       0
 0       0      20       0       0       0       0       0       0       0       0       0       0       0       0
 31       0       0       0      65       0       0       0       0       0       0       0       0       0      13
 64       0       0       0       0       0       0       0       0       0     8.6       0      34       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0      20       0       0       0       0       0       0      18     5.5       0       0       0       0
 3.5       0       0       0       0       0       0       0       0       0       0       0       0      49       0
 0       0       0       0       0       0     3.1       0       0      50       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0      21       0       0      24
 0       0      56       0      25       0       0       0       0       0       0       0     2.3       0      11
 0       0       0       0       0       0       0       0       0       0       0      63    0.11       0       0
 5.9       0      54      35       0      66       0       0       0       0       0       0       0       0       0
 0       0       0       0       0     3.5       0       0       0       0       0       0       0       0       0
 size 2700 matrix finished in 58 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0     8.8       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       2       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 0       0       0       0       0       0       0       0      29       0       0       0       0       0       0
 0       0       0    0.95       0       0       0       0       0       0       0       0    0.74     3.8       0
 54       0       0      35       0     2.4       0      16       0       0       0       0       0      74      26
 1.5       0     8.7     1.3       0       0       0       0       0       0       0       0       0       0       0
 82      65      55       0       0       0       0       0       0      11       0       0       0      64       0
 0       0       0       0      13      20       0       4       0      48       0       0       0      77      75
 9.2       0       0      22       0       0       0      51       0       0       0       0       0       0      80
 0       0       0       0       0       0       0     4.8       0       0       0       0       0     1.3       0
 23       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0     8.1       0       0       0       0       0     2.2       0       0       0       0       0       0
 4.5       0      52       0     9.7       0       0      16       0       0       0       0       0      26       0
 0       0       0       0      10       0       0       0      40       0       0      33      67      19      18
 0      71       0      13       0     1.7       0      25       0       0     5.8      18       0       0       0
 0       0       0      19       0     4.4       0       0       0       0       0       0     9.2       0       0
 0      23       0       0       0       0       0     8.5       0       0       0       0       0      55       0
 size 2800 matrix finished in 64 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0     9.4       0       0       0     5.1       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0     2.1       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0     8.5       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 0       0       0       0       0       0       0       0      54       0       0       0       0      45       0
 0       0     1.2       0       0      17       0       0      70       0       0       0      15      60       0
 0     9.5       0       0      17      42       0       0       0       0       0       0       0       0       0
 0       0       0      67       0       0       0       0       0      53       0       0     9.4       0       0
 0      25       0       0      26       0      37       0       0       0     1.3       0       0     2.6      36
 0      12     8.9       0       0 1.1e+02       0       0       0 1.1e+02       0       0       0     3.3      37
 0 1.3e+02       0      76       0      31       0       0       0      40     5.7       0       0       0      21
 0       0       0      42       0       0      33       0       0       0       0       0      35       0      69
 0     2.7       0       0      15      22       0       0      10       0       0       0       0     5.7       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0      25       0       0       0       0       0       0      49       0      40       0       0
 0       0      78       0      37       0       0       0       0       0       0       0      57       0       8
 0       0       0       0     6.3      13       0       0       0      99       0       0       0      17       0
 24       0       0    0.76       0       0       0       0       0       0    0.34       0       0       0       0
 0       0       0      42       0       0       0       0       0       0       0       0       0      14       0
 size 2900 matrix finished in 71 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0     7.7       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 2.5       0      50       0      48       0       0       0       0       0      36       0       0       0       0
 0       0       0       0       0       0       0       0      15       0      34       0      24      62       0
 0       0      31       0       0       0       0     5.2      34       0       0       0       7     1.4       0
 0       0       0       0       0      12       0       0       0       0       0       0       0       0       0
 0       0       0       0       0      26       0       0       0       0     3.5       0       0       0       0
 0       0       0       0       0      24       0       0       0       0       0      40      70      60       0
 16     7.3       0      83       0       0       0       0       0       0      22       0       0    0.62       0
 0 1.2e+02       0       0       0       0       0       0      35       0       0       0       0       0       0
 8.8       0       0       0       0       0       0       0     3.5       0     7.8       0       0       0       0
 0      41       0       0    0.78       0       7       0       0     5.7     4.9       0       0       0       0
 0      80       0       0       0       0       0       0       0      14      23       0       0       0       0
 0       0       0       0 1.3e+02       0      13      27       0      11       0       0       0     4.4       0
 0       0       0       0      28       0       0       0       0     5.7      12       0       0       0       0
 39       0       0       0     5.4      10      64       0       0       0       0       0     6.1      50       0
 5.2      17      50       0      15       0       0      13 1.1e+02       0       0       0       0      71       0
 size 3000 matrix finished in 77 seconds
 
 Printing Input DynMat...
 
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0    0.86       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 0       0       0       0       0       0       0       0       0      32      26       0      24       0       0
 14       0       0       0       0       0     5.1       0       0       0   1e+02       0       0       0       0
 0       0       0       0       0       0      28       0       0      66       0      36       0      77       0
 0       0       0       0       0       0       0       0       0      47       0       0       0      24       0
 5       0       0      59       0       0       0      70       0       0   0.012      12       0       0       0
 0       0       0       0       0       0       0       0       0      50       0      50      33     2.2       4
 0     3.9       0       0       0       0       0       0       0       0       0       0       0       0       0
 8.2       0       0      36       0       0       0       0      58       0   0.017       0       0       0       0
 2.7       0      63       0       0       0    0.34       0       0      29       0       0       0       0       0
 0       0      22       0       0       0      21       0      54       0       0       0      25       0       0
 0       0      34       0       0       0       0      46      83       0       0       0       0       0       0
 0       0       0       0       0       0      25       0       0       0     7.3      20       0       0      29
 0       0       0       0       0      32      28       0       0       0      34       0       0       0       0
 0      16       0       0       0       0     0.7       0      44      84       0       0       0       0       0
 0      45       0       0      10       0       0      18       0       0      65       0       0       0      45
 size 3100 matrix finished in 92 seconds
 
 Printing Input DynMat...
 
 0       0       0    0.86       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 3.8       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0    0.22       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 0       0       0       0       0       0       0       0       0       0       0       0       0       0       0
 Printing Output DynMat...
 
 3.3       0       0      79       0       0       0      13       0       0       0       0      43      23       0
 0       0       0       0       0       0       0       5     3.4       0    0.44       0       0      15      25
 0       0       0       0       0       0       0     1.4       0       0       0       0       0     5.7       0
 0       0       0      15       0      21       0       0       0       0       0       0       0       0       0
 1.1       0      70       0      13       0       0     2.2      47       0       0       0       0       0       0
 0     3.8       0       0       0       0       0     2.4       0      45      27       0       0       0       0
 0       0     0.1     2.3      45       0       0      33       0       0      11       0       0      30      21
 1.3       0       0       0       0       0       0       0     9.4       0       0       0      69       0       0
 35       0       0       0       0      51       0      20       0       0       0       0     3.1       0     9.2
 0       0       0      18       0      53       0       0       0       0      29       0       0       0       0
 0       0      50       0       0       0      25      20       0       0       0       0       0       0       0
 0       0      35       0       0       0       0       0     2.9       0       0       0       0       0      51
 0       0       0       0       0      60       0       0       0     5.9      10       0       0       0       0
 0      28       0      14       0      25       0       0      11       0      17       0      32      32       0
 0       0       0       0       0       0       0       0       0       0       0      89       0      15       0
 size 3200 matrix finished in 92 seconds
 
 
 
 
 --------------------------
 
 END OF MULTIPLICATION TIMING
 
 --------------------------
 
 
 
 Process finished with exit code 0
 
 */

   //
   // Created by Justin on 10/13/17.
   //

/***********************
 *
 * PART B
 *
 ***********************/

#include <SparseMat.h>
#include <fstream>
#include <utility>
#include <vector>

using namespace std;

template <class dataType>
class SparseMatWMult : public SparseMat<dataType> {
protected:
   typedef FHlist<MatNodePtB<dataType> > MatRow;
   typedef SparseMatWMult<dataType> mat;
   static constexpr dataType defaultValue = 0.0;
   
public:
   SparseMatWMult(int rows, int cols, dataType& zeroVal)
   : SparseMat<dataType>(rows, cols, zeroVal) { }
   
   bool matMult(mat& matA, mat& matB) {
      this->clear();
      
      int matACols = matA.getColSize(); int matBCols = matB.getColSize();
      int matARows = matA.getRowSize(); int matBRows = matB.getRowSize();
      
      if (matACols != matBRows or
          matACols < 1 or matBCols < 1 or
          matARows < 1 or matBRows < 1) {
         return false;
      }
      
         // for each row in matA
      for (int rowA = 0 ; rowA < matA.getRowSize() ; rowA++) {
         MatRow matARow = matA.rows[rowA];
         if (!matARow.empty()) { // iterate through matARow
            for (int colA = 0 ; colA < matACols ; colA++) {
               dataType valA = matA.get(rowA, colA);
               if (valA != defaultValue) {
                  MatRow matBRow = matB.rows[colA];
                  if (!matBRow.empty()) { // iterate through matBRow
                     for (int colB = 0 ; colB < matBCols ; colB++) {
                        dataType valB = matB.get(colA, colB);
                        if (valB != defaultValue) {
                              //TODO: add += functionality
                           dataType valC = this->get(rowA, colB) + (valA*valB);
                           this->set(rowA, colB, valC);
                        }
                     }
                  }
               }
            }
         }
      }
      return true;
   }
};

#define TYPE double
#define SIZE 5
#define RAND_RANGE 100

template <class dataType>
void randPopulateSparseMat(SparseMatWMult<dataType>& matIn, float percentPopulated) {
   int rows = matIn.getRowSize();
   int cols = matIn.getColSize();
   int fields = rows * cols;
   int filledVals = (int) ( (percentPopulated/100)*(fields) );
   
      //TODO: improve randomization
   while (filledVals--) {
      int randRow = (int) (((float)rand()/(float)RAND_MAX)*rows);
      int randCol = (int) (((float)rand()/(float)RAND_MAX)*cols);
      dataType randVal = (dataType) (((double)rand()/(double)RAND_MAX)*RAND_RANGE);
      matIn.set(randRow, randCol, randVal);
   }
}

template <class dataType>
void readtoMat(string& matStr, SparseMatWMult<dataType>& matIn)
{
   matIn.clear();
   stringstream str(matStr);
   dataType inputVal;
   
   int numRows = matIn.getRowSize();
   int numCols = matIn.getColSize();
   for (int row = 0; row < numRows ; row++) {
      for (int col = 0 ; col < numCols ; col++) {
         str >> inputVal;
         matIn.set(row, col ,inputVal);
      }
   }
}

class CSVWriter {
   
   string filePath;
   string delimiter;
   int lineNum;
   
public:
   explicit CSVWriter(string pathIn, string delim = ",") :
   filePath(move(pathIn)),
   delimiter(move(delim)),
   lineNum(0) { }
   
   template<class col1, class col2>
   void vectorToRow(col1 size, vector<col2> timeVec) {
      fstream csvFile;
      csvFile.open(filePath, ios::out | ( lineNum ? ios::app : ios::trunc) );
      
      csvFile << size;
      for (col2 data : timeVec) {
         csvFile << delimiter << data;
      } csvFile << "\n";
      lineNum++;
      csvFile.close();
   }
};


void correctnessTest() {
   
   typedef SparseMatWMult<TYPE> typeMat;
   
   int size = SIZE;
   TYPE defaultVal = TYPE();
   
      // Create and populate matA
   typeMat matA(size, size, defaultVal);
   string matAStr =
   " 01.00 02.00 03.00 04.00 05.00 "
   " -1.00 -2.00 -3.00 -4.00 -5.00 "
   " 01.00 03.00 01.00 03.00 01.00 "
   " 00.00 01.00 00.00 01.00 00.00 "
   " -1.00 -1.00 -1.00 -1.00 -1.00 ";
   readtoMat(matAStr, matA);
   cout << "printing matA..." << endl;
   matA.showSubSquare(0, size);
   
      // Create and populate matB
   typeMat matB(size, size, defaultVal);
   string matBStr =
   " 02.00 01.00 05.00 00.00 02.00 "
   " 01.00 04.00 03.00 02.00 07.00 "
   " 04.00 04.00 04.00 04.00 04.00 "
   " 07.00 01.00 -1.00 -1.00 -1.00 "
   " 00.00 00.00 08.00 -1.00 -6.00 ";
   readtoMat(matBStr, matB);
   cout << "printing matB..." << endl;
   matB.showSubSquare(0, size);
   
   typeMat matC(size, size, defaultVal);
   matC.matMult(matA, matB);
   cout << "printing matC = matA * matB..." << endl;
   matC.showSubSquare(0, size);
   
   typeMat matD(size, size, defaultVal);
   matD.matMult(matB, matA);
   cout << "printing matD = matB * matA..." << endl;
   matD.showSubSquare(0, size);
   
   cout << "\n\nEND OF CORRECTNESS TEST" <<
   "-----------------------\n\n";
}

void timeTest(bool testingOtherFills = false) {
   
   typedef SparseMatWMult<TYPE> typeMat;
   
   TYPE defaultVal = TYPE();
   string filePath = "/Users/justin/Desktop/partB-CSV.csv";
   CSVWriter writer(filePath);
   vector<float> percentVec = {1};
   if (testingOtherFills) {
      percentVec.push_back(0.5);
      percentVec.push_back(0.1);
   }
   writer.vectorToRow("size", percentVec);
   
   for (int size = 0 ; size < 4200 ; size += 100) {
      vector <double> times;
      for (float percentFilled : percentVec) {
         typeMat matA(size, size, defaultVal);
         randPopulateSparseMat(matA, percentFilled);
         
         typeMat matB(size, size, defaultVal);
         randPopulateSparseMat(matB, percentFilled);
         
         typeMat matC(size, size, defaultVal);
         
         clock_t startTime = clock();
         matC.matMult(matA, matB);
         clock_t stopTime = clock();
         double elapsedTime = (double)(stopTime - startTime)/(double)CLOCKS_PER_SEC;
         times.push_back(elapsedTime);
      }
      double time1 = times[0];
      cout << size << "x" << size << " matrix finished in " << time1 << " seconds\n\n";
      writer.vectorToRow(size, times);
   }
}


int main() {
   
   correctnessTest();
   timeTest();
   
   return 0;
   
}

/* ------------------------------
 *
 * WRITTEN PORTION (PART B)
 *
 * ------------------------------
 
 1) Keeping "sparseness" constant (10% fill rate),
 running times using sparse matrix multiplication
 are longer for large input matrices (larger than ~1400x1400).
 This difference becomes especially pronounced at very large input sizes.
 However, the sparse matrix multiplication is much more efficient at lower
 fill rates (1% and 0.5% were tested)
 
 2) Keeping "sparseness" constant (10% fill rate),
 growth rates using sparse matrix multiplication are also much larger.
 (see question 3)
 
 3) starting at (n=100)
 double n    (n=200), run time increases by factor of 9.34
 compare to 7.49
 triple n    (n=300), run time increases by factor of 36.41
 compare to 26.5
 quadruple n (n=400), run time increases by factor of 95.73
 compare to 60.73
 
 size     1%          0.5%       0.1%
 0       0.001098     0.000367    0.000132
 100    0.001023    0.000514    0.000106
 200    0.009554    0.00457     0.001097
 300    0.03725     0.015417    0.002349
 400    0.097929    0.035199    0.00499
 500    0.201835    0.06794     0.008605
 600    0.380672    0.137197    0.017381
 700    0.632136    0.223941    0.032197
 800    1.02434     0.319353    0.043856
 900    1.57369     0.532668    0.063024
 1000     2.37442     0.77863     0.088436
 1100     3.54265     1.0692    0.102382
 1200     4.63087     1.40326     0.160019
 1300     6.24099     1.92005     0.18252
 1400     8.15674     2.36069     0.245413
 1500     10.7656     3.27869     0.341219
 1600     14.7449     4.14804     0.400181
 1700     18.2479     5.14163     0.505512
 1800     22.5152     6.10892     0.605588
 1900     26.6456     7.78431     0.709269
 2000     32.1826     8.23299     0.726341
 
 4) I decided to stop at an input size of 3800x3800, where the runtime
 was 400.03 seconds
 
 5) This algorithm becomes increasingly favorable as "sparseness" increases.
 Using fill percentages of 0.5% and 0.1%, this algorithm runs much faster
 than the "normal" multiplication algorithm and scales much more slowly
 with input size.
 
 *-------------------------------
 *
 * PASTED OUTPUT
 *
 * ------------------------------
 
 "/Users/justin/Documents/Documents/Projects/CLion Projects/CS2C/cmake-build-debug/assignment3B"
 
 printing matA...
 1.00 2.00 3.00 4.00 5.00 
 -1.00 -2.00 -3.00 -4.00 -5.00 
 1.00 3.00 1.00 3.00 1.00 
 0.00 1.00 0.00 1.00 0.00 
 -1.00 -1.00 -1.00 -1.00 -1.00 
 
 
 printing matB...
 2.00 1.00 5.00 0.00 2.00 
 1.00 4.00 3.00 2.00 7.00 
 4.00 4.00 4.00 4.00 4.00 
 7.00 1.00 -1.00 -1.00 -1.00 
 0.00 0.00 8.00 -1.00 -6.00 
 
 
 printing matC = matA * matB...
 44.00 25.00 59.00 7.00 -6.00
 -44.00 -25.00 -59.00 -7.00 6.00
 30.00 20.00 23.00 6.00 18.00
 8.00 5.00 2.00 1.00 6.00
 -14.00 -10.00 -19.00 -4.00 -6.00
 
 
 printing matD = matB * matA...
 4.00 15.00 6.00 17.00 8.00
 -7.00 -2.00 -13.00 -8.00 -19.00
 0.00 12.00 0.00 12.00 0.00
 6.00 9.00 18.00 21.00 30.00
 14.00 29.00 14.00 29.00 14.00
 
 
 
 
 END OF CORRECTNESS TEST-----------------------
 
 
 
 size 0 matrix finished in 0.001098 seconds
 
 size 100 matrix finished in 0.001023 seconds
 
 size 200 matrix finished in 0.009554 seconds
 
 size 300 matrix finished in 0.03725 seconds
 
 size 400 matrix finished in 0.097929 seconds
 
 size 500 matrix finished in 0.201835 seconds
 
 size 600 matrix finished in 0.380672 seconds
 
 size 700 matrix finished in 0.632136 seconds
 
 size 800 matrix finished in 1.02434 seconds
 
 size 900 matrix finished in 1.57369 seconds
 
 size 1000 matrix finished in 2.37442 seconds
 
 size 1100 matrix finished in 3.54265 seconds
 
 size 1200 matrix finished in 4.63087 seconds
 
 size 1300 matrix finished in 6.24099 seconds
 
 size 1400 matrix finished in 8.15674 seconds
 
 size 1500 matrix finished in 10.7656 seconds
 
 size 1600 matrix finished in 14.7449 seconds
 
 size 1700 matrix finished in 18.2479 seconds
 
 size 1800 matrix finished in 22.5152 seconds
 
 size 1900 matrix finished in 26.6456 seconds
 
 size 2000 matrix finished in 32.1826 seconds
 
 size 2100 matrix finished in 36.6236 seconds
 
 size 2200 matrix finished in 46.7392 seconds
 
 size 2300 matrix finished in 54.4964 seconds
 
 size 2400 matrix finished in 66.0816 seconds
 
 size 2500 matrix finished in 78.4951 seconds
 
 size 2600 matrix finished in 93.8166 seconds
 
 size 2700 matrix finished in 105.641 seconds
 
 size 2800 matrix finished in 123.514 seconds
 
 size 2900 matrix finished in 132.924 seconds
 
 size 3000 matrix finished in 156.233 seconds
 
 size 3100 matrix finished in 193.959 seconds
 
 size 3200 matrix finished in 214.608 seconds
 
 
 Process finished with exit code 0
 */
