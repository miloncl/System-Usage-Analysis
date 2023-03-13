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
from keras.layers import Dense, LSTM
from sklearn.preprocessing import MinMaxScaler
from sklearn.metrics import mean_squared_error
from keras.layers import Bidirectional
import tensorflow as tf

# for plots
import matplotlib.pyplot as plt
import seaborn as sns

# others
import warnings
warnings.filterwarnings("ignore")


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

