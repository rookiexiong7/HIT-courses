import numpy as np

# 计算RMS误差函数

def root_mean_square(given_function, fitted_function):

    return np.sqrt(np.mean((given_function - fitted_function) ** 2))

def sin_sin(t, a, w1, b, w2):

    return a * np.sin(w1 * t) + b * np.sin(w2 * t)

def sin_cos(t, a, w1, b, w2):
    return a * np.sin(w1 * t) + b * np.cos(w2 * t)