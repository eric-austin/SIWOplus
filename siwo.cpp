//
//  siwo.cpp
//  SIWO
//
//  Created by Eric Austin on 2022-02-27.
//

#include "siwo.h"
#include <iostream>
#include <experimental/algorithm>

SIWOplus::SIWOplus(Graph* graph, char mean) : m_graph{graph}, m_rng{ 1 }, m_currentCommunityId{ 0 }, m_mean{ mean } {
    m_currentCommunity = new std::unordered_set<Node*>();
    m_unclusteredNodes = new std::unordered_set<Node*>();
    for (auto& pair : *m_graph->getNodes()) {
        m_unclusteredNodes->insert(pair.second);
    }
    m_clusteredNodes = new std::unordered_set<Node*>();
    m_shell = new std::unordered_map<Node*, double>();
    m_communities = new std::vector<std::unordered_set<Node*>*>();
}

SIWOplus::~SIWOplus() {
    delete m_currentCommunity;
    delete m_unclusteredNodes;
    delete m_clusteredNodes;
    delete m_shell;
    delete m_communities;
}

Node* SIWOplus::selectStartNode() {
    // randomly select a starting node from the unclustered nodes
    Node* sampleIt[1]{};
    std::experimental::sample(m_unclusteredNodes->begin(), m_unclusteredNodes->end(), sampleIt, 1, m_rng);
    return sampleIt[0];
}

void SIWOplus::addNodeToCommunity(Node* node) {
    m_currentCommunity->insert(node);
    m_clusteredNodes->insert(node);
    m_unclusteredNodes->erase(node);
    m_shell->erase(node);
    node->setCommunity(m_currentCommunityId);
}

void SIWOplus::calculateSupport(Node *node1, Node *node2) {
    // calculate the support of edge between given nodes
    double totalSupport{ 0.0 };
    // need to get edge weight between the two nodes
    double w1 = node1->getNeighbors()->at(node2)->weight;
    // iterate through all neighbors of node1
    for (auto& node_edge : *(node1->getNeighbors())) {
        // if neighbor is also neighbor of node2 then it is a common neighbour
        auto edgeData = node2->getNeighbors()->find(node_edge.first);
        if (edgeData != node2->getNeighbors()->end()) {
            // get both edges to the common neighbour and average three edges for support
            double w2 = node_edge.second->weight;
            double w3 = edgeData->second->weight;
            double support{ 0 };
            if (m_mean == 'a') {
                support = (w1 + w2 + w3) / 3.0;
            } else {
                support = std::pow( w1*w2*w3, 1.0 / 3.0);
            }
            totalSupport += support;
        }
    }
    // assign support value to the edge and mark as support calculated
    node1->getNeighbors()->at(node2)->support = totalSupport;
    node1->getNeighbors()->at(node2)->supportCalculated = true;
    // update max support of both nodes
    if (totalSupport > node1->getMaxSupport())
        node1->setMaxSupport(totalSupport);
    if (totalSupport > node2->getMaxSupport())
        node2->setMaxSupport(totalSupport);
}

void SIWOplus::calculateLocalSupport(Node* node) {
    // calculate support of all edges of node
    for (auto& node_edge : *node->getNeighbors()) {
        // check whether edge already has support calculated and if not perform calc
        if (!node_edge.second->supportCalculated)
            calculateSupport(node, node_edge.first);
    }
    // all connected edges now have support calculated so mark node as support calculated
    node->setSupportCalculated(true);
}

void SIWOplus::calculateLocalStrength(Node* node) {
    // first need to calculate local support if not already done
    if (!node->hasSupportCalculated())
        calculateLocalSupport(node);
    // now every neighbour also needs local support calculated since max supports needed
    for (auto& node_edge : *node->getNeighbors()) {
        // if the neighbor already has strength calculated then the edge will have strength and don't need to re-calc
        if (!node_edge.first->hasStrengthCalculated()) {
            // if neighbor not yet strength calculated may need to calculate support
            if (!node_edge.first->hasSupportCalculated())
                calculateLocalSupport(node_edge.first);
            // now that both nodes on this edge have max support can calculate strength
            double s1;
            if (node->getMaxSupport() > 0.0) {
                s1 = node_edge.second->support / node->getMaxSupport();
            } else {
                s1 = 0.0;
            }
            double s2;
            if (node_edge.first->getMaxSupport() > 0.0) {
                s2 = node_edge.second->support / node_edge.first->getMaxSupport();
            } else {
                s2 = 0.0;
            }
            node_edge.second->strength = s1 + s2 - 1.0;
            node_edge.second->strengthCalculated = true;
        }
    }
    node->setStrengthCalculated(true);
}

void SIWOplus::updateShell(Node *node) {
    // when a node is added to community we need to update the shell with the new node's neighbors
    for (auto& node_edge : *node->getNeighbors()) {
        // first check whether neighbor is already in a community
        if (m_clusteredNodes->find(node_edge.first) != m_clusteredNodes->end())
            continue;
        // next check whether neighbor is already in shell set
        auto node_contribution = m_shell->find(node_edge.first);
        if (node_contribution != m_shell->end()) {
            // since neighbor already in shell set, just need to add edge strength to contribution
            node_contribution->second += node_edge.second->strength;
        } else {
            // otherwise add neighbor to shell with contribution equal to edge strength
            m_shell->insert(std::make_pair(node_edge.first, node_edge.second->strength));
        }
    }
}

