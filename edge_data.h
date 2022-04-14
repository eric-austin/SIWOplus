//
//  edge_data.h
//  SIWO
//
//  Created by Eric Austin on 2022-02-25.
//

#ifndef edge_data_h
#define edge_data_h

struct EdgeData {
    double weight{};
    double strength{};
    double support{};
    bool supportCalculated{};
    bool strengthCalculated{};
};

#endif /* edge_data_h */
