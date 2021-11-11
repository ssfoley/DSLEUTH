from igrid import IGrid
from logger import Logger
from processing import Processing
from statsVal import StatsVal
from pgrid import PGrid
from scenario import Scenario
from globals import Globals
from coeff import Coeff
from ugm_defines import UGMDefines
import sys
import math
import _pickle
import os.path


class StatsInfo:
    def __init__(self):
        self.area = -1.0
        self.edges = -1.0
        self.clusters = -1.0
        self.pop = -1.0
        self.x_mean = -1.0
        self.y_mean = -1.0
        self.radius = -1.0
        self.average_slope = -1.0
        self.mean_cluster_size = -1.0
        self.percent_urban = -1.0

    def calculate_line_fit(self, key):
        fields = {
            "area": self.area,
            "edges": self.edges,
            "clusters": self.clusters,
            "pop": self.pop,
            "x_mean": self.x_mean,
            "y_mean": self.y_mean,
            "radius": self.radius,
            "average_slope": self.average_slope,
            "mean_cluster_size": self.mean_cluster_size,
            "percent_urban": self.percent_urban
        }
        nobs = IGrid.igrid.get_num_urban() - 1
        dependent = []
        independent = []
        for i in range(1, nobs + 1):
            dependent.append(Stats.actual[i].get_field_by_name(key))
            independent.append(Stats.average[i].get_field_by_name(key))
        val = StatsInfo.line_fit(dependent, independent, nobs)
        setattr(self, key, val)
        # fields[key] = val

    def get_field_by_name(self, name):
        fields = {
            "area": self.area,
            "edges": self.edges,
            "clusters": self.clusters,
            "pop": self.pop,
            "x_mean": self.x_mean,
            "y_mean": self.y_mean,
            "radius": self.radius,
            "average_slope": self.average_slope,
            "mean_cluster_size": self.mean_cluster_size,
            "percent_urban": self.percent_urban
        }
        return fields[name]

    @staticmethod
    def line_fit(depend, independ, num_obs):
        depend_avg = 0.0
        independ_avg = 0.0

        for i in range(num_obs):
            depend_avg += depend[i]
            independ_avg += independ[i]

        if num_obs > 0:
            depend_avg /= num_obs
            independ_avg /= num_obs
        else:
            raise ZeroDivisionError("Number of Observation is 0")

        cross = 0
        depend_sum = 0
        independ_sum = 0

        for i in range(num_obs):
            cross += (depend[i] - depend_avg) * (independ[i] - independ_avg)
            depend_sum += (depend[i] - depend_avg) * (depend[i] - depend_avg)
            independ_sum += (independ[i] - independ_avg) * (independ[i] - independ_avg)

        r = 0
        if depend_sum * independ_sum >= 1e-11:
            r = cross / pow(depend_sum * independ_sum, 0.5)

        return r * r