Node* SIWOplus::findBestCandidate() {
    // find the node that provides the highest total strength contribution
    Node* bestNode{};
    double bestContribution{ - std::numeric_limits<double>::infinity() };
    for (auto& node_contribution : *m_shell) {
        if (node_contribution.second > bestContribution) {
            bestNode = node_contribution.first;
            bestContribution = node_contribution.second;
        }
    }
    return bestNode;
}

void SIWOplus::detectCommunity() {
    // first thing we need is a starting node
    // right now this is selected randomly from the currently unclustered nodes
    Node* startNode = selectStartNode();
    // add start node to community
    addNodeToCommunity(startNode);
    // calculate strength of all edges of node
    calculateLocalStrength(startNode);
    // add neighbors to shell set with stregth contribution equal to strength of single edge connecting to community
    updateShell(startNode);
    // need to keep looping while there are nodes to be potentially added to the community
    while (m_shell->size() > 0) {
        // shell set has been updated after adding a node so we know the strength contribution of nodes
        Node* bestNode = findBestCandidate();
        // check whether the net contribution of best candidate is positive, if not we are done this community
        if (m_shell->at(bestNode) <= 0)
            break;
        // since node's contribution is positive we add to community and update strengths/shell set
        addNodeToCommunity(bestNode);
        calculateLocalStrength(bestNode);
        updateShell(bestNode);
    }
    // current community is finished so add to list of communities and reset current community and shell
    m_communities->push_back(m_currentCommunity);
    m_currentCommunity = new std::unordered_set<Node*>();
    m_shell->clear();
    m_currentCommunityId += 1;
}

void SIWOplus::detectAllCommunities() {
    // iteratively detect communities until the entire graph is partitioned
    while (m_unclusteredNodes->size() > 0) {
        detectCommunity();
    }
}

int SIWOplus::findClosestCommunity(std::unordered_set<Node*>* community) {
    // need to track which of the adjacent communities have strongest ties to this small community
    std::unordered_map<int, double> similarCommunities{};
    // go through each node in small community and find the edge weights connecting to other communities
    for (Node* node : *community) {
        for (auto& node_edge : *node->getNeighbors()) {
            // don't count neighbors in same community
            if (node_edge.first->getCommunity() == node->getCommunity()) {
                continue;
            }
            double edge_weight{ node_edge.second->weight };
            int neighborCommunityId{ node_edge.first->getCommunity() };
            // check whether neighbors community is already in map
            auto comm_weight{ similarCommunities.find(neighborCommunityId) };
            if (comm_weight != similarCommunities.end()) {
                // since community already in map just increment value
                comm_weight->second += edge_weight;
            } else {
                // otherwise we insert a new key, value pair into map
                similarCommunities.insert(std::make_pair(neighborCommunityId, edge_weight));
            }
        }
    }
    // now we find the best community
    int bestCommunityId{ -1 };
    double bestConnection{ -1.0 };
    for (auto& id_connection : similarCommunities) {
        if (id_connection.second > bestConnection) {
            bestCommunityId = id_connection.first;
            bestConnection = id_connection.second;
        }
    }
    return bestCommunityId;
}

void SIWOplus::mergeSmallCommunities() {
    // it is possible for nodes that have no common neighbors or very few common neighbors with
    // other nodes (e.g. dangling nodes) to form small communities of 1 or 2 nodes, which we consider too small to be
    // genuine communities, since there can be no triangles in a community of 1 or 2.
    // Therefore we merge these communities with the neighboring community with which there is the
    // highest overall connection (i.e. sum of edge weights)
    for (auto community : *m_communities) {
        if (community->size() < 3) {
            int bestCommunityID{ findClosestCommunity(community) };
            // merge smaller community into larger community
            for (Node* node : *community) {
                node->setCommunity(bestCommunityID);
                m_communities->at(bestCommunityID)->insert(node);
            }
            community->clear();
        }
    }
}

void SIWOplus::printCommunity(int commIndex) {
    for (auto& node : *m_communities->at(commIndex))
        std::cout << node->m_id << ", ";
    std::cout << '\n';
}

void SIWOplus::mergeDanglingNodes() {
    // go through each node in network
    for (auto& node : *m_graph->getNodes()) {
        // check whether it is a dangling node ie. degree 1
        if (node.second->getDegree() == 1) {
            // remember old community id
            int oldID{ node.second->getCommunity() };
            // get the community id of the one neighbour
            int commID = -1;
            for (auto& node_edge : *node.second->getNeighbors()) {
                commID = node_edge.first->getCommunity();
            }
            // assign node to neighbour's community and clear its own community
            node.second->setCommunity(commID);
            m_communities->at(commID)->insert(node.second);
            m_communities->at(oldID)->clear();
        }
    }
}
