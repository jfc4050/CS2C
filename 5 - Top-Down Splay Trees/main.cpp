   //
   //  FHsplayTree.hpp
   //  assignment5
   //
   //  Created by Justin on 10/27/17.
   //  Copyright © 2017 Justin. All rights reserved.
   //

#ifndef FHsplayTree_hpp
#define FHsplayTree_hpp

#include "FHsearch_tree.h"

template <class Comparable>
class FHsplayTree : public FHsearch_tree<Comparable>{
   
protected:
      // New Protected Methods
   void splay(FHs_treeNode<Comparable>*& root, const Comparable& x);
   void rotateWithLeftChild(FHs_treeNode<Comparable>*& k2);
   void rotateWithRightChild(FHs_treeNode<Comparable>*& k2);
   
public:
      // Override Public Parent Methods
   bool insert(const Comparable& x);
   bool remove(const Comparable& x);
   const Comparable& find(const Comparable& x);
   bool contains(const Comparable& x);
   
      // New Public Method
   const Comparable& showRoot();
   
};

/***********************
 * New Protected Methods
 ***********************/

template <class Comparable>
void FHsplayTree<Comparable>::splay(FHs_treeNode<Comparable>*& root, const Comparable& x) {
   
   FHs_treeNode<Comparable>* lftTree    = nullptr;
   FHs_treeNode<Comparable>* lftTreeMax = nullptr;
   FHs_treeNode<Comparable>* rtTree     = nullptr;
   FHs_treeNode<Comparable>* rtTreeMin  = nullptr;
   
   while (root != nullptr) {                 // protects against null parameter
      if (x < root->data) {
         if (!root->lftChild)                // x not in tree
            break;
         if (x < root->lftChild->data) {     // (left) zig zig
            rotateWithLeftChild(root);
            if (root->lftChild == nullptr)   // x not in tree
               break;
         }
         if (!rtTree) {
            rtTree = root;
            rtTreeMin = rtTree;
         } else {
            rtTreeMin->lftChild = root;
            rtTreeMin = rtTreeMin->lftChild; // update rtTreeMin
         }
         root = root->lftChild;              // update new working root
         rtTreeMin->lftChild = nullptr;      // disconnect
      }
      
      else if (root->data < x) {
         if (!root->rtChild)       // x not in tree
            break;
         if (root->rtChild->data < x) {      // (right) zig zig
            rotateWithRightChild(root);
            if (!root->rtChild)    // x not in tree
               break;
         }
         if (!lftTree) {
            lftTree = root;
            lftTreeMax = lftTree;
         } else {
            lftTreeMax->rtChild = root;
            lftTreeMax = lftTreeMax->rtChild;   // update rtTreeMin
         }
         root = root->rtChild; // update new working root
         lftTreeMax->rtChild = nullptr;     // disconnect
      }
      else break; // found x @ root
   }
   
      // Reassemble
   if (lftTree) {
      if (root->lftChild)
         lftTreeMax->rtChild = root->lftChild;
      root->lftChild = lftTree;
   }
   if (rtTree) {
      if (root->rtChild)
         rtTreeMin->lftChild = root->rtChild;
      root->rtChild = rtTree;
   }
}

template <class Comparable>
void FHsplayTree<Comparable>::rotateWithLeftChild(FHs_treeNode<Comparable>*& k2) {
   FHs_treeNode<Comparable>* k1 = k2->lftChild;
   k2->lftChild = k1->rtChild;
   k1->rtChild  = k2;
   k2 = k1;
}

template <class Comparable>
void FHsplayTree<Comparable>::rotateWithRightChild(FHs_treeNode<Comparable>*& k2) {
   FHs_treeNode<Comparable>* k1 = k2->rtChild;
   k2->rtChild  = k1->lftChild;
   k1->lftChild = k2;
   k2 = k1;
}

/***********************
 * Override Parent Public Methods
 ***********************/

template <class Comparable>
bool FHsplayTree<Comparable>::insert(const Comparable& x) {
   
   if (contains(x))  // tree now splayed
      return false;
   
   FHs_treeNode<Comparable>* newNode = new FHs_treeNode<Comparable>(x);
   if (!this->mRoot) {     // empty tree
      ;
   } else if (this->mRoot->data > x) {
      newNode->lftChild     = this->mRoot->lftChild;
      this->mRoot->lftChild = nullptr;
      newNode->rtChild      = this->mRoot;
   } else if (x > this->mRoot->data) {
      newNode->rtChild      = this->mRoot->rtChild;
      this->mRoot->rtChild  = nullptr;
      newNode->lftChild     = this->mRoot;
   }
   
   this->mRoot = newNode;
   this->mSize++;
   return true;
}


template <class Comparable>
bool FHsplayTree<Comparable>::remove(const Comparable& x) {
   
   if (!contains(x))    // tree now splayed
      return false;
   
   FHs_treeNode<Comparable>* newRoot;
   if (!this->mRoot->lftChild) {
      newRoot = this->mRoot->rtChild;
   } else {
      newRoot = this->mRoot->lftChild;
      splay(newRoot, x);
      newRoot->rtChild = this->mRoot->rtChild;
   }
   delete this->mRoot;
   this->mRoot = newRoot;
   return true;
}

