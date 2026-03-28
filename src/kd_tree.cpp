#include <vector>
#include <queue>
#include <utility>
#include <algorithm>
#include "Track.h"
#include "kd_tree.h"
#include "distance.h"

using namespace std;

// Helper function called by constructor to build a KD Tree from a vector of Tracks
KDTree::Node* KDTree::build_recursive(vector<Track>& tracks, int depth, int lower_idx, int upper_idx) {
    if (upper_idx < lower_idx) return nullptr;
    
    // Cycle through the audio feature dimensions through each layer of the tree
    int axis = depth % n_dims;
    auto lower_it = tracks.begin() + lower_idx;
    auto upper_it = tracks.begin() + upper_idx + 1;

    struct {
        int dim;
        
        bool operator()(const Track& a, const Track& b) const {
            return a.audio_features[dim] < b.audio_features[dim];
        }
    } sort_comparator{axis};

    // Sort the current subarray on the current axis so the median can become this subtree's root
    sort(lower_it, upper_it, sort_comparator);

    int med_idx = lower_idx + (upper_idx - lower_idx) / 2;
    auto med_it = tracks.begin() + med_idx;

    // Store the median track at this node to keep the tree roughly balanced
    Node* node = new Node(*med_it, axis);
    n_nodes++;

    // Everything left of the median goes to the left subtree, and everything right goes to the right subtree
    node->left = build_recursive(tracks, depth + 1, lower_idx, med_idx - 1);
    node->right = build_recursive(tracks, depth + 1, med_idx + 1, upper_idx);

    return node;
}

// Helper function called by destructor to delete all nodes in the KD Tree
void KDTree::delete_recursive(Node* node) {
    if (node == nullptr) return;

    // postorder traversal for deletion
    delete_recursive(node->left);
    delete_recursive(node->right);
    delete node;
}

// Helper function called by k_nearest_neighbors to recursively search the KD Tree
void KDTree::search_recursive(Node* node, const Track& query, int k, priority_queue<pair<double, Node*>>& best) const {
    if (node == nullptr) return;
    
    double dist = compute_distance(query.audio_features, node->track.audio_features);

    if (query.track_id != node->track.track_id) { // Skip the query track itself
        // Keep the current k nearest tracks in a max heap
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

    // Search the side of the split that contains the query point first
    if (query.audio_features[axis] <= node->track.audio_features[axis]) {
        primary = node->left;
        secondary = node->right;
    } else {
        primary = node->right;
        secondary = node->left;
    }

    search_recursive(primary, query, k, best);

    // The distance from the query to the splitting plane is the minimum possible distance
    // to anything in the other subtree, so only search it if it could contain a track closer than
    // the current worst result
    double plane_dist = abs(query.audio_features[axis] - node->track.audio_features[axis]); 
    if (best.size() < k || plane_dist <= best.top().first) {
        search_recursive(secondary, query, k, best);
    }
}

// Constructor builds the full KD Tree from a copy of the track vector
KDTree::KDTree(const vector<Track>& tracks) {
    this->n_dims = tracks[0].audio_features.size();
    this->n_nodes = 0;

    if (tracks.empty()) return;

    vector<Track> tracks_v(tracks);
    this->root = build_recursive(tracks_v, 0, 0, tracks_v.size() - 1);
}

// Destructor deletes every node in the KD Tree
KDTree::~KDTree() {
    delete_recursive(this->root);
}

// Public function to return the k nearest neighbors of a query track
vector<pair<double, Track>> KDTree::k_nearest_neighbors(const Track& query, int k) const {
    priority_queue<pair<double, Node*>> best;
    search_recursive(this->root, query, k, best);

    vector<pair<double, Track>> results;
    // The max heap stores the farthest of the current best matches on top, so pop until empty
    while (!best.empty()) {
        const auto r = best.top();
        results.push_back({r.first, r.second->track});
        best.pop();
    }

    return results;
}

// Returns the total number of nodes currently stored in the KD Tree
int KDTree::size() const {
    return this->n_nodes;
}
