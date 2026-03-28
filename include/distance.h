#pragma once
#include <vector>
#include <cmath>

using namespace std;


// Helper function to compute the Euclidean distance between two feature vectors
inline double compute_distance(const vector<double>& v1, const vector<double>& v2) {
    if (v1.size() != v2.size()) return -1;

    double sum = 0;
    for (size_t i = 0; i < v1.size(); i++) {
        double dim_i_diff = v2[i] - v1[i];
        sum += dim_i_diff * dim_i_diff;
    }

    return sqrt(sum);
}
