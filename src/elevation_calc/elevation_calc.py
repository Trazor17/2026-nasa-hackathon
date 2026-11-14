import cv2
import pandas as pd
import numpy as np

IMAGE_NAME = "src/images/cropped_topographic.png"
KEY_FILE_NAME = "src/images/topographic_key.png"

UPPER_ELEVATION_RANGE = 21229
LOWER_ELEVATION_RANGE = -8200

img = cv2.imread(IMAGE_NAME)
if img is None:
    raise FileNotFoundError(f"Image file '{IMAGE_NAME}' not found.")

key_img = cv2.imread(KEY_FILE_NAME)

if key_img is None:
    raise FileNotFoundError(f"Key image file '{KEY_FILE_NAME}' not found.")

max_colors = 512

legend_colors = np.mean(key_img, axis=0).astype(np.float32)
step = max(1, len(legend_colors) // max_colors)
legend_colors = legend_colors[::step]

elevations = np.linspace(LOWER_ELEVATION_RANGE, UPPER_ELEVATION_RANGE, len(legend_colors), dtype=np.float32)

# print("legend color count:", len(legend_colors))
# print("legend elevation min/max:", float(legend_elevations.min()), float(legend_elevations.max()))

downscale = 0.25
h, w = img.shape[:2]
target_h = max(1, int(h * downscale))
target_w = max(1, int(w * downscale))

small_map = cv2.resize(img, (target_w, target_h), interpolation=cv2.INTER_AREA).astype(np.float32)
small_flat = small_map.reshape(-1, 3)

distances = np.linalg.norm(small_flat[:, None, :] - legend_colors[None, :, :], axis=2)
nearest_idx = distances.argmin(axis=1)
elevation_small = elevations[nearest_idx].reshape(target_h, target_w)

elevation_data = cv2.resize(elevation_small.astype(np.float32), (w, h), interpolation=cv2.INTER_LINEAR)

# print("height", elevation_data.shape[0], "width", elevation_data.shape[1])
# print("elevation min/max:", float(np.nanmin(elevation_data)), float(np.nanmax(elevation_data)))

pd.DataFrame(elevation_data).to_csv("src/elevation_calc/elevation_data.csv", index=False, header=False)