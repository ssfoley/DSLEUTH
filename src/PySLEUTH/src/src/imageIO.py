from PIL import Image, ImageDraw
from scenario import Scenario
from logger import Logger
from timer import TimerUtility


class ImageIO:
    _red_mask = 0XFF0000
    _green_mask = 0X00FF00
    _blue_mask = 0X0000FF
    _date_x = 1
    _date_y = -1

    @staticmethod
    def write_color_key(colortable, fname, should_log, ncols):
        if should_log:
            Logger.log(f"writing Gif to {fname}")
            Logger.log(f"\t colortable name: {colortable.name}")
            Logger.log(f"\t colortable reference: {colortable} rows: {len(colortable.color)} cols: {ncols}")

        # open output gif file
        file = open(fname, 'w')
        # file = open(fname + "_PY", 'w')
        sx = ncols  # IGrid.igrid.slope.ncols
        sy = len(colortable.color)
        im = Image.new('RGB', (sx, sy))
        for i in range(sy):
            color = colortable.color[i]
            # color_string = " ".join(map(str,color))
            # file.write(color_string + "\n")
            for j in range(sx):
                im.putpixel((j, i), color)

        im.save(fname)
        im.close()
        file.close()

    @staticmethod
    def read_gif(grid, filename, grid_nrows, grid_ncols):
        TimerUtility.start_timer('gdif_ReadGIF')
        im = Image.open(filename).convert('RGB')
        ncols, nrows = im.size
        # print(f"{ncols} {nrows}  == {grid_ncols} {grid_nrows}")
        if ncols != grid_ncols or nrows != grid_nrows:
            print(f"{filename}: {ncols} x {nrows} image does not match expected size {grid_ncols}x{nrows}")
            raise Exception
        max_pixel = -1
        min_pixel = 300

        test_file = open(f"{Scenario.get_scen_value('output_dir')}ReadingRoad", "w")

        for j in range(grid_ncols):
            for i in range(grid_nrows):
                red, green, blue = im.getpixel((j, i))
                # red, green, blue = Gdif.__hex_to_rgb(pixel_val)
                # Check that the image is a true grayscale image
                if red == green and red == blue:
                    index = i * grid_ncols + j
                    grid.gridData[index] = red
                    test_file.write(f"{red}\n")
                    if red > max_pixel:
                        max_pixel = red
                    if red < min_pixel:
                        min_pixel = red
                else:
                    print(f'File is not a true gray scale image -> {red} {green} {blue}')

        test_file.close()
        im.close()
        grid.max = max_pixel
        grid.min = min_pixel
        TimerUtility.stop_timer('gdif_ReadGIF')

    @staticmethod
    def write_gif(grid, colortable, fname, date, grid_nrows, grid_ncols):
        date_color = Scenario.get_scen_value("date_color")
        TimerUtility.start_timer('gdif_WriteGIF')
        if Scenario.get_scen_value("logging") and Scenario.get_scen_value("log_writes"):
            Logger.log(f"Writing GIF {fname}")
            Logger.log(f"colortable name={colortable.name} date={date}")
            Logger.log(f"rows={grid_nrows} cols={grid_ncols}")
            Logger.log(f"date color index = {date_color}")

        ImageIO._date_y = grid_nrows - 16

        file = open(fname, 'w')

        im = Image.new('RGB', (grid_ncols, grid_nrows))

        index = 0
        for i in range(grid_nrows):
            for j in range(grid_ncols):
                offset = i * grid_ncols + j
                color_component = grid.gridData[offset]
                color_component = int(color_component)

                color = colortable.color[color_component]
                im.putpixel((j, i), color)
                index += 1

        if date is not None:
            d = ImageDraw.Draw(im)
            hex_color = date_color[2:]
            (r, g, b) = tuple(int(hex_color[i:i+2], 16) for i in (0, 2, 4))
            d.text((ImageIO._date_x, ImageIO._date_y), date, fill=(r, g, b))

        im.save(fname)
        im.close()
        file.close()
        TimerUtility.stop_timer('gdif_WriteGIF')

    @staticmethod
    def get_size(filepath):
        img = Image.open(filepath)

        # get width and height
        width = img.width
        height = img.height

        return width, height
