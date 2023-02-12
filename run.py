#!/usr/bin/env python

import sys
import json
import sqlite3

import pandas as pd
import matplotlib as plt
import numpy as np
import datetime



sys.path.insert(0, 'src/data')
sys.path.insert(0, 'src/model')

from get_data import get_data
from model import model

def main(targets):
    '''
    Runs the main project pipeline logic, given the targets.
    targets must contain: 'data'. 
    '''
    if 'data' in targets:
        with open('config/data-params.json') as fh:
            data_cfg = json.load(fh)

        # make the data target
        string_df, string_ull = get_data(**data_cfg)
        
    if 'model' in targets:
        model(string_df)
        
    if 'test' in targets:
        with open('config/data-params.json') as fh:
            data_cfg = json.load(fh)

        # make the data target
        string_df, string_ull = get_data(**data_cfg)
        
        model(string_df)


if __name__ == '__main__':
    # run via:
    # python main.py test
    targets = sys.argv[1:]
    main(targets)