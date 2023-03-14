# for data preprocessing
import os
import json
import sqlite3

import pandas as pd
import numpy as np
from collections import defaultdict

import datetime
import time
from datetime import date, timedelta

import sys

sys.path.append('../src/features')
from build_features import feats

# for LSTM/RNN
from sklearn.model_selection import train_test_split
from keras.models import Sequential
from keras.layers import Dense, LSTM, SimpleRNN
from sklearn.preprocessing import MinMaxScaler
from sklearn.metrics import mean_squared_error, mean_absolute_error, r2_score
from keras.layers import Bidirectional
import tensorflow as tf

# for plots
import matplotlib.pyplot as plt
import seaborn as sns

# others
import warnings
warnings.filterwarnings("ignore")


########## Codes for the first LSTM problem statement ##########
def predict(model, X_test, y_test, time_step=5, n_features=1, draw_graph=False):
    """Predict based on LSTM model"""
    predicted = []
    for x in X_test:
        x_input = np.array(x)
        x_input = x_input.reshape((1, time_step, n_features))
        yhat = model.predict(x_input, verbose=0)
        predicted.append(yhat)
    
    preds = []
    for i in predicted:
        preds.append(i[0][0])

    out_df = pd.DataFrame({
        'test_val': y_test,
        'test_pred': preds
    })    
    
    if draw_graph:
        sns.lineplot(out_df)

    return out_df

def test_acc(out_df, at_most = 180):
    mse = np.sqrt(mean_squared_error(out_df['test_val'], out_df['test_pred']))
    print('RMSE =', mse)

    indices = pd.Series(np.arange(out_df.shape[0]))
    out_df['acc'] = indices.apply(lambda x: 1 if abs(out_df['test_val'][x] - out_df['test_pred'][x]) <= at_most else 0)
    acc = (sum(out_df['acc']) / out_df.shape[0])*100
    print('ACC =', acc)

def bi_lstm(apps, app_name='chrome.exe', at_most=1):
    chrome = apps[app_name]
    time_step = 5
    n_features = 1
    X_train, X_test, y_train, y_test = feats(chrome, time_step=5, test_ratio=0.2)

    # define model
    model = Sequential()
    model.add(Bidirectional(LSTM(128, activation='relu'), input_shape=(time_step, n_features)))
    model.add(Dense(1, activation='relu'))
    model.compile(optimizer='adam', loss=tf.keras.losses.MeanAbsoluteError(
        reduction="auto", name="mean_absolute_error"
    ), metrics=['accuracy'])

    model.fit(X_train, y_train, epochs=200, verbose=0)
    
    out_df = predict(model, X_test, y_test, time_step=5, n_features=1)
    test_acc(out_df, at_most = at_most)

    return model, out_df, X_train, X_test, y_train, y_test

########## Codes for the second LSTM problem statement ##########
def load_data(filepath, model_type="enum"):
    # Load the data into a pandas DataFrame
    df = pd.read_csv(filepath)
    if model_type == "enum":
        df = df.drop(['Unnamed: 0'], axis = 1)

    scaler = MinMaxScaler()
    df['Total_Usage'] = scaler.fit_transform(df[['Total_Usage']])

    # Split the data into features (X) and target (y)
    X = df[['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12',
        '13', '14', '15', '16', '17', '18', '19', '20', '21', '22', '23',
            'Application']].values
    y = df.Total_Usage.values

    # Reshape the data for use in an RNN
    if model_type != "enum":
        X = X.reshape(X.shape[0], X.shape[1], 1)
    else:
        X = np.reshape(X, (X.shape[0], 1, X.shape[1]))

    return df, X, y

def vanilla_rnn(df, X, y):
    # Build the RNN model
    model = Sequential()
    model.add(SimpleRNN(60, input_shape=(X.shape[1], X.shape[2])))
    model.add(Dense(1, activation="relu"))
    model.add(Dense(1, activation="linear"))
    model.compile(loss="binary_crossentropy", optimizer="adam", metrics=["accuracy"])

    # Fit the RNN model to the data
    model.fit(X, y, epochs=10, batch_size=32)

    # Use the model to make predictions
    predictions = model.predict(X)

    # Add the predictions to the DataFrame as a new column named "PRED"
    df["PRED"] = predictions.ravel()

    return df

def pred_vs_true(df):
    """To visualize the pred and true vals"""
    x = list(df['Total_Usage'])
    y = list(df['PRED'])

    fig, ax = plt.subplots()
    bar_width = 0.35
    opacity = 0.8

    rects1 = ax.bar(x, y, bar_width,
                    alpha=opacity, color='b',
                    label='PRED')

    rects2 = ax.bar([i + bar_width for i in x], x, bar_width,
                    alpha=opacity, color='g',
                    label='True_val')

    ax.set_xlabel('True_val')
    ax.set_ylabel('PRED')
    ax.set_title('Comparison')
    ax.legend()

    plt.tight_layout()
    plt.show()

def binning(df):
    y_true = df['Total_Usage']
    y_pred = df['PRED']

    binned_true = []
    binned_pred = []
    for t in y_true:
        if(t <= .01):
            binned_true.append(1)
        elif(t >.01 and t <= .02):
            binned_true.append(2)
        elif(t > .02 and t<= .2):
            binned_true.append(3)
        else:
            binned_true.append(4)
    for t in y_pred:
        if(t <= .01):
            binned_pred.append(1)
        elif(t >.01 and t <= .02):
            binned_pred.append(2)
        elif(t > .02 and t<= .2):
            binned_pred.append(3)
        else:
            binned_pred.append(4)
    return binned_pred, binned_true

def calculate_metrics(y_true, y_pred):
    """ # Accuracy, tp, tn, fp, fn calcs """
    tp, tn, fp, fn = 0, 0, 0, 0
    for i in range(len(y_true)):
        if y_true[i] == y_pred[i]:
            if y_true[i] == 1:
                tp += 1
            else:
                tn += 1
        else:
            if y_true[i] == 1:
                fn += 1
            else:
                fp += 1
    accuracy = (tp + tn) / (tp + tn + fp + fn)
    return tp, tn, fp, fn, accuracy

def enumerated_lstm(df, X, y):
    """Enum LSTM"""
    # build the LSTM model
    model = Sequential()
    model.add(LSTM(16, input_shape=(X.shape[1], X.shape[2]), return_sequences=True))
    model.add(LSTM(16))
    model.add(Dense(64, activation='relu'))
    model.add(Dense(1))

    # compile the model
    model.compile(loss='mean_squared_error', optimizer='adam')

    # fit the model to the training data
    model.fit(X, y, epochs=100, batch_size=32, verbose=0)

    # make predictions using the model
    y_pred = model.predict(X)

    # add the predictions as a column in the dataframe
    df['PRED'] = y_pred.flatten()
    scaler = MinMaxScaler()
    df['PRED'] = scaler.fit_transform(df[['PRED']])
    return df
    

