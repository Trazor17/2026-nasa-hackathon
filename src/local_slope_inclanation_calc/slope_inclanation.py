import numpy as np
import pandas as pd
import cv2

"""
The image that we are using for this calculation only contains dark red to dark blue between the hue values of 0 through 240
We only need to get the range to figure out how much each change in hue would correspond to a change in slope inclination.
"""

MAX_VAL = 24
MIN_VAL = 0

img = cv2.imread("src/images/local_slope_inclanation_cropped.png")
img_hue = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)[:, :, 0] * 2

df = pd.DataFrame(img_hue)

df_cond = df.map(lambda x: MIN_VAL + (1 - (x / 240)) * (MAX_VAL - MIN_VAL))
arr_cond = df_cond.to_numpy(dtype=np.float32)
arr_cond = cv2.resize(arr_cond, (1105, 1230), interpolation=cv2.INTER_LINEAR)
pd.DataFrame(arr_cond).to_csv("src/local_slope_inclanation_calc/local_slope_inclanation.csv", index=False, header=False)