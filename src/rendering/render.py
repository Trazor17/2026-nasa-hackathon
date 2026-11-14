from PIL import Image, ImageDraw, ImageTk
import tkinter as tk
import math

IMG_SRC_MOSAIC = "src/images/cropped_black_white_mosaic.png"
IMG_SRC_TOPOGRAPHIC = "src/images/cropped_topographic.png"

class Render:
    def __init__(self, base_img:str, header_location:list, route:list = None):
        self.base_img = base_img
        self.header_location = header_location
        self.route = route

    def render(self, header_size = 20, header_rotation = 0, include_route = False):
        base = Image.open(self.base_img).convert("RGBA")
        path = Image.new("RGBA", base.size, (0, 0, 0, 0))
        draw_path = ImageDraw.Draw(path)
        overlay = Image.new("RGBA", base.size, (0, 0, 0, 0))
        draw_overlay = ImageDraw.Draw(overlay)

        if include_route:
            for point in range(len(self.route)):
                if (point < len(self.route) - 1):
                    draw_path.line([self.route[point], self.route[point + 1]], fill=(255, 255, 255, 255), width=3)

        draw_overlay.regular_polygon((self.header_location[0], self.header_location[1], header_size), n_sides=3, rotation=header_rotation, fill=(255, 0, 0, 255), width=3)

        result = Image.alpha_composite(base, Image.alpha_composite(path, overlay))

        root = tk.Tk()
        root.title("Mars Minimap")

        tk_image = ImageTk.PhotoImage(result)

        label = tk.Label(root, image=tk_image)
        label.pack()

        root.mainloop()

    def __get_path_angle(self, pt1:list, pt2:list):
        rad = math.atan2(pt2[1] - pt1[1], pt2[0] - pt1[0])
        return math.degrees(rad)

if __name__ == "__main__":
    route = [(350, 490), (420, 810), (580, 920)]
    renderer = Render(IMG_SRC_MOSAIC, route[0], route)
    renderer.render(include_route=True)
