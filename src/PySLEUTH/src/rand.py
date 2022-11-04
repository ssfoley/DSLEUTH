import random

#TODO: replace these wrappers with direct calls when random values are needed

class Random:
    @staticmethod
    def set_seed(seed):
        random.seed(seed)

    @staticmethod
    def get_int(min, max):
        return random.randint(min, max)

    @staticmethod
    def get_float():
        return random.random()

    @staticmethod
    def get_element(lst):
        return random.choice(lst)

    @staticmethod
    def get_unique_elements(lst, num):
        return random.sample(lst, num)
