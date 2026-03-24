#pragma once
#include <utility>
#include "Track.h"

using namespace std;

class KDTree {
    struct Node {
        Track track;
        int axis;
        Node* left;
        Node* right;

        Node(const Track& track, int axis) : track(track), axis(axis), left(nullptr), right(nullptr) {}
    };

    int n_dims;
    int n_nodes;
    Node* root;

    Node* build_recursive(vector<Track>& tracks, int depth, int lower_idx, int upper_idx);

public:
    KDTree(const vector<Track>& tracks);
    ~KDTree();
    vector<pair<Track, double>> k_nearest_neighbors(const Track& query, int k) const;
    int size() const;
};