#!/usr/bin/env python

import sys
import json
import sqlite3

import pandas as pd
import matplotlib as plt
import numpy as np
import datetime



sys.path.insert(0, 'src/data')
sys.path.insert(0, 'src/models')

from make_dataset import get_data
from hmm_model import model

def main(targets):
    '''
    This function takes in the "targets" which contain the data
    and runs the main logic of the project
    '''
    if 'data' in targets:
        with open('config/data-params.json') as fh:
            data_configuration = json.load(fh)

        # make data
        df = get_data(**data_configuration)
        
        if df is None:
            return

        model(df)


if __name__ == '__main__':
    targets = sys.argv[1:]
    main(targets)