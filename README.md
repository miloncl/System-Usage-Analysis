# Intel & UCSD HDSI -- Data Science Capstone Project -- 2022-2023

## Introduction
- Hello everyone, we are Thy Nguyen, Milon Chakkalakal, and Pranav Thaenraj
- Our project mentors are Jamel Tayeb, Bijan Arbab, Scruti Sahani, Oumaima Makhlouk, Praveen Polasam, and Chansik Im 
- Our capstone project is "Discover User-App Interactions and Solutions to Reducing the Initial User-CPU Latency"
- This is our Github repo including all the source codes and data files to do the analysis

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
├── references         <- Data dictionaries, explanatory materials.
├── requirements.txt   <- For reproducing the analysis environment, e.g.
│                         generated with `pip freeze > requirements.txt`
├── src                <- Source code for use in this project.
│   ├── data           <- Scripts to download or generate data.
│   │   └── make_dataset.py
│   ├── features       <- Scripts to turn raw data into features for modeling.
│   │   └── build_features.py
│   ├── models         <- Scripts to train models and make predictions.
│   │   ├── predict_model.py
│   │   └── train_model.py
│   └── visualization  <- Scripts to create exploratory and results-oriented viz.
│       └── visualize.py
├── config
│   ├── data-params.json <- Save the inputs for the function calls
│   └── submission.json <- GitHub repo and Docker image links
│
├── .ipynb_checkpoints <- temporary notebooks containing work in progress

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
- Source Code: <a href="https://github.com/miloncl/System-Usage-Analysis/blob/main/notebooks/HMM.ipynb">HMM.ipynb</a>
- Outputs: <a href="https://github.com/miloncl/System-Usage-Analysis/tree/main/outputs/HMM">HMM outfiles</a>

### Week 4:
- Presentation: <a href="https://github.com/miloncl/System-Usage-Analysis/blob/main/references/weekly_presentation/%5BDSC%20180B%5D%20-%20Quarter%202%20Week%204.pdf">Study LSTM: Data Prep, Data Viz, Research on RNN/LSTM</a>
- Source Code: <a href="https://github.com/miloncl/System-Usage-Analysis/blob/main/notebooks/Process%20and%20EDA.ipynb"> Process_and_EDA.ipynb</a>

### Week 5:
- Presentation: <a href="https://github.com/miloncl/System-Usage-Analysis/blob/main/references/weekly_presentation/%5BDSC%20180B%5D%20-%20Quarter%202%20Week%205.pdf">RNN (Vanilla + LSTM)</a>
- Source Code:

### Week 6:
- Presentation: <a href="https://github.com/miloncl/System-Usage-Analysis/blob/main/references/weekly_presentation/%5BDSC%20180B%5D%20%20-%20Quarter%202%20Week%206.pdf">LSTM Experiments</a>
- Source Code:
