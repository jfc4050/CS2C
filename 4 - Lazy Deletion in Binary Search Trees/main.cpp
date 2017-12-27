   // File FHlazySearchTree.h
   // Template definitions for FHsearchTrees, which are general trees
#ifndef FHLAZYSEARCHTREE_H
#define FHLAZYSEARCHTREE_H

#include <cstddef>
#include <iostream>
#include <cassert>
#include <vector>

using namespace std;

/*********************************************
 *
 * FHlazySearchTreeNode prototype
 *
 *********************************************/

template <class Comparable>
class FHlazySearchTreeNode {
public:
   FHlazySearchTreeNode( const Comparable & d = Comparable(),
                        FHlazySearchTreeNode *lt = NULL,
                        FHlazySearchTreeNode *rt = NULL, bool isDeleted=false)
   : lftChild(lt), rtChild(rt), data(d), deleted(isDeleted)
   { }
   
   FHlazySearchTreeNode *lftChild, *rtChild;
   Comparable data;
   bool deleted;
   
};

/*********************************************
 *
 * FHlazySearchTree prototype
 *
 *********************************************/


template <class Comparable>
class FHlazySearchTree {
protected:
   int mSize;
   int mSizeHard;
   FHlazySearchTreeNode<Comparable> *mRoot;
   
   vector<Comparable> allVals;
   vector<Comparable> notDeleted;
   bool asserting;
   
public:
   FHlazySearchTree( ) : mSize(0), mSizeHard(0), mRoot(nullptr),
   allVals{}, notDeleted{}, asserting(true) { }
   
   FHlazySearchTree(const FHlazySearchTree &rhs) {
      mRoot = NULL;
      mSize = 0;
      mSizeHard = 0;
      *this = rhs;
      asserting = false;
   }
   
   ~FHlazySearchTree() { clear(); }
   
   int size() const { return mSize; }
   int sizeHard() const { return mSizeHard; }
   const FHlazySearchTree & operator=(const FHlazySearchTree &rhs);
   
   void clear() { makeEmpty(mRoot); }
   bool empty() const { return (mSize == 0); }
   const Comparable& findMin() const;
   bool remove(const Comparable &x);
   template <class Processor>
   void traverse(Processor& func) const { traverse(mRoot, func); }
   
   
   /***********************************
    * (Public) Lazy Function Declarations
    ***********************************/
   
   const Comparable& findMaxAlt() const {
      if (mRoot == nullptr or mSize == 0 or mSizeHard == 0)
         throw EmptyTreeException();
      FHlazySearchTreeNode<Comparable>* maxPtr = nullptr;
      FHlazySearchTreeNode<Comparable>* newMax = findMaxAlt(mRoot, maxPtr);
      if (newMax != nullptr)
         return newMax->data;
      else
         throw NotFoundException();
   }
   
   const Comparable& findMinAlt() const {
      if (mRoot == nullptr or mSize == 0 or mSizeHard == 0)
         throw EmptyTreeException();
      FHlazySearchTreeNode<Comparable>* minPtr = nullptr;
      FHlazySearchTreeNode<Comparable>* newMin = findMinAlt(mRoot, minPtr);
      if (newMin != nullptr)
         return newMin->data;
      else
         throw NotFoundException();
   }
   
   bool containsLazy(const Comparable &x) const {
      FHlazySearchTreeNode<Comparable>* ptr = findLazy(mRoot, x);
      return (ptr != nullptr);
   }
   
   const Comparable& findLazy(const Comparable &x) const;
   
   bool insertLazy(const Comparable &x);
   
   bool removeLazy(const Comparable &x);
   
   const Comparable& findMinLazy() const {
      if (mRoot == nullptr or mSize == 0 or mSizeHard == 0)
         throw EmptyTreeException();
      FHlazySearchTreeNode<Comparable>* minPtr = findMinLazy(mRoot);
      if (minPtr != nullptr)
         return minPtr->data;
      else
         throw NotFoundException();
   }
   
   const Comparable& findMaxLazy() const {
      if (mRoot == nullptr or mSize == 0 or mSizeHard == 0)
         throw EmptyTreeException();
      FHlazySearchTreeNode<Comparable>* maxPtr = findMaxLazy(mRoot);
      if (maxPtr != nullptr)
         return maxPtr->data;
      else
         throw NotFoundException();
   }
   
   template <class Processor>
   void traverseLazy(Processor func) const { traverseLazy(mRoot, func); }
   
   void collectGarbage() { collectGarbage(mRoot); }
   
protected:
   FHlazySearchTreeNode<Comparable> *clone( FHlazySearchTreeNode<Comparable> *root) const;
   FHlazySearchTreeNode<Comparable> *findMin(FHlazySearchTreeNode<Comparable> *root) const;
   bool remove(FHlazySearchTreeNode<Comparable> * &root, const Comparable &x);
   void makeEmpty(FHlazySearchTreeNode<Comparable> * &subtreeToDelete);
   template <class Processor>
   void traverse(FHlazySearchTreeNode<Comparable> *treeNode, Processor& func, int level = -1) const;
   
   
   
   /****************************************
    * (Protected) Lazy Function Declarations
    ****************************************/
   
   FHlazySearchTreeNode<Comparable>*
   findMaxAlt(FHlazySearchTreeNode<Comparable> *root, FHlazySearchTreeNode<Comparable>*& ptrToMax) const {
      
      if (ptrToMax == nullptr and !root->deleted)
         ptrToMax = root;
      
      if (root->rtChild != nullptr)
         if (!root->rtChild->deleted) {
            if (ptrToMax == nullptr) ptrToMax = root->rtChild;
            else if (root->rtChild->data > ptrToMax->data)
               ptrToMax = root->rtChild;
         }
      
      if (root->lftChild != nullptr) {
         if (!root->lftChild->deleted) {
            if (ptrToMax == nullptr) ptrToMax = root->lftChild;
            else if (root->lftChild->data > ptrToMax->data)
               ptrToMax = root->lftChild;
         }
      }
      
      if (root->rtChild != nullptr) {
         findMaxAlt(root->rtChild, ptrToMax);
      }
      if (root->lftChild != nullptr) {
         findMaxAlt(root->lftChild, ptrToMax);
      }
      
      return ptrToMax;
   }
   
   
   FHlazySearchTreeNode<Comparable>*
   findMinAlt(FHlazySearchTreeNode<Comparable> *root,
              FHlazySearchTreeNode<Comparable>*& ptrToMin) const {
      
      if (ptrToMin == nullptr and !root->deleted)
         ptrToMin = root;
      
      if (root->lftChild != nullptr) {
         if (!root->lftChild->deleted) {
            if (ptrToMin == nullptr) ptrToMin = root->lftChild;
            else if (root->lftChild->data < ptrToMin->data)
               ptrToMin = root->lftChild;
         }
      }
      
      if (root->rtChild != nullptr)
         if (!root->rtChild->deleted) {
            if (ptrToMin == nullptr) ptrToMin = root->rtChild;
            else if (root->rtChild->data < ptrToMin->data)
               ptrToMin = root->rtChild;
         }
      
      if (root->lftChild != nullptr)
         findMinAlt(root->lftChild, ptrToMin);
      if (root->rtChild != nullptr)
         findMinAlt(root->rtChild, ptrToMin);
      
      return ptrToMin;
   }
   
   bool insertLazy(FHlazySearchTreeNode<Comparable> * &root,
                   const Comparable &x);
   
   bool removeLazy(FHlazySearchTreeNode<Comparable> * &root,
                   const Comparable &x);
   
   FHlazySearchTreeNode<Comparable>*
   findLazy(FHlazySearchTreeNode<Comparable> *root,
            const Comparable &x) const {
      if (root == nullptr)
         return nullptr;
      
      if (x < root->data)
         return findLazy(root->lftChild, x);
      if (root->data < x)
         return findLazy(root->rtChild, x);
      if (root->deleted)
         return nullptr;
      return root;
   }
   
   FHlazySearchTreeNode<Comparable>*
   findMinLazy(FHlazySearchTreeNode<Comparable> *root) const {
      if (root->lftChild != nullptr) {
         FHlazySearchTreeNode<Comparable>* min = findMinLazy(root->lftChild);
         if (min != nullptr)
            return min;
      }
      if (!root->deleted)
         return root;
      return nullptr;
   }
   
   
   
   FHlazySearchTreeNode<Comparable>*
   findMaxLazy(FHlazySearchTreeNode<Comparable> *root) const {
      if (root->rtChild != nullptr) {
         FHlazySearchTreeNode<Comparable>* max = findMaxLazy(root->rtChild);
         if (max != nullptr)
            return max;
      }
      if (!root->deleted)
         return root;
      return nullptr;
   }
   
