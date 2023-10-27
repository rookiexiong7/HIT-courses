import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
from sklearn.linear_model import Ridge
from sklearn.linear_model import RANSACRegressor, HuberRegressor

from function_tools import sin_sin, sin_cos
from robust_test_nonlinear import generate_data


# 绘制结果
def basic_visualize(t, y, y_n, outpoint_index):
    plt.scatter(t, y_n, color='r', label='Points with Noise')
    plt.scatter(t[outpoint_index], y_n[outpoint_index], color='black', label='OutPoints')
    plt.plot(t, y, 'b', label='Original Function')
    plt.xlabel('t')
    plt.ylabel('y')

def func_visualize(t, y, string):
    plt.plot(t, y, color='darkorange', label=string + ' Fitted Function')
    plt.legend()
    plt.title(string + ' Function Optimize Fitting')
    plt.show()


# 曲线拟合
def curve(t, y):
    params, params_covariance = curve_fit(sin_cos, t, y, p0=[1,1,0.5,2])
    y_fit = sin_cos(t, params[0], params[1], params[2], params[3])
    func_visualize(t, y_fit, 'Curve')

    return y_fit

# 岭回归拟合
def ridge(t, y):
    degree = 12
    X = np.vander(t, degree + 1, increasing=True)

    alpha_ridge = 0.01  # 正则化参数
    model_ridge = Ridge(alpha=alpha_ridge)
    model_ridge.fit(X, y)
    y_fit = model_ridge.predict(X)
    func_visualize(t, y_fit, 'Ridge Regression')

    return X, model_ridge

num_points = 100  # 数据点数量
noise = 0.1 # 噪声水平

def main():
    a, w1, b, w2 = 1.0, 1.0, 0.5, 2.0
    t, y, y_n, outpoint_index = generate_data(a, w1, b, w2, num_points, noise)

    basic_visualize(t, y, y_n, outpoint_index)
    ridge(t, y_n)
    basic_visualize(t, y, y_n, outpoint_index)
    curve(t, y_n)




if __name__ == "__main__":
    main()
