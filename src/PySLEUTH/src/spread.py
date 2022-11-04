from coeff import Coeff
from igrid import IGrid
from processing import Processing
from ugm_defines import UGMDefines
from scenario import Scenario
from logger import Logger
from rand import Random
from stats import Stats
from utilities import Utilities
from timer import TimerUtility
import sys
import math

class Spread:

    @staticmethod
    def spread(z, avg_slope):
        TimerUtility.start_timer('spr_spread')
        sng = 0
        sdc = 0
        og = 0
        rt = 0

        nrows = IGrid.nrows
        ncols = IGrid.ncols
        total_pixels = nrows * ncols

        road_gravity = Coeff.get_current_road_gravity()
        diffusion = Coeff.get_current_diffusion()
        breed = Coeff.get_current_breed()
        spread = Coeff.get_current_spread()

        excld = IGrid.igrid.get_excld_grid()
        roads = IGrid.igrid.get_road_grid_by_year(Processing.get_current_year())
        slope = IGrid.igrid.get_slope_grid()

        nrows = IGrid.nrows
        ncols = IGrid.ncols

        # Zero the growth array for this time period
        delta = [0] * (nrows * ncols)

        # Get slope rates
        slope_weights = Spread.get_slope_weights()

        # Phase 1N3 - Spontaneous Neighborhood Growth and Spreading
        sng, sdc = Spread.phase1n3(diffusion, breed, z.gridData, delta, slope, excld, slope_weights, sng, sdc)

        # Phase 4 - Organic Growth
        og = Spread.phase4(spread, z.gridData, excld, delta, slope, slope_weights, og)

        # Phase 5 - Road Influence Growth
        rt = Spread.phase5(road_gravity, diffusion, breed, z.gridData, delta, slope, excld, roads, slope_weights, rt)

        Utilities.condition_gt_gif(delta, UGMDefines.PHASE5G, delta, 0)
        Utilities.condition_ge_gif(excld, 100, delta, 0)

        # Now place growth array into current array
        num_growth_pix = 0
        avg_slope = 0.0

        for i in range(total_pixels):
            if z.gridData[i] == 0 and delta[i] > 0:
                # New growth being placed into array
                avg_slope += slope[i]
                z.gridData[i] = delta[i]
                num_growth_pix += 1
        pop = 0
        for pixels in z.gridData:
            if pixels >= UGMDefines.PHASE0G:
                pop += 1

        if num_growth_pix == 0:
            avg_slope = 0.0
        else:
            avg_slope /= num_growth_pix

        TimerUtility.stop_timer('spr_spread')
        return avg_slope, num_growth_pix, sng, sdc, og, rt, pop

    @staticmethod
    def get_slope_weights():
        slope_weight_array_size = 256
        slope_weights = []

        exp = Coeff.get_current_slope_resistance() / (UGMDefines.MAX_SLOPE_RESISTANCE_VALUE / 2)
        critical_slope = int(float(Scenario.get_scen_value('critical_slope')))
        for i in range(critical_slope):
            val = (critical_slope - 1 / critical_slope)
            slope_weights.append(1.0 - math.pow(val, exp))

        for i in range(critical_slope, slope_weight_array_size):
            slope_weights.append(1.0)

        if Scenario.get_scen_value('logging') and Scenario.get_scen_value('log_slope_weights'):
            Logger.log("***** LOG OF SLOPE WEIGHTS *****")
            Logger.log(f"Critical Slope =  {critical_slope}")
            Logger.log(f"Current Slope Resist = {Coeff.get_current_slope_resistance()}")
            Logger.log(f"Max Slope Resistance value = {UGMDefines.MAX_SLOPE_RESISTANCE_VALUE}")
            for i, weight in enumerate(slope_weights):
                if i < critical_slope:
                    Logger.log(f"weight[{i}] = {weight}")

            Logger.log(f"All other values to slope weights = 1.0")

        return slope_weights

    @staticmethod
    def phase1n3(diffusion_coeff, breed_coeff, z, delta, slope, excld, slope_weights, sng, sdc):
        TimerUtility.start_timer("spr_phase1n3")
        diffusion_value = Spread.calculate_diffusion_value(diffusion_coeff)
        nrows = IGrid.nrows
        ncols = IGrid.ncols

        for k in range(1 + int(diffusion_value)):
            # get a random row and col index
            i = Random.get_int(0, nrows - 1)
            j = Random.get_int(0, ncols - 1)

            # check if it is an interior point
            if 0 < i < nrows - 1 and 0 < j < ncols - 1:
                success, sng = Spread.urbanize(i, j, z, delta, slope, excld,
                                               slope_weights, UGMDefines.PHASE1G, sng)
                if success and Random.get_int(0, 100) < breed_coeff:
                    count = 0
                    max_tries = 8
                    for tries in range(max_tries):
                        urbanized, sdc, i_neigh, j_neigh = Spread.urbanize_neighbor(i, j, z, delta, slope,
                                                                                    excld, slope_weights,
                                                                                    UGMDefines.PHASE3G, sdc)
                        if urbanized:
                            count += 1
                        if count == UGMDefines.MIN_NGHBR_TO_SPREAD:
                            break
        TimerUtility.stop_timer('spr_phase1n3')
        return sng, sdc

    @staticmethod
    def phase4(spread_coeff, z, excld, delta, slope, slope_weights, og):
        TimerUtility.start_timer('spr_phase4')
        nrows = IGrid.nrows
        ncols = IGrid.ncols
        neighbor_options = [(-1, -1), (0, -1), (1, -1), (1, 0), (1, 1), (0, 1), (-1, 1), (-1, 0), (-1, -1)]

        # Loop over the interior pixels looking for urban from which to perform organic growth
        for row in range(1, nrows - 1):
            for col in range(1, ncols - 1):
                offset = row * ncols + col

                # Is this an urban pixel and do we pass the random spread coefficient test?
                if z[offset] > 0 and Random.get_int(0, 100) < spread_coeff:

                    # Examine the eight cell neighbors
                    # Spread at random if at least 2 are urban
                    # Pixel itself must be urban (3)
                    urb_count = Spread.count_neighbor(z, row, col)
                    if 2 <= urb_count < 8:
                        x_neigh, y_neigh = Random.get_element(neighbor_options)
                        row_neighbor = row + x_neigh
                        col_neighbor = col + y_neigh
                        success, og = Spread.urbanize(row_neighbor, col_neighbor, z, delta,
                                                      slope, excld, slope_weights, UGMDefines.PHASE4G, og)
        TimerUtility.stop_timer('spr_phase4')
        return og

    @staticmethod
    def phase5(road_gravity, diffusion_coeff, breed_coeff, z, delta, slope, excld, roads, slope_weights, rt):
        TimerUtility.start_timer('spr_phase5')
        nrows = IGrid.nrows
        ncols = IGrid.ncols
        total_pixels = nrows * ncols

        # Determine the total growth count and save the row and col locations of the new growth
        growth_tracker = []
        growth_count = 0
        for i in range(total_pixels):
            if delta[i] > 0:
                growth_tracker.append((int(i / ncols), i % ncols))
                growth_count += 1

        # Phase 5: Road Trips
        # If there is new growth, begin processing road trips
        if growth_count > 0:
            for i in range(1 + int(breed_coeff)):
                """Determine the Max Index into the Global_Road_Seach_Incices Array
                 for road_gravity of 1 we have 8 values
                 for road_gravity of 2 we have 16 values
                 for road_gravity of 3 we have 24 values
                    and so on...
                    
                if we need to cover N road_gravity values, then the total number of 
                indexed values woud be
                8 + 16 + 24 + ... + (8*N) = 8 *(1+2+3+...+N) = 8*(N(1+N))/2
                """

                int_road_gravity = Spread.get_road_gravity_val(road_gravity)
                max_search_index = 4 * (int_road_gravity * (1 + int_road_gravity))
                max_search_index = max(max_search_index, nrows)
                max_search_index = max(max_search_index, ncols)

                # Randomly select a growth pixel to start search for road
                growth_row, growth_col = Random.get_element(growth_tracker)


                # Search for road about this growth point
                road_found, i_road_start, j_road_start = Spread.road_search(growth_row, growth_col,
                                                                            max_search_index, roads)

                # If there is a road found, then walk along it
                i_road_end = 0
                j_road_end = 0
                spread = False
                if road_found:
                    #print(roads)
                    spread, i_road_end, j_road_end = Spread.road_walk(i_road_start, j_road_start, roads, diffusion_coeff)


                if spread:
                    urbanized, rt, i_neigh, j_neigh = Spread.urbanize_neighbor(i_road_end, j_road_end, z, delta, slope,
                                                                               excld, slope_weights,
                                                                               UGMDefines.PHASE5G, rt)
                    if urbanized:
                        max_tries = 3
                        for tries in range(3):
                            urbanized, rt, i_neigh_neigh, j_neigh_neigh = Spread.urbanize_neighbor(i_neigh, j_neigh, z,
                                                                                                   delta, slope, excld,
                                                                                                   slope_weights,
                                                                                                   UGMDefines.PHASE5G, rt)
        TimerUtility.stop_timer('spr_phase5')
        return rt






    @staticmethod
    def calculate_diffusion_value(diffusion_coeff):
        rows_sq = IGrid.nrows * IGrid.nrows
        cols_sq = IGrid.ncols * IGrid.ncols

        # diffusion value maximum (if diffusion == 100) will be 5% of the Image Diagonal
        return (diffusion_coeff * 0.005) * math.sqrt(rows_sq + cols_sq)

    @staticmethod
    def urbanize(row, col, z, delta, slope, excld, slope_weights, pixel_val, stat):
        nrows = IGrid.nrows
        ncols = IGrid.ncols
        offset = row * ncols + col

        flag = False
        if z[offset] == 0:
            if delta[offset] == 0:
                if Random.get_float() > slope_weights[slope[offset]]:
                    if excld[offset] < Random.get_int(0, 99):
                        flag = True
                        delta[offset] = pixel_val
                        stat += 1
                    else:
                        Stats.increment_excluded_failure()
                else:
                    Stats.increment_slope_failure()
            else:
                Stats.increment_delta_failure()
        else:
            Stats.increment_z_failure()

        return flag, stat

    @staticmethod
    def urbanize_neighbor(row, col, z, delta, slope, excld, slope_weights, pixel_val, stat):
        nrows = IGrid.nrows
        ncols = IGrid.ncols
        status = False
        neigh_row = neigh_col = 0
        if 0 <= row < nrows and 0 <= col < ncols:
            neigh_row, neigh_col = Spread.get_valid_neighbor(row, col)
            status, stat = Spread.urbanize(neigh_row, neigh_col, z, delta, slope, excld, slope_weights, pixel_val, stat)

        return status, stat, neigh_row, neigh_col
        #return neigh_row, neigh_col, status, stat

    @staticmethod
    def get_valid_neighbor(row, col):
        nrows = IGrid.nrows
        ncols = IGrid.ncols
        neighbor_options = [(-1, -1), (0, -1), (1, -1), (1, 0), (1, 1), (0, 1), (-1, 1), (-1, 0), (-1, -1)]
        idx = Random.get_int(0, 7)

        valid = False
        neigh_row = 0
        neigh_col = 0
        while not valid:
            idx_x, idx_y = neighbor_options[idx]
            neigh_row = row + idx_x
            neigh_col = col + idx_y
            if 0 <= neigh_row < nrows and 0 <= neigh_col < ncols:
                valid = True
            else:
                # The neighbor chosen isn't valid, go to next index in neighbor list and try again
                idx = (idx + 1) % 8

        return neigh_row, neigh_col

    @staticmethod
    def get_neighbor(row, col):
        neighbor_options = [(-1, -1), (0, -1), (1, -1), (1, 0), (1, 1), (0, 1), (-1, 1), (-1, 0), (-1, -1)]
        idx = Random.get_int(0, 7)

        idx_x, idx_y = neighbor_options[idx]
        neigh_row = row + idx_x
        neigh_col = col + idx_y

        return idx, int(neigh_row), int(neigh_col)

    @staticmethod
    def get_next_neighbor(idx, row, col):
        neighbor_options = [(-1, -1), (0, -1), (1, -1), (1, 0), (1, 1), (0, 1), (-1, 1), (-1, 0), (-1, -1)]
        idx = (idx + 1) % 8

        idx_x, idx_y = neighbor_options[idx]
        neigh_row = row + idx_x
        neigh_col = col + idx_y
        return idx, int(neigh_row), int(neigh_col)

    @staticmethod
    def count_neighbor(z, row, col):
        neighbor_options = [(-1, -1), (0, -1), (1, -1), (1, 0), (1, 1), (0, 1), (-1, 1), (-1, 0), (-1, -1)]
        urb_count = 0
        for x, y in neighbor_options:
            neighbor_row = row + x
            neighbor_col = col + y
            offset = neighbor_row * IGrid.ncols + neighbor_col
            if z[offset] > 0:
                urb_count += 1

        return urb_count

    @staticmethod
    def get_road_gravity_val(road_gravity):
        nrows = IGrid.nrows
        ncols = IGrid.ncols

        # Road gravity val's maximum (if rg_coeff == 100)
        # will be 1/16 of the Image dimensions

        return int ((road_gravity / UGMDefines.MAX_ROAD_VALUE) * ((nrows + ncols) / 16))

    @staticmethod
    def road_search(i_grwth_center, j_grwth_center, max_search_index, roads):

        nrows = IGrid.nrows
        ncols = IGrid.ncols
        road_found = False
        i_road = 0
        j_road = 0
        for search_idx in range(max_search_index):
            i_offset, j_offset = Spread.spiral(search_idx)
            i = i_grwth_center + i_offset
            j = j_grwth_center + j_offset

            if 0 <= i < nrows and 0 <= j < ncols:
                offset = int(i * ncols + j)
                if roads[offset] != 0:
                    road_found = True
                    i_road = i
                    j_road = j
                    break
        return road_found, i_road, j_road

    @staticmethod
    def spiral(index):
        nrows = IGrid.nrows
        ncols = IGrid.ncols

        bn_found = False
        bn = 0
        for i in range(1, max(nrows, ncols)):
            total = 8 * ((1 + i) * i) / 2
            if total > index:
                bn_found = True
                bn = i
                break

        if not bn_found:
            Logger.log("Unable to find road search band, bn.")
            print("Unable to find road search band, bn")
            sys.exit(1)
        bo = index - 8 * ((bn - 1) * bn / 2)
        left_ln = right_ln = bn * 2 + 1
        top_ln = bot_ln = bn * 2 - 1
        range1 = left_ln
        range2 = left_ln + bot_ln
        range3 = left_ln + bot_ln + right_ln
        range4 = left_ln + bot_ln + right_ln + top_ln
        if bo < range1:
            region_offset = bo % range1
            i = -bn + region_offset
            j = -bn
        elif bo < range2:
            region_offset = (bo - range1) % range2
            i = bn
            j = -bn + 1 + region_offset
        elif bo < range3:
            region_offset = (bo - range2) % range3
            i = bn - region_offset
            j = bn
        elif bo < range4:
            region_offset = (bo - range3) % range4
            i = -bn
            j = bn - 1 - region_offset
        else:
            msg = "Unable to calculate (i, j) for road search"
            Logger.log(msg)
            print(msg)
            sys.exit(1)
        return i, j

    @staticmethod
    def road_walk(i_road_start, j_road_start, roads, diffusion_coeff):
        nrows = IGrid.nrows
        ncols = IGrid.ncols
        i = i_road_start
        j = j_road_start
        end_of_road = False
        spread = False
        run = 0
        i_road_end = 0
        j_road_end = 0
        while not end_of_road:
            end_of_road = True
            #print("********************************************")
            idx, i_neigh, j_neigh = Spread.get_neighbor(i, j)
            #print(f"i: {i_neigh} j: {j_neigh}")
            for k in range(8):
                if 0 <= i_neigh < nrows and 0 <= j_neigh < ncols:
                    offset = i_neigh * ncols + j_neigh
                    if roads[offset] != 0:
                        end_of_road = False
                        run += 1
                        i = i_neigh
                        j = j_neigh
                        break
                idx, i_neigh, j_neigh = Spread.get_next_neighbor(idx, i, j)
                #print(f"i: {i_neigh} j: {j_neigh}")
            #print("********************************************")
            offset = int(i * ncols + j)
            run_value = int(roads[offset] / UGMDefines.MAX_ROAD_VALUE * diffusion_coeff)
            #print(f"run: {run} run_value {run_value} diffusionCoeff {diffusion_coeff}")
            if run > run_value:
                end_of_road = True
                spread = True
                i_road_end = i
                j_road_end = j

        return spread, i_road_end, j_road_end







