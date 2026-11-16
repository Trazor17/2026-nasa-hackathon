
#ifndef PATHFINDING_H
#define PATHFINDING_H

#include <string>
#include <utility>
#include <vector>
#include <utility>
using namespace std;

struct Pixel {
    int x;
    int y;
    double data;
};

struct LongLatPixel {
    int x;
    int y;
    double longitude;
    double latitude;
};

struct AStarNode {
    int x;
    int y;
    double fScore;

    bool operator>(const AStarNode& other) const {
        return fScore > other.fScore;
    }
};

vector<vector<string>> getDataFromFile(
    const string& filepath
);

vector<vector<Pixel>> parseData(
    const vector<vector<string>>& data
);

vector<vector<LongLatPixel>> parseLongLatData(
    const vector<vector<string>>& longLatData,
    int height,
    int width
);

LongLatPixel getCoordsFromLongLat(
    const vector<vector<LongLatPixel>>& pixels,
    double longitude,
    double latitude
);

double calcMoveCost(
    int x1, int y1, int x2, int y2,
    vector<vector<Pixel>>& elevation,
    vector<vector<Pixel>>& slope,
    vector<vector<Pixel>>& shade,
    vector<vector<Pixel>>& wind
);

vector<pair<int, int>> findPathAStar(
    int startX, int startY,
    int endX, int endY,
    int width, int height,
    vector<vector<Pixel>>& elevation,
    vector<vector<Pixel>>& slope,
    vector<vector<Pixel>>& shade,
    vector<vector<Pixel>>& wind
);

vector<pair<int,int>> generatePath(double,double,double,double);

#endif // PATHFINDING_H
