import time
from logger import Logger
from globals import Globals


class Error(Exception):
    pass


class TimeError(Exception):
    def __init__(self, message):
        self.message = message


class Timer:
    def __init__(self, name):
        self.name = name
        self.start_time = 0.0
        self.stop_time = 0.0
        self.num_calls = 0
        self.is_running = False
        self.total_time = 0.0
        self.average_time = 0.0

    def start(self):
        if self.is_running:
            raise TimeError("Timer is running, use .stop() to stop it")
        self.start_time = time.perf_counter()
        self.is_running = True
        self.num_calls += 1

    def read(self):
        if not self.is_running:
            raise TimeError("Timer is not running. Use .start() to start it")
        current_time = time.perf_counter()
        return ((current_time - self.start_time) * 1000) + self.total_time

    def stop(self):
        if not self.is_running:
            raise TimeError("Timer is not running. Use .start() to start it")

        self.stop_time = time.perf_counter()
        self.total_time += (self.stop_time - self.start_time) * 1000
        self.average_time = self.total_time / self.num_calls
        self.is_running = False

    def __str__(self):
        return f"{self.name:15s} {self.num_calls:5}     {self.average_time:8.2f}      " \
               f"{self.total_time:10.2f} = {self.format_total_time()}"

    def format_total_time(self):
        seconds = self.total_time / 1000
        days = int(seconds / (60 * 60 * 24))
        seconds -= days

        hours = int(seconds / (60 * 60))
        seconds -= hours

        minutes = int(seconds / 60)
        seconds -= minutes

        seconds = int(seconds) % 60

        return f"{days:02d}:{hours:02d}:{minutes:02d}:{seconds:02d}"


class TimerUtility:
    timers = {
        "spr_spread": Timer("spr_spread"),
        "spr_phase1n3": Timer("spr_phase1n3"),
        "spr_phase4": Timer("spr_phase4"),
        "spr_phase5": Timer("spr_phase5"),
        "gdif_WriteGIF": Timer("gdif_WriteGIF"),
        "gdif_ReadGIF": Timer("gdif_ReadGIF"),
        "delta_deltatron": Timer("delta_deltatron"),
        "delta_phase1": Timer("delta_phase1"),
        "delta_phase2": Timer("delta_phase2"),
        "grw_growth": Timer("grw_growth"),
        "drv_driver": Timer("drv_driver"),
        "total_time": Timer("total_time")
    }

    @staticmethod
    def start_timer(key):
        timer = TimerUtility.timers[key]
        timer.start()

    @staticmethod
    def read_timer(key):
        timer = TimerUtility.timers[key]
        return timer.read()

    @staticmethod
    def stop_timer(key):
        timer = TimerUtility.timers[key]
        timer.stop()

    @staticmethod
    def log_timers():
        Logger.log("\n\n****************************LOG OF TIMINGS***********************************")
        Logger.log("        Routine #Calls    Avg Time    Total Time")
        Logger.log("                         (millisec)   (millisec)")
        for key in TimerUtility.timers:
            timer = TimerUtility.timers[key]
            Logger.log(timer)

        Logger.log(f"Number of CPUs = {Globals.npes}")
