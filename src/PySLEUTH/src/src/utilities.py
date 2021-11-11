from ugm_defines import UGMDefines
from igrid import IGrid
from scenario import Scenario
from rand import Random
from processing import Processing
from globals import Globals
from imageIO import ImageIO
from color import Color


class Utilities:
    @staticmethod
    def init_grid(gif):
        for i in range(len(gif)):
            gif[i] = 0

    @staticmethod
    def get_neighbor(i_in, j_in):
        neighbor_options = [(-1, -1), (0, -1), (1, -1), (1, 0), (1, 1), (0, 1), (-1, 1), (-1, 0), (-1, -1)]
        i_offset, j_offset = Random.get_element(neighbor_options)

        return i_in + i_offset, j_in + j_offset

    @staticmethod
    def condition_gif(source, target):
        for i in range(len(source)):
            if source[i] > 0:
                target[i] = UGMDefines.PHASE0G

    @staticmethod
    def condition_gt_gif(source, cmp_value, target, set_value):
        for i in range(len(source)):
            if source[i] > cmp_value:
                target[i] = set_value

    @staticmethod
    def condition_ge_gif(source, cmp_value, target, set_value):
        for i in range(len(source)):
            if source[i] >= cmp_value:
                target[i] = set_value

    @staticmethod
    def write_z_prob_grid(z, name):
        # copy background int z_prob_ptr and remap background pixels
        # which collide with the seed, prob colors, and date
        nrows = IGrid.nrows
        ncols = IGrid.ncols
        total_pix = nrows * ncols

        background = IGrid.igrid.get_background_grid()
        prob_color_cnt = len(Scenario.get_scen_value('probability_color'))

        lower_bounds = [UGMDefines.SEED_COLOR_INDEX, UGMDefines.DATE_COLOR_INDEX]
        upper_bounds = [UGMDefines.SEED_COLOR_INDEX + prob_color_cnt, UGMDefines.DATE_COLOR_INDEX]
        indices = [UGMDefines.SEED_COLOR_INDEX + prob_color_cnt + 1, UGMDefines.DATE_COLOR_INDEX - 1]

        z_prob = Utilities.map_grid_to_index(background, lower_bounds, upper_bounds, indices, total_pix)

        if Processing.get_processing_type() == Globals.mode_enum['predict']:
            # Map z_ptr pixels into desired prob indices and save in overlay
            prob_list = Scenario.get_scen_value('probability_color')
            lower_bounds = []
            upper_bounds = []
            indices = []
            for i, prob in enumerate(prob_list):
                lower_bounds.append(prob.lower_bound)
                upper_bounds.append(prob.upper_bound)
                indices.append(i + 2)

            indices[0] = 0
            overlay = Utilities.map_grid_to_index(z, lower_bounds, upper_bounds, indices, total_pix)

            # Overlay overlay grid onto the z_prob grid
            z_prob = Utilities.overlay(z_prob, overlay)

            # Overlay urban_seed into the z_prob grid
            z_prob = Utilities.overlay_seed(z_prob, total_pix)
        else:
            # TESTING
            # Map z grid pixels into desired seed_color_index and save in overlay pt
            lower_bounds = [1]
            upper_bounds = [100]
            indices = [UGMDefines.SEED_COLOR_INDEX]

            overlay = Utilities.map_grid_to_index(z.gridData, lower_bounds, upper_bounds, indices, total_pix)

            # Overlay overlay grid onto the z_prob grid
            z_prob = Utilities.overlay(z_prob, overlay)

        # The file writer needs to take in a Grid, so we're going to wrap our z_prob list in a grid
        z_prob_grid = IGrid.wrap_list(z_prob)
        if IGrid.using_gif:
            filename = f"{Scenario.get_scen_value('output_dir')}{IGrid.igrid.get_location()}" \
                       f"{name}{Processing.get_current_year()}.gif"
        else:
            filename = f"{Scenario.get_scen_value('output_dir')}{IGrid.igrid.get_location()}" \
                       f"{name}{Processing.get_current_year()}.tif"
            IGrid.echo_meta(f"{Scenario.get_scen_value('output_dir')}"
                            f"{IGrid.igrid.get_location()}{name}{Processing.get_current_year()}.tfw", "urban")

        date = f"{Processing.get_current_year()}"
        ImageIO.write_gif(z_prob_grid, Color.get_probability_table(), filename, date, IGrid.nrows, IGrid.ncols)

    @staticmethod
    def map_grid_to_index(grid_in, l_bound, u_bound, index, total_pix):
        grid_out = [0] * total_pix
        for i in range(total_pix):
            for j in range(len(l_bound)):
                if l_bound[j] <= grid_in[i] <= u_bound[j]:
                    grid_out[i] = index[j]
                    break
                else:
                    grid_out[i] = grid_in[i]

        return grid_out

    @staticmethod
    def overlay(layer0, layer1):
        nrows = IGrid.nrows
        ncols = IGrid.ncols
        total_pix = nrows * ncols

        out = [0] * total_pix

        for i in range(total_pix):
            if layer1[i] > 0:
                out[i] = layer1[i]
            else:
                out[i] = layer0[i]

        return out

    @staticmethod
    def overlay_seed(z_prob, total_pix):
        urban_seed = IGrid.igrid.get_urban_grid(0)

        lower_bounds = [1]
        upper_bounds = [255]
        index = [UGMDefines.SEED_COLOR_INDEX]

        urban_overlay = Utilities.map_grid_to_index(urban_seed, lower_bounds, upper_bounds, index, total_pix)

        z_prob = Utilities.overlay(z_prob, urban_overlay)
        return z_prob

    @staticmethod
    def img_intersection(grid1, grid2):
        count = 0
        for i in range(len(grid1)):
            if grid1[i] == grid2[i]:
                count += 1

        return count










