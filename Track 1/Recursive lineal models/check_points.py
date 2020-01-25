import pandas as pd
import numpy as np
import random
import os
np.random.seed(42)

def score(satellite_predicted_values, satellite_true_values): 
    # the division, addition and subtraction are pointwise 
    smape =  np.mean(np.abs((satellite_predicted_values - satellite_true_values) 
        / (np.abs(satellite_predicted_values) + np.abs(satellite_true_values))))
    return 100 * (1 - smape)


raw = pd.read_csv("train.csv")[['x_sim', 'y_sim', 'z_sim', 'Vx_sim', 'Vy_sim', 'Vz_sim']].values
real = pd.read_csv("train.csv")[['x', 'y', 'z', 'Vx', 'Vy', 'Vz']].values
improved = pd.read_csv('submission.csv')[['x', 'y', 'z', 'Vx', 'Vy', 'Vz']].values

print(score(raw, real))
print(score(improved, real))