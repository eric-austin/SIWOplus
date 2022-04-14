//
//  node.cpp
//  SIWO
//
//  Created by Eric Austin on 2022-02-25.
//

#include "node.h"

Node::Node(int id) : m_id{ id }, m_supportCalculated{ false }, m_strengthCalculated{ false }, m_maxSupport{ 0.0 }, m_communityId{ 0 } {
    m_neighbors = new std::unordered_map<Node*, EdgeData*>();
}

Node::~Node() {
    delete m_neighbors;
}

void Node::addNeighbor(Node* neighbor, EdgeData* edgeData) {
    m_neighbors->insert(std::make_pair(neighbor, edgeData));
}

int Node::getID() {
    return m_id;
}

double Node::getMaxSupport() {
    return m_maxSupport;
}

void Node::setMaxSupport(double support) {
    m_maxSupport = support;
}

const std::unordered_map<Node*, EdgeData*>* Node::getNeighbors() {
    return m_neighbors;
}

bool Node::hasSupportCalculated() {
    return m_supportCalculated;
}

void Node::setSupportCalculated(bool s) {
    m_supportCalculated = s;
}

bool Node::hasStrengthCalculated() {
    return m_strengthCalculated;
}

void Node::setStrengthCalculated(bool s) {
    m_strengthCalculated = s;
}

int Node::getDegree() {
    return static_cast<int>(m_neighbors->size());
}

int Node::getCommunity() {
    return m_communityId;
}

void Node::setCommunity(int communityId){
    m_communityId = communityId;
}
