# Intel & UCSD HDSI -- Data Science Capstone Project -- 2022-2023

## Introduction
- Hello everyone, we are Thy Nguyen, Milon Chakkalakal, and Pranav Thaenraj from UC San Diego
- Our advisors are Jamel Tayeb, Bijan Arbab, Scruti Sahani, Oumaima Makhlouk, Praveen Polasam, and Chansik Im from Intel
- This is our Github repo including all the source codes and data files to do data collection and analysis for our capstone project _"Discover User-App Interactions and Solutions to Reducing the Initial User-CPU Latency"_

## Overview
- We try to closely follow the template for Data Science projects by <a href="https://drivendata.github.io/cookiecutter-data-science/">Cookie Cutter Data Sciece </a>
- Please check out the below template to understand how to navigate our repo
```
Project
├── .gitignore         <- Files to keep out of version control (e.g. data/binaries).
├── run.py             <- run.py with calls to functions in src.
├── README.md          <- The top-level README for developers using this project.
├── data
│   ├── temp           <- Intermediate data that has been transformed.
│   ├── out            <- The final, canonical data sets for modeling.
│   └── raw            <- The original, immutable data dump.
├── notebooks          <- Jupyter notebooks (presentation only).
|   ├── Process and EDA.ipynb
|   ├── HMM.ipynb
|   └── LSTM_RNN.ipynb
├── references         <- Data dictionaries, explanatory materials.
├── requirements.txt   <- For reproducing the analysis environment, 
├── src                <- Source code for use in this project.
│   ├── data           <- Scripts to download or generate data.
│   │   ├── make_dataset.py
│   │   └── foreground
|   |       ├── foreground.c
|   |       └── foreground.h 
│   ├── features       <- Scripts to turn raw data into features for modeling.
│   │   └── build_features.py
│   ├── models         <- Scripts to train models and make predictions.
│   │   ├── hmm_model.py
│   │   └── lstm_model.py
│   └── visualization  <- Scripts to create exploratory and results-oriented viz.
│       └── visualize.py
├── outputs 
|   └── HMM           <- the output results of the HMM model (LSTM/RNN model results are inside the notebook)
│       └── emission_mt_user1.txt
|       ├── emission_mt_user2.txt
|       ├── transition_mt_user1_top15apps.txt
|       ├── transition_mt_user1_top1app.txt
|       ├── transition_mt_user2_top15apps.txt
|       └── transition_mt_user2_top1app.txt
└── config
    ├── data-params.json <- Save the inputs for the function calls
    └── submission.json <- GitHub repo and Docker image links

```

## Instruction to Run the code
- For the Methodology Staff
    
    On DSMLP,
    1. Cloning our GitHub repository.
    2. Launching a container with your Docker image.
    3. Running ```python run.py test```.

## Specific Links to Presentations and Source Code

### Week 1: 
- Introduction
### Week 2:
- Presentation: <a href="https://github.com/miloncl/System-Usage-Analysis/blob/main/references/weekly_presentation/%5BDSC%20180B%5D%20-%20Quarter%202%20Week%202.pdf">Evaluate Data Quality and Conduct EDA</a>
- Source Code: <a href="https://github.com/miloncl/System-Usage-Analysis/blob/main/notebooks/Process%20and%20EDA.ipynb">Process_and_EDA.ipynb</a>

### Week 3:
- Presentation: <a href="https://github.com/miloncl/System-Usage-Analysis/blob/main/references/weekly_presentation/%5BDSC%20180B%5D%20-%20Quarter%202%20Week%203.pdf">HMM: Transition Matrix, Model Accuracy, and Emission Matrix</a>
- Source Code: <a href="https://github.com/miloncl/System-Usage-Analysis/blob/main/notebooks/HMM.ipynb">HMM.ipynb</a>,  <a href=https://github.com/miloncl/System-Usage-Analysis/blob/main/src/models/hmm_model.py>hmm_model.py</a>
- Outputs: <a href="https://github.com/miloncl/System-Usage-Analysis/tree/main/outputs/HMM">HMM outfiles</a>

### Week 4:
- Presentation: <a href="https://github.com/miloncl/System-Usage-Analysis/blob/main/references/weekly_presentation/%5BDSC%20180B%5D%20-%20Quarter%202%20Week%204.pdf">Study LSTM: Data Prep, Data Viz, Research on RNN/LSTM</a>
- Source Code: <a href="https://github.com/miloncl/System-Usage-Analysis/blob/main/notebooks/Process%20and%20EDA.ipynb"> Process_and_EDA.ipynb</a>

### Week 5:
- Presentation: <a href="https://github.com/miloncl/System-Usage-Analysis/blob/main/references/weekly_presentation/%5BDSC%20180B%5D%20-%20Quarter%202%20Week%205.pdf">RNN (Vanilla + LSTM)</a>
- Source Code: <a href="https://github.com/miloncl/System-Usage-Analysis/blob/main/notebooks/LSTM_RNN.ipynb">LSTM_RNN.ipynb</a>,  <a href=https://github.com/miloncl/System-Usage-Analysis/blob/main/src/models/lstm_model.py>lstm_model.py</a>

### Week 6:
- Presentation: <a href="https://github.com/miloncl/System-Usage-Analysis/blob/main/references/weekly_presentation/%5BDSC%20180B%5D%20%20-%20Quarter%202%20Week%206.pdf">LSTM Experiments</a>
- Source Code: <a href="https://github.com/miloncl/System-Usage-Analysis/blob/main/notebooks/LSTM_RNN.ipynb">LSTM_RNN.ipynb</a>, <a href=https://github.com/miloncl/System-Usage-Analysis/blob/main/src/models/lstm_model.py>lstm_model.py</a>

### Week 7-9:
- Project Poster: 
- Practice presentation and elevator pitch in class
