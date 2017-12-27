   //
   // Created by Justin on 10/6/17.
   //

#include <cstddef>
#include <FHvector.h>
#include <FHlist.h>
#include <iostream>
#include <iomanip>
using namespace std;

   // MatNode Class Prototype
template <class dataType>
class MatNode {
   
   template <class data>
   friend class SparseMat;
   
protected:
   const int column;
   dataType  data;
   
public:
   explicit MatNode(dataType dataIn = dataType(), int col = int() )
   : column(col), data( move(dataIn) ) { }
};


   // SparseMat class Prototype
template <class dataType>
class SparseMat {
   typedef FHvector< FHlist< MatNode<dataType>* >* > vectorOfLstPtrs;
   typedef FHlist< MatNode<dataType>* > lstOfMatNodePtrs;
   
protected:
   const dataType&  defaultVal;
   const int        printWidth;
   const int        ROWS;
   const int        COLS;
   vectorOfLstPtrs* const vecPtr;
   
   const bool badInput(int row, int col) const;
      //TODO: overloaded .set() : find more elegant way to handle silent failure
   dataType trashVal;
   
public:
   SparseMat(const int numRows, const int numCols,
             const dataType& zeroVal, const int printWidthIn=3)
   : ROWS(numRows), COLS(numCols),
   defaultVal(zeroVal), printWidth(printWidthIn),
   vecPtr(new vectorOfLstPtrs(ROWS)) { }
   
   ~SparseMat() { delete vecPtr; }
   
      // Mutator Functions
   void clear();
   bool set(int row, int col, const dataType& x);
   dataType& set(int row, int col);
   
      // Accessor Functions
   const dataType& get(int row, int col) const;
   void showSubSquare(int start, int size) const;
   
      // Custom Exceptions
   class InvalidInputException {};
   class InvalidSubsquareException {};
   
};


   // Implementation

   // Delete all lists associated with SparseMat, reset all pointers
template <class dataType>
void SparseMat<dataType>::clear() {
   for (lstOfMatNodePtrs* &pointer : *vecPtr) {
      delete pointer;
      pointer = nullptr;
   }
}

   // Check for out of bounds input
template <class dataType>
const bool SparseMat<dataType>::badInput(int row, int col) const {
   return row >= ROWS or row < 0 or col >= COLS or col < 0;
}

   // Write to SparseMat
template <class dataType>
bool SparseMat<dataType>::set(int row, int col, const dataType& x) {
   if (badInput(row, col))
      return false;
   
   lstOfMatNodePtrs* &lstPtr = (*vecPtr)[row];
   
   if (lstPtr == nullptr) {
         // If list doesn't exist, make a new one
      lstPtr  = new FHlist<MatNode<dataType>*>();
   } else {
         // Check if Node already exists, overwrite if it does
      typename lstOfMatNodePtrs::iterator iter  = lstPtr->begin();
      typename lstOfMatNodePtrs::iterator last  = lstPtr->end();
      bool erasingNode = (x == defaultVal);
      for ( ; iter != last ; iter++ ) {
         if ( (*iter)->column  == col ) {
            if (erasingNode) {
               lstPtr->erase(iter);
               break;
            } else // Overwrite Node Data
               (*iter)->data = x;
            return true;
         }
      }
   }
      // If Node doesn't exist yet, add Node to list
   MatNode<dataType>* matNodePtr = new MatNode<dataType>(x, col);
   lstPtr->push_front(matNodePtr);
   matNodePtr = nullptr;
   return true;
}


   //Overloaded for use with assignment operator
template <class dataType>
dataType& SparseMat<dataType>::set(int row, int col) {
   if (badInput(row, col)) {
      dataType& trash = trashVal;
      return trash;
   }
   lstOfMatNodePtrs*& lstPtr = (*vecPtr)[row];
   
   if (lstPtr == nullptr) {
         // If list doesn't exist, make a new one
      lstPtr  = new FHlist<MatNode<dataType>*>();
   } else {
         //TODO: overloaded .set() : add erase functionality
         // Check if Node already exists, overwrite if it does
      typename lstOfMatNodePtrs::iterator iter  = lstPtr->begin();
      typename lstOfMatNodePtrs::iterator last  = lstPtr->end();
      for ( ; iter != last ; iter++)
         if ( (*iter)->column  == col ) {
            return (*iter)->data;
         }
   }
      // If Node doesn't exist yet, add Node to list
   auto* matNodePtr = new MatNode<dataType>(defaultVal, col);
   lstPtr->push_front(matNodePtr);
   return matNodePtr->data;
}

   // Find data in row and column of interest, return defaultVal if not found
