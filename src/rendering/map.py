from PIL import Image, ImageDraw
import math
import sys
import json
from pathlib import Path

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

    def render(self, include_route, header_size = 20, header_rotation = 0, header_location = None):
        if include_route:
            header_location = self.route[0]

            path = Image.new("RGBA", self.base.size, (0, 0, 0, 0))
            draw_path = ImageDraw.Draw(path)

            for point in range(len(self.route)):
                if (point < len(self.route) - 1):
                    draw_path.line([self.route[point], self.route[point + 1]], fill=(255, 255, 255, 255), width=3)

            header = Image.new("RGBA", self.base.size, (0, 0, 0, 0))
            draw_header = ImageDraw.Draw(header)

            draw_header.regular_polygon((header_location[0], header_location[1], header_size), n_sides=3, rotation=header_rotation, fill=(255, 0, 0, 255), width=5)

            result = Image.alpha_composite(self.base, Image.alpha_composite(path, header))

        else:
            header = Image.new("RGBA", self.base.size, (0, 0, 0, 0))
            draw_header = ImageDraw.Draw(header)

            draw_header.regular_polygon((header_location[0], header_location[1], header_size), n_sides=3, rotation=header_rotation, fill=(255, 0, 0, 255), width=5)

            result = Image.alpha_composite(self.base, header)

        if self.crop_to_region:
            x_dist = round(self.map_dimensions[0] / 2)
            y_dist = round(self.map_dimensions[1] / 2)
            region = [header_location[0] - x_dist, header_location[1] - y_dist, header_location[0] + x_dist, header_location[1] + y_dist]
            result = result.crop(region)

        result.save(Path(RENDER_PATH + "render.png"))

    def __get_path_angle(self, pt1:list, pt2:list):
        rad = math.atan2(pt2[1] - pt1[1], pt2[0] - pt1[0])
        return math.degrees(rad)

if __name__ == "__main__":

    base_img = sys.argv[1]
    map_dimensions = [int(sys.argv[2]), int(sys.argv[3])]
    route = json.loads(sys.argv[4])

    map = Map(base_img, map_dimensions=map_dimensions, route=route)
    map.render(True)

    '''route = [(552, 588), (420, 810), (580, 920)]
    map = Map(IMG_SRC_MOSAIC, route=route)
    map.render(True)'''
