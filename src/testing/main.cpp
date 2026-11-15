#include <vector>
#include <nlohmann/json.hpp>
#include <utility>
#include "pathfinding.h"
using nlohmann::json;
using namespace std;

int main()
{
    /*vector<vector<double>> waypoints = {
        {137.44163, -4.58947},
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

    json j_w = waypoints;

    string cmd = "uv run src/rendering/map.py -b \"src/images/cropped_black_white_mosaic.png\" -x 1000 -y 1000 -d 73 -r \"" + j.dump() + "\"";

    system(cmd.c_str());

    vector<double> start_coords = {, };
    vector<double> end_coords = {};*/

    vector<pair<int, int>> route = generatePath(137.44163, -4.58947, 137.38036, -4.72476);

    json j = route;

    string cmd = "uv run src/rendering/map.py -b \"src/images/cropped_topographic.png\" -x 1000 -y 1000 -d 73 -r \"" + j.dump() + "\"";

    system(cmd.c_str());

    return 0;
}