template <class dataType>
const dataType& SparseMat<dataType>::get(int row, int col) const {
   if ( badInput(row, col) )
      throw InvalidInputException();
   
   lstOfMatNodePtrs* &lstPtr = (*vecPtr)[row];
   
      // if there is no list, return defaultVal
   if (lstPtr == nullptr) { return defaultVal; }
   
   for (MatNode<dataType>* &matNodePtr : *lstPtr) {
      if (matNodePtr->column == col)
         return matNodePtr->data;
   }
   
   return defaultVal;
}

template <class dataType>
void SparseMat<dataType>::showSubSquare(int start, int size) const {
   if (start < 0 or start + size > ROWS)
      throw InvalidSubsquareException();
   
   cout << "\n";
   int stop = start + size;
   for (int row = start ; row < stop ; row++) {
      for (int col = start ; col < stop ; col++) {
         cout << setw(printWidth) << get(row, col) << " ";
      }
      cout << endl;
   }
}


#define MAT_SIZE 50
#define PRINT_WIDTH 2

   // FUNCTIONS FOR TESTING ------------------

void testFloats() {
   cout << "\n\ntesting floats..." << endl;
   typedef SparseMat<float> SpMat;
   float defaultFloat = 0;
   SpMat mat(MAT_SIZE, MAT_SIZE, defaultFloat, PRINT_WIDTH);
   
      //Test Mutators
   mat.set(-1, 0) = 5;
   mat.clear();
   mat.set(0, 14, 5);
   mat.set(1, 1) = 3;
   mat.set(1, 1) += 3;
   mat.clear();
   mat.set(0, 14) = 5;
   mat.set(2, 5, 10);               // will be overwritten
   mat.set(2, 6, 11);               // on existing list
   mat.set(2, 7, 12);               // again on existing list
   mat.set(2, 5, 35);               // should overwrite the 10
   mat.set(3, 9, 21);
   mat.set(MAT_SIZE, 1, 5);         // should fail silently
   mat.set(9, 9, mat.get(3, 9));    // should copy the 21 here
   mat.set(4, 4, -9);
   mat.set(4, 4, 0);                // should remove the -9 node entirely
   mat.set(MAT_SIZE-1, MAT_SIZE-1, 99);
   
      //Test Accessors and Exception Handling
   try {
      cout << mat.get(7, 8)  << endl;   // 0    (Nothing Here)
      cout << mat.get(2, 5)  << endl;   // 35   (10 Overwritten)
      cout << mat.get(2, 6)  << endl;   // 11   (existing list)
      cout << mat.get(2, 7)  << endl;   // 12   (again on existing list)
      cout << mat.get(4, 4)  << endl;   // 0    (erased)
      cout << mat.get(9, 9)  << endl;   // 21   (copied)
      cout << mat.get(MAT_SIZE-1, MAT_SIZE-1) << endl;   // 99
      cout << mat.get(-4, 7) << endl;   // oops (exception)
   } catch (...) {
      cout << "oops" << endl;
   }
   
      // show top left 15x15
   mat.showSubSquare(0, 15);
   
      // show bottom right 15x15
   mat.showSubSquare(MAT_SIZE - 15, 15);
}

