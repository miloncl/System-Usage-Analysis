import pandas as pd
import numpy as np

from sklearn.model_selection import train_test_split
from collections import defaultdict

### Initial Setup For Evaluation Purposes ###
def split_train_test(pairs, state):
        """Split train/test sets by a ratio of 80/20"""
        X = [x[0] for x in pairs] # x[0] is ~ the "current" exe file
        y = [x[1] for x in pairs] # x[1] is the "next" exe file
        X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=state)
        return [X_train, y_train, X_test, y_test]

def get_pair_frequency(X, y):
        """Get the frequency of the pairs of exe files that happen next to each other"""
        pair_freq = defaultdict(int)
        for index in range(len(X)):
            pair = (X[index], y[index])
            pair_freq[pair] += 1
        return pair_freq

def get_n_next_app(n, matrix, app):
        """ Find n next apps whose immediate previous app is "app" """
        matrix = matrix.T
        if app in matrix.columns:
            top_vals = matrix.nlargest(n, app).index # nlargest: Return the first n rows ordered by columns in descending order.
            return top_vals
        else:
            return ['chrome.exe'] # if app not in matrix return the most common app

def get_accuracy(X, y, matrix, n):
    """Accuracy of the HMM model"""
    preds = []
    for i in range(len(X)):
        pred = get_n_next_app(n, matrix, X[i])
        if y[i] in pred:
            preds.append(1)
        else:
            preds.append(0)        
    return sum(preds)/ len(preds)

def save_to_file(matrix, outfile_name, outfile_ext = 'csv'):
    """Save matrices to files after finding them"""
    out = outfile_name + '.' + outfile_ext
    matrix.to_csv(out, index=True)

### HMM Model ###
def hmm_transition_mt(df, n=1, save_output=True):
    """HMM Model with Transition Matrix"""

    def get_clean_data(df):
        """Get only the column containing the exe files"""
        return df[df['ID_INPUT'] == 4].reset_index(drop = True)

    def get_all_pairs(df):
        """Get pairs of exe files happening next to each other"""
        #pairs = [('s0', df.iloc[1]['VALUE'])] # initial pair would be (delimiter, first exe)
        pairs = []
        for index in range(len(df) - 1):
            pair = (df.iloc[index]['VALUE'], df.iloc[index+1]['VALUE'])
            pairs.append(pair)       
        return pairs

    def get_transition_probability(pair_freq, X):
        """Get the transition probability, for ex: from chrome.exe --> cmd.exe,
        P(cmd.exe | chrome.exe) = P(chrome.exe, cmd.exe) / P(chrome.exe)
                                = (# chrome.exe and cmd.exe) / (# all occurrences of chrome.exe)
                                = (pair occurrences) / (# all occurrences of chrome.exe)
        """
        transition_prob = defaultdict(int)
        for pair in pair_freq:
            total_occ = sum([x == pair[0] for x in X])
            transition_prob[pair] += pair_freq[pair] / total_occ
        return transition_prob
    
    def get_unique_states(X):
        """Get the unique executables (i.e. the "states" in HMM)"""
        return np.unique(X)
    
    def get_transition_matrix(trans_prob, X):
        """Create the transition matrix"""
        all_exes = get_unique_states(X)
        probs_for_matrix = []
        for row in all_exes:
            exe_probs = []
            for col in all_exes:
                pair = (row, col)
                if pair in trans_prob:
                    exe_probs.append(trans_prob[pair])
                else:
                    exe_probs.append(0)
            probs_for_matrix.append(exe_probs)
        
        matrix = pd.DataFrame(probs_for_matrix, index = all_exes, columns = all_exes)
        return matrix

    def predict_HMM(df, n, rand_state):
        """Put everything together for the HMM model"""
        df = get_clean_data(df)
        all_pairs = get_all_pairs(df)
        
        X_tr, y_tr, X_test, y_test = split_train_test(all_pairs, rand_state)
        pair_freq = get_pair_frequency(X_tr, y_tr)
        transition_prob = get_transition_probability(pair_freq, X_tr)
        transition_matrix = get_transition_matrix(transition_prob, X_tr)
        
        accuracy = get_accuracy(X_test, y_test, transition_matrix, n)
        return [transition_matrix, accuracy]
    
    matrix, accuracy = predict_HMM(df, n=n, rand_state=20)
    if save_output:
        save_to_file(matrix, "outputs/HMM/transition_mt_checkpoint", outfile_ext = 'txt')
    print(accuracy)
    return matrix, accuracy

