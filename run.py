#!/usr/bin/env python

import sys
import json

sys.path.insert(0, 'src/data')
sys.path.insert(0, 'src/models')
sys.path.insert(0, 'src/features')

from make_dataset import *
from hmm_model import *
from lstm_model import *
from build_features import *

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

        # HMM model
        hmm_transition_mt(df)

        # LSTM model
        apps = lstm_data(filepath)
        model, out_df, X_train, X_test, y_train, y_test = bi_lstm(apps)
        test_acc(out_df, at_most = 1)

if __name__ == '__main__':

    targets = sys.argv[1:]
    main(targets)