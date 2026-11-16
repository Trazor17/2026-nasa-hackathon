#include <vector>
#include <nlohmann/json.hpp>
#include <utility>
#include <string>
#include "pathfinding.h"
using nlohmann::json;
using namespace std;

int main()
{
    int max_width = 1105;
    int max_height = 1230;

    int render_width = 700;
    int render_height = 700;

    int header_rotation = 170;

    vector<vector<double>> waypoints_longlat = {
        {137.44163, 4.58947},
        {137.44793, -4.59006},
        {137.44833, -4.59001},
        {137.42628, -4.60707},
        {137.40217, -4.63634},
        {137.37724, -4.63634},
        {137.37724, -4.66241},
        {137.3714, -4.66794},
        {137.36569, -4.67907},
        {137.35778, -4.67856},
        {137.35334, -4.6954},
        {137.35722, -4.7112},
        {137.36832, -4.72053},
        {137.37273, -4.72369},
        {137.38036, -4.72476}
    };
    vector<vector<string>> longLatData = getDataFromFile("src/LongLatCalculator/pixelLongitudes&Latitudes.csv");
    vector<vector<LongLatPixel>> allLongLatPixels = parseLongLatData(longLatData, max_height, max_width);

    vector<vector<int>> waypoints = {};
    for (int i = 0; i < waypoints_longlat.size(); i++) {
        LongLatPixel pixel = getCoordsFromLongLat(allLongLatPixels, waypoints_longlat[i][0], waypoints_longlat[i][1]);
        waypoints.push_back({pixel.x, pixel.y});
    }
    json j_w = waypoints;

    vector<pair<int, int>> route = generatePath(137.5, -4.6, 137.5, -4.6);
    json j = route;

    vector<int> center = {553, 615};
    json j_c = center;

    string cmd = "uv run src/rendering/map.py -b \"src/images/cropped_topographic.png\" -x " + to_string(render_width) + " -y " + to_string(render_height) + " -d " + to_string(header_rotation) + " -r \"" + j.dump() + "\" -w \"" + j_w.dump() + "\" " + " -c \"" + j_c.dump() + "\"";

    system(cmd.c_str());

    return 0;
}