   template <class Processor>
   void traverseLazy( FHlazySearchTreeNode<Comparable> *treeNode,
                     Processor func, int level = -1) const {
      
      if (treeNode == nullptr)
         return;
      
      traverseLazy(treeNode->lftChild, func, level + 1);
      
      func(treeNode->data, treeNode->deleted);
      
      traverseLazy(treeNode->rtChild, func, level + 1);
   }
   
   void collectGarbage(FHlazySearchTreeNode<Comparable>* treeNode) {
      
      if (treeNode == nullptr)
         return;
      
      collectGarbage(treeNode->lftChild);
      collectGarbage(treeNode->rtChild);
      
      if (treeNode->deleted) {
         remove(mRoot, treeNode->data);
         
         --mSizeHard;
      }
   }
   
public:
      // for exception throwing
   class EmptyTreeException {};
   class NotFoundException {};
};

/*********************************************
 *
 * FHlazySearchTree public method definitions
 *
 *********************************************/


template <class Comparable>
const Comparable & FHlazySearchTree<Comparable>::findMin() const {
      // Calls Protected findMin()
   if (mRoot == NULL)
      throw EmptyTreeException();
   return findMin(mRoot)->data;
}


   // TODO: check assignment operator
template <class Comparable>
const FHlazySearchTree<Comparable> &FHlazySearchTree<Comparable>::operator=
(const FHlazySearchTree &rhs) {
   if (&rhs != this) {
      clear();
      mRoot = clone(rhs.mRoot);
      mSize = rhs.size();
      mSizeHard = rhs.sizeHard();
   }
   return *this;
}

template <class Comparable>
bool FHlazySearchTree<Comparable>::remove(const Comparable &x) {
   if (remove(mRoot, x)) {
      mSize--;
      mSizeHard--;
      return true;
   }
   return false;
}

template <class Comparable>
template <class Processor>
void FHlazySearchTree<Comparable>::
traverse( FHlazySearchTreeNode<Comparable> *treeNode,
         Processor& func, int level) const
{
   if (treeNode == NULL)
      return;
      // we're not doing anything with level but its there in case we want it
   traverse(treeNode->lftChild, func, level + 1);
   func(treeNode->data, treeNode->deleted);
   traverse(treeNode->rtChild, func, level + 1);
}

/****************************************
 * (Public) Lazy Function Definitions
 ****************************************/

template <class Comparable>
const Comparable &FHlazySearchTree<Comparable>::
findLazy(const Comparable &x) const {
   
   FHlazySearchTreeNode<Comparable> *resultNode;
   resultNode = findLazy(mRoot, x);
   
   if (resultNode == nullptr)
      throw NotFoundException();
   
   return resultNode->data;
}

template <class Comparable>
bool FHlazySearchTree<Comparable>::insertLazy(const Comparable &x) {
   if (insertLazy(mRoot, x)) {
      mSize++;
      if (asserting) {
         assert(mSize <= mSizeHard);
      }
      return true;
   }
   return false;
}

template <class Comparable>
bool FHlazySearchTree<Comparable>::removeLazy(const Comparable &x) {
   if (removeLazy(mRoot, x)) {
      mSize--;
      if (asserting) {
         assert(mSize >= 0);
         assert(mSize <= mSizeHard);
      }
      return true;
   }
   return false;
}



/*********************************************
 *
 * FHlazySearchTree Protected Method Definitions
 *
 *********************************************/

   // TODO: Check clone() functionality
template <class Comparable>
FHlazySearchTreeNode<Comparable> *FHlazySearchTree<Comparable>::clone(
                                                                      FHlazySearchTreeNode<Comparable> *root) const {
   FHlazySearchTreeNode<Comparable> *newNode;
   if (root == NULL)
      return NULL;
   
   newNode =  new FHlazySearchTreeNode<Comparable>(
                                                   root->data,
                                                   clone(root->lftChild), clone(root->rtChild), root->deleted);
   return newNode;
}

template <class Comparable>
FHlazySearchTreeNode<Comparable> *FHlazySearchTree<Comparable>::
findMin(FHlazySearchTreeNode<Comparable> *root) const {
   if (root == NULL)
      return NULL;
   if (root->lftChild == NULL)
      return root;
   return findMin(root->lftChild);
}



