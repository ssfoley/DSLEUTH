from igrid import IGrid
import random
from landClass import LandClass
from ugm_defines import UGMDefines
from utilities import Utilities
from scenario import Scenario
from processing import Processing
from imageIO import ImageIO
from color import Color
from timer import TimerUtility


class Deltatron:
    @staticmethod
    def deltatron(new_indices, landuse_classes, class_indices, deltatron, urban_land,
                  land_out, slope, drive, class_slope, ftransition):
        TimerUtility.start_timer('delta_deltatron')

        phase1_land = Deltatron.phase1(drive, urban_land.gridData, slope.gridData, deltatron.gridData,
                                       landuse_classes, class_indices, new_indices, class_slope, ftransition)

        phase2_land = Deltatron.phase2(urban_land.gridData, phase1_land, deltatron.gridData, landuse_classes, new_indices, ftransition)

        for i in range(len(phase2_land)):
            land_out.gridData[i] = phase2_land[i]

        TimerUtility.stop_timer('delta_deltatron')

    @staticmethod
    def phase1(drive, urban_land, slope, deltatron, landuse_classes, class_indices,
               new_indices, class_slope, ftransition):
        TimerUtility.start_timer('delta_phase1')
        nrows = IGrid.nrows
        ncols = IGrid.ncols
        phase1_land = []
        # Copy input land grid into output land grid
        for urban in urban_land:
            phase1_land.append(urban)

        # Try to make Transitions
        for tries in range(drive):
            # Select a transition pixel to e center of spreading cluster
            offset, i_center, j_center = Deltatron.get_rand_landuse_offset()
            index = new_indices[urban_land[offset]]
            while not landuse_classes[index].trans:
                offset, i_center, j_center = Deltatron.get_rand_landuse_offset()
                index = new_indices[urban_land[offset]]

            # Randomly choose new landuse number
            new_landuse = Deltatron.get_new_landuse(class_indices, landuse_classes, slope[offset], class_slope)

            # Test transition probability for new cluster
            new_i = new_indices[urban_land[offset]]
            new_j = new_indices[new_landuse]
            trans_offset = new_i * LandClass.get_num_landclasses() + new_j
            if random.random() < ftransition[trans_offset]:
                # Transition the center pixel
                phase1_land[offset] = new_landuse
                deltatron[offset] = 1

                # Try building up cluster around this center pixel
                i = i_center
                j = j_center
                for regions in range(UGMDefines.REGION_SIZE):
                    # Occasionally Reset to center of cluster
                    random_int = random.randint(0, 7)
                    if random_int == 7:
                        i = i_center
                        j = j_center
                    # Get a neighbor
                    i, j = Utilities.get_neighbor(i, j)
                    if 0 <= i < nrows and 0 <= j < ncols:
                        # Test new pixel against transition probability
                        offset = i * ncols + j
                        # print(f"{len(urban_land)} | {i} {j} -> {offset}")
                        urban_index = urban_land[offset]
                        new_i = new_indices[urban_index]
                        new_j = new_indices[new_landuse]
                        trans_offset = new_i * LandClass.get_num_landclasses() + new_j
                        if random.random() < ftransition[trans_offset]:
                            # If the immediate pixel is allowed to transition, then change it
                            index = new_indices[urban_land[offset]]
                            if landuse_classes[index].trans:
                                phase1_land[offset] = new_landuse
                                deltatron[offset] = 1

                            # Try to transition a neighboring pixel
                            i, j = Utilities.get_neighbor(i, j)
                            if 0 <= i < nrows and 0 <= j < ncols:
                                offset = i * ncols + j
                                index = new_indices[urban_land[offset]]
                                if landuse_classes[index].trans:
                                    phase1_land[offset] = new_landuse
                                    deltatron[offset] = 1
        TimerUtility.stop_timer('delta_phase1')
        return phase1_land

    @staticmethod
    def phase2(urban_land, phase1_land, deltatron, landuse_classes, new_indices, ftransition):
        TimerUtility.start_timer('delta_phase2')
        nrows = IGrid.nrows
        ncols = IGrid.ncols
        phase2_land = []

        # Copy current land to phase2_land
        for pixel in phase1_land:
            phase2_land.append(pixel)

        # For each interior point
        for i in range(1, nrows - 1):
            for j in range(1, ncols - 1):
                offset = i * ncols + j
                index = new_indices[phase1_land[offset]]
                if landuse_classes[index].trans and deltatron[offset] == 0:
                    """
                    I,J is a Transitional Pixel which was not transitioned within the last 
                    min_years_between_transitions years; count its neighbors which have transitioned
                    in previous year (IE Deltatron == 2)
                    """
                    deltatron_neighbors = Deltatron.count_neighbor(deltatron, i, j)
                    random_int = 1 + random.randint(0, 1)
                    if deltatron_neighbors >= random_int:
                        max_tries = 16
                        for tries in range(max_tries):
                            i_neigh, j_neigh = Utilities.get_neighbor(i, j)
                            offset_neigh = i_neigh * ncols + j_neigh
                            index = new_indices[phase1_land[offset_neigh]]
                            if deltatron[offset_neigh] == 2 and landuse_classes[index]:
                                trans_i = new_indices[phase2_land[offset]]
                                trans_j = new_indices[urban_land[offset_neigh]]
                                offset_trans = trans_i * LandClass.get_num_landclasses() + trans_j
                                if random.random() < ftransition[offset_trans]:
                                    phase2_land[offset] = urban_land[offset_neigh]
                                    deltatron[offset] = 1
                                break
        if Scenario.get_scen_value('view_deltatron_aging'):
            if IGrid.using_gif:
                filename = f"{Scenario.get_scen_value('output_dir')}deltatron_{Processing.get_current_run()}_" \
                           f"{Processing.get_current_monte()}_{Processing.get_current_year()}.gif"
            else:
                filename = f"{Scenario.get_scen_value('output_dir')}deltatron_{Processing.get_current_run()}_" \
                           f"{Processing.get_current_monte()}_{Processing.get_current_year()}.tif"

            date = f"{Processing.get_current_year()}"
            ImageIO.write_gif(deltatron, Color.get_deltatron_table(), filename, date, nrows, ncols)

        # Age the Deltatrons
        for i in range(nrows * ncols):
            if deltatron[i] > 0:
                deltatron[i] += 1

        # Kill old deltatrons
        Utilities.condition_gt_gif(deltatron, UGMDefines.MIN_YEARS_BETWEEN_TRANSITIONS, deltatron, 0)
        TimerUtility.stop_timer("delta_phase2")
        return phase2_land




    @staticmethod
    def get_rand_landuse_offset():
        nrows = IGrid.nrows
        ncols = IGrid.ncols

        i_center = random.randint(0, nrows - 1)
        j_center = random.randint(0, ncols - 1)
        return int(i_center * ncols + j_center), i_center, j_center

    @staticmethod
    def get_new_landuse(class_indices, landuse_classes, local_slope, class_slope):

        # Find two unique land classes
        first_choice, second_choice = random.sample(class_indices, 2)

        # Choose landuse with the most similar topographical slope
        slope_diff1 = local_slope - class_slope[first_choice.idx]
        slope_diff2 = local_slope - class_slope[second_choice.idx]

        if slope_diff1 * slope_diff2 < slope_diff2 * slope_diff2:
            new_landuse = landuse_classes[first_choice.idx].num
        else:
            new_landuse = landuse_classes[second_choice.idx].num

        return new_landuse

    @staticmethod
    def count_neighbor(deltatron, row, col):
        neighbor_options = [(-1, -1), (0, -1), (1, -1), (1, 0), (1, 1), (0, 1), (-1, 1), (-1, 0), (-1, -1)]
        neigh_count = 0
        for x, y in neighbor_options:
            neighbor_row = row + x
            neighbor_col = col + y
            offset = neighbor_row * IGrid.ncols + neighbor_col
            if deltatron[offset] == 2:
                neigh_count += 1

        return neigh_count