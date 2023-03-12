import pandas as pd
import matplotlib as plt

def basic_stats(df, schema, all_time_df):
    """Basic Statistics for the df"""
    print("========================================EDA========================================")
    print("Schema\n", schema[['ID_INPUT', 'INPUT_NAME', 'INPUT_DESCRIPTION']])
    print('\nCheck Data Types\n', df.dtypes)

    df = df.sort_values(by = ['MEASUREMENT_TIME'], ignore_index = True)
    print("\nCurrently, the data we have is collected from", df['MEASUREMENT_TIME'].min(), "to", df['MEASUREMENT_TIME'].max())
    print("Number of rows in total: " + str(df.shape[0]))
    print("Number of unique data points (based on the unique mouse clicks): " + str(df.shape[0] / 12) )

    unique_apps = df[df.ID_INPUT == 4].VALUE.unique()
    print("\nSome of the unique apps", unique_apps[:5])
    print("There are", len(unique_apps), "in total")

    total_times = all_time_df.groupby('VALUE')['Time_Used'].sum().sort_values(ascending = False)
    print("\nSome Total Usage Time\n", total_times[:5])

def some_viz(df, all_time_df):
    """Some visualizations"""
    # first graph
    plot = df[df['ID_INPUT'] == 4]['VALUE'].value_counts()[:5].plot.barh(x = 'index', y = 'VALUE')
    plot.set_axisbelow(True)
    plot.set_xlabel('Number of Times in Foreground')
    plot.set_ylabel('Executable')
    plot.set_title('Frequency of Executables Corresponding to the Top Used Apps')
    plot.invert_yaxis()
    plt.pyplot.grid(axis = 'x')
    plt.pyplot.show()

    # second graph
    print("Apps vs Time Used")
    total_times = all_time_df.groupby('VALUE')['Time_Used'].sum().sort_values(ascending = False)
    top_10_apps = all_time_df[all_time_df['VALUE'].isin(total_times[:10].index)]
    top_10_apps.plot.scatter(y = 'VALUE', x = 'Time_Used')

    # third graph
    all_time_df['Hour'] = all_time_df['MEASUREMENT_TIME'].dt.hour
    time_spent = all_time_df.groupby('VALUE')['Time_Used'].sum().sort_values(ascending = False)
    print("Plot top 5 apps with the most time spent on for the whole DataFrame")
    plot = time_spent[:5].plot.barh(color = '#3D8C40')
    plot.set_axisbelow(True)
    plot.set_xlabel('Use Time (in hours)')
    plot.set_ylabel('Application')
    plot.set_title('Top Used Apps')
    plot.invert_yaxis()

    # fourth graph
    if False:
        print("Top 5 apps per day for the User")
        all_time_df= all_time_df.assign(Date = all_time_df['MEASUREMENT_TIME'].apply(lambda x: x.strftime("%Y-%m-%d")))
        time_per_date = all_time_df.groupby("Date").apply(lambda x: x.groupby("VALUE")["Time_Used"].sum()).sort_values(ascending = False)
        val = pd.DataFrame(time_per_date).reset_index(level=1, inplace=False)
        grouped = val.groupby("Date")

        for key, item in grouped:
            item.sort_values(by='Time_Used', ascending=False, inplace=True)
            item = item.head(5)
            plt.pyplot.barh(item['VALUE'], item['Time_Used'])
            plt.pyplot.title(f'{key}')
            plt.pyplot.show()
    
    # fifth graph
    all_time_df['Day'] = all_time_df['MEASUREMENT_TIME'].dt.day_name()
    time_per_day = all_time_df.groupby("Day").apply(lambda x: x.groupby("VALUE")["Time_Used"].sum()).sort_values(ascending = False)
    val = pd.DataFrame(time_per_day).reset_index(level=1, inplace=False)
    grouped = val.groupby("Day")

    for key, item in grouped:
        item.sort_values(by='Time_Used', ascending=False, inplace=True)
        item = item.head(5)
        plt.pyplot.barh(item['VALUE'], item['Time_Used'])
        plt.pyplot.title(f'{key}')
        plt.pyplot.show()