template <class Comparable>
bool FHlazySearchTree<Comparable>::
remove(FHlazySearchTreeNode<Comparable>* &root, const Comparable &x) {
   
   if (root == nullptr)
      return false;
   
   if (root->data > x)
      return remove(root->lftChild, x);
   if (root->data < x)
      return remove(root->rtChild, x);
   
      // found the node,
   if (root->lftChild != NULL and root->rtChild != NULL) {
      FHlazySearchTreeNode<Comparable>* minNode = findMin(root->rtChild);
      root->data = minNode->data;
      root->deleted = minNode->deleted;
      remove(root->rtChild, minNode->data);
   } else {
      FHlazySearchTreeNode<Comparable>* nodeToRemove = root;
      root = (root->lftChild != NULL)? root->lftChild : root->rtChild;
      delete nodeToRemove;
   }
   return true;
}

/****************************************
 * (Protected) Lazy Function Definitions
 ****************************************/

template <class Comparable>
bool FHlazySearchTree<Comparable>::
insertLazy(FHlazySearchTreeNode<Comparable>*& root, const Comparable &x) {
   
   if (root == nullptr) {
      mSizeHard++;
      root = new FHlazySearchTreeNode<Comparable>(x, nullptr, nullptr);
      if (asserting) {
         assert(root->lftChild == nullptr);
         assert(root->rtChild == nullptr);
         assert(!root->deleted);
      }
      return true;
   }
   
   if (root->deleted) {
      bool hasLft = (root->lftChild != nullptr);
      bool hasRt = (root->lftChild != nullptr);
      bool leaf = (!hasLft and !hasRt);
      
      if (leaf or root->data == x) {
         root->data = x;
         root->deleted = false;
         return true;
      }
      if (hasLft and !hasRt)
         return insertLazy(root->lftChild, x);
      if (hasRt and !hasLft)
         return insertLazy(root->lftChild, x);
      if (asserting)
         assert(hasRt and hasLft);
   }
   
   if (x < root->data)
      return insertLazy(root->lftChild, x);
   else if (root->data < x)
      return insertLazy(root->rtChild, x);
   
   return false;
}


template <class Comparable>
bool FHlazySearchTree<Comparable>::
removeLazy(FHlazySearchTreeNode<Comparable>*& root, const Comparable& x) {
   
   if (root == nullptr)
      return false;
   else if (root->data > x)
      return removeLazy(root->lftChild, x);
   else if (root->data < x)
      return removeLazy(root->rtChild, x);
   
      // node found
   root->deleted = true;
   return true;
}

template <class Comparable>
void FHlazySearchTree<Comparable>::
makeEmpty(FHlazySearchTreeNode<Comparable> * &subtreeToDelete) {
   
   if (subtreeToDelete == NULL)
      return;
   
      // remove children
   makeEmpty(subtreeToDelete->lftChild);
   makeEmpty(subtreeToDelete->rtChild);
   
      // clear client's pointer
   delete subtreeToDelete;
   subtreeToDelete = NULL;
   --mSize;
}



#endif


   //
   // Created by Justin on 10/20/17.
   //

#include "FHlazySearchTree.h"
#include "FHsearch_tree.h"
#include <vector>
#include <cassert>
#include <iomanip>


template <class Printable>
class PrinterFunc {
public:
   void operator()(Printable data, bool deleted = false) {
      cout << data << ((deleted)? "(d) " : " ");
   }
};

template <class Comparable>
class gatherFunc {
private:
   vector<Comparable> deletedVals;
   vector<Comparable> vals;
public:
   gatherFunc() : deletedVals{}, vals{} { }
   
   void operator()(Comparable in, bool deleted) {
      if (deleted)
         deletedVals.push_back(in);
      if (not deleted)
         vals.push_back(in);
   }
   
   void clearData() {
      deletedVals = {};
      vals = {};
   }
   
   vector<Comparable> getDeleted() { return deletedVals; }
   vector<Comparable> getVals() { return vals; }
};

template <class Comparable>
class TreeTester {
private:
   gatherFunc<Comparable> gatherer;
public:
   FHsearch_tree<Comparable> tree;
   FHlazySearchTree<Comparable> lazyTree;
   PrinterFunc<Comparable> printer;
   
   TreeTester() : tree{}, lazyTree{}, printer{}
   { }
   
   TreeTester(vector<Comparable> initVals) : TreeTester() {
      cout << "** after inserting array members **" << endl;
      for (Comparable val : initVals) {
         add(val, false, false);
      } print(); tests();
   }
   
   int treeSize() const {
      return tree.size();
   }
   
   pair<int,int> lazyTreeSizes() const {
      return make_pair(lazyTree.size(), lazyTree.sizeHard());
   }
   
   void add(int num, bool printing=true, bool testing = true) {
      if (printing)
         cout << "** adding " << num << " **" << endl;
      tree.insert(num);
      lazyTree.insertLazy(num);
      if (printing)
         print();
      if (testing) {
         assert(lazyTree.containsLazy(num));
         tests();
      }
      
   }
   
