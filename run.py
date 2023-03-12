#!/usr/bin/env python

import sys
import json

sys.path.insert(0, 'src/data')
sys.path.insert(0, 'src/models')

from make_dataset import *
from hmm_model import *

def main(targets):
    '''
    This function takes in the "targets" which contain the data
    and runs the main logic of the project
    '''
    # Case 1
    if 'data' in targets:
        with open('config/data-params.json') as fh:
            data_configuration = json.load(fh)
        
        df = get_data(**data_configuration)
    
    # Case 2
    if 'test' in targets:
        with open('config/data-params.json') as fh:
            data_configuration = json.load(fh)

        # make data
        df = get_data(**data_configuration)
        
        if df is None:
            print('No available data')
            return

        hmm_transition_mt(df)

if __name__ == '__main__':

    targets = sys.argv[1:]
    main(targets)