from igrid import IGrid
from landClass import LandClass, Constants
from logger import Logger


class Transition:
    ftransition = [0] * (Constants.MAX_NUM_CLASSES * Constants.MAX_NUM_CLASSES)
    transition = [0] * (Constants.MAX_NUM_CLASSES * Constants.MAX_NUM_CLASSES)
    class_slope = [0.0] * Constants.MAX_NUM_CLASSES
    class_count0 = [0.0] * Constants.MAX_NUM_CLASSES
    class_count1 = [0.0] * Constants.MAX_NUM_CLASSES
    class_count_sum0 = 0
    class_count_sum1 = 0

    @staticmethod
    def get_ftransition():
        return Transition.ftransition

    @staticmethod
    def get_class_slope():
        return Transition.class_slope

    @staticmethod
    def create_matrix():
        num_classes = LandClass.get_num_landclasses()

        landuse0 = IGrid.igrid.get_landuse_idx(0)
        landuse1 = IGrid.igrid.get_landuse_idx(1)
        slope = IGrid.igrid.get_slope()
        trans_count = 0

        for i in range(IGrid.nrows):
            for j in range(IGrid.ncols):
                index_offset = i * IGrid.ncols + j
                land0_val = landuse0.gridData[index_offset]
                land1_val = landuse1.gridData[index_offset]

                land0_idx = LandClass.new_indices[land0_val]
                land1_idx = LandClass.new_indices[land1_val]

                Transition.class_count0[land0_idx] += 1.0
                Transition.class_count1[land1_idx] += 1.0

                Transition.class_slope[land1_idx] += slope.gridData[index_offset]

                trans_offset = land0_idx * num_classes + land1_idx
                Transition.transition[trans_offset] += 1

                if land0_val != land1_val:
                    trans_count += 1

        for i in range(num_classes):

            Transition.class_count_sum0 += Transition.class_count0[i]
            Transition.class_count_sum1 += Transition.class_count1[i]
            lsum = 0
            for j in range(num_classes):
                trans_offset = i * num_classes + j
                Transition.ftransition[trans_offset] = Transition.transition[trans_offset]
                lsum += Transition.transition[trans_offset]

            for j in range(num_classes):
                trans_offset = i * num_classes + j
                Transition.ftransition[trans_offset] = 0.0 if lsum == 0 else (Transition.ftransition[trans_offset] / lsum)

    @staticmethod
    def log_transition():
        Logger.log("\n**************LOGGING TRANSITION MATRICES**************")
        Logger.log(f"Land 1 classed pixel count (class_count_sum0) = {int(Transition.class_count_sum0)}")
        Logger.log(f"Land 2 classed pixel count (class_count_sum1) = {int(Transition.class_count_sum1)}")
        Logger.log("")

        # Print Pixel Count Transitions for Land Cover Data
        Logger.log("       LOGGING CLASS PER PIXEL TRANSITION    ")
        temp = "        "
        land_classes = LandClass.get_landclasses()
        for land_class in land_classes:
            temp += f"{land_class.name:>9} "
        Logger.log(temp)

        temp = ""
        num_classes = LandClass.get_num_landclasses()
        for i in range(num_classes):
            temp += f"{land_classes[i].name:>8}"
            for j in range(num_classes):
                trans_offset = i * num_classes + j
                temp += f" {int(Transition.transition[trans_offset]):>8} "
            Logger.log(temp)
            temp = ""

        # Print Annual Transition Probabilities for Land Cover Data
        Logger.log("")
        Logger.log("       LOGGING ANNUAL TRANSITION PROBABILITIES ")
        temp = "        "
        for land_class in land_classes:
            temp += f"{land_class.name:>9} "
        Logger.log(temp)

        temp = ""
        num_classes = LandClass.get_num_landclasses()
        for i in range(num_classes):
            temp += f"{land_classes[i].name:>8}"
            for j in range(num_classes):
                trans_offset = i * num_classes + j
                temp += f" {100 * Transition.ftransition[trans_offset]:>8.2f} "
            Logger.log(temp)
            temp = ""

        # Print Average Slope per Class for Land Cover Data
        Logger.log("")
        Logger.log("       LOGGING LAND CLASS AVERAGE SLOPES ")
        Logger.log("                        Land1             Land2         Average")
        Logger.log("Class Totals:   count[pct_change]  count[pct_change]     Slope")
        temp = ""

        for i in range(num_classes):
            temp += f"{land_classes[i].name:>12} "
            pct = (Transition.class_slope[i] / Transition.class_count1[i]) if Transition.class_count1[i] > 0 else 0.0
            Transition.class_slope[i] = pct

            avg0 = 100.0 * Transition.class_count0[i] / Transition.class_count_sum0
            avg1 = 100.0 * Transition.class_count1[i] / Transition.class_count_sum1
            temp += f"{int(Transition.class_count0[i]):>10} [{avg0:>5.1f}] {int(Transition.class_count1[i]):>10} [{avg1:>5.1f}]     {pct:>7.3f}"
            Logger.log(temp)
            temp = ""


            