   void remove(int num, bool printing=true, bool testing = true) {
      if (printing)
         cout << "** removing " << num << " **" << endl;
      tree.remove(num);
      lazyTree.removeLazy(num);
      if (printing)
         print();
      if (testing) {
         assert(!lazyTree.containsLazy(num));
         tests();
      }
      
   }
   
   void collectGarbage(bool printing = true, bool testing = true) {
      cout << "** collecting garbage **" << endl;
      lazyTree.collectGarbage();
      gatherer.getDeleted().clear();
      if (printing)
         print();
      if (testing)
         tests();
   }
   
   void print() {
      int mSize = treeSize();
      cout << "Tree: mSize = " << mSize << endl;
      tree.traverse(printer); cout << endl;
      pair<int,int> lazySizes = lazyTreeSizes();
      cout << "LazyTree: mSize = " << lazySizes.first
      << " sizeHard = " << lazySizes.second << endl;
      lazyTree.traverseLazy(printer);
   }
   
   void tests() {
      gatherer.clearData();
      lazyTree.traverse(gatherer);
      
      vector<Comparable> deletedVals = gatherer.getDeleted();
      vector<Comparable> vals = gatherer.getVals();
      
      int mSize = treeSize();
      pair<int,int> lazySizes = lazyTreeSizes();
      
      assert(mSize == lazySizes.first);
      assert(mSize <= lazySizes.second);
      assert(mSize == vals.size());
      assert(lazySizes.second == vals.size() + deletedVals.size());
      
      if (vals.size() > 0 and deletedVals.size() > 0) {
         assert(*max_element(vals.begin(), vals.end()) ==
                lazyTree.findMaxAlt());
         assert(*min_element(vals.begin(), vals.end()) ==
                lazyTree.findMinAlt());
      }
      
      try {
         cout << "\nmin value: " << lazyTree.findMinLazy() << endl;
      } catch (FHlazySearchTree<int>::NotFoundException) {
         cout << "ERROR: not found by findMinLazy() - min is not on lft spine"
         << endl;
      } catch (FHlazySearchTree<int>::EmptyTreeException) {
         cout << "ERROR: empty tree" << endl;
      }
      try {
         cout << "min(alt) value: " << lazyTree.findMinAlt() << endl;
      } catch (FHlazySearchTree<int>::NotFoundException) {
         cout << "ERROR: not found by findMinAlt()" << endl;
      } catch (FHlazySearchTree<int>::EmptyTreeException) {
         cout << "ERROR: empty tree" << endl;
      }
      try {
         cout << "max value: " << lazyTree.findMaxLazy() << endl;
      } catch (FHlazySearchTree<int>::NotFoundException) {
         cout << "ERROR: not found by findMaxLazy() - max is not on rt spine"
         << endl;
      } catch (FHlazySearchTree<int>::EmptyTreeException) {
         cout << "ERROR: empty tree" << endl;
      }
      try {
         cout << "max(alt) value: " << lazyTree.findMaxAlt() << endl;
      } catch (FHlazySearchTree<int>::NotFoundException) {
         cout << "ERROR: not found by findMaxAlt()" << endl;
      } catch (FHlazySearchTree<int>::EmptyTreeException) {
         cout << "ERROR: empty tree" << endl;
      }
      cout << "******************\n\n";
   }
   
};

#define INIT_VALS {50, 20, 70, 10, 30, 60}

