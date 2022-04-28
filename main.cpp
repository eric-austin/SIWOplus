//
//  main.cpp
//  SIWO
//
//  Created by Eric Austin on 2022-02-25.
//

#include <iostream>
#include <random>
#include <vector>
#include <queue>
#include <chrono>
#include <fstream>
#include "node.h"
#include "edge_data.h"
#include "graph.h"
#include "siwo.h"


int main(int argc, const char * argv[]) {
    std::string inPath{ argv[1] };
    std::string outPath{ argv[2] };
    std::string meanType{ argv[3] };

    auto start = std::chrono::steady_clock::now();
    
    if (meanType == "a")
        std::cout << "arithmetic mean\n";
    if (meanType == "g")
        std::cout << "geometric mean\n";

    Graph* g = new Graph(inPath);
    SIWOplus* siwo = new SIWOplus(g, meanType[0]);

    siwo->detectAllCommunities();
//    siwo->mergeSmallCommunities();
//    siwo->mergeDanglingNodes();
    
    auto end = std::chrono::steady_clock::now();
    
    auto diff = end - start;
    
    std::cout << std::chrono::duration<double, std::milli>(diff).count() << "ms\n";
    
    std::ofstream outStream{ outPath };
    
    for (auto& community : *siwo->m_communities) {
        if (community->size() > 0) {
            for (auto& node : *community) {
                outStream << node->getID() << ' ';
            }
            outStream << '\n';
        }
    }
    
    outStream.close();

    return 0;
}
