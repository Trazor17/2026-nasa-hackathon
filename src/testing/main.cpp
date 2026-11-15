#include <vector>
#include <nlohmann/json.hpp>
using nlohmann::json;
using namespace std;

int main()
{
    /*vector<vector<int>> route = {
        {552, 588},
        {420, 810},
        {580, 920}
    };

    json j = route;

    string cmd = "uv run src/rendering/map.py \"src/images/cropped_black_white_mosaic.png\" 1000 1000 \"" + j.dump() + "\"";

    system(cmd.c_str());*/

    vector<double> start_coords = {300, 400};
    vector<double> end_coords = {};

    vector<vector<int>> route = {}; // get path

    json j = route;

    string cmd = "uv run src/rendering/map.py \"src/images/cropped_black_white_mosaic.png\" 1000 1000 \"" + j.dump() + "\"";

    system(cmd.c_str());

    return 0;
}
