import numpy as np
import matplotlib.pyplot as plt
from sklearn.linear_model import RANSACRegressor

from function_tools import root_mean_square as rms

# 绘制结果
def basic_visualize(x, y, x_n, y_m):
    plt.scatter(x_n, y_m, color='b', label='Points with Noise')
    plt.plot(x, y, 'r', label='Original Line')
    plt.xlabel('x')
    plt.ylabel('y')

def func_visualize(x, k, b, string):
    plt.plot(x, k * x + b, color='darkorange', label= string + ' Fitted line')
    plt.legend()
    plt.title(string + ' Line Fitting')
    plt.show()

    print("k_fit =", k)
    print("b_fit =", b)

# 最小二乘拟合
def least_sqaure(x, y):
    A = np.vstack([x, np.ones(num_points)]).T
    k_fit, b_fit = np.linalg.lstsq(A, y, rcond=None)[0]

    print("最小二乘拟合直线的参数:")
    func_visualize(x, k_fit, b_fit, 'Least Squares')

    return k_fit * x + b_fit

# RANSAC拟合
def ransac(x,y):
    X = x.reshape(-1, 1)
    ransac = RANSACRegressor()
    ransac.fit(X, y)

    k_fit, b_fit = ransac.estimator_.coef_[0], ransac.estimator_.intercept_

    print("ransac拟合直线的参数:")
    func_visualize(x, k_fit, b_fit, 'RANSAC')
    return k_fit * x + b_fit

# 随机抽样数据
def generate_data(a, b, c, num_points, noise):
    x_oringal = np.linspace(-10, 10, num_points)
    y_oringal = -(a * x_oringal + c) / b
    # 添加噪声
    noise_n = np.random.normal(0, noise, num_points)
    noise_m = np.random.normal(0, noise, num_points)
    x_n = x_oringal + noise_n
    y_m = y_oringal + noise_m

    return x_oringal,y_oringal,x_n,y_m


num_points = 100

def main():
    a_given, b_given, c_given, noise = 2.0, -1.0, -6.0, 1
    x, y, x_n, y_m = generate_data(a_given, b_given, c_given, num_points, noise)
    y_right = a_given * x_n + c_given

    basic_visualize(x, y, x_n, y_m)
    y_predict = least_sqaure(x_n, y_m)
    print(f"RMS误差评价为：{rms(y_right, y_predict)}")
    print("")
    basic_visualize(x, y, x_n, y_m)
    y_predict = ransac(x_n, y_m)
    print(f"RMS误差评价为：{rms(y_right, y_predict)}")


if __name__ == "__main__":
    main()
