#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>
#include <limits>
#include <queue>
#include <algorithm>
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
    double fScore; // total estimated cost(exact path cost + estimated future cost)

    // flips queue organization so the lowest score items are on top
    bool operator>(const AStarNode& other) const {
        return fScore > other.fScore;
    }
};

vector<vector<string>> getDataFromFile(const string&);
vector<vector<Pixel>> parseData(const vector<vector<string>> &);
vector<vector<LongLatPixel>> parseLongLatData(const vector<vector<string>> &,int,int);
LongLatPixel getCoordsFromLongLat(const vector<vector<LongLatPixel>>&,double,double);
double calcMoveCost(int,int,int,int,vector<vector<Pixel>>&,vector<vector<Pixel>>&,vector<vector<Pixel>>&,vector<vector<Pixel>>&);
vector<pair<int,int>> findPathAStar(int,int,int,int,int,int,vector<vector<Pixel>>&,vector<vector<Pixel>>&,vector<vector<Pixel>>&,vector<vector<Pixel>>&);

int main() {
    int imageWidth = 1105;
    int imageHeight = 1230;

    vector<vector<string>> longLatData = getDataFromFile("pixelLongitudes&Latitudes.csv");
    vector<vector<string>> elevationData = getDataFromFile("elevation_data.csv");
    vector<vector<string>> slopeData = getDataFromFile("local_slope_inclanation.csv");
    vector<vector<string>> shadeData = getDataFromFile("shade_data.csv");
    vector<vector<string>> windData = getDataFromFile("surface_wind_stress.csv");

    vector<vector<LongLatPixel>> allLongLatPixels = parseLongLatData(longLatData, imageHeight, imageWidth);
    vector<vector<Pixel>> allHeightPixels = parseData(elevationData);
    vector<vector<Pixel>> allSlopePixels = parseData(slopeData);
    vector<vector<Pixel>> allShadePixels = parseData(shadeData);
    vector<vector<Pixel>> allWindPixels = parseData(windData);

    // top left
    double startLongitude = 137.13782;
    double startLatitude = -4.26659;
    // bottom right
    double endLongitude = 137.70911;
    double endLatitude = -4.89144;

    LongLatPixel startPixel = getCoordsFromLongLat(allLongLatPixels, startLongitude, startLatitude);
    LongLatPixel endPixel = getCoordsFromLongLat(allLongLatPixels, endLongitude, endLatitude);

    auto path = findPathAStar(startPixel.x, startPixel.y, endPixel.x, endPixel.y, imageWidth, imageHeight, allHeightPixels, allSlopePixels, allShadePixels, allWindPixels);

    //basic testing for now
    if (path.empty()) {
        cout << "no valid path" << endl;
    }
    else {
        cout << "path found with " << path.size() << " steps" << endl;
    }

    for (auto p : path) {
        cout << p.first << " " << p.second << endl;
    }
    return 0;
}

vector<vector<string>> getDataFromFile(const string& filepath) {
    vector<vector<string>> fileData;
    string tempLine;
    ifstream longLatFile(filepath);
    if (!longLatFile.is_open()) {
        cerr << "Error: file couldn't open" << endl;
        return fileData;
    }

    // Read the file line by line
    while (getline(longLatFile, tempLine)) {
        vector<string> row;
        stringstream ss(tempLine);
        string cell;

        // Split each line by the comma delimiter
        while (getline(ss, cell, ',')) {
            row.push_back(cell);
        }

        fileData.push_back(row);
    }

    longLatFile.close();
    return fileData;
}

vector<vector<Pixel>> parseData(const vector<vector<string>> &data) {
    vector<vector<Pixel>> pixels;
    for (int y = 0; y < data.size(); y++) {
        const auto& row = data[y];
        vector<Pixel> pixelRow;
        for (int x = 0; x < row.size(); x++) {
            Pixel pixel{};
            pixel.x = x;
            pixel.y = y;
            pixel.data = stod(row[x]);
            pixelRow.push_back(pixel);
        }
        pixels.push_back(pixelRow);
    }

    return pixels;
}

vector<vector<LongLatPixel>> parseLongLatData(const vector<vector<string>> &longLatData, const int height, const int width) {
    vector<vector<LongLatPixel>> pixels(height, vector<LongLatPixel>(width));
    // i starts at 1 because row 0 is a header in longLatData
    for (int i = 1; i < longLatData.size(); i++) {
        const auto& row = longLatData[i];

        try {
            LongLatPixel pixel{};
            pixel.x = stoi(row[0]);
            pixel.y = stoi(row[1]);
            pixel.longitude = stod(row[2]);
            pixel.latitude = stod(row[3]);

            pixels[pixel.y][pixel.x] = pixel;
        } catch (const exception& e) {
            cerr << "Skipping invalid row " << i << ": " << e.what() << endl;
        }
    }

    return pixels;
}