void testInts() {
   cout << "\n\ntesting ints..." << endl;
   typedef SparseMat<int> SpMat;
   int defaultInt = 0;
   SpMat mat(MAT_SIZE, MAT_SIZE, defaultInt, PRINT_WIDTH);
   
      //Test Mutators
   for (int row = 0 ; row < 15 ; row++) {
      for (int col = 0 ; col < 15 ; col++) {
         mat.set(row,col) = row + col;
      }
   }
   
   
   mat.set(-1, 0) = 5;
   mat.clear();
   mat.set(0, 14, 5);
   mat.set(1, 1) = 3;
   mat.clear();
   mat.set(1, 1) += 3;
   mat.set(0, 14) = 5;
   mat.set(2, 5, 10);               // will be overwritten
   mat.set(2, 6, 11);               // on existing list
   mat.set(2, 7, 12);               // again on existing list
   mat.set(2, 5, 35);               // should overwrite the 10
   mat.set(3, 9, 21);
   mat.set(MAT_SIZE, 1, 5);         // should fail silently
   mat.set(9, 9, mat.get(3, 9));    // should copy the 21 here
   mat.set(4, 4, -9);
   mat.set(4, 4, 0);                // should remove the -9 node entirely
   mat.set(MAT_SIZE-1, MAT_SIZE-1, 99);
   
      //Test Accessors and Exception Handling
   try {
      cout << mat.get(7, 8)  << endl;   // 0    (Nothing Here)
      cout << mat.get(2, 5)  << endl;   // 35   (10 Overwritten)
      cout << mat.get(2, 6)  << endl;   // 11   (existing list)
      cout << mat.get(2, 7)  << endl;   // 12   (again on existing list)
      cout << mat.get(4, 4)  << endl;   // 0    (erased)
      cout << mat.get(9, 9)  << endl;   // 21   (copied)
      cout << mat.get(MAT_SIZE-1, MAT_SIZE-1) << endl;   // 99
      cout << mat.get(-4, 7) << endl;   // oops (exception)
   } catch (...) {
      cout << "oops" << endl;
   }
   
      // show top left 15x15
   mat.showSubSquare(0, 15);
   
      // show bottom right 15x15
   mat.showSubSquare(MAT_SIZE - 15, 15);
}

void testStrings() {
   cout << "\n\ntesting strings..." << endl;
   typedef SparseMat<string> SpMat;
   SpMat mat(MAT_SIZE, MAT_SIZE, "_", PRINT_WIDTH);
   
      //Test Mutators
   mat.set(0, -1) = "I am out of bounds";
   mat.set(0, 0) = "(0,0)";
   mat.set(0, 0) = "";
   mat.set(0, 0, "_");
   mat.set(0, 0) = "string";
   
   mat.set(0, 14) = "abc";
      //   mat.clear();
   mat.set(0, 14) += "+appended";
   
   mat.set(2, 5, "def");               // will be overwritten
   mat.set(2, 6, "ghi");               // on existing list
   mat.set(2, 7, "jkl");               // again on existing list
   mat.set(2, 5, "mno");               // should overwrite the 10
   mat.set(3, 9, "pqr");               // will be copied
   mat.set(MAT_SIZE, 1, "stu");        // should fail silently
   mat.set(9, 9, mat.get(3, 9));       // should copy the 21 here
   mat.set(4, 4, "vwx");               // will be deleted
   mat.set(4, 4, "_");                 // delete Node @ (4,4)
   mat.set(4, 5, "yz");                // should remove the -9 node entirely
   mat.set(MAT_SIZE-1, MAT_SIZE-1, "now i know my abcs");
   
      //Test Accessors and Exception Handling
   try {
         //      cout << "(7, 8)  -> " << mat.get(7, 8)  << endl;   // 0    (Nothing Here)
      cout << mat.get(2, 5)  << endl;   // 35   (10 Overwritten)
      cout << mat.get(2, 6)  << endl;   // 11   (existing list)
      cout << mat.get(2, 7)  << endl;   // 12   (again on existing list)
      cout << mat.get(4, 4)  << endl;   // 0    (erased)
      cout << mat.get(9, 9)  << endl;   // 21   (copied)
      cout << mat.get(MAT_SIZE-1, MAT_SIZE-1) << endl;   // 99
      cout << mat.get(-4, 7) << endl;   // oops (exception)
   } catch (...) {
      cout << "oops" << endl;
   }
   
   try {
         // show top left 15x15
      mat.showSubSquare(0, 15);
         // show bottom right 15x15
      mat.showSubSquare(MAT_SIZE - 15, 15);
   } catch (...) {
      cout << "whoops" << endl;
   }
}