def hmm_emission_mt(df, n=1, save_output=True):
    """HMM Emission MT"""
    ### First, we process the data ###
    def get_clean_data_for_tabs(df):
        """Clean the tab names by removing PIIs"""
        # Get data and clean Missing Strings
        exes = df[df['ID_INPUT'] == 4]['VALUE'].reset_index()['VALUE']
        df = df[df['ID_INPUT'] == 3].reset_index()
        lst = df['VALUE'].apply(lambda r: "File Explorer" if (r == "Missing String." or pd.isnull(r)) else r).tolist()

        arr = []
        indx = []
        # Find list items that are associated w/ Google Chrome
        for item in lst:
            if ("Google Chrome" in item) or ("google chrome" in item):
                arr.append(item)
                indx.append(lst.index(item))
        
        twos = [] # list of 2 items in a tab name (ex: 'Online C Compiler - Google Chrome')
        threes = [] # list of 3 items in a tab name (ex: 'Process and EDA - Jupyter Notebook - Google Chrome')
        fours = [] # list of 4 items in a tab name (ex: 'Dsc 180B - Quarter 2 Week 2 - Google Slides - Google Chrome')
        fives = [] # list of 5 items in a tab name (ex: currently none)
        sixes = [] # list of 6 items in a tab name 
        # (ex: 'DSC 140A - Probabilistic Modeling and ML - LE [A00] - Course Podcasts - UC San Diego - Google Chrome')
        # Find lists of k items in tab names
        for item in arr:
            x = item.split("-")
            if len(x) == 2:
                twos.append(item)
            elif(len(x) == 3):
                threes.append(item)
            elif(len(x) == 4):
                fours.append(item)
            elif(len(x) == 5):
                fives.append(item)
            else:
                sixes.append(item)

        splits = []
        # Conduct the splits
        for item in arr:
            x = item.split('-')
            if(len(x) == 1):
                splits.append(item)
            elif(len(x) in [2,3,4]):
                splits.append(x[len(x)-2] + "-" + x[len(x)-1])
            else:
                splits.append(x[len(x)-3] + "-" + x[len(x)-2] + "-" + x[len(x)-1])
        

        changed_items = []
        # Get all the items with their processed names
        for item in splits:
            changed_items.append(item.strip())

        x = []
        count = 0
        # Apply the changes into the real dataframe column
        for item in lst:
            if ("Google Chrome" in item) or ("google chrome" in item):
                x.append(changed_items[count])
                count = count + 1
            else:
                x.append(item)
        
        df = df.assign(VALUE = x, exes = exes)

        return df

    def get_clean_data_for_emission(df, date = "2023-01-19", activate_date = False):
        df = df.assign(date = df["MEASUREMENT_TIME"].astype(str).apply(lambda x: x[:10])) # extract only the date

        def preproc_before_emission(df, date = "2023-01-19", activate_date = False):
            """Get the series of executables and the apps"""
            if activate_date:
                tmp_df = df[df["date"] == date]
            else:
                tmp_df = df
            executables = tmp_df['exes'].reset_index()['exes']
            apps = tmp_df['VALUE'].reset_index()['VALUE']
            return (executables, apps)

        return preproc_before_emission(df, date = "2023-01-19", activate_date = False)

    # Get processed data for user
    df_processed = get_clean_data_for_tabs(df)
    executables, apps = get_clean_data_for_emission(df_processed, date = "2023-01-19", activate_date = False)

    def find_exe_prob(executables, exe_name):
        """Find the probabilities of executable files
        Ex: P(chrome.exe) = (#chrome.exe) / (all exe's)"""
        numerator = sum(executables == exe_name)
        denominator = len(executables)
        return numerator / denominator

    co_appears = []
    def find_joint_prob(executables, apps, from_exe, to_app):
        """Find the probability of the pair occurrence bw the executable file and the app
        Ex: P(A,B) = (# times we found pair A and B) / (# all entries)"""
        fromExe_indices = np.where(executables == from_exe)[0]
        toApp_indices = np.where(apps == to_app)[0]
        co_appears.append([fromExe_indices, toApp_indices])
        co_appear = len(set(fromExe_indices + 1) & set(toApp_indices)) # these are indices where "to_app" appears after "from_exe"
        return co_appear / len(executables)

    def find_emission_prob(executables, apps, from_exe, to_app):
        """Find the emission probability
        P(to_app | from_exe) = P(from_exe, to_app) / P(from_exe)"""
        emission_numer = find_joint_prob(executables, apps, from_exe, to_app)
        emission_denom = find_exe_prob(executables, from_exe) 
        return emission_numer / emission_denom

    def emission_dict(executables, apps):
        """Find a dictionary of emission probabilities"""
        unique_exes = executables.unique()
        unique_apps = apps.unique()
        emission_prob = {}
        for ex in unique_exes:
            emission_prob[ex] = {}
            for app in unique_apps:
                emission_prob[ex][app] = find_emission_prob(executables, apps, ex, app)
        return emission_prob

    def emission_mt(executables, apps):
        """Find the emission matrix"""
        emission_prob = emission_dict(executables, apps)
        emission_matrix = pd.DataFrame.from_dict(emission_prob)
        return (emission_prob, emission_matrix.T)

    emission_prob, emission_matrix = emission_mt(executables, apps)

    if save_output:
        save_to_file(emission_matrix, "outputs/HMM/emission_mt_checkpoint", outfile_ext = 'txt')

    #X_tr, y_tr, X_test, y_test = split_train_test(co_appears, 20)
    #accuracy = get_accuracy(X_test, y_test, emission_matrix, n)
    #print(accuracy)

    return emission_prob, emission_matrix
