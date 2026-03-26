#pragma once
#include <vector>
#include <queue>
#include "Track.h"

using namespace std;

class VPTree {
    struct Node {
        Track track;
        double radius;
        Node* left;
        Node* right;

        Node(const Track& track, double radius) : track(track), radius(radius), left(nullptr), right(nullptr) {}
    };

    int n_nodes;
    Node* root;

    Node* build_recursive(vector<Track>& tracks, int lower_idx, int upper_idx);
    void delete_recursive(Node* node);
    void search_recursive(Node* node, const Track& query, int k, priority_queue<pair<double, Node*>>& best) const;

public:
    VPTree(const vector<Track>& tracks);
    ~VPTree();
    vector<pair<double, Track>> k_nearest_neighbors(const Track& query, int k) const;
    int size() const;
};