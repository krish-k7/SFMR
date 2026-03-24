#include <vector>
#include <utility>
#include <algorithm>
#include "Track.h"
#include "kd_tree.h"

using namespace std;

KDTree::Node* KDTree::build_recursive(vector<Track>& tracks, int depth, int lower_idx, int upper_idx) {
    if (upper_idx < lower_idx) return nullptr;
    
    int axis = depth % n_dims;
    auto lower_it = tracks.begin() + lower_idx;
    auto upper_it = tracks.begin() + upper_idx + 1;

    struct {
        int dim;
        
        bool operator()(const Track& a, const Track& b) const {
            return a.audio_features[dim] < b.audio_features[dim];
        }
    } sort_comparator{axis};

    sort(lower_it, upper_it, sort_comparator);

    int med_idx = lower_idx + (upper_idx - lower_idx) / 2;
    auto med_it = tracks.begin() + med_idx;

    Node* node = new Node(*med_it, axis);
    n_nodes++;

    node->left = build_recursive(tracks, depth + 1, lower_idx, med_idx - 1);
    node->right = build_recursive(tracks, depth + 1, med_idx + 1, upper_idx);

    return node;
}

KDTree::KDTree(const vector<Track>& tracks) {
    this->n_dims = 9;
    this->n_nodes = 0;
    vector<Track> tracks_v(tracks);
    this->root = build_recursive(tracks_v, 0, 0, tracks_v.size() - 1);
}