LongLatPixel getCoordsFromLongLat(const vector<vector<LongLatPixel>>& pixels, const double longitude, const double latitude) {
    if (pixels.empty()) {
        cerr << "Error: pixel list is empty!" << endl;
        return LongLatPixel{-1, -1, 0.0, 0.0};
    }
    LongLatPixel closestPixel = pixels[0][0];
    double minDiagDist = numeric_limits<double>::infinity();

    for (const auto & row : pixels) {
        for (const auto & pixel : row) {
            double longDiff = longitude - pixel.longitude;
            double latDiff = latitude - pixel.latitude;
            double dist = hypot(longDiff, latDiff); // sqrt(dx^2 + dy^2)

            if (dist < minDiagDist) {
                minDiagDist = dist;
                closestPixel = pixel;
            }
        }
    }
    return closestPixel;
}

double calcMoveCost(int x1, int y1, int x2, int y2, vector<vector<Pixel>>& elevation, vector<vector<Pixel>>& slope, vector<vector<Pixel>>& shade, vector<vector<Pixel>>& wind) {
    // MOVEMENT WEIGHTS
    const double ELEVATION_CHANGE_WEIGHT = 3.0;
    const double SLOPE_WEIGHT = 5.0;
    const double SHADE_WEIGHT = 0.5;
    const double WIND_WEIGHT = 2.0;

    double baseDistance = 1;
    if (x1 != x2 && y1 != y2) {
        baseDistance = sqrt(2); // diagonal movement
    }

    double elevationChange = abs(elevation[y2][x2].data - elevation[y1][x1].data);
    double slopeAtNext = slope[y2][y1].data;
    double shadeAtNext = shade[y2][x2].data;
    double windAtNext = wind[y2][x2].data;

    /* If we want to do hard cutoffs we can do something like this:
    if(slopeAtNext >= 45.0){
        return numeric_limits<double>::infinity();
    }
    */
    double totalPenalty = 1.0 + (ELEVATION_CHANGE_WEIGHT * elevationChange) + (SLOPE_WEIGHT * slopeAtNext) + (SHADE_WEIGHT * shadeAtNext) + (WIND_WEIGHT * windAtNext);

    return baseDistance * totalPenalty;
}

vector<pair<int,int>> findPathAStar(int startX,int startY,int endX,int endY,int width,int height,vector<vector<Pixel>>& elevation,vector<vector<Pixel>>& slope,vector<vector<Pixel>>& shade,vector<vector<Pixel>>& wind) {
    // queue of nodes to visit, sorted so the lowest fScore is on top
    priority_queue<AStarNode, vector<AStarNode>, greater<>> openSet;

    // 2d grid of cheapest known cost to reach pixel. starts at infinity since we haven't visited anything yet
    vector<vector<double>> gScore(height, vector<double>(width, numeric_limits<double>::infinity()));
    gScore[startY][startX] = 0.0; // starting point requires no distance to reach

    // 2d grid that stores the coordinates of the pixel we used to get to the pixel at the current coordinate. starts at -1, -1 to signify the starting point
    vector<vector<pair<int,int>>> parent(height, vector<pair<int,int>>(width, {-1,-1}));

    // 2d grid storing whether we've been to a pixel yet
    vector<vector<bool>> closedSet(height, vector<bool>(width, false));

    // lambda function that calculates heuristic(estimated future cost)
    auto heuristic = [&](int x, int y) {
        return hypot(endX - x, endY - y);
    };

    // parallel arrays that give each direction as numbers to add to indices from the middle(when [i] = 0, the direction is up and left (-1,-1), when [i] = 1, the direction is up(0,-1), etc)
    const int directionX[8] = {-1,0,1,-1,1,-1,0,1};
    const int directionY[8] = {-1,-1,-1,0,0,1,1,1};

    // adds a starting point for the loop
    openSet.push({startX, startY, heuristic(startX, startY)});

    while (!openSet.empty()) {
        AStarNode currentNode = openSet.top();
        openSet.pop();

        int currentX = currentNode.x;
        int currentY = currentNode.y;

        if (currentX == endX && currentY == endY) {
            // reconstruct the path from end to start
            vector<pair<int,int>> path;
            while (currentX != -1 && currentY != -1) {
                path.emplace_back(currentX,currentY);
                auto p = parent[currentY][currentX];
                currentX = p.first;
                currentY = p.second;
            }
            // reverses path since we built the path from end to finish
            reverse(path.begin(), path.end());
            return path;
        }

        if (closedSet[currentY][currentX]) continue;
        closedSet[currentY][currentX] = true;

        // check neighbors
        for (int i = 0; i < 8; i++) {
            int nextX = currentX + directionX[i];
            int nextY = currentY + directionY[i];

            // out of bounds check
            if (nextX < 0 || nextX >= width || nextY < 0 || nextY >= height) continue;
            // if next spot already visited, skip
            if (closedSet[nextY][nextX]) continue;

            double moveCost = calcMoveCost(currentX, currentY, nextX, nextY, elevation, slope, shade , wind);
            //if (isinf(moveCost)) continue; does nothing unless we have hard cutoffs in calcMoveCost

            double tentativeGScore = gScore[currentY][currentX] + moveCost;

            if (tentativeGScore < gScore[nextY][nextX]) {
                parent[nextY][nextX] = {currentX, currentY};
                gScore[nextY][nextX] = tentativeGScore;
                double fScore = tentativeGScore + heuristic(nextX, nextY);
                openSet.push({nextX, nextY, fScore});
            }
        }
    }
    // no valid path
    return {};
}