int main() {
   
   vector<int> initVals = INIT_VALS;
   TreeTester<int> intTester(initVals);
   
   cout << "\n\n**********************" << endl;
   cout << "testing basic operations..." << endl;
   cout << "**********************\n\n" << endl;
   
   intTester.collectGarbage();
   intTester.add(22);
   intTester.remove(10);
   intTester.remove(20);
   intTester.remove(22);
   intTester.remove(30);
   
   cout << "\n\n**********************" << endl;
   cout << "checks on copy constructor" << endl;
   cout << "**********************\n\n" << endl;
   
   FHlazySearchTree<int> otherTree = intTester.lazyTree;
   PrinterFunc<int> printer;
   
   cout << "**traversing otherTree..." << endl;
   otherTree.traverse(printer); cout << "\n";
   cout << "**inserting 20" << endl;
   otherTree.insertLazy(20);
   otherTree.traverse(printer); cout << "\n";
   cout << "**removing 50" << endl;
   otherTree.removeLazy(50);
   otherTree.traverse(printer); cout << "\n";
   cout << "**collecting garbage" << endl;
   otherTree.collectGarbage();
   otherTree.traverse(printer); cout << "\n\n";
   cout << "**finding min" << endl;
   
   cout << "\n\n**********************" << endl;
   cout << "continue testing operations..." << endl;
   cout << "**********************\n\n" << endl;
   
   intTester.add(20);
   intTester.collectGarbage();
   intTester.remove(50);
   intTester.add(25);
   intTester.add(70);
   intTester.add(5);
   intTester.remove(5);
   intTester.add(3);
   intTester.remove(3);
   intTester.remove(70);
   intTester.add(80);
   
   
   cout << "\n\n********************" << endl;
   cout << "checks on find functions" << endl;
   cout << "********************\n\n" << endl;
   
   cout << "contains 80 (not deleted): "
   << intTester.lazyTree.containsLazy(80) << endl;
   cout << "contains 3 (soft deleted): "
   << intTester.lazyTree.containsLazy(3) << endl;
   cout << "contains 90 (doesnt exist): "
   << intTester.lazyTree.containsLazy(90) << endl;
   
   
   try {
      cout << "found 80 (not deleted): " << intTester.lazyTree.findLazy(80)
      << endl;
   } catch (FHlazySearchTree<int>::NotFoundException) {
      cout << "ERROR: Not Found!" << endl;
   }
   try {
      cout << "found 3 (soft deleted): " << intTester.lazyTree.findLazy(3)
      << endl;
   } catch (FHlazySearchTree<int>::NotFoundException) {
      cout << "ERROR: Not Found!" << endl;
   }
   try {
      cout << "found 90 (doesnt exist): " << intTester.lazyTree.findLazy(90)
      << "\n\n";
   } catch (FHlazySearchTree<int>::NotFoundException) {
      cout << "ERROR: Not Found!\n\n";
   }
   
   cout << "\n\n**********************" << endl;
   cout << "checks on soft-empty trees" << endl;
   cout << "**********************\n\n" << endl;
   
   intTester.remove(20);
   intTester.remove(25);
   intTester.remove(60);
   intTester.remove(80);
   
   try {
      cout << "minimum: " << intTester.lazyTree.findMinLazy() << "\n\n";
   } catch  (FHlazySearchTree<int>::EmptyTreeException) {
      cout << "ERROR: Empty Tree!\n\n";
   }
   try {
      cout << "minimum (alt): " << intTester.lazyTree.findMinAlt() << "\n\n";
   } catch  (FHlazySearchTree<int>::EmptyTreeException) {
      cout << "ERROR: Empty Tree!\n\n";
   }
   try {
      cout << "maximum: " << intTester.lazyTree.findMaxLazy() << "\n\n";
   } catch  (FHlazySearchTree<int>::EmptyTreeException) {
      cout << "ERROR: Empty Tree!\n\n";
   }
   try {
      cout << "maximum (alt): " << intTester.lazyTree.findMaxAlt() << "\n\n";
   } catch  (FHlazySearchTree<int>::EmptyTreeException) {
      cout << "ERROR: Empty Tree!\n\n";
   }
   
   
   
   
   cout << "\n\n**********************" << endl;
   cout << "checks on hard-empty trees" << endl;
   cout << "**********************\n\n" << endl;
   
   intTester.collectGarbage();
   
   try {
      cout << "minimum: " << intTester.lazyTree.findMinLazy() << "\n\n";
   } catch  (FHlazySearchTree<int>::EmptyTreeException) {
      cout << "ERROR: Empty Tree!\n\n";
   }
   try {
      cout << "minimum (alt): " << intTester.lazyTree.findMinAlt() << "\n\n";
   } catch  (FHlazySearchTree<int>::EmptyTreeException) {
      cout << "ERROR: Empty Tree!\n\n";
   }
   try {
      cout << "maximum: " << intTester.lazyTree.findMaxLazy() << "\n\n";
   } catch  (FHlazySearchTree<int>::EmptyTreeException) {
      cout << "ERROR: Empty Tree!\n\n";
   }
   try {
      cout << "maximum (alt): " << intTester.lazyTree.findMaxAlt() << "\n\n";
   } catch  (FHlazySearchTree<int>::EmptyTreeException) {
      cout << "ERROR: Empty Tree!\n\n";
   }
   
   
   cout << "garbage collection on hard-empty tree" << endl;
   intTester.collectGarbage();
   
}