class Stats:
    size_cir_q = 5000
    urbanization_attempt = None
    record = None
    average = []  # list of statsVal
    std_dev = []  # list of statsVal
    running_total = [None] * UGMDefines.MAX_URBAN_YEARS  # list of statsVal
    actual = []  # list of stat infos
    regression = StatsInfo()
    aggregate = {
        "fmatch": 0.0,
        "actual": 0.0,
        "simulated": 0.0,
        "compare": 0.0,
        "leesalee": 0.0,
        "product": 0.0
    }

    @staticmethod
    def set_base_stats():
        Stats.record = Record()
        urban_num = IGrid.igrid.get_num_urban()
        slope = IGrid.igrid.get_slope().gridData
        for i in range(urban_num):
            urban = IGrid.igrid.get_urban_idx(i).gridData
            stats_info = StatsInfo()
            Stats.compute_stats(urban, slope, stats_info)
            road_pixel_count = IGrid.get_road_pixel_count(Processing.get_current_year())
            excluded_pixel_count = IGrid.get_excld_count()

            percent_urban = 100.0 * 100.0 * (stats_info.pop + road_pixel_count) / \
                            (IGrid.nrows * IGrid.ncols - road_pixel_count - excluded_pixel_count)

            stats_info.percent_urban = percent_urban
            Stats.actual.append(stats_info)

    @staticmethod
    def compute_stats(urban, slope, stats_info):
        # compute the number fo edge pixels
        Stats.set_edge(urban, stats_info)

        # compute the number of clusters
        Stats.set_num_cluster(urban, stats_info)

        # compute means
        Stats.set_circle(urban, slope, stats_info)

    @staticmethod
    def update(num_growth_pix):
        # print(f"Num_growth_pix: {num_growth_pix}")
        nrows = IGrid.nrows
        ncols = IGrid.ncols
        total_pixels = nrows * ncols
        road_pixel_count = IGrid.get_road_pixel_count(Processing.get_current_year())
        excluded_pixel_count = IGrid.get_excld_count()

        # Compute this year stats
        Stats.compute_cur_year_stats()
        # Set num growth pixels
        Stats.set_num_growth_pixels(num_growth_pix)
        # Calibrate growth rate
        Stats.cal_growth_rate()
        # Calibrate Percent Urban
        Stats.cal_percent_urban(total_pixels, road_pixel_count, excluded_pixel_count)

        output_dir = Scenario.get_scen_value('output_dir')
        cur_run = Processing.get_current_run()
        cur_year = Processing.get_current_year()
        if IGrid.test_for_urban_year(Processing.get_current_year()):
            Stats.cal_leesalee()
            filename = f"{output_dir}grow_{cur_run}_{cur_year}.log"
            Stats.save(filename)

        if Processing.get_processing_type() == Globals.mode_enum['predict']:
            filename = f"{output_dir}grow_{cur_run}_{cur_year}.log"
            Stats.save(filename)

    @staticmethod
    def cal_leesalee():
        z = PGrid.get_z()
        urban = IGrid.igrid.get_urban_grid_by_yr(Processing.get_current_year())
        Stats.record.this_year.leesalee = 1.0
        if Processing.get_processing_type() != Globals.mode_enum['predict']:
            Stats.compute_leesalee(z.gridData, urban)

    @staticmethod
    def compute_leesalee(z, urban):
        nrows = IGrid.nrows
        ncols = IGrid.ncols
        union = 0
        intersection = 0
        total_pix = nrows * ncols

        for i in range(total_pix):
            if z[i] != 0 or urban[i] != 0:
                union += 1
            if z[i] != 0 and urban[i] != 0:
                intersection += 1

        Stats.record.this_year.leesalee = intersection / union

    @staticmethod
    def compute_cur_year_stats():
        z = PGrid.get_z()
        slope = IGrid.igrid.get_slope_grid()
        stats_info = StatsInfo()
        Stats.compute_stats(z.gridData, slope, stats_info)
        #print(f"avg slope: {stats_info.average_slope}")
        Stats.record.set_stats_info_to_record(stats_info)

    @staticmethod
    def cal_growth_rate():
        Stats.record.this_year.growth_rate = Stats.record.this_year.num_growth_pix / Stats.record.this_year.pop * 100
        # print(f"\nngp: {Stats.record.this_year.num_growth_pix} pop:{Stats.record.this_year.pop}")

    @staticmethod
    def cal_percent_urban(total_pix, road_pix, excld_pix):
        numerator = 100 * (Stats.record.this_year.pop + road_pix)
        denominator = total_pix - road_pix - excld_pix
        Stats.record.this_year.percent_urban = numerator / denominator

    @staticmethod
    def set_edge(urban, stats_info):
        nrows = IGrid.nrows
        ncols = IGrid.ncols

        edges = 0
        area = 0
        for i in range(nrows):
            for j in range(ncols):
                offset = Stats.offset(i, j)
                if urban[offset] != 0:
                    area += 1
                    # Check all 4 neighbors
                    # If a neighbor is 0, it is an edge
                    if Stats.check_all_neighbor_edge(i, j, urban):
                        edges += 1
        stats_info.edges = edges
        stats_info.area = area

    @staticmethod
    def offset(i, j):
        return i * IGrid.ncols + j

    @staticmethod
    def check_neighbor_edge(row, col, urban):
        return Stats.in_array_bounds_edge(row, col) and urban[Stats.offset(row, col)] == 0

    @staticmethod
    def check_all_neighbor_edge(cur_row, cur_col, urban):
        # left neighbor
        neigh_col = cur_col
        neigh_row = cur_row - 1
        if Stats.check_neighbor_edge(neigh_row, neigh_col, urban):
            return True

        # right neighbor
        neigh_row = cur_row + 1
        if Stats.check_neighbor_edge(neigh_row, neigh_col, urban):
            return True

        # up neighbor
        neigh_col = cur_col - 1
        neigh_row = cur_row
        if Stats.check_neighbor_edge(neigh_row, neigh_col, urban):
            return True

        # down neighbor
        neigh_col = cur_col + 1
        if Stats.check_neighbor_edge(neigh_row, neigh_col, urban):
            return True

        return False

    @staticmethod
    def in_array_bounds_edge(row, col):
        return IGrid.nrows > row >= 0 and \
               IGrid.ncols > col >= 0

    @staticmethod
    def check_all_neighbor_cluster(cur_row, cur_col, clusters, visited, queue):
        sum = 0
        # left
        temp_row = cur_row - 1
        temp_col = cur_col
        result = Stats.check_neighbor_cluster(temp_row, temp_col, clusters, visited, queue)
        sum += result

        # right
        temp_row = cur_row + 1
        temp_col = cur_col
        result = Stats.check_neighbor_cluster(temp_row, temp_col, clusters, visited, queue)
        sum += result

        # up
        temp_row = cur_row
        temp_col = cur_col - 1
        result = Stats.check_neighbor_cluster(temp_row, temp_col, clusters, visited, queue)
        sum += result

        # down
        temp_row = cur_row
        temp_col = cur_col + 1
        result = Stats.check_neighbor_cluster(temp_row, temp_col, clusters, visited, queue)
        sum += result

        return sum

    @staticmethod
    def check_neighbor_cluster(row, col, clusters, visited, queue):
        offset = Stats.offset(row, col)
        if Stats.in_array_bounds_edge(row, col):
            if clusters[offset] == 1 and visited[offset] == 0:
                visited[offset] = 1
                queue.append((row, col))
                return 1
        return 0

    @staticmethod
    def set_num_cluster(urban, stats_info):
        # stats_cluster
        nrows = IGrid.nrows
        ncols = IGrid.ncols
        total_pixels = nrows * ncols
        visited = [0] * total_pixels
        pop = 0
        clusters = [0] * total_pixels
        num_clusters = 0
        sum = 0

        queue = []

        for i in range(total_pixels):
            if urban[i] != 0:
                clusters[i] = 1
                pop += 1
        # print(f"Pop: {pop}")
        stats_info.pop = pop
        for j in range(ncols):
            clusters[Stats.offset(0, j)] = 0
            clusters[Stats.offset(nrows - 1, j)] = 0

        for i in range(nrows):
            clusters[Stats.offset(i, 0)] = 0
            clusters[Stats.offset(i, ncols - 1)] = 0

        for i in range(1, nrows - 1):
            for j in range(1, ncols - 1):
                offset = Stats.offset(i, j)
                if clusters[offset] == 1 and visited[offset] == 0:
                    sum += 1
                    temp_row = i
                    temp_col = j
                    visited[offset] = 1

                    # Queue Store
                    queue.append((temp_row, temp_col))

                    once_flag = False
                    while not once_flag or len(queue) > 0:
                        # Queue Retrieve
                        row, col = queue.pop()

                        temp_row = row - 1
                        temp_col = col
                        result = Stats.check_neighbor_cluster(temp_row, temp_col, clusters, visited, queue)
                        sum += result

                        # right
                        temp_row = row + 1
                        temp_col = col
                        result = Stats.check_neighbor_cluster(temp_row, temp_col, clusters, visited, queue)
                        sum += result

                        # up
                        temp_row = row
                        temp_col = col - 1
                        result = Stats.check_neighbor_cluster(temp_row, temp_col, clusters, visited, queue)
                        sum += result

                        # down
                        temp_row = row
                        temp_col = col + 1
                        result = Stats.check_neighbor_cluster(temp_row, temp_col, clusters, visited, queue)
                        sum += result

                        once_flag = True

                    num_clusters += 1

        stats_info.clusters = num_clusters
        if num_clusters > 0:
            stats_info.mean_cluster_size = sum / num_clusters
        else:
            msg = "NUMBER OF CLUSTERS WAS 0, NOT ABLE TO CALCULATE MEAN CLUSTER SIZE"
            print(msg)
            Logger.log(msg)
            sys.exit(1)

    @staticmethod
    def set_circle(urban, slope, stats_info):
        nrows = IGrid.nrows
        ncols = IGrid.ncols
        slope_mean = 0.0
        y_mean = 0.0
        x_mean = 0.0
        count = 0.0

        # first compute the means
        for i in range(nrows):
            for j in range(ncols):
                offset = Stats.offset(i, j)
                if urban[offset] > 0:
                    slope_mean += slope[offset]
                    x_mean += j
                    y_mean += i
                    count += 1

        if count <= 0:
            msg = "Something is wrong with urban, all values are zero"
            print(msg)
            Logger.log(msg)
            sys.exit(1)

        x_mean /= count
        y_mean /= count
        stats_info.x_mean = x_mean
        stats_info.y_mean = y_mean
        stats_info.average_slope = slope_mean / count
        #print(f" {slope_mean} / {count} = avg slp: {slope_mean / count}")

        # compute the radius of the circle with same area as the count
        stats_info.radius = math.pow((stats_info.area / math.pi), 0.5)

    @staticmethod
    def init_urbanization_attempts():
        Stats.urbanization_attempt = UrbanizationAttempt()

    @staticmethod
    def increment_z_failure():
        Stats.urbanization_attempt.z_failure += 1

    @staticmethod
    def increment_delta_failure():
        Stats.urbanization_attempt.delta_failure += 1

    @staticmethod
    def increment_slope_failure():
        Stats.urbanization_attempt.slope_failure += 1

    @staticmethod
    def increment_excluded_failure():
        Stats.urbanization_attempt.excluded_failure += 1

    @staticmethod
    def increment_urban_success():
        Stats.urbanization_attempt.successes += 1

    @staticmethod
    def set_sng(val):
        Stats.record.this_year.sng = val

    @staticmethod
    def set_sdg(val):
        Stats.record.this_year.sdg = val

    @staticmethod
    def set_sdc(val):
        Stats.record.this_year.sdc = val

    @staticmethod
    def set_og(val):
        Stats.record.this_year.og = val

    @staticmethod
    def set_rt(val):
        Stats.record.this_year.rt = val

    @staticmethod
    def set_pop(val):
        Stats.record.this_year.pop = val

    @staticmethod
    def get_growth_rate():
        return Stats.record.this_year.growth_rate

    @staticmethod
    def get_percent_urban():
        return Stats.record.this_year.percent_urban

    @staticmethod
    def get_num_growth_pixels():
        return Stats.record.this_year.num_growth_pix

    @staticmethod
    def set_num_growth_pixels(val):
        Stats.record.this_year.num_growth_pix = val

    @staticmethod
    def update_running_total(idx: int):
        if Stats.running_total[idx] is not None:
            Stats.running_total[idx].update_stat_val(Stats.record)
        else:
            new_stat = StatsVal()
            new_stat.update_stat_val(Stats.record)
            Stats.running_total[idx] = new_stat

    @staticmethod
    def analyze(fmatch):
        output_dir = Scenario.get_scen_value('output_dir')
        run = Processing.get_current_run()
        write_avg_file = Scenario.get_scen_value('write_avg_file')
        avg_filename = f'{output_dir}avg.log'
        write_std_dev_file = Scenario.get_scen_value('write_std_dev_file')
        std_filename = f'{output_dir}std_dev.log'
        control_filename = f'{output_dir}control_stats.log'

        if write_avg_file:
            if not os.path.isfile(avg_filename):
                Stats.create_stats_val_file(avg_filename)

        if write_std_dev_file:
            if not os.path.isfile(std_filename):
                Stats.create_stats_val_file(std_filename)

        if Processing.get_processing_type() != Globals.mode_enum['predict']:
            if not os.path.isfile(control_filename):
                Stats.create_control_file(control_filename)

            # start at i = 1; i = 0 is the initial seed
            # I think I need to put a dummy stats_val to represent the initial seed
            Stats.average.append(StatsVal())
            for i in range(1, IGrid.igrid.get_num_urban()):
                year = IGrid.igrid.get_urban_year(i)
                Stats.calculate_averages(i)
                Stats.process_grow_log(run, year)

                if write_avg_file:
                    Stats.write_stats_val_line(avg_filename, run, year, Stats.average[i], i)
                if write_std_dev_file:
                    Stats.write_stats_val_line(std_filename, run, year, Stats.std_dev[i], i)

            Stats.do_regressions()
            Stats.do_aggregate(fmatch)
            Stats.write_control_stats(control_filename)

        if Processing.get_processing_type() == Globals.mode_enum['predict']:
            start = int(Scenario.get_scen_value('prediction_start_date'))
            stop = Processing.get_stop_year()

            for year in range(start + 1, stop + 1):
                Stats.clear_stats()
                Stats.process_grow_log(run, year)
                if write_avg_file:
                    Stats.write_stats_val_line(avg_filename, run, year, Stats.average[0], 0)
                if write_std_dev_file:
                    Stats.write_stats_val_line(std_filename, run, year, Stats.std_dev[0], 0)

        Stats.clear_stats()

    @staticmethod
    def do_regressions():
        Stats.regression.calculate_line_fit("area")
        Stats.regression.calculate_line_fit("edges")
        Stats.regression.calculate_line_fit("clusters")
        Stats.regression.calculate_line_fit("pop")
        Stats.regression.calculate_line_fit("x_mean")
        Stats.regression.calculate_line_fit("y_mean")
        Stats.regression.calculate_line_fit("radius")
        Stats.regression.calculate_line_fit("average_slope")
        Stats.regression.calculate_line_fit("mean_cluster_size")
        Stats.regression.calculate_line_fit("percent_urban")

    @staticmethod
    def do_aggregate(fmatch):
        last_index = IGrid.igrid.get_num_urban() - 1

        actual = Stats.actual[last_index].pop
        simulated = Stats.average[last_index].pop
        leesalee = 0.0

        for i in range(1, IGrid.igrid.get_num_urban()):
            leesalee += Stats.average[i].leesalee
        leesalee /= last_index

        if actual > simulated:
            if actual != 0.0:
                compare = simulated / actual
            else:
                raise ZeroDivisionError("Aggregate actual value is zero")
        else:
            if simulated != 0.0:
                compare = actual / simulated
            else:
                raise ZeroDivisionError("Aggregate simulated value is zero")

        fmatch_tmp = 1.0
        if len(Scenario.get_scen_value('landuse_data_file')) > 0:
            fmatch_tmp = fmatch

        product = compare * leesalee * Stats.regression.edges * Stats.regression.clusters * \
                  Stats.regression.pop * Stats.regression.x_mean * Stats.regression.y_mean * \
                  Stats.regression.radius * Stats.regression.average_slope * Stats.regression.mean_cluster_size * \
                  Stats.regression.percent_urban * fmatch_tmp

        Stats.aggregate['fmatch'] = fmatch
        Stats.aggregate['actual'] = actual
        Stats.aggregate['simulated'] = simulated
        Stats.aggregate['leesalee'] = leesalee
        Stats.aggregate['compare'] = compare
        Stats.aggregate['product'] = product

    @staticmethod
    def calculate_averages(idx):
        temp = StatsVal()
        total_mc = int(Scenario.get_scen_value('monte_carlo_iterations'))
        temp.calculate_averages(total_mc, Stats.running_total[idx])
        Stats.average.append(temp)

    @staticmethod
    def calculate_stand_dev(idx):
        temp = StatsVal()
        total_mc = int(Scenario.get_scen_value('monte_carlo_iterations'))
        if idx == 0 and Processing.get_processing_type() != Globals.mode_enum['predict']:
            raise ValueError()
        temp.calculate_sd(total_mc, Stats.record, Stats.average[idx])
        Stats.std_dev.append(temp)

    @staticmethod
    def process_grow_log(run, year):
        output_dir = Scenario.get_scen_value('output_dir')
        filename = f'{output_dir}grow_{run}_{year}.log'
        mc_iters = int(Scenario.get_scen_value('monte_carlo_iterations'))
        mc_count = 0
        grow_records = []
        # if Processing.get_processing_type() != Globals.mode_enum['predict']:
        with (open(filename, "rb")) as openfile:
            while True:
                try:
                    grow_records.append(_pickle.load(openfile))
                except EOFError:
                    break

        """print(f"****************Year {year}***********************")
        for record in grow_records:
            print(record)
        print("***************************************")"""
        if len(grow_records) > int(Scenario.get_scen_value('monte_carlo_iterations')):
            raise AssertionError("Num Records is larger than Monte Carlo iters")
        if Processing.get_processing_type() == Globals.mode_enum['predict']:
            for record in grow_records:
                Stats.record = record
                Stats.update_running_total(0)
            Stats.calculate_averages(0)

        for record in grow_records:
            Stats.record = record
            if mc_count >= mc_iters:
                Logger.log("mc_count >= scen_GetMonteCarloIterations ()")
                sys.exit(1)
            if Processing.get_processing_type() != Globals.mode_enum['predict']:
                index = IGrid.igrid.urban_yr_to_idx(Stats.record.year)
                Stats.calculate_stand_dev(index)
            else:
                Stats.calculate_stand_dev(0)

            mc_count += 1
        os.remove(filename)

    @staticmethod
    def clear_stats():
        Stats.average = []  # list of statsVal
        Stats.std_dev = []  # list of statsVal
        Stats.running_total = [None] * UGMDefines.MAX_URBAN_YEARS  # list of statsVal
        Stats.regression = StatsInfo()

    @staticmethod
    def log_base_stats():
        count = IGrid.igrid.get_num_urban()

        Logger.log("\n************************LOG OF BASE STATISTICS FOR URBAN INPUT DATA********************")
        Logger.log(" Year       Area      Edges   Clusters         Pop       Mean Center        " +
                   "Radius   Avg Slope  MeanClusterSize")

        for i in range(count):
            stat = Stats.actual[i]
            temp = f" {IGrid.igrid.get_urban_year(i)}   {stat.area:8.2f}   {stat.edges:8.2f}   " \
                   f"{stat.clusters:8.2f}    {stat.pop:8.2f}  ({stat.x_mean:8.2f}," \
                   f"{stat.y_mean:8.2f})   {stat.radius:8.2f}  {stat.average_slope:10.2f}" \
                   f"      {stat.mean_cluster_size:6.3f}"
            Logger.log(temp)

    @staticmethod
    def create_control_file(filename):
        control_file = open(filename, "w")
        header = Stats.log_control_stats_hdr()
        control_file.write(header)
        control_file.close()

    @staticmethod
    def create_stats_val_file(filename):
        stats_file = open(filename, "w")
        header = Stats.log_stat_val_hdr()
        stats_file.write(header)
        stats_file.close()

    @staticmethod
    def log_control_stats_hdr():
        temp = "                                               Cluster\n"
        temp += "  Run  Product Compare     Pop   Edges Clusters   "
        temp += "Size Leesalee  Slope "
        temp += " %%Urban   Xmean   Ymean     Rad  Fmatch "
        temp += "Diff  Brd Sprd  Slp   RG\n"
        return temp

    @staticmethod
    def log_stat_val_hdr():
        col_name = ["sng", "sdg", "sdc", "og", "rt", "pop", "area", "edges", "clusters", "xmean",
                    "ymean", "rad", "slope", "cl_size", "diffuse", "spread", "breed", "slp_res",
                    "rd_grav", "%urban", "%road", "grw_rate", "leesalee", "grw_pix"]

        temp = "  run year index "
        for name in col_name:
            temp += f"{name:8s} "

        return temp + "\n"

    @staticmethod
    def write_stats_val_line(filename, run, year, stats_val, index):
        f = open(filename, "a")
        f.write(f"{run:5} {year:4} {index:2}")
        f.write(f"{stats_val}\n")
        f.close()

    @staticmethod
    def write_control_stats(filename):
        control_file = open(filename, 'a')
        str = f"{Processing.get_current_run():5} " \
              f"{Stats.aggregate['product']:8.5f} " \
              f"{Stats.aggregate['compare']:7.5f} " \
              f"{Stats.regression.pop:7.5f} " \
              f"{Stats.regression.edges:7.5f} " \
              f"{Stats.regression.clusters:7.5f} " \
              f"{Stats.regression.mean_cluster_size:7.5f} " \
              f"{Stats.aggregate['leesalee']:7.5f} " \
              f"{Stats.regression.average_slope:7.5f} " \
              f"{Stats.regression.percent_urban:7.5f} " \
              f"{Stats.regression.x_mean:7.5f} " \
              f"{Stats.regression.y_mean:7.5f} " \
              f"{Stats.regression.radius:7.5f} " \
              f"{Stats.aggregate['fmatch']:7.5f} " \
              f"{Coeff.get_saved_diffusion():4.0f} " \
              f"{Coeff.get_saved_breed():4.0f} " \
              f"{Coeff.get_saved_spread():4.0f} " \
              f"{Coeff.get_saved_slope_resistance():4.0f} " \
              f"{Coeff.get_saved_road_gravity():4.0f}\n"

        control_file.write(str)
        control_file.close()

    @staticmethod
    def log_urbanization_attempts():
        total = Stats.urbanization_attempt.add_attempts()
        Logger.log("\nLOG OF URBANIZATION ATTEMPTS")
        Logger.log(f"Num Success                = {Stats.urbanization_attempt.successes}")
        Logger.log(f"Num Z Type Failures        = {Stats.urbanization_attempt.z_failure}")
        Logger.log(f"Num Delta Type Failures    = {Stats.urbanization_attempt.delta_failure}")
        Logger.log(f"Num Slope Type Failures    = {Stats.urbanization_attempt.slope_failure}")
        Logger.log(f"Num Excluded Type Failures = {Stats.urbanization_attempt.excluded_failure}")
        Logger.log(f"Total Attempts             = {total}")

    @staticmethod
    def save(filename):
        Stats.record.run = Processing.get_current_run()
        Stats.record.monte_carlo = Processing.get_current_monte()
        Stats.record.year = Processing.get_current_year()
        index = 0
        if Processing.get_processing_type() != Globals.mode_enum['predict']:
            index = IGrid.igrid.urban_yr_to_idx(Stats.record.year)

        Stats.update_running_total(index)

        # Now we are writing the record to file for now...
        if Stats.record.monte_carlo == 0:
            # Create file
            with open(filename, 'wb') as output:  # Overwrites any existing file.
                _pickle.dump(Stats.record, output, -1)
        else:
            with open(filename, 'ab') as output:
                _pickle.dump(Stats.record, output, -1)


