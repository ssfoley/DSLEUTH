# -*- coding: utf-8 -*-
"""
Created on Mon Nov 16 17:08:30 2020

@author: elise
"""

from os import listdir
from os.path import isfile, join
from PIL import Image


def main():
    path = "C:/cygwin64/home/elise/DSLEUTH/Input/demo200"
    files = [f for f in listdir(path) if isfile(join(path, f))]
    
    count = 0
    print()
    while count < len(files):
        myfile = files[count]
        dfile = files[count + 1]
        
        
        '''
        print(type(myfile))
        print(type(dfile))
        print("___")
        '''
        im0 = Image.open(path + "\\" + myfile).convert('RGB')
        im1 = Image.open(path + "\\" + dfile).convert('RGB')
        x0, y0 = im0.size
        x1, y1 = im1.size
        print(f"{x0} == {x1} | {y0} == {y1}")
        if x0 != x1 or y0 != y1:
            print("ERROR")
            return
            
        for j in range(y0):
            for i in range(x0):
                r0, g0, b0 = im0.getpixel((j, i))
                r1, g1, b1 = im1.getpixel((j, i))
                #red, green, blue = Gdif.__hex_to_rgb(pixel_val)
                # Check that the image is a true grayscale image
                if r0 != r1 or g0 != g1 or b0 != b1:
                    print(f'{myfile} {dfile} FILES NOT THE SAME')
                    
        im0.close()
        im1.close()
        
        count += 2 






if __name__ == '__main__':
    main()