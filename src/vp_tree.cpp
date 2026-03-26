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

void VPTree::search_recursive(Node* node, const Track& query, int k, priority_queue<pair<double, Node*>>& best) const {
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

    Node* primary = nullptr;
    Node* secondary = nullptr;
    bool took_left = false;

    if (dist <= node->radius) {
        primary = node->left;
        secondary = node->right;
        took_left = true;
    } else {
        primary = node->right;
        secondary = node->left;
    }

    search_recursive(primary, query, k, best);

    bool take_secondary = false;

    if (best.size() < k) {
        take_secondary = true;
    } else if (took_left) {
        if (dist + best.top().first >= node->radius) take_secondary = true;
    } else {
        if (dist - best.top().first <= node->radius) take_secondary = true;
    }

    if (take_secondary) search_recursive(secondary, query, k, best);
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

vector<pair<double, Track>> VPTree::k_nearest_neighbors(const Track& query, int k) const {
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

int VPTree::size() const {
    return this->n_nodes;
}