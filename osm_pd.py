# Read the SLEUTH output control_stats.log file and compute OSM, then sort them in descending order.
# Author: Gargi Chaudhuri
# Date: December 8, 2022

import pandas as pd
import os

# Run in the directory where the input file is

data = 'control.stats.log'# input() when run from the console?
df = pd.read_csv(data, skiprows = 1, header=0, sep='\s+', engine='python')

df['osm'] = 0
df['osm'] = df["Compare"]*df["Pop"]*df["Edges"]*df["Clusters"]*df["Slope"]*df["Xmean"]* df["Ymean"]
df_osm = df.sort_values(by=['osm'], ascending=False)
df_osm_50 = df_osm.iloc[0:50, :]
df_osm_50 = df_osm.loc[:, ['Diff', 'Brd', 'Sprd', 'Slp', 'RG', 'osm']]
df_osm_50.to_excel("top50_osm.xlsx")
