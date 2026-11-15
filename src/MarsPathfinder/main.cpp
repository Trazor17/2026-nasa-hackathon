#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>
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
vector<vector<LongLatPixel>> parseLongLatData(const vector<vector<string>> &, const int, const int);
LongLatPixel getCoordsFromLongLat(const vector<LongLatPixel>&, double, double);

int main() {
    int imageWidth = 1105;
    int imageHeight = 1230;
    int vectorLength = imageWidth * imageHeight;
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

    int rowNum = 0;
    int totalNum = 0;
    cout << "LongLat" << endl;
    for (const auto& row : allLongLatPixels) {
        for (const auto& pixel : row) {
            //cout << pixel.data << " ";
            //if (pixel.data != 0) {
            totalNum++;
            //}
        }
        //cout << "endline" << endl;'
        rowNum++;
    }
    cout << totalNum << "(totalNum)" << endl;
    cout << rowNum << "(rowNum)" << endl;
    cout << (totalNum / rowNum) << "(colNum)" << endl;

    rowNum = 0;
    totalNum = 0;
    cout << "Height" << endl;
    for (const auto& row : allHeightPixels) {
        for (const auto& pixel : row) {
            //cout << pixel.data << " ";
            //if (pixel.data != 0) {
            totalNum++;
            //}
        }
        //cout << "endline" << endl;'
        rowNum++;
    }
    cout << totalNum << "(totalNum)" << endl;
    cout << rowNum << "(rowNum)" << endl;
    cout << (totalNum / rowNum) << "(colNum)" << endl;

    rowNum = 0;
    totalNum = 0;
    cout << "Slope" << endl;
    for (const auto& row : allSlopePixels) {
        for (const auto& pixel : row) {
            //cout << pixel.data << " ";
            //if (pixel.data != 0) {
            totalNum++;
            //}
        }
        //cout << "endline" << endl;'
        rowNum++;
    }
    cout << totalNum << "(totalNum)" << endl;
    cout << rowNum << "(rowNum)" << endl;
    cout << (totalNum / rowNum) << "(colNum)" << endl;

    rowNum = 0;
    totalNum = 0;
    cout << "Shade" << endl;
    for (const auto& row : allShadePixels) {
        for (const auto& pixel : row) {
            //cout << pixel.data << " ";
            //if (pixel.data != 0) {
                totalNum++;
            //}
        }
        //cout << "endline" << endl;'
        rowNum++;
    }
    cout << totalNum << "(totalNum)" << endl;
    cout << rowNum << "(rowNum)" << endl;
    cout << (totalNum / rowNum) << "(colNum)" << endl;

    rowNum = 0;
    totalNum = 0;
    cout << "Wind" << endl;
    for (const auto& row : allWindPixels) {
        for (const auto& pixel : row) {
            //cout << pixel.data << " ";
            //if (pixel.data != 0) {
            totalNum++;
            //}
        }
        //cout << "endline" << endl;'
        rowNum++;
    }
    cout << totalNum << "(totalNum)" << endl;
    cout << rowNum << "(rowNum)" << endl;
    cout << (totalNum / rowNum) << "(colNum)" << endl;

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

LongLatPixel getCoordsFromLongLat(const vector<LongLatPixel>& pixels, double longitude, double latitude) {
    if (pixels.empty()) {
        cerr << "Error: pixel list is empty!" << endl;
        return LongLatPixel{-1, -1, 0.0, 0.0};
    }
    double minLongDiff = abs(longitude - pixels[0].longitude);
    double minLatDiff = abs(latitude - pixels[0].latitude);
    double minTotalDiff = minLongDiff + minLatDiff;
    int minIndex = 0;
    for (int i = 1; i < pixels.size(); i++) {
        double longDiff = abs(longitude - pixels[i].longitude);
        double latDiff = abs(latitude - pixels[i].latitude);
        double totalDiff = longDiff + latDiff;
        if (totalDiff < minTotalDiff) {
            minTotalDiff = totalDiff;
            minIndex = i;
        }
    }
    return pixels[minIndex];
}