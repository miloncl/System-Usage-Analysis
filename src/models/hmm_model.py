import pandas as pd
import numpy as np

from sklearn.model_selection import train_test_split
from collections import defaultdict

def hmm_model(df):
    """HMM Model with Transition Matrix"""

    def get_clean_data(df):
        """Get only the column containing the exe files"""
        return df[df['ID_INPUT'] == 4].reset_index(drop = True)

    def get_all_pairs(df):
        """Get pairs of exe files happening next to each other"""
        pairs = [('S0', df.iloc[0]['VALUE'])] # initial pair would be (delimiter, first exe)
        #pairs = []
        for index in range(len(df) - 1):
            pair = (df.iloc[index]['VALUE'], df.iloc[index+1]['VALUE'])
            pairs.append(pair)       
        return pairs

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
    
    def save_to_file(matrix, outfile_name, outfile_ext = 'csv'):
        """Save matrices to files after finding them"""
        out = outfile_name + '.' + outfile_ext
        matrix.to_csv(out, index=True)
    
    matrix, accuracy = predict_HMM(df, n=1, rand_state=20)
    save_to_file(matrix, "outputs/HMM/transition_mt_checkpoint", outfile_ext = 'txt')
    print(accuracy)