template <class Comparable>
const Comparable& FHsplayTree<Comparable>::find(const Comparable& x) {
   if (!this->mRoot)                           // empty tree
      throw typename FHsearch_tree<Comparable>::EmptyTreeException();
   
   splay(this->mRoot, x);
   
   if (x > this->mRoot->data or this->mRoot->data > x)   // x not in tree
      throw typename FHsearch_tree<Comparable>::NotFoundException();
   
   return x;
}

template <class Comparable>
bool FHsplayTree<Comparable>::contains(const Comparable &x) {
   try {
      find(x);
   } catch (typename FHsearch_tree<Comparable>::NotFoundException&) {
      ;  // error message
      return false;
   } catch (typename FHsearch_tree<Comparable>::EmptyTreeException&) {
      ;  // error message
      return false;
   }
   return true;
}

/***********************
 * New Public Method
 ***********************/

template <class Comparable>
const Comparable& FHsplayTree<Comparable>::showRoot() {
   if (this->mRoot)
      return this->mRoot->data;
   static Comparable defaultVal = Comparable();
   return defaultVal;
}


#endif /* FHsplayTree_hpp */





   //
   //  assignment5.cpp
   //  assignment5
   //
   //  Created by Justin on 10/27/17.
   //  Copyright © 2017 Justin. All rights reserved.
   //

   // Assignment #5 Instructor Solution
   // CS 2C, Foothill College, Michael Loceff

   // Client / Splay Tree ----------------------------------------------

#include <iostream>
#include "FHsplayTree.hpp"
using namespace std;

template <typename Object>
class PrintObject {
public:
   void operator()(Object obj) {
      cout << obj << " ";
   }
};

void sanityChecks() {
   
   PrintObject<int> intPrinter;
   FHsplayTree<int> saneTree;
   cout << "\n\n";
   cout << "initial traversal" << endl;
   saneTree.traverse(intPrinter); cout << endl;
   cout << "removing from empty tree" << endl;
   saneTree.remove(5);
   saneTree.traverse(intPrinter); cout << endl;
   cout << "inserting 10" << endl;
   saneTree.insert(10);
   saneTree.traverse(intPrinter); cout << endl;
   cout << "attempting to insert duplicate" << endl;
   saneTree.insert(10);
   saneTree.traverse(intPrinter); cout << endl;
   cout << "removing object that doesn't exist" << endl;
   saneTree.remove(7);
   saneTree.traverse(intPrinter); cout << endl;
   cout << "checking if 10 in tree" << endl;
   cout << saneTree.contains(10) << endl;
   cout << saneTree.find(10) << endl;
   saneTree.traverse(intPrinter); cout << endl;
   cout << "checking if 9 in tree" << endl;
   cout << saneTree.contains(9) << endl;
   try {
      cout << saneTree.find(9) << endl;
   } catch(...) {
      cout << "not found!" << endl;
   }
   saneTree.traverse(intPrinter); cout << endl;
}

int main() {
   
   FHsplayTree<int> searchTree;
      //FHsearch_tree<int> searchTree;
   PrintObject<int> intPrinter;
   
   searchTree.traverse(intPrinter);
   cout << "Root: " << searchTree.showRoot() << endl;
   
   cout << "Initial size: " << searchTree.size() << endl;
   for (int k = 1; k <= 32; k++)
      searchTree.insert(k);
   cout << "New size: " << searchTree.size() << endl;
   
   cout << "\nTraversal: \n";
   searchTree.traverse(intPrinter);
   cout << endl << endl;
   
   for (int k = -1; k < 10; k++) {
         // searchTree.contains(k);  // alt to find() - different error return
      try {
         searchTree.find(k);
      }
      catch( ... ) {
         cout << " oops ";
      }
      cout << "splay " << k << " --> root: " << searchTree.showRoot()
      << " height: " << searchTree.showHeight() << endl;
   }
   
   sanityChecks();
   
   return 0;
}

/***********************
 *
 * Sample Run
 *
 ***********************
 
 
 Root: 0
 Initial size: 0
 New size: 32
 
 Traversal:
 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32
 
 oops splay -1 --> root: 1 height: 16
 oops splay 0 --> root: 1 height: 16
 splay 1 --> root: 1 height: 16
 splay 2 --> root: 2 height: 9
 splay 3 --> root: 3 height: 6
 splay 4 --> root: 4 height: 6
 splay 5 --> root: 5 height: 5
 splay 6 --> root: 6 height: 6
 splay 7 --> root: 7 height: 6
 splay 8 --> root: 8 height: 7
 splay 9 --> root: 9 height: 8
 
 
 initial traversal
 
 removing from empty tree
 
 inserting 10
 10
 attempting to insert duplicate
 10
 removing object that doesn't exist
 10
 checking if 10 in tree
 1
 10
 10
 checking if 9 in tree
 0
 not found!
 10
 Program ended with exit code: 0
 
 
 ***********************/




