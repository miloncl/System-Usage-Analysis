import os
import sqlite3
import pandas as pd
import numpy as np
import datetime

# Get data
def get_data(filepath, read_from_notebook=False, out_schema=False):
    """Get data for the models"""
    if not read_from_notebook:
        if filepath not in ['./data/raw/user1/', './data/raw/user2/']:
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
    
    if not out_schema:
        return df
    return df, schema, time_df

def save_time_measurement(time_df, filepath, saved=False):
    """Save some data files"""
    all_time_df = pd.concat(time_df, ignore_index = True)
    all_time_df = all_time_df.fillna(0)

    if saved:
        if 'user1' in filepath:
            all_time_df.to_csv('timeuser1.csv')
        else:
            all_time_df.to_csv('timeuser2.csv')

    return all_time_df

def lstm_data(filepath):
    data = []
    time_df = []

    for filename in os.listdir(filepath):
        if 'temp' in filename:
            continue
        if filename == '.ipynb_checkpoints':
            continue
        cnx = sqlite3.connect(filepath + filename)
        cnx.text_factory = lambda b: b.decode(errors = 'ignore') #https://stackoverflow.com/questions/22751363/sqlite3-operationalerror-could-not-decode-to-utf-8-column
        df_string = pd.read_sql_query("SELECT * FROM COUNTERS_STRING_TIME_DATA", cnx)
        df_ull = pd.read_sql_query("SELECT * FROM COUNTERS_ULL_TIME_DATA", cnx)
        df_data = pd.concat([df_string, df_ull], ignore_index = True)
        start_val = pd.DataFrame({'MEASUREMENT_TIME': df_data.loc[0][0], 'ID_INPUT': 4, 'VALUE': 's0', 'PRIVATE_DATA': 0}, index =[0])
        df_data = pd.concat([start_val, df_data])
        data.append(pd.DataFrame(df_data))
        
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

    df['MEASUREMENT_TIME'] = pd.to_datetime(df['MEASUREMENT_TIME'])
    df['PRIVATE_DATA'] = df['PRIVATE_DATA'].astype(int)
    df['VALUE'] = df['VALUE'].str.lower()

    all_time_df = pd.concat(time_df, ignore_index = True)
    all_time_df = all_time_df.fillna(0)
    all_time_df['Hour'] = all_time_df['MEASUREMENT_TIME'].dt.hour
    all_time_df['Date'] = all_time_df["MEASUREMENT_TIME"].astype(str).apply(lambda x: x[:10])
    all_time_df['Minute'] = all_time_df['MEASUREMENT_TIME'].dt.minute
    all_time_df['Day'] = all_time_df['MEASUREMENT_TIME'].dt.day
    all_time_df['Month'] = all_time_df['MEASUREMENT_TIME'].dt.month
    all_time_df['Day_Week'] = all_time_df['MEASUREMENT_TIME'].dt.day_name()
    all_time_df = all_time_df.assign(Week_Year = all_time_df['MEASUREMENT_TIME'].apply(lambda x: x.strftime("%W")))
    all_time_df = all_time_df.assign(Week_Day = all_time_df['Week_Year'] + '_' + all_time_df['Day_Week'])

    def date_range(start, end):
        dates = []
        delta = end - start
        for i in range(delta.days + 2):
            date = start + datetime.timedelta(days=i)
            date = date.strftime("%Y-%m-%d")
            dates.append(date)
        return dates

    all_columns = [x for x in range(0,24)]
    all_columns.append('Total_Usage')
    all_index = date_range(all_time_df['MEASUREMENT_TIME'].min(), all_time_df['MEASUREMENT_TIME'].max())
    apps = {}
    for app_name in all_time_df['VALUE'].unique():
        if app_name == 's0':
            continue
        app = all_time_df[all_time_df['VALUE'] == app_name]
        #print(app)
        app = app.reset_index()
        zero_data = np.zeros(shape=(len(all_index),len(all_columns)))
        app_df = pd.DataFrame(zero_data, index = all_index, columns = all_columns)
        for x in range(len(app)):
            timestamp = app.loc[x]["MEASUREMENT_TIME"]
            usage = app.loc[x]['Time_Used']
            end_timestamp = timestamp + datetime.timedelta(seconds = usage)
            start_hour = app.loc[x]['Hour']
            end_hour = int(end_timestamp.strftime("%H"))
            date = app.loc[x]['Date']
            if start_hour == end_hour:
                app_df.loc[date][start_hour] += usage
                app_df.loc[date]['Total_Usage'] += usage
            else:
                remaining_seconds = 3600 - (app.loc[x]['Minute'] * 60 + int(timestamp.strftime("%S")))
                app_df.loc[date][start_hour] += remaining_seconds
                app_df.loc[date]['Total_Usage'] += remaining_seconds
                overflow = usage - remaining_seconds
                curr_hour = start_hour + 1
                while overflow > 3600:
                    if curr_hour > 23:
                        date = app.loc[x]["MEASUREMENT_TIME"] + datetime.timedelta(days = 1)
                        date = date.strftime("%Y-%m-%d")
                        curr_hour = 0
                    app_df.loc[date][curr_hour] += 3600
                    app_df.loc[date]['Total_Usage'] += 3600
                    curr_hour = curr_hour + 1
                    overflow -= 3600

                if curr_hour > 23:
                    date = app.loc[x]["MEASUREMENT_TIME"] + datetime.timedelta(days = 1)
                    date = date.strftime("%Y-%m-%d")
                    curr_hour = 0
                app_df.loc[date][curr_hour] += overflow
                app_df.loc[date]['Total_Usage'] += overflow
                
        apps[app_name] = app_df.reset_index(drop = True)

    return apps