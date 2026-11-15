#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>
#include <limits>
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

vector<vector<string>> getDataFromFile(const string&);
vector<vector<Pixel>> parseData(const vector<vector<string>> &);
vector<vector<LongLatPixel>> parseLongLatData(const vector<vector<string>> &, int, int);
LongLatPixel getCoordsFromLongLat(const vector<vector<LongLatPixel>>&, double, double);

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

    double startLongitude;
    double startLatitude;
    double endLongitude;
    double endLatitude;

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