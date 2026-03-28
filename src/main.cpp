#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>

#include "CSVLoader.h"
#include "Track.h"
#include "kd_tree.h"
#include "vp_tree.h"

using namespace std;

string toLower(string s) {
    for (char& c : s) {
        c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    }
    return s;
}

Track* findTrack(vector<Track>& tracks, const string& input) {
    size_t dash = input.find(" - ");
    if (dash == string::npos) {
        return nullptr;
    }

    string title = toLower(input.substr(0, dash));
    string artist = toLower(input.substr(dash + 3));

    if (title.empty() || artist.empty()) {
        return nullptr;
    }

    for (auto& track : tracks) {
        if (toLower(track.track_name) == title && toLower(track.artists) == artist) {
            return &track;
        }
    }

    return nullptr;
}

void printResults(const vector<pair<double, Track>>& results) {
    if (results.empty()) {
        cout << "No similar songs found.\n";
        return;
    }

    for (int i = static_cast<int>(results.size()) - 1, rank = 1; i >= 0; --i, ++rank) {
        cout << rank << ". " << results[i].second.track_name
             << " - " << results[i].second.artists
             << " (distance: " << results[i].first << ")\n";
    }
}

int main() {
    vector<Track> tracks;
    string csvPath;

    // cout << "SFMR: Simple & Fast Music Recommendations\n";
    cout << R"( ________  ________ _____ ______   ________
|\   ____\|\  _____\\   _ \  _   \|\   __  \
\ \  \___|\ \  \__/\ \  \\\__\ \  \ \  \|\  \
 \ \_____  \ \   __\\ \  \\|__| \  \ \   _  _\
  \|____|\  \ \  \_| \ \  \    \ \  \ \  \\  \|
    ____\_\  \ \__\   \ \__\    \ \__\ \__\\ _\
   |\_________\|__|    \|__|     \|__|\|__|\|__|
   \|_________|
    )" << "\n";
    cout << "\n\n";

    cout << "Enter CSV file path: ";
    getline(cin, csvPath);

    try {
        CSVLoader::loadTracks(csvPath, tracks);
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
        return 1;
    }

    if (tracks.empty()) {
        cout << "No tracks loaded.\n";
        return 1;
    }

    cout << "Loaded " << tracks.size() << " tracks.\n";

    cout << "\nChoose data structure:\n";
    cout << "1. KD Tree\n";
    cout << "2. VP Tree\n";
    cout << "Choice: ";

    string choice;
    getline(cin, choice);

    if (choice != "1" && choice != "2") {
        cout << "Invalid choice.\n";
        return 1;
    }

    KDTree* kdTree = nullptr;
    VPTree* vpTree = nullptr;

    if (choice == "1") {
        kdTree = new KDTree(tracks);
        cout << "Built KD Tree with " << tracks.size() << " tracks.\n\n";
    } else {
        vpTree = new VPTree(tracks);
        cout << "Built VP Tree with " << tracks.size() << " tracks.\n\n";
    }

    cout << "Number of recommendations: ";
    string kInput;
    getline(cin, kInput);
    int k = stoi(kInput);

    while (true) {
        string songInput;
        cout << "\nEnter song as Title - Artist (or q to quit): ";
        getline(cin, songInput);

        if (songInput == "q" || songInput == "Q") {
            break;
        }

        if (songInput.empty()) {
            cout << "Please enter a song title and artist.\n";
            continue;
        }

        if (songInput.find(" - ") == string::npos) {
            cout << "Please use the format: Title - Artist\n";
            continue;
        }

        Track* queryTrack = findTrack(tracks, songInput);

        if (queryTrack == nullptr) {
            cout << "Song not found.\n";
            continue;
        }

        vector<pair<double, Track>> results;

        if (choice == "1") {
            results = kdTree->k_nearest_neighbors(*queryTrack, k);
        } else {
            results = vpTree->k_nearest_neighbors(*queryTrack, k);
        }

        cout << "Similar songs to \"" << queryTrack->track_name << "\":\n";
        printResults(results);
    }

    cout << "Goodbye.\n";

    return 0;
}
