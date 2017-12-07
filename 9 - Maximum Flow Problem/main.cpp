//
//  main.cpp
//  9 - Maximum Flow Problem
//
//  Created by Justin Chiu on 12/5/17.
//  Copyright © 2017 Justin Chiu. All rights reserved.

#include <string>
#include <iostream>
#include "FlowGraph.h"
using namespace std;


void testWithStrings() {
    cout << "\nTESTING WITH STRINGS" << endl;
    FlowGraph<string, int> stringGraph;
    
    stringGraph.addEdge("s","a", 3);
    stringGraph.addEdge("s","b", 2);
    stringGraph.addEdge("a","b", 1);
    stringGraph.addEdge("a","c", 3);
    stringGraph.addEdge("a","d", 4);
    stringGraph.addEdge("b","d", 2);
    stringGraph.addEdge("c","t", 2);
    stringGraph.addEdge("d","t", 3);
    
    // show the original flow graph
    stringGraph.showAdjTable();
    
    stringGraph.setStartVert("s");
    stringGraph.setEndVert("t");
    cout << "\nMAX FLOW: " << stringGraph.findMaxFlow() << endl;
    stringGraph.showAdjTable();
}


void testWithFloatCosts() {
    cout << "\nTESTING WITH FLOAT COSTS" << endl;
    FlowGraph<string, float> floatCostGraph;
    
    floatCostGraph.addEdge("s","a", 3.5);
    floatCostGraph.addEdge("s","b", 2.2);
    floatCostGraph.addEdge("a","b", 1.1);
    floatCostGraph.addEdge("a","c", 3.3);
    floatCostGraph.addEdge("a","d", 4.6);
    floatCostGraph.addEdge("b","d", 2.1);
    floatCostGraph.addEdge("c","t", 2.6);
    floatCostGraph.addEdge("d","t", 3.4);
    
    // show the original flow graph
    floatCostGraph.showAdjTable();
    
    floatCostGraph.setStartVert("s");
    floatCostGraph.setEndVert("t");
    cout << "\nMAX FLOW: " << floatCostGraph.findMaxFlow() << endl;
    floatCostGraph.showAdjTable();
}


void testWithInts() {
    cout << "\nTESTING WITH INTS" << endl;
    FlowGraph<int, int> intGraph;
    
    intGraph.addEdge(1, 2, 2);
    intGraph.addEdge(1, 4, 3);
    intGraph.addEdge(2, 5, 3);
    intGraph.addEdge(2, 3, 5);
    intGraph.addEdge(4, 3, 1);
    intGraph.addEdge(5, 6, 4);
    intGraph.addEdge(3, 6, 2);
    
    intGraph.showAdjTable();
    
    intGraph.setStartVert(1);
    intGraph.setEndVert(6);
    cout << "\nMAX FLOW: " << intGraph.findMaxFlow() << endl;
    intGraph.showAdjTable();
}


void sanityChecks() {
    cout << "\nSANITY CHECKS" << endl;
    
    FlowGraph<string, int> saneGraph;
    
    try {
        saneGraph.addEdge("s", "t", -1);
    } catch (typename FlowGraph<string,int>::InvalidInputException) {
        cout << "Negative Edge Capacity!" << endl;
    }
    
    try {
        saneGraph.findMaxFlow();
    } catch (typename FlowGraph<string, int>::NoSourceOrSinkException) {
        cout << "Missing Source or Sink Vertex!" << endl;
    }
    
}


int main() {
    sanityChecks();
    testWithInts();
    testWithStrings();
    testWithFloatCosts();

    return 0;
}
