   // ===========================================================================
   // CS 2C Assignment #1 Part A - int Version


#include <utility>
#include <vector>
#include <iostream>
#include <cmath>
using namespace std;

   // --------------- Sublist Class ---------------

class Sublist {
private:
   int sum;
   vector<int>* originalObjects;
   vector<int> indices;
   
public:
   explicit Sublist(vector<int>* orig = nullptr)
   : sum(0), originalObjects(orig) { }
   
   
   Sublist(vector<int> indicesIn, vector<int>* orig, int sumIn)
      //called if dataSet elements sum up exactly to target value
   : indices (move(indicesIn)), originalObjects(orig), sum(sumIn) { }
   
   
   Sublist addItem(const int& indexOfItemToAdd ) {
         //creates a new Sublist with an additional element, specified in parameter
      Sublist augmented = Sublist(*this);
      (augmented.indices).push_back(indexOfItemToAdd);
      augmented.sum += (*originalObjects)[indexOfItemToAdd];
      
      return augmented;
   }
   
   
   void showSublist() const {
      vector<int> output = *originalObjects;
      cout << "{";
      for (int i = 0 ; i < indices.size() ; ++i) {
         int index = indices[i];
         int element = output[index];
         string endingPunctuation = ((i != indices.size()-1) ? "," : "}");
         
         cout << element << endingPunctuation;
      }
      cout << endl;
   }
   
   
   int getSum() const { return sum; }
};

   // ------------------ Global Helper Functions ------------------

void printOutput(Sublist& lstOut, int target) {
   int sum = lstOut.getSum();
   
   cout << "Target: " << target << endl;
   cout << "-----------------" << endl;
   cout << "Sum: " << sum << endl;
   lstOut.showSublist();
   cout << "-----------------" << endl;
}


int compareToTarget(vector<int>& dataIn, int target) {
   int sum = 0;
   for (int val : dataIn) {
      sum += val;
   }
   if (sum < target) {
      cout << "FINISHED: Target is larger than sum of all elements in master set";
      return (sum-target);
   } else if (sum == target) {
      cout << "FINISHED: List is perfect match" << endl;
      vector<int>* outPtr = &dataIn;
      
      vector<int> indices;
      for (int i = 0 ; i < dataIn.size() ; ++i) {
         indices.push_back(i);
      }
      
      Sublist out = Sublist(indices, outPtr, sum);
      printOutput(out, target);
      return 0;
   }
   return 1;
}

   // ------------------ main ------------------


int main() {
   int target = 180;
   
      //Create and populate dataSet
   vector<int> dataSet;
   dataSet.push_back(20); dataSet.push_back(12); dataSet.push_back(22);
   dataSet.push_back(15); dataSet.push_back(25);
   dataSet.push_back(19); dataSet.push_back(29);
   dataSet.push_back(18);
   dataSet.push_back(11); dataSet.push_back(13); dataSet.push_back(17);
   
   if (compareToTarget(dataSet, target) <= 0) {
         //terminate (after printing output) if target <= sum(dataSet)
      return 0;
   }
   
      //Create Col, populate with empty sublist
   vector<int>* origPtr = &dataSet;
   Sublist zeroLst(origPtr);
   vector<Sublist> col{};
   col.reserve(2500);
   col.push_back(move(zeroLst));
   
   for (int j = 0 ; j < dataSet.size() ; j++) {
      int currentNum = dataSet[j];
      
         //for every Sublist already in col...
      auto end = col.end();
      for (auto iter = col.begin() ; iter != end ; ++iter) {
         int currentSum = iter->getSum();
         
         int total = currentSum + currentNum;
         if (total == target) {
            Sublist ansLst = iter->addItem(j);
            cout << "FINISHED: Found perfect match: ";
            printOutput(ansLst, target);
               //terminate program if perfect match found
            return 0;
         } else if (total <= target) {
            Sublist augLst = iter->addItem(j);
            col.push_back(move(augLst));
         }
      }
   }
   
      //Find maximum subset-sum and print output
   int maxVal = INT8_MIN;
   Sublist maxLst;
   for (Sublist subLst : col) {
      if (subLst.getSum() > maxVal){
            //update max sublist and sum
         maxVal = subLst.getSum();
         maxLst = subLst;
      }
   }
   cout << "FINISHED: Did not find perfect match" << endl;
   printOutput(maxLst, target);
}


/* ----------------- output ---------------------
 "/Users/justin/Documents/Documents/Projects/CLion Projects/CS2C/cmake-build-debug/assignment1A"
 FINISHED: Did not find perfect match
 Target: 180
 -----------------
 Sum: 179
 {20,12,15,25,19,29,18,11,13,17}
 -----------------
 
 Process finished with exit code 0
 */







   // ===========================================================================
   // CS 2C Assignment #1 Part B - iTunes Version
   //
   // Created by Justin on 10/4/17.
   //

#include <iTunes.cpp>

typedef vector<iTunesEntry>::iterator iTunesIter;