void testChars() {
   cout << "testing chars..." << endl;
   typedef SparseMat<char> SpMat;
   char defaultChar = ' ';
   SpMat mat(MAT_SIZE, MAT_SIZE, defaultChar, PRINT_WIDTH);
   
      //Test Mutators
   mat.set(-1, 0) = 'z';
   mat.clear();
   mat.set(0, 14, 'y');
   mat.set(1, 1) = 'a';
   mat.clear();
   mat.set(1, 1) += 'b';
   mat.set(0, 14) = 'c';
   mat.set(2, 5, 'd');               // will be overwritten
   mat.set(2, 6, 'e');               // on existing list
   mat.set(2, 7, 'f');               // again on existing list
   mat.set(2, 5, 'g');               // should overwrite the 10
   mat.set(3, 9, 'h');
   mat.set(MAT_SIZE, 1, 'i');         // should fail silently
   mat.set(9, 9, mat.get(3, 9));    // should copy the 21 here
   mat.set(4, 4, 'j');
   mat.set(4, 4, 'k');                // should remove the -9 node entirely
   mat.set(MAT_SIZE-1, MAT_SIZE-1, 'l');
   
      //Test Accessors and Exception Handling
   try {
      cout << mat.get(7, 8)  << endl;   // 0    (Nothing Here)
      cout << mat.get(2, 5)  << endl;   // 35   (10 Overwritten)
      cout << mat.get(2, 6)  << endl;   // 11   (existing list)
      cout << mat.get(2, 7)  << endl;   // 12   (again on existing list)
      cout << mat.get(4, 4)  << endl;   // 0    (erased)
      cout << mat.get(9, 9)  << endl;   // 21   (copied)
      cout << mat.get(MAT_SIZE-1, MAT_SIZE-1) << endl;   // 99
      cout << mat.get(-4, 7) << endl;   // oops (exception)
   } catch (...) {
      cout << "oops" << endl;
   }
   
      // show top left 15x15
   mat.showSubSquare(0, 15);
   
      // show bottom right 15x15
   mat.showSubSquare(MAT_SIZE - 15, 15);
}

int main() {
   testFloats();
      //   testInts();
   testStrings();
      //   testChars();
   return 0;
}




/* OUTPUT ---------------------------
 "/Users/justin/Documents/Documents/Projects/CLion Projects/CS2C/cmake-build-debug/assignment2A"
 
 
 testing floats...
 0
 35
 11
 12
 0
 21
 99
 oops
 
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  5
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0 35 11 12  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0 21  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0 21  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0 99
 
 
 testing strings...
 mno
 ghi
 jkl
 _
 pqr
 now i know my abcs
 oops
 
 string  _  _  _  _  _  _  _  _  _  _  _  _  _ abc+appended
 _  _  _  _  _  _  _  _  _  _  _  _  _  _  _
 _  _  _  _  _ mno ghi jkl  _  _  _  _  _  _  _
 _  _  _  _  _  _  _  _  _ pqr  _  _  _  _  _
 _  _  _  _  _ yz  _  _  _  _  _  _  _  _  _
 _  _  _  _  _  _  _  _  _  _  _  _  _  _  _
 _  _  _  _  _  _  _  _  _  _  _  _  _  _  _
 _  _  _  _  _  _  _  _  _  _  _  _  _  _  _
 _  _  _  _  _  _  _  _  _  _  _  _  _  _  _
 _  _  _  _  _  _  _  _  _ pqr  _  _  _  _  _
 _  _  _  _  _  _  _  _  _  _  _  _  _  _  _
 _  _  _  _  _  _  _  _  _  _  _  _  _  _  _
 _  _  _  _  _  _  _  _  _  _  _  _  _  _  _
 _  _  _  _  _  _  _  _  _  _  _  _  _  _  _
 _  _  _  _  _  _  _  _  _  _  _  _  _  _  _
 
 _  _  _  _  _  _  _  _  _  _  _  _  _  _  _
 _  _  _  _  _  _  _  _  _  _  _  _  _  _  _
 _  _  _  _  _  _  _  _  _  _  _  _  _  _  _
 _  _  _  _  _  _  _  _  _  _  _  _  _  _  _
 _  _  _  _  _  _  _  _  _  _  _  _  _  _  _
 _  _  _  _  _  _  _  _  _  _  _  _  _  _  _
 _  _  _  _  _  _  _  _  _  _  _  _  _  _  _
 _  _  _  _  _  _  _  _  _  _  _  _  _  _  _
 _  _  _  _  _  _  _  _  _  _  _  _  _  _  _
 _  _  _  _  _  _  _  _  _  _  _  _  _  _  _
 _  _  _  _  _  _  _  _  _  _  _  _  _  _  _
 _  _  _  _  _  _  _  _  _  _  _  _  _  _  _
 _  _  _  _  _  _  _  _  _  _  _  _  _  _  _
 _  _  _  _  _  _  _  _  _  _  _  _  _  _  _
 _  _  _  _  _  _  _  _  _  _  _  _  _  _ now i know my abcs
 
 Process finished with exit code 0
 
 */

