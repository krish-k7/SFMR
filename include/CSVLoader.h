//
// Created by alexk on 3/23/2026.
//

#ifndef CSVLOADER_H
#define CSVLOADER_H

#include <string>
#include <vector>
#include "Track.h"

using namespace std;

class CSVLoader {
public:
    static vector<Track> loadTracks(const string& filePath);

private:
    static vector<string> parseCSVLine(const string& line);
    static string trim(const string& s);
    static string unquote(const string& s);
};

#endif //CSVLOADER_H
