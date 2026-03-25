//
// Created by alexk on 3/22/2026.
//

#ifndef TRACK_H
#define TRACK_H

#include <string>
#include <vector>

using namespace std;

struct Track {
    // int index = -1;

    string track_id;
    string track_name;
    string artists;
    string album_name;
    string track_genre;

    int popularity = 0;
    int duration_ms = 0;
    bool explicit_flag = false;

    // double danceability = 0.0;
    // double energy = 0.0;
    // double loudness = 0.0;
    // double speechiness = 0.0;
    // double acousticness = 0.0;
    // double instrumentalness = 0.0;
    // double liveness = 0.0;
    // double valence = 0.0;
    // double tempo = 0.0;

    vector<double> audio_features;
};

#endif
