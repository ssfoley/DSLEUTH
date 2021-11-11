class StatsVal:

    def __init__(self):
        self.sng = 0.0
        self.sdg = 0.0
        self.sdc = 0.0
        self.og = 0.0
        self.rt = 0.0
        self.pop = 0.0
        self.area = 0.0
        self.edges = 0.0
        self.clusters = 0.0
        self.xmean = 0.0
        self.ymean = 0.0
        self.rad = 0.0
        self.slope = 0.0
        self.mean_cluster_size = 0.0
        self.diffusion = 0.0
        self.spread = 0.0
        self.breed = 0.0
        self.slope_resistance = 0.0
        self.road_gravity = 0.0
        self.percent_urban = 0.0
        self.percent_road = 0.0
        self.growth_rate = 0.0
        self.leesalee = 0.0
        self.num_growth_pix = 0.0

    def update_stat_val(self, record):
        self.sng += record.this_year.sng
        self.sdg += record.this_year.sdg
        self.sdc += record.this_year.sdc
        self.og += record.this_year.og
        self.rt += record.this_year.rt
        self.pop += record.this_year.pop
        self.area += record.this_year.area
        self.edges += record.this_year.edges
        self.clusters += record.this_year.clusters
        self.xmean += record.this_year.xmean
        self.ymean += record.this_year.ymean
        self.rad += record.this_year.rad
        self.slope += record.this_year.slope
        self.mean_cluster_size += record.this_year.mean_cluster_size
        self.diffusion += record.this_year.diffusion
        self.spread += record.this_year.spread
        self.breed += record.this_year.breed
        self.slope_resistance += record.this_year.slope_resistance
        self.road_gravity += record.this_year.road_gravity
        self.percent_urban += record.this_year.percent_urban
        self.percent_road += record.this_year.percent_road
        self.growth_rate += record.this_year.growth_rate
        self.leesalee += record.this_year.leesalee
        self.num_growth_pix += record.this_year.num_growth_pix

    def calculate_averages(self, total_monte, running_total):
        self.sng = running_total.sng / total_monte
        self.sdg = running_total.sdg / total_monte
        self.sdc = running_total.sdc / total_monte
        self.og = running_total.og / total_monte
        self.rt = running_total.rt / total_monte
        self.pop = running_total.pop / total_monte
        self.area = running_total.area / total_monte
        self.edges = running_total.edges / total_monte
        self.clusters = running_total.clusters / total_monte
        self.xmean = running_total.xmean / total_monte
        self.ymean = running_total.ymean / total_monte
        self.rad = running_total.rad / total_monte
        self.slope = running_total.slope / total_monte
        self.mean_cluster_size = running_total.mean_cluster_size / total_monte
        self.diffusion = running_total.diffusion / total_monte
        self.spread = running_total.spread / total_monte
        self.breed = running_total.breed / total_monte
        self.slope_resistance = running_total.slope_resistance / total_monte
        self.road_gravity = running_total.road_gravity / total_monte
        self.percent_urban = running_total.percent_urban / total_monte
        self.percent_road = running_total.percent_road / total_monte
        self.growth_rate = running_total.growth_rate / total_monte
        self.leesalee = running_total.leesalee / total_monte
        self.num_growth_pix = running_total.num_growth_pix / total_monte

    def calculate_sd(self, total_mc, record, average):
        self.sng = StatsVal.sd_equation(record.this_year.sng - average.sng, total_mc)
        self.sdg = StatsVal.sd_equation(record.this_year.sdg - average.sdg, total_mc)
        self.sdc = StatsVal.sd_equation(record.this_year.sdc - average.sdc, total_mc)
        self.og = StatsVal.sd_equation(record.this_year.og - average.og, total_mc)
        self.rt = StatsVal.sd_equation(record.this_year.rt - average.rt, total_mc)
        self.pop = StatsVal.sd_equation(record.this_year.pop - average.pop, total_mc)
        self.area = StatsVal.sd_equation(record.this_year.area - average.area, total_mc)
        self.edges = StatsVal.sd_equation(record.this_year.edges - average.edges, total_mc)
        self.clusters = StatsVal.sd_equation(record.this_year.clusters - average.clusters, total_mc)
        self.xmean = StatsVal.sd_equation(record.this_year.xmean - average.xmean, total_mc)
        self.ymean = StatsVal.sd_equation(record.this_year.ymean - average.ymean, total_mc)
        self.rad = StatsVal.sd_equation(record.this_year.rad - average.rad, total_mc)
        self.slope = StatsVal.sd_equation(record.this_year.slope - average.slope, total_mc)
        self.mean_cluster_size = StatsVal.sd_equation(record.this_year.mean_cluster_size - average.mean_cluster_size, total_mc)
        self.diffusion = StatsVal.sd_equation(record.this_year.diffusion - average.diffusion, total_mc)
        self.spread = StatsVal.sd_equation(record.this_year.spread - average.spread, total_mc)
        self.breed = StatsVal.sd_equation(record.this_year.breed - average.breed, total_mc)
        self.slope_resistance = StatsVal.sd_equation(record.this_year.slope_resistance - average.slope_resistance, total_mc)
        self.road_gravity = StatsVal.sd_equation(record.this_year.road_gravity - average.road_gravity, total_mc)
        self.percent_urban = StatsVal.sd_equation(record.this_year.percent_urban - average.percent_urban, total_mc)
        self.percent_road = StatsVal.sd_equation(record.this_year.percent_road - average.percent_road, total_mc)
        self.growth_rate = StatsVal.sd_equation(record.this_year.growth_rate - average.growth_rate, total_mc)
        self.leesalee = StatsVal.sd_equation(record.this_year.leesalee - average.leesalee, total_mc)
        self.num_growth_pix = StatsVal.sd_equation(record.this_year.num_growth_pix - average.num_growth_pix, total_mc)

    def get_field_by_name(self, name):
        fields = {
            "area": self.area,
            "edges": self.edges,
            "clusters": self.clusters,
            "pop": self.pop,
            "x_mean": self.xmean,
            "y_mean": self.ymean,
            "radius": self.rad,
            "average_slope": self.slope,
            "mean_cluster_size": self.mean_cluster_size,
            "percent_urban": self.percent_urban
        }
        return fields[name]

    @staticmethod
    def sd_equation(val, total_mc):
        return pow((val * val / total_mc), 0.5)

    def __str__(self):
        return f"{self.sng:8.2f} {self.sdg:8.2f} {self.sdc:8.2f} {self.og:8.2f} {self.rt:8.2f} {self.pop:8.2f} " \
               f"{self.area:8.2f} {self.edges:8.2f} {self.clusters:8.2f} {self.xmean:8.2f} {self.ymean:8.2f} " \
               f"{self.rad:8.2f} {self.slope:8.2f} {self.mean_cluster_size:8.2f} {self.diffusion:8.2f} " \
               f"{self.spread:8.2f} {self.breed:8.2f} {self.slope_resistance:8.2f} {self.road_gravity:8.2f} " \
               f"{self.percent_urban:8.2f} {self.percent_road:8.2f} {self.growth_rate:8.2f} " \
               f"{self.leesalee:8.2f} {self.num_growth_pix:8.2f}"
