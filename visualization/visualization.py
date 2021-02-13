import rasterio
import os
import numpy as np
import imageio
from argparse import ArgumentParser
import sys
from changemaps import *


global directory, input_directory, output_directory
directory = os.getcwd()


def dir_path(string):
    if os.path.isdir(string):
        return string
    else:
        raise NotADirectoryError


def get_args():
    parser = ArgumentParser(prog="python3 {}".format(sys.argv[0]))
    parser.add_argument('-input', help="path to directory with input data", type=dir_path, default=directory)
    parser.add_argument('-output', help="specify location to put output data", type=dir_path, default=directory)
    return parser.parse_args()


def check_name(arg):
    is_int = True
    try:
        int(arg)
    except ValueError:
        is_int = False
    return is_int


def produce_change_maps(files, file_num):
    os.chdir(output_directory)
    if not os.path.exists('change_maps'):
        os.makedirs('change_maps')

    for i in range(0, file_num - 1):
        generate_changes_image(input_directory, output_directory, files[i], files[i + 1])

    generate_changes_image(input_directory, output_directory, files[0], files[file_num - 1])
    generate_changes_image(input_directory, output_directory, files[0], files[file_num - 1], 1)


def create_animation(files):
    images = []
    os.chdir(output_directory)
    if not os.path.exists('animation'):
        os.makedirs('animation')

    os.chdir(input_directory)
    for file in files:
        images.append(imageio.imread(file))

    os.chdir(output_directory)
    imageio.mimsave('./animation/animation.gif', images)


if __name__ == "__main__":
    parser = get_args()
    input_directory = parser.input
    output_directory = parser.output
    files = []

    for file in os.listdir(input_directory):
        if file.endswith(".gif"):
            x = str(file).split(".")[1]
            if check_name(x):
                files.append(file)
    files.sort()
    produce_change_maps(files, len(files))
    create_animation(files)
