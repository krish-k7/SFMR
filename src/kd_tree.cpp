#include <vector>
#include <queue>
#include <utility>
#include <algorithm>
#include <cmath>
#include "Track.h"
#include "kd_tree.h"

using namespace std;

double compute_distance(const vector<double>& v1, const vector<double>& v2) {
    if (v1.size() != v2.size()) return -1;

    double sum = 0;
    for (size_t i = 0; i < v1.size(); i++) {
        double dim_i_diff = v2[i] - v1[i];
        sum += dim_i_diff * dim_i_diff;
    }

    return sqrt(sum);
}

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

void KDTree::delete_recursive(Node* node) {
    if (node == nullptr) return;

    // postorder traversal for deletion
    delete_recursive(node->left);
    delete_recursive(node->right);
    delete node;
}

void KDTree::search_recursive(Node* node, const Track& query, int k, priority_queue<pair<double, Node*>>& best) const {
    if (node == nullptr) return;
    
    double dist = compute_distance(query.audio_features, node->track.audio_features);

    if (query.track_id != node->track.track_id) {
        if (best.size() < k) {
            best.push({dist, node});
        } else if (dist < best.top().first) {
            best.pop();
            best.push({dist, node});
        }
    }

    int axis = node->axis;
    Node* primary = nullptr;
    Node* secondary = nullptr;

    if (query.audio_features[axis] <= node->track.audio_features[axis]) {
        primary = node->left;
        secondary = node->right;
    } else {
        primary = node->right;
        secondary = node->left;
    }

    search_recursive(primary, query, k, best);

    double plane_dist = abs(query.audio_features[axis] - node->track.audio_features[axis]); 
    if (best.size() < k || plane_dist <= best.top().first) {
        search_recursive(secondary, query, k, best);
    }
}

KDTree::KDTree(const vector<Track>& tracks) {
    this->n_dims = tracks[0].audio_features.size();
    this->n_nodes = 0;

    if (tracks.empty()) return;

    vector<Track> tracks_v(tracks);
    this->root = build_recursive(tracks_v, 0, 0, tracks_v.size() - 1);
}

KDTree::~KDTree() {
    delete_recursive(this->root);
}

vector<pair<double, Track>> KDTree::k_nearest_neighbors(const Track& query, int k) const {
    priority_queue<pair<double, Node*>> best;
    search_recursive(this->root, query, k, best);

    vector<pair<double, Track>> results;
    while (!best.empty()) {
        const auto r = best.top();
        results.push_back({r.first, r.second->track});
        best.pop();
    }

    return results;
}

int KDTree::size() const {
    return this->n_nodes;
}