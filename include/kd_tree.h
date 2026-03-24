#pragma once
#include <vector>
#include <queue>
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
    void delete_recursive(Node* node);
    void search_recursive(Node* node, const Track& query, int k, priority_queue<pair<double, Node*>>& best) const;

public:
    KDTree(const vector<Track>& tracks);
    ~KDTree();
    vector<pair<double, Track>> k_nearest_neighbors(const Track& query, int k) const;
    int size() const;
};