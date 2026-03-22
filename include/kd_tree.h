#pragma once
#include <utility>
#include "track.h"

using namespace std;

class KDTree {
    struct Node {
        Track track;
        int axis;
        Node* left;
        Node* right;
    };

    int n_dims;
    int n_nodes;
    Node* root;

public:
    KDTree(const vector<Track>& tracks);
    vector<pair<Track, int>> k_nearest_neighbors(const Track& query, int k) const;
    int size();
};