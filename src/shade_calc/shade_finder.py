import cv2
import pandas as pd
import numpy as np

img = cv2.imread("src/images/cropped_topographic.png")

value_img = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)[:, :, 2]

df = pd.DataFrame(value_img)
# print(df.min())
df_cond = df.map(lambda x: 1 if x < 50 else 0)

df_cond.to_csv("src/shade_calc/shade_data.csv", index=False, header=False)