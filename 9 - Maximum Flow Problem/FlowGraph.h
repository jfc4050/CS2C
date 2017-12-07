//
//  FlowGraph.h
//  9 - Maximum Flow Problem
//
//  Created by Justin Chiu on 12/5/17.
//  Copyright © 2017 Justin Chiu. All rights reserved.
//

#ifndef FlowGraph_h
#define FlowGraph_h

#include <queue>
#include "FlowVertex.h"
#include "FlowEdge.h"

template <class Comparable, class CostType>
class FlowGraph {
    typedef FlowVertex<Comparable, CostType> Vertex;
    typedef FlowEdge<Comparable, CostType> Edge;
protected:
    Vertex* sourceVertex;
    Vertex* sinkVertex;
    set<Vertex> vertexSet;
    set<Vertex*> vertexPtrSet;
    set<Edge> edgeSet;
public:
    FlowGraph()
        : sourceVertex(nullptr),
          sinkVertex(nullptr),
          vertexSet{},
          vertexPtrSet{},
          edgeSet{} { }
    
    Vertex* addVertex(const Comparable& dataIn);
    void addEdge(const Comparable& sourceData,
                 const Comparable& destData, const CostType& capacity);
    void setStartVert(const Comparable& dataIn)
        {sourceVertex = addVertex(dataIn);}
    void setEndVert(const Comparable& dataIn)
        {sinkVertex = addVertex(dataIn);}
    
    CostType findMaxFlow();
    void showAdjTable() const
        {for (const Vertex& vert : vertexSet) vert.showAdjList();}
    
    class InvalidInputException{};
    class NoSourceOrSinkException{};
};


template <class Comparable, class CostType>
FlowVertex<Comparable, CostType>* FlowGraph<Comparable, CostType>::
addVertex(const Comparable& dataIn) {
    pair<typename set<Vertex>::iterator, bool> retVal =
        vertexSet.insert( Vertex(dataIn) );
    Vertex* vertPtr = (Vertex*) &*retVal.first;
    if (retVal.second) vertexPtrSet.insert(vertPtr);
    return vertPtr;
}


template <class Comparable, class CostType>
void FlowGraph<Comparable, CostType>::addEdge(const Comparable &sourceData,
                                              const Comparable& destData,
                                              const CostType& capacity) {
    if (capacity < 0) throw InvalidInputException();
    
    Vertex* sourcePtr = addVertex(sourceData);
    Vertex* destPtr   = addVertex(destData);
    pair<typename set<Edge>::iterator, bool> retVal =
        edgeSet.insert(Edge(sourcePtr, destPtr, capacity));
    Edge* edgePtr = (Edge*) &*retVal.first;
    
    if (retVal.second) {
        sourcePtr->forwardEdges.insert(edgePtr);
        destPtr->backwardEdges.insert(edgePtr);
    }
}


template <class Comparable, class CostType>
CostType FlowGraph<Comparable, CostType>::findMaxFlow() {
    //assume all edge flows = 0
    if (sourceVertex == nullptr or sinkVertex == nullptr)
        throw NoSourceOrSinkException();
    
    sourceVertex->potentialFlow = (CostType) INT_MAX;
    
    CostType maxFlow{};
    queue<Vertex*> vertQueue{};
    vertQueue.push(sourceVertex);
    
    while (!vertQueue.empty()) {
        Vertex* i = vertQueue.front();
        vertQueue.pop();
        
        for (Edge* edgeij : i->forwardEdges) {
            Vertex* j = edgeij->dest;
            if (!j->discovered) {
                CostType capacityij = edgeij->remainingCapacity();
                if (0 < capacityij) {
                    j->potentialFlow = min(i->potentialFlow, capacityij);
                    j->lastEdge = edgeij;
                    j->discovered = true;
                    vertQueue.push(j);
                }
            }
        }
        
        for (Edge* edgeji : i->backwardEdges) {
            Vertex* j = edgeji->source;
            if (!j->discovered) {
                CostType flowij = edgeji->currentFlow;
                if (0 < flowij) {
                    j->potentialFlow = min(i->potentialFlow, flowij);
                    j->lastEdge = edgeji;
                    j->discovered = true;
                    vertQueue.push(j);
                }
            }
        }
        
        if (sinkVertex->discovered) {
            Vertex* currentVert = sinkVertex;
            CostType amt = sinkVertex->potentialFlow;
            maxFlow += amt;
            while (currentVert != sourceVertex) {
                Edge* lastEdge = currentVert->lastEdge;
                if (lastEdge->dest != currentVert) {
                    lastEdge->reduceFlow(amt);
                    currentVert = lastEdge->dest;
                } else {
                    lastEdge->augmentFlow(amt);
                    currentVert = lastEdge->source;
                }
            }
            for (Vertex* vertPtr : vertexPtrSet) {
                if (vertPtr != sourceVertex)
                    vertPtr->discovered = false;
            }
            while (!vertQueue.empty()) vertQueue.pop();
            vertQueue.push(sourceVertex);
        }
    }
    return maxFlow;
}

#endif /* FlowGraph_h */
