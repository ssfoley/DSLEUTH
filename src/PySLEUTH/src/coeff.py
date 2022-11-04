from logger import Logger
from scenario import Scenario
from ugm_defines import UGMDefines


class CoeffInfo:
    def __init__(self, diffusion=0, spread=0, breed=0, slope_resistance=0, road_gravity=0):
        self.diffusion = int(diffusion)
        self.spread = int(spread)
        self.breed = int(breed)
        self.slope_resistance = int(slope_resistance)
        self.road_gravity = int(road_gravity)

    def __str__(self):
        return f"{self.diffusion} {self.breed} {self.spread} {self.slope_resistance} {self.road_gravity}"

class Coeff:
    # These should be static
    saved_coefficient = CoeffInfo()
    current_coefficient = CoeffInfo()
    step_coeff = CoeffInfo()
    start_coeff = CoeffInfo()
    stop_coeff = CoeffInfo()
    best_fit_coeff = CoeffInfo()
    coeff_filename = ""

    @staticmethod
    def set_saved_coeff(diffusion, spread, breed, slope_resistance, road_gravity):
        Coeff.saved_coefficient = CoeffInfo(diffusion, spread, breed, slope_resistance, road_gravity)

    @staticmethod
    def set_current_coeff(diffusion, spread, breed, slope_resistance, road_gravity):
        Coeff.set_current_diffusion(diffusion)
        Coeff.set_current_spread(spread)
        Coeff.set_current_breed(breed)
        Coeff.set_current_slope_resistance(slope_resistance)
        Coeff.set_current_road_gravity(road_gravity)
        # Coeff.current_coefficient = CoeffInfo(diffusion, spread, breed, slope_resistance, road_gravity)

    @staticmethod
    def set_start_coeff(diffusion, spread, breed, slope_resistance, road_gravity):
        Coeff.start_coeff = CoeffInfo(diffusion, spread, breed, slope_resistance, road_gravity)

    @staticmethod
    def set_step_coeff(diffusion, spread, breed, slope_resistance, road_gravity):
        Coeff.step_coeff = CoeffInfo(diffusion, spread, breed, slope_resistance, road_gravity)

    @staticmethod
    def set_stop_coeff(diffusion, spread, breed, slope_resistance, road_gravity):
        Coeff.stop_coeff = CoeffInfo(diffusion, spread, breed, slope_resistance, road_gravity)

    @staticmethod
    def set_best_fit_coeff(diffusion, spread, breed, slope_resistance, road_gravity):
        Coeff.best_fit_coeff = CoeffInfo(diffusion, spread, breed, slope_resistance, road_gravity)


    @staticmethod
    def set_coeff_filename(filename):
        Coeff.coeff_filename = filename

    @staticmethod
    def create_coeff_file(filename, overwrite):
        Coeff.coeff_filename = filename
        if overwrite:
            coeff_file = open(filename, "w")
        else:
            coeff_file = open(filename, "a")

        header = "  Run    MC Year Diffusion   Breed   Spread SlopeResist RoadGrav\n"
        coeff_file.write(header)
        coeff_file.close()

    @staticmethod
    def get_current_diffusion():
        return Coeff.current_coefficient.diffusion

    @staticmethod
    def set_current_diffusion(diffusion):
        if diffusion == 0:
            Coeff.current_coefficient.diffusion = 1
            Coeff.saved_coefficient.diffusion = 1
        else:
            Coeff.current_coefficient.diffusion = diffusion
            Coeff.saved_coefficient.diffusion = diffusion

    @staticmethod
    def get_current_spread():
        return Coeff.current_coefficient.spread

    @staticmethod
    def set_current_spread(spread):
        if spread == 0:
            Coeff.current_coefficient.spread = 1
            Coeff.saved_coefficient.spread = 1
        else:
            Coeff.current_coefficient.spread = spread
            Coeff.saved_coefficient.spread = spread

    @staticmethod
    def get_current_breed():
        return Coeff.current_coefficient.breed

    @staticmethod
    def set_current_breed(breed):
        if breed == 0:
            Coeff.current_coefficient.breed = 1
            Coeff.saved_coefficient.breed = 1
        else:
            Coeff.current_coefficient.breed = breed
            Coeff.saved_coefficient.breed = breed

    @staticmethod
    def get_current_slope_resistance():
        return Coeff.current_coefficient.slope_resistance

    @staticmethod
    def set_current_slope_resistance(slope_resistance):
        if slope_resistance == 0:
            Coeff.current_coefficient.slope_resistance = 1
            Coeff.saved_coefficient.slope_resistance = 1
        else:
            Coeff.current_coefficient.slope_resistance = slope_resistance
            Coeff.saved_coefficient.slope_resistance = slope_resistance

    @staticmethod
    def get_current_road_gravity():
        return Coeff.current_coefficient.road_gravity

    @staticmethod
    def set_current_road_gravity(road_gravity):
        if road_gravity == 0:
            Coeff.current_coefficient.road_gravity = 1
            Coeff.saved_coefficient.road_gravity = 1
        else:
            Coeff.current_coefficient.road_gravity = road_gravity
            Coeff.saved_coefficient.road_gravity = road_gravity

    @staticmethod
    def get_start_diffusion():
        return Coeff.start_coeff.diffusion

    @staticmethod
    def get_start_spread():
        return Coeff.start_coeff.spread

    @staticmethod
    def get_start_breed():
        return Coeff.start_coeff.breed

    @staticmethod
    def get_start_slope_resistance():
        return Coeff.start_coeff.slope_resistance

    @staticmethod
    def get_start_road_gravity():
        return Coeff.start_coeff.road_gravity

    @staticmethod
    def get_stop_diffusion():
        return Coeff.stop_coeff.diffusion

    @staticmethod
    def get_stop_spread():
        return Coeff.stop_coeff.spread

    @staticmethod
    def get_stop_breed():
        return Coeff.stop_coeff.breed

    @staticmethod
    def get_stop_slope_resistance():
        return Coeff.stop_coeff.slope_resistance

    @staticmethod
    def get_stop_road_gravity():
        return Coeff.stop_coeff.road_gravity

    @staticmethod
    def get_step_diffusion():
        return Coeff.step_coeff.diffusion

    @staticmethod
    def get_step_spread():
        return Coeff.step_coeff.spread

    @staticmethod
    def get_step_breed():
        return Coeff.step_coeff.breed

    @staticmethod
    def get_step_slope_resistance():
        return Coeff.step_coeff.slope_resistance

    @staticmethod
    def get_step_road_gravity():
        return Coeff.step_coeff.road_gravity

    @staticmethod
    def get_start_step_stop_diffusion():
        return Coeff.start_coeff.diffusion, Coeff.step_coeff.diffusion, Coeff.stop_coeff.diffusion

    @staticmethod
    def get_start_step_stop_spread():
        return Coeff.start_coeff.spread, Coeff.step_coeff.spread, Coeff.stop_coeff.spread

    @staticmethod
    def get_start_step_stop_breed():
        return Coeff.start_coeff.breed, Coeff.step_coeff.breed, Coeff.stop_coeff.breed

    @staticmethod
    def get_start_step_stop_slope_resistance():
        return Coeff.start_coeff.slope_resistance, Coeff.step_coeff.slope_resistance, Coeff.stop_coeff.slope_resistance

    @staticmethod
    def get_start_step_stop_road_gravity():
        return Coeff.start_coeff.road_gravity, Coeff.step_coeff.road_gravity, Coeff.stop_coeff.road_gravity

    @staticmethod
    def get_saved_diffusion():
        return Coeff.saved_coefficient.diffusion

    @staticmethod
    def set_saved_diffusion(diffusion):
        Coeff.saved_coefficient.diffusion = diffusion

    @staticmethod
    def get_saved_spread():
        return Coeff.saved_coefficient.spread

    @staticmethod
    def set_saved_spread(spread):
        Coeff.saved_coefficient.spread = spread

    @staticmethod
    def get_saved_breed():
        return Coeff.saved_coefficient.breed

    @staticmethod
    def set_saved_breed(breed):
        Coeff.saved_coefficient.breed = breed

    @staticmethod
    def get_saved_slope_resistance():
        return Coeff.saved_coefficient.slope_resistance

    @staticmethod
    def set_saved_slope_resistance(slope_resistance):
        Coeff.saved_coefficient.slope_resistance = slope_resistance

    @staticmethod
    def get_saved_road_gravity():
        return Coeff.saved_coefficient.road_gravity

    @staticmethod
    def set_saved_road_gravity(road_gravity):
        Coeff.saved_coefficient.road_gravity = road_gravity

    @staticmethod
    def get_best_diffusion():
        return Coeff.best_fit_coeff.diffusion

    @staticmethod
    def set_best_diffusion(diffusion):
        Coeff.best_fit_coeff.diffusion = diffusion

    @staticmethod
    def get_best_spread():
        return Coeff.best_fit_coeff.spread

    @staticmethod
    def set_best_spread(spread):
        Coeff.best_fit_coeff.spread = spread

    @staticmethod
    def get_best_breed():
        return Coeff.best_fit_coeff.breed

    @staticmethod
    def set_best_breed(breed):
        Coeff.best_fit_coeff.breed = breed

    @staticmethod
    def get_best_slope_resistance():
        return Coeff.best_fit_coeff.slope_resistance

    @staticmethod
    def set_best_slope_resistance(slope_resistance):
        Coeff.best_fit_coeff.slope_resistance = slope_resistance

    @staticmethod
    def get_best_road_gravity():
        return Coeff.best_fit_coeff.road_gravity

    @staticmethod
    def set_best_road_gravity(road_gravity):
        Coeff.best_fit_coeff.road_gravity = road_gravity

    @staticmethod
    def self_modify(growth_rate, percent_urban):
        slope_sensitivity = float(Scenario.get_scen_value('slope_sensitivity'))
        road_grav_sensitivity = float(Scenario.get_scen_value('road_grav_sensitivity'))

        # boom year
        if growth_rate > float(Scenario.get_scen_value('critical_high')):
            Coeff.current_coefficient.slope_resistance -= (percent_urban * slope_sensitivity)
            if Coeff.current_coefficient.slope_resistance <= UGMDefines.MIN_SLOPE_RESISTANCE_VALUE:
                Coeff.current_coefficient.slope_resistance = 1.0

            Coeff.current_coefficient.road_gravity += (percent_urban * road_grav_sensitivity)
            if Coeff.current_coefficient.road_gravity > UGMDefines.MAX_ROAD_GRAVITY_VALUE:
                Coeff.current_coefficient.road_gravity = UGMDefines.MAX_ROAD_GRAVITY_VALUE

            if Coeff.current_coefficient.diffusion < UGMDefines.MAX_DIFFUSION_VALUE:
                boom = float(Scenario.get_scen_value('boom'))
                Coeff.current_coefficient.diffusion *= boom
                if Coeff.current_coefficient.diffusion > UGMDefines.MAX_DIFFUSION_VALUE:
                    Coeff.current_coefficient.diffusion = UGMDefines.MAX_DIFFUSION_VALUE

                Coeff.current_coefficient.breed *= boom
                if Coeff.current_coefficient.breed > UGMDefines.MAX_BREED_VALUE:
                    Coeff.current_coefficient.breed = UGMDefines.MAX_BREED_VALUE

                Coeff.current_coefficient.spread *= boom
                if Coeff.current_coefficient.spread > UGMDefines.MAX_SPREAD_VALUE:
                    Coeff.current_coefficient.spread = UGMDefines.MAX_SPREAD_VALUE

        # bust year
        if growth_rate < float(Scenario.get_scen_value('critical_low')):
            Coeff.current_coefficient.slope_resistance += (percent_urban * slope_sensitivity)
            if Coeff.current_coefficient.slope_resistance > UGMDefines.MAX_SLOPE_RESISTANCE_VALUE:
                Coeff.current_coefficient.slope_resistance = UGMDefines.MAX_SLOPE_RESISTANCE_VALUE

            Coeff.current_coefficient.road_gravity -= (percent_urban * road_grav_sensitivity)
            if Coeff.current_coefficient.slope_resistance <= UGMDefines.MAX_SLOPE_RESISTANCE_VALUE:
                Coeff.current_coefficient.slope_resistance = 1.0

            if growth_rate < float(Scenario.get_scen_value('critical_low')) and Coeff.current_coefficient.diffusion > 0:
                bust = float(Scenario.get_scen_value('bust'))
                Coeff.current_coefficient.diffusion *= bust
                if Coeff.current_coefficient.diffusion <= UGMDefines.MIN_DIFFUSION_VALUE:
                    Coeff.current_coefficient.diffusion = 1.0

                Coeff.current_coefficient.spread *= bust
                if Coeff.current_coefficient.spread <= UGMDefines.MIN_SPREAD_VALUE:
                    Coeff.current_coefficient.spread = 1.0

                Coeff.current_coefficient.breed *= bust
                if Coeff.current_coefficient.breed <= UGMDefines.MIN_BREED_VALUE:
                    Coeff.current_coefficient.breed = 1.0

    @staticmethod
    def write_current_coeff(cur_run, cur_mc, cur_yr):
        if Scenario.get_scen_value('write_coeff_file'):
            file = open(Coeff.coeff_filename, "a")

            file.write(f"{cur_run: 5} "
                       f"{cur_mc: 5} "
                       f"{cur_yr: 4} "
                       f"{Coeff.current_coefficient.diffusion: 8.2f} "
                       f"{Coeff.current_coefficient.breed: 8.2f} "
                       f"{Coeff.current_coefficient.spread: 8.2f} "
                       f"{Coeff.current_coefficient.slope_resistance: 8.2f} "
                       f"{Coeff.current_coefficient.road_gravity: 8.2f}\n")
            file.close()

    @staticmethod
    def log_current():
        Logger.log("*********Current Coeff**********")
        Logger.log(f"Diffusion        = {Coeff.current_coefficient.diffusion}")
        Logger.log(f"Spread           = {Coeff.current_coefficient.spread}")
        Logger.log(f"Breed            = {Coeff.current_coefficient.breed}")
        Logger.log(f"Slope Resistance = {Coeff.current_coefficient.slope_resistance}")
        Logger.log(f"Road Gravity     = {Coeff.current_coefficient.road_gravity}")
        Logger.log("")

    @staticmethod
    def log_saved():
        Logger.log("*********Saved Coeff**********")
        Logger.log(f"Diffusion        = {Coeff.saved_coefficient.diffusion}")
        Logger.log(f"Spread           = {Coeff.saved_coefficient.spread}")
        Logger.log(f"Breed            = {Coeff.saved_coefficient.breed}")
        Logger.log(f"Slope Resistance = {Coeff.saved_coefficient.slope_resistance}")
        Logger.log(f"Road Gravity     = {Coeff.saved_coefficient.road_gravity}")
        Logger.log("")

"""
    saved_coefficient = None
    current_coefficient = None
    step_coeff = None
    start_coeff = None
    stop_coeff = None
    best_fit_coeff = None
"""


