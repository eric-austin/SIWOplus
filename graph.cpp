//
//  graph.cpp
//  SIWO
//
//  Created by Eric Austin on 2022-02-26.
//

#include <fstream>
#include "graph.h"

Graph::Graph() {
    m_nodes = new std::unordered_map<int, Node*>();
}

Graph::~Graph() {
    delete m_nodes;
}

Graph::Graph(std::string edgelistPath) : Graph{} {
    // open up the edgelist file
    std::ifstream inStream(edgelistPath);
    
    int nodeID1;
    int nodeID2;
    double weight;
    
    // while there are lines we want to get the node ids and edge weight
    while (inStream >> nodeID1 >> nodeID2 >> weight) {
        addEdge(nodeID1, nodeID2, weight);
    }
    
    inStream.close();
}

Node* Graph::addNode(int nodeID) {
    // check whether node already added to graph
    auto pair = m_nodes->find(nodeID);
    if (pair == m_nodes->end()){
        // node not added so insert into map
        m_nodes->insert(std::make_pair(nodeID, new Node(nodeID)));
        // return pointer of newly added node
        return m_nodes->at(nodeID);
    } else {
        // node already exists so just return pointer
        return pair->second;
    }
}

void Graph::addEdge(int nodeID1, int nodeID2, double weight) {
    // add nodes to graph and get pointers to node objects
    Node* node1 = addNode(nodeID1);
    Node* node2 = addNode(nodeID2);
    
    // create edge data struct and give pointers to both nodes
    EdgeData* edgeData = new EdgeData{weight, 0.0, 0.0, false, false};
    node1->addNeighbor(node2, edgeData);
    node2->addNeighbor(node1, edgeData);
}

const std::unordered_map<int, Node*>* Graph::getNodes() {
    return m_nodes;
}
