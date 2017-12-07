//
//  FlowVertex.h
//  9 - Maximum Flow Problem
//
//  Created by Justin Chiu on 12/5/17.
//  Copyright © 2017 Justin Chiu. All rights reserved.
//

#ifndef FlowVertex_h
#define FlowVertex_h

#include <set>
#include <iostream>
#include "FlowEdge.h"
using namespace std;

template <class Comparable, class CostType> class FlowGraph;


template <class Comparable, class CostType>
class FlowVertex {
    friend class FlowGraph<Comparable, CostType>;
    typedef FlowEdge<Comparable, CostType> Edge;
protected:
    Comparable data;
    bool discovered;
    CostType potentialFlow;
    Edge* lastEdge;
    set<Edge*> forwardEdges, backwardEdges;
public:
    explicit FlowVertex(const Comparable& dataIn)
        : data(dataIn),
          discovered(false),
          potentialFlow(),
          lastEdge(nullptr),
          forwardEdges(),
          backwardEdges() { }
    
    bool operator<(const FlowVertex<Comparable, CostType>& rhs) const
        { return data < rhs.data; }
    
    void showAdjList() const;
};


template<class Comparable, class CostType>
void FlowVertex<Comparable, CostType>::showAdjList() const {
    cout << "Adj List for " << data << ": ";
    for (Edge* edgePtr : forwardEdges) {
        cout << edgePtr->dest->data << "("
             << edgePtr->currentFlow << "/"
             << edgePtr->maxCapacity << ") ";
    }
    cout << endl;
}


#endif /* FlowVertex_h */
