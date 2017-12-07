//
//  FlowEdge.h
//  9 - Maximum Flow Problem
//
//  Created by Justin Chiu on 12/5/17.
//  Copyright © 2017 Justin Chiu. All rights reserved.
//

#ifndef FlowEdge_h
#define FlowEdge_h

template <class Comparable, typename CostType> class FlowVertex;
template <class Comparable, typename CostType> class FlowGraph;


template <class Comparable, typename CostType>
class FlowEdge {
    friend class FlowVertex<Comparable, CostType>;
    friend class FlowGraph<Comparable, CostType>;
    typedef FlowVertex<Comparable, CostType> Vertex;
protected:
    Vertex* source;
    Vertex* dest;
    CostType resCapacity, currentFlow;
    const CostType maxCapacity;
public:
    FlowEdge(Vertex* sourcePtr, Vertex* destPtr, const CostType& capacity)
        : source(sourcePtr),
          dest(destPtr),
          resCapacity(capacity),
          maxCapacity(capacity),
          currentFlow() { }
    
    CostType remainingCapacity()
        { return resCapacity = maxCapacity - currentFlow; }
    void augmentFlow(const CostType& amt)
        { currentFlow += amt; remainingCapacity(); }
    void reduceFlow(const CostType& amt)
        { currentFlow -= amt; remainingCapacity(); }
    bool operator<(const FlowEdge<Comparable, CostType>& rhs) const
        { return (*source < *rhs.source or *dest < *rhs.dest); }
};


#endif /* FlowEdge_h */