class UgmMapping:
    def __init__(self):
        self.row = 0
        self.col = 0

    def set_row(self, row):
        self.row = row

    def set_col(self, col):
        self.col = col

    def set_row_col(self, row, col):
        self.row = row
        self.col = col

    def get_row(self):
        return self.row

    def get_col(self):
        return self.col

    def __str__(self):
        return f"{self.row} {self.col}"


class UrbanizationAttempt:
    def __init__(self):
        self.successes = 0
        self.z_failure = 0
        self.delta_failure = 0
        self.slope_failure = 0
        self.excluded_failure = 0

    def add_attempts(self):
        return self.successes + self.z_failure + self.delta_failure + self.slope_failure + self.excluded_failure


class Record:
    def __init__(self):
        self.this_year = StatsVal()
        self.year = 0
        self.monte_carlo = 0
        self.run = 0

    def set_stats_info_to_record(self, stats_info):
        self.this_year.area = stats_info.area
        self.this_year.edges = stats_info.edges
        self.this_year.clusters = stats_info.clusters
        self.this_year.pop = stats_info.pop
        self.this_year.xmean = stats_info.x_mean
        self.this_year.ymean = stats_info.y_mean
        self.this_year.slope = stats_info.average_slope
        self.this_year.rad = stats_info.radius
        self.this_year.mean_cluster_size = stats_info.mean_cluster_size
        self.this_year.diffusion = Coeff.get_current_diffusion()
        self.this_year.spread = Coeff.get_current_spread()
        self.this_year.breed = Coeff.get_current_breed()
        self.this_year.slope_resistance = Coeff.get_current_slope_resistance()
        self.this_year.road_gravity = Coeff.get_current_road_gravity()

    def __str__(self):
        return f"{self.this_year.diffusion} {self.this_year.spread} {self.this_year.breed} {self.this_year.slope_resistance} {self.this_year.road_gravity}"
