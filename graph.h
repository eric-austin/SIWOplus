//
//  graph.h
//  SIWO
//
//  Created by Eric Austin on 2022-02-26.
//

#ifndef graph_h
#define graph_h

#include <unordered_map>
#include "node.h"
#include "edge_data.h"

class Graph {
private:
    std::unordered_map<int, Node*>* m_nodes;
    
public:
    Graph();
    
    ~Graph();
    
    Graph(std::string edgelistPath);
    
    Node* addNode(int nodeID);
    
    void addEdge(int nodeID1, int nodeID2, double weight);
    
    const std::unordered_map<int, Node*>* getNodes();
};

#endif /* graph_h */
