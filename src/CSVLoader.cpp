//
// Created by alexk on 3/23/2026.
//

#include "CSVLoader.h"

#include <fstream>
#include <unordered_map>
#include <stdexcept>
#include <algorithm>
#include <cctype>

using namespace std;

string CSVLoader::trim(const string& s) {
    size_t start = 0;
    while (start < s.size() && isspace(static_cast<unsigned char>(s[start]))) {
        ++start;
    }

    size_t end = s.size();
    while (end > start && isspace(static_cast<unsigned char>(s[end - 1]))) {
        --end;
    }

    return s.substr(start, end - start);
}

string CSVLoader::unquote(const string& s) {
    string out = trim(s);

    if (out.size() >= 2 && out.front() == '"' && out.back() == '"') {
        out = out.substr(1, out.size() - 2);
    }

    string result;
    for (size_t i = 0; i < out.size(); ++i) {
        if (out[i] == '"' && i + 1 < out.size() && out[i + 1] == '"') {
            result += '"';
            ++i;
        } else {
            result += out[i];
        }
    }

    return result;
}

vector<string> CSVLoader::parseCSVLine(const string& line) {
    vector<string> fields;
    string current;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];

        if (c == '"') {
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                current += '"';
                ++i;
            } else {
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            fields.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }

    fields.push_back(current);
    return fields;
}

int CSVLoader::toIntSafe(const string& s) {
    string t = trim(s);
    if (t.empty()) return 0;
    return stoi(t);
}

double CSVLoader::toDoubleSafe(const string& s) {
    string t = trim(s);
    if (t.empty()) return 0.0;
    return stod(t);
}

bool CSVLoader::toBoolSafe(const string& s) {
    string t = trim(s);
    transform(t.begin(), t.end(), t.begin(),
              [](unsigned char c) { return static_cast<char>(tolower(c)); });
    return t == "true" || t == "1";
}

vector<Track> CSVLoader::loadTracks(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        throw runtime_error("Could not open CSV file: " + filePath);
    }

    string headerLine;
    if (!getline(file, headerLine)) {
        throw runtime_error("CSV file is empty.");
    }

    vector<string> headers = parseCSVLine(headerLine);
    unordered_map<string, size_t> col;

    for (size_t i = 0; i < headers.size(); ++i) {
        col[unquote(headers[i])] = i;
    }

    auto getField = [&](const vector<string>& row, const string& key) -> string {
        auto it = col.find(key);
        if (it == col.end() || it->second >= row.size()) return "";
        return unquote(row[it->second]);
    };

    vector<Track> tracks;
    string line;
    int idx = 0;

    while (getline(file, line)) {
        if (line.empty()) continue;

        vector<string> row = parseCSVLine(line);

        Track t;
        t.index = idx++;

        t.track_id = getField(row, "track_id");
        t.track_name = getField(row, "track_name");
        t.artists = getField(row, "artists");
        t.album_name = getField(row, "album_name");
        t.track_genre = getField(row, "track_genre");

        t.popularity = toIntSafe(getField(row, "popularity"));
        t.duration_ms = toIntSafe(getField(row, "duration_ms"));
        t.explicit_flag = toBoolSafe(getField(row, "explicit"));

        // t.danceability = toDoubleSafe(getField(row, "danceability"));
        // t.energy = toDoubleSafe(getField(row, "energy"));
        // t.loudness = toDoubleSafe(getField(row, "loudness"));
        // t.speechiness = toDoubleSafe(getField(row, "speechiness"));
        // t.acousticness = toDoubleSafe(getField(row, "acousticness"));
        // t.instrumentalness = toDoubleSafe(getField(row, "instrumentalness"));
        // t.liveness = toDoubleSafe(getField(row, "liveness"));
        // t.valence = toDoubleSafe(getField(row, "valence"));
        // t.tempo = toDoubleSafe(getField(row, "tempo"));

        t.audio_features.push_back(toDoubleSafe(getField(row, "danceability")));
        t.audio_features.push_back(toDoubleSafe(getField(row, "energy")));
        t.audio_features.push_back(toDoubleSafe(getField(row, "loudness")));
        t.audio_features.push_back(toDoubleSafe(getField(row, "speechiness")));
        t.audio_features.push_back(toDoubleSafe(getField(row, "acousticness")));
        t.audio_features.push_back(toDoubleSafe(getField(row, "instrumentalness")));
        t.audio_features.push_back(toDoubleSafe(getField(row, "liveness")));
        t.audio_features.push_back(toDoubleSafe(getField(row, "valence")));
        t.audio_features.push_back(toDoubleSafe(getField(row, "tempo")));

        if (!t.track_name.empty()) {
            tracks.push_back(t);
        }
    }

    for (int i = 0; i < static_cast<int>(tracks.size()); ++i) {
        tracks[i].index = i;
    }

    return tracks;
}
