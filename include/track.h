#pragma once
#include <string>
#include <vector>

using namespace std;

struct Track {
    int id;
    string title;
    string artist;
    vector<double> features;
};