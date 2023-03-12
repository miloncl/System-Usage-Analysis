from datetime import timedelta
import numpy as np
import pandas as pd

def rnn_feats(all_time_df, save_chrome_time=False, 
              save_all_apps_time=False, save_top_5_app_time=False, save_enum_app_time=False):
    all_time_df['Minute'] = all_time_df['MEASUREMENT_TIME'].dt.minute
    all_time_df['Day'] = all_time_df['MEASUREMENT_TIME'].dt.day
    all_time_df['Month'] = all_time_df['MEASUREMENT_TIME'].dt.month
    all_time_df['Day_Week'] = all_time_df['MEASUREMENT_TIME'].dt.day_name()
    all_time_df = all_time_df.assign(Week_Year = all_time_df['MEASUREMENT_TIME'].apply(lambda x: x.strftime("%W")))
    all_time_df = all_time_df.assign(Week_Day = all_time_df['Week_Year'] + '_' + all_time_df['Day_Week'])

    # table for subtask 1
    time_per_week_day = all_time_df.copy()
    time_per_week_day['Time_Used'] = time_per_week_day['Time_Used'] / 60 / 60
    time_per_week_day = time_per_week_day.groupby("Week_Day").apply(lambda x: x.groupby("VALUE")["Time_Used"].sum())
    time_per_week_day = pd.DataFrame(time_per_week_day).reset_index(level=1, inplace=False)
    df_pivot = time_per_week_day.pivot_table(index = 'Week_Day', columns='VALUE', values='Time_Used').fillna(0)

    # table for subtask 2
    def date_range(start, end):
        dates = []
        delta = end - start
        for i in range(delta.days + 2):
            date = start + timedelta(days=i)
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
        app = app.reset_index()
        zero_data = np.zeros(shape=(len(all_index),len(all_columns)))
        app_df = pd.DataFrame(zero_data, index = all_index, columns = all_columns)
        for x in range(len(app)):
            timestamp = app.loc[x]["MEASUREMENT_TIME"]
            usage = app.loc[x]['Time_Used']
            end_timestamp = timestamp + timedelta(seconds = usage)
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
                        date = app.loc[x]["MEASUREMENT_TIME"] + timedelta(days = 1)
                        date = date.strftime("%Y-%m-%d")
                        curr_hour = 0
                    app_df.loc[date][curr_hour] += 3600
                    app_df.loc[date]['Total_Usage'] += 3600
                    curr_hour = curr_hour + 1
                    overflow -= 3600

                if curr_hour > 23:
                    date = app.loc[x]["MEASUREMENT_TIME"] + timedelta(days = 1)
                    date = date.strftime("%Y-%m-%d")
                    curr_hour = 0
                app_df.loc[date][curr_hour] += overflow
                app_df.loc[date]['Total_Usage'] += overflow
                
        apps[app_name] = app_df.reset_index(drop = True)

        if save_chrome_time:
            apps['chrome.exe'].to_csv('../data/out/chrome_time.csv')
        
        time_spent = all_time_df.groupby('VALUE')['Time_Used'].sum().sort_values(ascending = False)
        top_5 = time_spent[:5].index
        app_dfs = []
        top_5_app_dfs = []
        enumerated_app_dfs = []
        app_dict = {app_name: index for index, app_name in enumerate(apps.keys())}
        for app_name, df in apps.items():
            df['Application'] = app_name
            app_dfs.append(df)
            if app_name in top_5:
                top_5_app_dfs.append(df)
            df['Application'] = app_dict[app_name]
            enumerated_app_dfs.append(df)
            
            
        combined_df = pd.concat(app_dfs)
        top_5_combined_df = pd.concat(top_5_app_dfs)
        enumerated_combined_df = pd.concat(enumerated_app_dfs)

        all_app_df = pd.get_dummies(combined_df, columns = ['Application'])
        top_5_app_df = pd.get_dummies(top_5_combined_df, columns = ['Application'])

        all_app_df = all_app_df.sort_index().reset_index(drop = True)
        if save_all_apps_time:
            all_app_df.to_csv('../data/out/all_app_time.csv')
        
        top_5_app_df = top_5_app_df.sort_index().reset_index(drop = True)
        if save_top_5_app_time:
            top_5_app_df.to_csv('../data/out/top_5_app_time.csv')

        enumerated_combined_df = enumerated_combined_df.sort_index().reset_index(drop = True)
        if save_enum_app_time:
            enumerated_combined_df.to_csv('../data/out/enumerated_app_time.csv')
        
        return df_pivot, apps['chrome.exe'], all_app_df, top_5_app_df, enumerated_combined_df