class Sublist {
private:
   int sum;
   vector<iTunesEntry>* originalObjects;
   vector<iTunesIter> iterators;
   
public:
   explicit Sublist(vector<iTunesEntry>* orig = nullptr)
   : sum(0), originalObjects(orig) { }
   
   
   Sublist addItem(iTunesIter newIter) {
         //creates a new Sublist with an additional element, as specified in parameter
      Sublist augmented = Sublist(*this);
      (augmented.iterators).push_back(newIter);
      augmented.sum += newIter->getTime();
      return augmented;
   }
   
   
   void showSublist() const {
      vector<iTunesEntry> output = *originalObjects;
      cout << "{";
      for (int i = 0 ; i < iterators.size() ; ++i) {
         iTunesIter currentIter = iterators[i];
         string title = currentIter->getTitle();
         title.pop_back();
         string artist = currentIter->getArtist();
         artist.pop_back();
         int time = currentIter->getTime();
         
         string endingPunctuation = ((i != iterators.size() - 1) ? "," : "}");
         cout << "[" << title << " by " << artist << "(" << time << ")]"
         << endingPunctuation;
         if (i % 4 == 0 and i != 0) cout << endl;
      }
      cout << endl;
   }
   
   
   int getSum() const { return sum; }
};


   //Global Helper Functions
void printOutput(Sublist& lstOut, int TARGET) {
   cout << "Target: " << TARGET << endl;
   cout << "-----------------" << endl;
   cout << "Sum: " << lstOut.getSum() << endl;
   lstOut.showSublist();
   cout << "-----------------" << endl;
}


int compareToTarget(vector<iTunesEntry>* dataPtr, int TARGET) {
      //check if algorithm needs to be run
   vector<iTunesEntry> dataSet = *dataPtr;
   int sum = 0;
   for (iTunesEntry& entry : dataSet) {
      sum += entry.getTime();
   }
   if (sum < TARGET) {
      cout << "FINISHED: Target is larger than sum of all elements in master set";
      return (sum - TARGET);
   } else if (sum == TARGET) {
      cout << "FINISHED: List is perfect match" << endl;
      return 0;
   }
   return 1;
}


int main() {
   const int TARGET = 3600;
   cout << "Target time: " << TARGET << endl;
   
      // read the data
   iTunesEntryReader tunes_input("/Users/justin/Desktop/itunesFile.txt");
   if (tunes_input.readError()) {
      cout << "couldn't open " << tunes_input.getFileName()
      << " for input.\n";
      exit(1);
   }
   
      // time the algorithm -------------------------
   clock_t startTime, stopTime;
   startTime = clock();
   
      // create dataSet, a vector of iTunesEntry objects:
   vector<iTunesEntry> dataSet;
   int array_size = tunes_input.getNumTunes();
   for (int k = 0; k < array_size; k++)
      dataSet.push_back(tunes_input[k]);
   
   
      //Create Col, populate with empty sublist
   vector<Sublist> col{};
   vector<iTunesEntry>* dataPtr = &dataSet;
   Sublist zeroLst(dataPtr);
   col.reserve(500000);
   col.push_back(move(zeroLst));
   
   
      //terminate (after printing output) if target <= sum(dataSet)
   if (compareToTarget(dataPtr, TARGET) <= 0) {
      return 0;
   }
   
   
   bool foundPerfect = false;
   for (iTunesIter dataIter = dataSet.begin() ; dataIter != dataSet.end() ; dataIter++) {
      int currentTime = dataIter->getTime();
      
         //for every Sublist already in col...
      vector<Sublist>::iterator colIter;
      auto end = col.end();
      for (colIter = col.begin() ; colIter != end ; ++colIter) {
         int currentSum = colIter->getSum();
         if (currentSum + currentTime == TARGET) {
            Sublist ansLst = colIter->addItem(dataIter);
            cout << "FINISHED: Found perfect match: " << endl;
            printOutput(ansLst, TARGET);
            foundPerfect = true;
            break;
         } else if (currentSum + currentTime <= TARGET) {
            Sublist augLst = colIter->addItem(dataIter);
            col.push_back(move(augLst));
         }
      }
      if (foundPerfect) break;
   }
   
   if (!foundPerfect) {
         //Find maximum subset-sum and print output
      int maxVal = INT8_MIN;
      Sublist maxLst;
      for (Sublist subLst : col) {
         if (subLst.getSum() > maxVal){
            maxVal = subLst.getSum();
            maxLst = subLst;
         }
      }
      cout << "FINISHED: Did not find perfect match" << endl;
      printOutput(maxLst, TARGET);
   }
   
   
      // report algorithm running time -------------------------
   stopTime = clock();
   cout << "\nAlgorithm Elapsed Time: "
   << (double)(stopTime - startTime)/(double)CLOCKS_PER_SEC
   << " seconds." << endl << endl;
   
   return 0;
}

/* ----------------- output ---------------------
 "/Users/justin/Documents/Documents/Projects/CLion Projects/CS2C/cmake-build-debug/assignment1A"
 FINISHED: Did not find perfect match
 Target: 180
 -----------------
 Sum: 179
 {20,12,15,25,19,29,18,11,13,17}
 -----------------
 
 Process finished with exit code 0
 */
