# Name: configParser.py
# Name: createConfig.py
# Purpose: Creates a config file to use when running DSLEUTH.
# Author(s): Heather Miller
# Created: 4/23/22
# Last Modified: 4/23/22

import configparser

class CreateConfig:

    sleuthPath = "src/SLEUTH/src/grow"
    sleuthMode = "SMP"
    phase = "calibrate"
    scenarioPath = "Scenarios/scenario.demo200_calibrate"
    processors = 5
    testing = False
    debug = False

    def create(self):
        config = configparser.ConfigParser()
        config['RUN_SETTINGS'] = {'SLEUTHPath': self.sleuthPath,
                                  'SLEUTHMode': self.sleuthMode,
                                  'Phase': self.phase,
                                  'ScenarioPath': self.scenarioPath,
                                  'Processors': self.processors,
                                  'IsInTestMode': self.testing,
                                  'IsInDebugMode': self.debug}

        with open('config.ini', 'w') as configfile:
            config.write(configfile)

if __name__ == '__main__':
    createConfig = CreateConfig()
    createConfig.create()