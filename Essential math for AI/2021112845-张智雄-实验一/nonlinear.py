import numpy as np
import matplotlib.pyplot as plt
from sklearn.linear_model import LinearRegression, RANSACRegressor
from sklearn.neural_network import MLPRegressor

from function_tools import sin_cos

# 绘制结果
def basic_visualize(t,y,y_n):
    plt.scatter(t, y_n, color='r', label='Points with Noise')
    plt.plot(t, y, 'b', label='Original Function')
    plt.xlabel('t')
    plt.ylabel('y')

def func_visualize(t, y, string):
    plt.plot(t, y, color='darkorange', label=string + ' Fitted Function')
    plt.legend()
    plt.title(string + ' Function Fitting')
    plt.show()

# 最小二乘法拟合
def least_squares(t, y, degree):
    X = np.vander(t, degree + 1, increasing=True)
    model = LinearRegression()
    model.fit(X, y)
    y_fit = model.predict(X)

    func_visualize(t, y_fit, 'Least Squares')

    return X, model

# RANSAC拟合
def ransac(t, y, degree):
    X = np.vander(t, degree + 1, increasing=True)
    model = RANSACRegressor(LinearRegression())
    model.fit(X, y)
    inlier_mask = model.inlier_mask_
    y_fit = model.predict(X)

    func_visualize(t, y_fit, 'RANSAC')

    return X, model

# 神经网络拟合
def neural_network(t, y):
    # 定义网络结构
    hidden_layers = (100, 100, 100, 100, 100, 100, 100, 100, 100, 100)
    model = MLPRegressor(hidden_layer_sizes=hidden_layers, activation='relu', alpha=0.002)
    model.fit(t.reshape(-1, 1), y)
    y_fit = model.predict(t.reshape(-1, 1))

    func_visualize(t, y_fit, 'Neural Network')

    return model

# 随机抽样数据
def generate_data(a, w1, b, w2, num_points, noise):
    t = np.linspace(0, 10, num_points)
    y_oringal = sin_cos(t, a, w1, b, w2)
    # 添加噪声
    noise = np.random.normal(0, noise, num_points)
    y_n = y_oringal + noise

    return t, y_oringal, y_n


num_points = 100  # 数据点数量
noise = 0.1 # 噪声水平

def main():
    a, w1, b, w2 = 1.0, 1.0, 0.5, 2.0
    t, y, y_n = generate_data(a, w1, b, w2, num_points, noise)
    degree = 10
    basic_visualize(t, y, y_n)
    ransac(t, y_n, degree+4)
    basic_visualize(t, y, y_n)
    least_squares(t, y_n, degree)
    basic_visualize(t, y, y_n)
    neural_network(t, y_n)


if __name__ == "__main__":
    main()