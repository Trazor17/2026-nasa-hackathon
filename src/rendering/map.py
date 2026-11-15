from PIL import Image, ImageDraw
from pathlib import Path
import numpy as np
import math
import json
import copy
import argparse

IMG_SRC_MOSAIC = "src/images/cropped_black_white_mosaic.png"
IMG_SRC_TOPOGRAPHIC = "src/images/cropped_topographic.png"
RENDER_PATH = "renders/"

class Map:
    def __init__(self, base_img:str, map_dimensions:list = None, route:list = None):
        self.base = Image.open(base_img).convert("RGBA")

        if map_dimensions:
            self.map_dimensions = map_dimensions
            self.crop_to_region = True
        else:
            self.map_dimensions = self.base.size
            self.crop_to_region = False

        self.route = route

    def render(self, include_route, header_rotation = 0, header_location = None, waypoints = None, labeled_waypoint = None):
        header_size = 20
        marker_size = 7

        result = copy.deepcopy(self.base)

        ### RENDER ROUTE ###
        if include_route:
            header_location = self.route[0]

            path = Image.new("RGBA", self.base.size, (0, 0, 0, 0))
            draw_path = ImageDraw.Draw(path)

            for point in range(len(self.route)):
                if (point < len(self.route) - 1):
                    draw_path.line([self.route[point], self.route[point + 1]], fill=(255, 255, 255, 255), width=3)

            draw_path.circle(self.route[-1], marker_size, fill=(255, 255, 255, 255), outline=(0, 0, 0, 255), width=2)

            result = Image.alpha_composite(result, path)
        else:
            if not header_location:
                raise ValueError("Map render: missing header location (or route not included)")

        ### RENDER WAYPOINTS ###
        if waypoints:
            markers = Image.new("RGBA", self.base.size, (0, 0, 0, 0))
            draw_markers = ImageDraw.Draw(markers)

            for waypoint in waypoints:
                draw_markers.circle(waypoint, marker_size, fill=(255, 255, 0, 255), outline=(0, 0, 0, 255), width=2)

            result = Image.alpha_composite(result, markers)

        ### RENDER HEADER ###
        header = Image.new("RGBA", self.base.size, (0, 0, 0, 0))
        draw_header = ImageDraw.Draw(header)

        header_corners = [[header_location[0] - 8, header_location[1] - 10], [header_location[0], header_location[1] + 15], [header_location[0] + 8, header_location[1] - 10]]
        header_corners = self.__rotate_header(header_corners, header_location, header_rotation)

        draw_header.polygon(header_corners, fill=(255, 0, 0, 255), width=5)

        result = Image.alpha_composite(result, header)


        ### CROP ###
        if self.crop_to_region:
            x_dist = round(self.map_dimensions[0] / 2)
            y_dist = round(self.map_dimensions[1] / 2)
            region = [header_location[0] - x_dist, header_location[1] - y_dist, header_location[0] + x_dist, header_location[1] + y_dist]
            result = result.crop(region)

        ### SAVE IMAGE ###
        result.save(Path(RENDER_PATH + "render.png"))

    def __rotate_header(self, corners, center, angle) -> list:
        rad = np.deg2rad(angle)

        R = np.array([
            [np.cos(rad), -np.sin(rad)],
            [np.sin(rad),  np.cos(rad)]
        ])

        np_corners = np.array(corners)
        np_center = np.array(center)

        rotated_corners = np.dot((np_corners - np_center), R.T)
        return (rotated_corners + np_center).tolist()

if __name__ == "__main__":

    parser = argparse.ArgumentParser()

    parser.add_argument("-b", "--base_img", type=str)
    parser.add_argument("-x", "--x_width", type=int)
    parser.add_argument("-y", "--y_height", type=int)
    parser.add_argument("-d", "--direction_header", type=int, default=0)
    parser.add_argument("-r", "--route", type=str, default="")
    parser.add_argument("-w", "--waypoints", type=str, default="")
    parser.add_argument("-l", "--labeled_waypoint", type=str, default="")

    args = parser.parse_args()

    base_img = args.base_img
    map_dimensions = [args.x_width, args.y_height]
    header_rotation = args.direction_header

    if args.route != "":
        route = json.loads(args.route)
        include_route = True
    else:
        route = None
        include_route = False

    if args.waypoints != "":
        waypoints = json.loads(args.waypoints)
    else:
        waypoints = None

    if args.labeled_waypoint != "":
        labeled_waypoint = json.loads(args.labeled_waypoint)
    else:
        labeled_waypoint = None


    map = Map(base_img, map_dimensions=map_dimensions, route=route)
    map.render(include_route, header_rotation=header_rotation, waypoints=waypoints, labeled_waypoint=labeled_waypoint)
