# Name: parseConfig.py
# Purpose: Parses the config.ini file for use in runs of DSLEUTH.
# Author(s): Heather Miller
# Created: 4/23/22
# Last Modified: 4/23/22

import configparser

class ParseConfig:

    sleuthPath = ""
    sleuthMode = ""
    phase = ""
    scenarioPath = ""
    processors = 0
    testing = False
    debug = False

    def parse(self):
        try:
            config = configparser.ConfigParser()
            config.read('config.ini')

            self.sleuthPath = config['RUN_SETTINGS']['SLEUTHPath']
            self.sleuthMode = config['RUN_SETTINGS']['SLEUTHMode']
            self.phase = config['RUN_SETTINGS']['Phase']
            self.scenarioPath = config['RUN_SETTINGS']['ScenarioPath']
            self.processors = int(config['RUN_SETTINGS']['Processors'])
            self.testing = config.getboolean('RUN_SETTINGS', 'IsInTestMode')
            self.debug = config.getboolean('RUN_SETTINGS', 'IsInDebugMode')

            return True
        except:
            return False

if __name__ == '__main__':
    parseConfig = ParseConfig()
    parseConfig.parse()

    print(parseConfig.sleuthPath)
    print(parseConfig.sleuthMode)
    print(parseConfig.phase)
    print(parseConfig.scenarioPath)
    print(parseConfig.processors)
    print(parseConfig.testing)
    print(parseConfig.debug)