/***********************
 *
 * RUN RESULTS
 *
 ***********************
 
 ** after inserting array members **
 Tree: mSize = 6
 10 20 30 50 60 70
 LazyTree: mSize = 6 sizeHard = 6
 10 20 30 50 60 70
 min value: 10
 min(alt) value: 10
 max value: 70
 max(alt) value: 70
 ******************
 
 
 
 **********************
 testing basic operations...
 **********************
 
 
 ** collecting garbage **
 Tree: mSize = 6
 10 20 30 50 60 70
 LazyTree: mSize = 6 sizeHard = 6
 10 20 30 50 60 70
 min value: 10
 min(alt) value: 10
 max value: 70
 max(alt) value: 70
 ******************
 
 ** adding 22 **
 Tree: mSize = 7
 10 20 22 30 50 60 70
 LazyTree: mSize = 7 sizeHard = 7
 10 20 22 30 50 60 70
 min value: 10
 min(alt) value: 10
 max value: 70
 max(alt) value: 70
 ******************
 
 ** removing 10 **
 Tree: mSize = 6
 20 22 30 50 60 70
 LazyTree: mSize = 6 sizeHard = 7
 10(d) 20 22 30 50 60 70
 min value: 20
 min(alt) value: 20
 max value: 70
 max(alt) value: 70
 ******************
 
 ** removing 20 **
 Tree: mSize = 5
 22 30 50 60 70
 LazyTree: mSize = 5 sizeHard = 7
 10(d) 20(d) 22 30 50 60 70
 min value: 50
 min(alt) value: 22
 max value: 70
 max(alt) value: 70
 ******************
 
 ** removing 22 **
 Tree: mSize = 4
 30 50 60 70
 LazyTree: mSize = 4 sizeHard = 7
 10(d) 20(d) 22(d) 30 50 60 70
 min value: 50
 min(alt) value: 30
 max value: 70
 max(alt) value: 70
 ******************
 
 ** removing 30 **
 Tree: mSize = 3
 50 60 70
 LazyTree: mSize = 3 sizeHard = 7
 10(d) 20(d) 22(d) 30(d) 50 60 70
 min value: 50
 min(alt) value: 50
 max value: 70
 max(alt) value: 70
 ******************
 
 
 
 **********************
 checks on copy constructor
 **********************
 
 
 **traversing otherTree...
 10(d) 20(d) 22(d) 30(d) 50 60 70
 **inserting 20
 10(d) 20 22(d) 30(d) 50 60 70
 **removing 50
 10(d) 20 22(d) 30(d) 50(d) 60 70
 **collecting garbage
 20 60 70
 
 **finding min
 
 
 **********************
 continue testing operations...
 **********************
 
 
 ** adding 20 **
 Tree: mSize = 4
 20 50 60 70
 LazyTree: mSize = 4 sizeHard = 7
 10(d) 20 22(d) 30(d) 50 60 70
 min value: 20
 min(alt) value: 20
 max value: 70
 max(alt) value: 70
 ******************
 
 ** collecting garbage **
 Tree: mSize = 4
 20 50 60 70
 LazyTree: mSize = 4 sizeHard = 4
 20 50 60 70
 min value: 20
 min(alt) value: 20
 max value: 70
 max(alt) value: 70
 ******************
 
 ** removing 50 **
 Tree: mSize = 3
 20 60 70
 LazyTree: mSize = 3 sizeHard = 4
 20 50(d) 60 70
 min value: 20
 min(alt) value: 20
 max value: 70
 max(alt) value: 70
 ******************
 
 ** adding 25 **
 Tree: mSize = 4
 20 25 60 70
 LazyTree: mSize = 4 sizeHard = 5
 20 25 50(d) 60 70
 min value: 20
 min(alt) value: 20
 max value: 70
 max(alt) value: 70
 ******************
 
 ** adding 70 **
 Tree: mSize = 4
 20 25 60 70
 LazyTree: mSize = 4 sizeHard = 5
 20 25 50(d) 60 70
 min value: 20
 min(alt) value: 20
 max value: 70
 max(alt) value: 70
 ******************
 
 ** adding 5 **
 Tree: mSize = 5
 5 20 25 60 70
 LazyTree: mSize = 5 sizeHard = 6
 5 20 25 50(d) 60 70
 min value: 5
 min(alt) value: 5
 max value: 70
 max(alt) value: 70
 ******************
 
 ** removing 5 **
 Tree: mSize = 4
 20 25 60 70
 LazyTree: mSize = 4 sizeHard = 6
 5(d) 20 25 50(d) 60 70
 min value: 20
 min(alt) value: 20
 max value: 70
 max(alt) value: 70
 ******************
 
 ** adding 3 **
 Tree: mSize = 5
 3 20 25 60 70
 LazyTree: mSize = 5 sizeHard = 6
 3 20 25 50(d) 60 70
 min value: 3
 min(alt) value: 3
 max value: 70
 max(alt) value: 70
 ******************
 
 ** removing 3 **
 Tree: mSize = 4
 20 25 60 70
 LazyTree: mSize = 4 sizeHard = 6
 3(d) 20 25 50(d) 60 70
 min value: 20
 min(alt) value: 20
 max value: 70
 max(alt) value: 70
 ******************
 
 ** removing 70 **
 Tree: mSize = 3
 20 25 60
 LazyTree: mSize = 3 sizeHard = 6
 3(d) 20 25 50(d) 60 70(d)
 min value: 20
 min(alt) value: 20
 max value: ERROR: not found by findMaxLazy() - max is not on rt spine
 max(alt) value: 60
 ******************
 
 ** adding 80 **
 Tree: mSize = 4
 20 25 60 80
 LazyTree: mSize = 4 sizeHard = 7
 3(d) 20 25 50(d) 60 70(d) 80
 min value: 20
 min(alt) value: 20
 max value: 80
 max(alt) value: 80
 ******************
 
 
 
 ********************
 checks on find functions
 ********************
 
 
 contains 80 (not deleted): 1
 contains 3 (soft deleted): 0
 contains 90 (doesnt exist): 0
 found 80 (not deleted): 80
 found 3 (soft deleted): ERROR: Not Found!
 found 90 (doesnt exist): ERROR: Not Found!
 
 
 
 **********************
 checks on soft-empty trees
 **********************
 
 
 ** removing 20 **
 Tree: mSize = 3
 25 60 80
 LazyTree: mSize = 3 sizeHard = 7
 3(d) 20(d) 25 50(d) 60 70(d) 80
 min value: ERROR: not found by findMinLazy() - min is not on lft spine
 min(alt) value: 25
 max value: 80
 max(alt) value: 80
 ******************
 
 ** removing 25 **
 Tree: mSize = 2
 60 80
 LazyTree: mSize = 2 sizeHard = 7
 3(d) 20(d) 25(d) 50(d) 60 70(d) 80
 min value: ERROR: not found by findMinLazy() - min is not on lft spine
 min(alt) value: 60
 max value: 80
 max(alt) value: 80
 ******************
 
 ** removing 60 **
 Tree: mSize = 1
 80
 LazyTree: mSize = 1 sizeHard = 7
 3(d) 20(d) 25(d) 50(d) 60(d) 70(d) 80
 min value: ERROR: not found by findMinLazy() - min is not on lft spine
 min(alt) value: 80
 max value: 80
 max(alt) value: 80
 ******************
 
 ** removing 80 **
 Tree: mSize = 0
 
 LazyTree: mSize = 0 sizeHard = 7
 3(d) 20(d) 25(d) 50(d) 60(d) 70(d) 80(d)
 min value: ERROR: empty tree
 min(alt) value: ERROR: empty tree
 max value: ERROR: empty tree
 max(alt) value: ERROR: empty tree
 ******************
 
 minimum: ERROR: Empty Tree!
 
 minimum (alt): ERROR: Empty Tree!
 
 maximum: ERROR: Empty Tree!
 
 maximum (alt): ERROR: Empty Tree!
 
 
 
 **********************
 checks on hard-empty trees
 **********************
 
 
 ** collecting garbage **
 Tree: mSize = 0
 
 LazyTree: mSize = 0 sizeHard = 0
 
 min value: ERROR: empty tree
 min(alt) value: ERROR: empty tree
 max value: ERROR: empty tree
 max(alt) value: ERROR: empty tree
 ******************
 
 minimum: ERROR: Empty Tree!
 
 minimum (alt): ERROR: Empty Tree!
 
 maximum: ERROR: Empty Tree!
 
 maximum (alt): ERROR: Empty Tree!
 
 garbage collection on hard-empty tree
 ** collecting garbage **
 Tree: mSize = 0
 
 LazyTree: mSize = 0 sizeHard = 0
 
 min value: ERROR: empty tree
 min(alt) value: ERROR: empty tree
 max value: ERROR: empty tree
 max(alt) value: ERROR: empty tree
 ******************
 
 Program ended with exit code: 0
 
 */

