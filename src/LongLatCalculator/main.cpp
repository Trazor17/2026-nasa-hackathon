#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>

using namespace std;

class LatLongPixel {
public:
    int x;
    int y;
    double longitude;
    double latitude;

    explicit LatLongPixel(int a = 0, int b = 0, double lon = 0.0, double lat = 0.0):
    x(a), y(b), longitude(lon), latitude(lat) {}

    void Print() const {
        cout << "Pixel(" << x << "," << y << ") has latitude " << latitude << " and longitude " << longitude << endl;
    }
};

class LatLongInterpolator {
    LatLongPixel topLeft;
    LatLongPixel topRight;
    LatLongPixel botLeft;
    LatLongPixel botRight;

    /* x is the current coordinate value, x1 is the first point, x2 is the final point
    val1 is the value at the first point, val2 is the value at the second point */
    double interpolate(int x, int x1, double val1, int x2, double val2) {
        return val1 + static_cast<double>(x-x1) * ((val2-val1) / static_cast<double>(x2 - x1));
    }
public:
    LatLongInterpolator(LatLongPixel tl,LatLongPixel tr,LatLongPixel bl,LatLongPixel br):
    topLeft(tl), topRight(tr), botLeft(bl), botRight(br){}

    LatLongPixel getLatLongFromPixel(int xCoord, int yCoord) {
        // 1. Interpolate Longitude along top and bottom rows using pixel x
        double topLon = interpolate(xCoord, topLeft.x, topLeft.longitude, topRight.x, topRight.longitude);
        double botLon = interpolate(xCoord, botLeft.x, botLeft.longitude, botRight.x, botRight.longitude);

        // 2. Interpolate vertically between topLon and botLon using pixel y
        double finalLon = interpolate(yCoord, topLeft.y, topLon, botLeft.y, botLon);

        // 3. Repeat the exact same process for Latitude
        double topLat = interpolate(xCoord, topLeft.x, topLeft.latitude, topRight.x, topRight.latitude);
        double botLat = interpolate(xCoord, botLeft.x, botLeft.latitude, botRight.x, botRight.latitude);
        double finalLat = interpolate(yCoord, topLeft.y, topLat, botLeft.y, botLat);

        return LatLongPixel(xCoord, yCoord, finalLon, finalLat);
    }
};
int main() {

    LatLongPixel topLeft(0,0,137.12251,-4.24913);
    LatLongPixel topRight(1104,0,137.7298,-4.24896);
    LatLongPixel botLeft(0,1229,137.12247,-4.92547);
    LatLongPixel botRight(1104,1229,137.72981,-4.92547);

    LatLongInterpolator coordGen(topLeft, topRight, botLeft, botRight);

    int imageWidth = 1105;
    int imageHeight = 1230;

    vector pixelArray(imageWidth, vector<LatLongPixel>(imageHeight));

    for (int j = 0; j < imageHeight; j++) {
        for (int i = 0; i < imageWidth; i++) {
            pixelArray[i][j] = coordGen.getLatLongFromPixel(i,j);
        }
    }

    pixelArray[imageWidth / 2][imageHeight / 2].Print();

    ofstream latLongFile("pixelLongitudes&Latitudes.csv");
    if (!latLongFile.is_open()) {
        cerr << "Error: Could not open file for writing.\n";
        return 1;
    }

    latLongFile << fixed << setprecision(6);

    latLongFile << "x,y,longitude,latitude\n";

    for (int y = 0; y < imageHeight; ++y) {
        for (int x = 0; x < imageWidth; ++x) {
            LatLongPixel p = coordGen.getLatLongFromPixel(x, y);
            latLongFile << p.x << ',' << p.y << ',' << p.longitude << ',' << p.latitude << '\n';
        }
    }

    latLongFile.close();
    cout << "Successfully exported to pixelLongitudes&Latitudes.csv\n";
    return 0;
}
