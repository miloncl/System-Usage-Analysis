import os
import sqlite3
import pandas as pd

# Get data
def get_data(filepath):
    """Get data for the models"""

    if filepath not in ['../data/raw/user1/', '../../../data/raw/user2/']:
        return None

    data = []
    time_df = []

    # Read from sql databases
    for filename in os.listdir(filepath):
        if filename == '.ipynb_checkpoints':
            continue
        cnx = sqlite3.connect(filepath + filename)
        df_string = pd.read_sql_query("SELECT * FROM COUNTERS_STRING_TIME_DATA", cnx)
        df_ull = pd.read_sql_query("SELECT * FROM COUNTERS_ULL_TIME_DATA", cnx)
        df_data = pd.concat([df_string, df_ull], ignore_index = True)
        start_val = pd.DataFrame({'MEASUREMENT_TIME': df_data.loc[0][0], 'ID_INPUT': 4, 'VALUE': 's0', 'PRIVATE_DATA': 0}, index =[0])
        df_data = pd.concat([start_val, df_data])
        data.append(pd.DataFrame(df_data))
        schema = pd.DataFrame(pd.read_sql_query("SELECT * FROM INPUTS", cnx))
        
        # get actual start time
        time_diff = pd.read_sql_query("SELECT * FROM DB_META_DATA", cnx)
        utc_open = pd.to_datetime(time_diff[time_diff['KEY'] == 'OPEN_TIME_UTC']['VALUE'].iloc[0])
        local_open = pd.to_datetime(time_diff[time_diff['KEY'] == 'OPEN_TIME_LOCAL']['VALUE'].iloc[0])
        time_difference = utc_open - local_open
        
        time_sub = df_data[df_data['ID_INPUT'] == 4].copy()
        time_sub['MEASUREMENT_TIME'] = pd.to_datetime(time_sub['MEASUREMENT_TIME'])
        time_sub['MEASUREMENT_TIME'] = time_sub['MEASUREMENT_TIME'] - time_difference
        time_sub['Time_Used'] = time_sub['MEASUREMENT_TIME'].diff().dt.total_seconds()
        time_sub['Time_Used'] = time_sub['Time_Used'].shift(periods = -1)
        time_sub = time_sub.drop(columns = ['PRIVATE_DATA', 'ID_INPUT'])
        time_df.append(time_sub)

    df = pd.concat(data, ignore_index = True)
    df['MEASUREMENT_TIME'] = pd.to_datetime(df['MEASUREMENT_TIME']) - time_difference
    df['PRIVATE_DATA'] = df['PRIVATE_DATA'].astype(int)

    df = df.sort_values(by = ['MEASUREMENT_TIME'], ignore_index = True)

    # Save data to files
    # if filepath == '../data/raw/user1/':
    #     df.to_csv('../data/temp/dataUser1.csv')
    # elif filepath == '../data/raw/user2/':
    #     df.to_csv('../data/temp/datauser2.csv')
    
    return df
    
    

    