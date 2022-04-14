//
//  siwo.h
//  SIWO
//
//  Created by Eric Austin on 2022-02-27.
//

#ifndef siwo_h
#define siwo_h

#include <random>
#include <unordered_set>
#include "edge_data.h"
#include "node.h"
#include "graph.h"

class SIWOplus {
public:
    Graph* m_graph;
    std::unordered_set<Node*>* m_currentCommunity;
    int m_currentCommunityId;
    std::unordered_set<Node*>* m_unclusteredNodes;
    std::unordered_set<Node*>* m_clusteredNodes;
    std::unordered_map<Node*, double>* m_shell; // track nodes in the shell set with their strength contributions
    std::vector<std::unordered_set<Node*>*>* m_communities;
    std::mt19937 m_rng;
    
public:
    SIWOplus(Graph* graph);
    
    ~SIWOplus();
    
    Node* selectStartNode();
    
    void addNodeToCommunity(Node* node);
    
    void calculateSupport(Node* node1, Node* node2);
    
    void calculateLocalSupport(Node* node1);
    
    void calculateLocalStrength(Node* node);
    
    void updateShell(Node* node);
    
    Node* findBestCandidate();
    
    void detectCommunity();
    
    void detectAllCommunities();
    
    int findClosestCommunity(std::unordered_set<Node*>* community);
    
    void mergeSmallCommunities();
    
    void printCommunity(int commIndex);
    
    void mergeDanglingNodes();
};

#endif /* siwo_h */
