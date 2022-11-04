class Logger:
    logfile = None
    log_opened = False

    @staticmethod
    def init(filepath):
        # remove old log file (this is for Elise purposes)
        Logger.logfile = open(filepath, "w")
        Logger.log_opened = True

    @staticmethod
    def log(message, end="\n"):
        if Logger.log_opened:
            Logger.logfile.write(f"{message}{end}")
        else:
            print("Error -> Log not opened, can't write to it")
            exit(1)

    @staticmethod
    def close():
        print("File Closed Now")
        if Logger.log_opened:
            Logger.logfile.close()
            Logger.log_opened = False