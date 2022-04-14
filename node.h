//
//  node.h
//  SIWO
//
//  Created by Eric Austin on 2022-02-25.
//

#ifndef node_h
#define node_h

#include <unordered_map>
#include "edge_data.h"

class Node {
public:
    int m_id;
    bool m_supportCalculated;
    bool m_strengthCalculated;
    double m_maxSupport;
    int m_communityId;
    std::unordered_map<Node*, EdgeData*>* m_neighbors;
    
public:
    Node(int id);
    
    ~Node();
    
    void addNeighbor(Node* neighbor, EdgeData* edgeData);
    
    int getID();
    
    double getMaxSupport();
    
    void setMaxSupport(double support);
    
    const std::unordered_map<Node*, EdgeData*>* getNeighbors();
    
    bool hasSupportCalculated();
    
    void setSupportCalculated(bool s);
    
    bool hasStrengthCalculated();
    
    void setStrengthCalculated(bool s);
    
    int getDegree();
    
    int getCommunity();
    
    void setCommunity(int communityId);

};

#endif /* node_h */
