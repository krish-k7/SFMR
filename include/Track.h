//
// Created by alexk on 3/22/2026.
//

#ifndef TRACK_H
#define TRACK_H

#include <string>
#include <vector>

using namespace std;

struct Track {

    string track_id;
    string track_name;
    string artists;
    string track_genre;

    int popularity = 0;
    int duration_ms = 0;

    vector<double> audio_features;
};

#endif
