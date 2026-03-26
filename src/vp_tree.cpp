#include <vector>
#include <queue>
#include <utility>
#include <algorithm>
#include "Track.h"
#include "vp_tree.h"
#include "distance.h"

using namespace std;

VPTree::Node* VPTree::build_recursive(vector<Track>& tracks, int lower_idx, int upper_idx) {
    if (upper_idx < lower_idx) return nullptr;
    
    // always use last element as vantage point
    // -> tree build is deterministic (vs random selection)
    Track vp = tracks[upper_idx];
    upper_idx--;

    if (upper_idx < lower_idx) {
        Node* node = new Node(vp, 0.0);
        n_nodes++;
        return node;
    }

    auto lower_it = tracks.begin() + lower_idx;
    auto upper_it = tracks.begin() + upper_idx + 1;

    struct {
        Track* target;

        bool operator()(const Track& a, const Track& b) const {
            double dist_a = compute_distance(a.audio_features, target->audio_features);
            double dist_b = compute_distance(b.audio_features, target->audio_features);
            return dist_a < dist_b;
        }
    } sort_comparator{&vp};

    sort(lower_it, upper_it, sort_comparator);

    int med_idx = lower_idx + (upper_idx - lower_idx) / 2;
    auto med_it = tracks.begin() + med_idx;

    double radius = compute_distance((*med_it).audio_features, vp.audio_features);
    Node* node = new Node(vp, radius);
    n_nodes++;

    node->left = build_recursive(tracks, lower_idx, med_idx);
    node->right = build_recursive(tracks, med_idx + 1, upper_idx);

    return node;
}

void VPTree::delete_recursive(Node* node) {
    if (node == nullptr) return;

    // postorder traversal for deletion
    delete_recursive(node->left);
    delete_recursive(node->right);
    delete node;
}

VPTree::VPTree(const vector<Track>& tracks) {
    this->n_nodes = 0;

    if (tracks.empty()) return;

    vector<Track> tracks_v(tracks);
    this->root = build_recursive(tracks_v, 0, tracks_v.size() - 1);
}

VPTree::~VPTree() {
    delete_recursive(this->root);
}

int VPTree::size() const {
    return this->n_nodes;
}