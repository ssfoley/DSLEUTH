import base64
import json

import folium
import os
import shapefile
import numpy
from PIL import Image
import gdal


global total_count, diff_count
total_count = 0
diff_count = 0


def calc_midpoint(x1, y1, x2, y2):
    midx = (x1+x2)/2
    midy = (y1+y2)/2
    return (midx, midy)

def read_shapefiles():
    myshp = open(os.getcwd() + "/kolkatta" + "/india_footprint.shp", "rb")
    mydbf = open(os.getcwd() + "/kolkatta" + "/india_footprint.dbf", "rb")
    myshx = open(os.getcwd() + "/kolkatta" + "/india_footprint.shx", "rb")

    reader = shapefile.Reader(shp=myshp, dbf=mydbf, shx=myshx)
    return reader

def get_bounds(r):
    shapes = r.shapes()
    bbox = shapes[0].bbox
    bounds = []

    coordinates = []
    for coord in bbox:
        coordinates.append(coord)

    bottom_left = (coordinates[1], coordinates[0])
    upper_right = (coordinates[3], coordinates[2])
    bounds.append(bottom_left)
    bounds.append(upper_right)
    return bounds

def color_values(x):
    x = int(x)

    colors = {
        0: (0, 0, 0, 0),
        1: (0.54, 0.13, 0.13, 1),  # red urban
        2: (1, 0.92, 0.54, 1),  # yellow agric
        3: (0.92, 0.60, 0.28, 1),  # orange range
        4: (0, 0.39, 0, 1),  # green forest
        5: (0.06, 0.31, 0.54, 1),  # blue water
        6: (0.28, 0.24, 0.54, 1),  # blue wetland
        7: (0.93, 0.77, 0.59, 1),  # tan barren
        8: (0.19, 0.19, 0.19, 1),  # black? tundra
        9: (1, 1, 1, 1),  # white snow/ice
        11: (1, 1, 1, 1),  # white
        10: (0, 0, 1, 1),  # blue
        12: (0.94, 0.94, 0.94, 1),  # grey
        255: (0, 0, 0, 0)  # black
    }
    r = colors.get(x)
    if r is None:
        r = (0, 0, 0, 0)
    return r

"""
Runs on program execution
"""
if __name__ == "__main__":
    # raster = gdal.Open(os.getcwd() + "/changeinput" + "/india_land_n_urban.2011.gif")
    # geotransform = raster.GetGeoTransform()
    # print(geotransform)

    r = read_shapefiles()
    data = {}
    sam = Image.open(os.getcwd()+"/changeinput"+"/india_land_n_urban.2011.gif")

    b = get_bounds(r)
    midpoint = calc_midpoint(b[0][0], b[0][1], b[1][0], b[1][1])
    m = folium.Map(location=[midpoint[0], midpoint[1]])
    m.fit_bounds(bounds=b)

    img = folium.raster_layers.ImageOverlay(
        name='sample data',
        image=sam,
        bounds=b,
        opacity=0.6,
        interactive=True,
        colormap=lambda x: color_values(x),
        mercator_project=True,

    )

    img.add_to(m)
    folium.LayerControl().add_to(m)
    m.save(
        'another_image.html'
    )
    print(midpoint)


