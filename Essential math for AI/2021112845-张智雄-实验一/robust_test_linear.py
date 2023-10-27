import numpy as np
import matplotlib.pyplot as plt

from linear import least_sqaure, ransac
from function_tools import root_mean_square as rms

# 绘制结果
def basic_visualize(x, y, x_n, y_m, outpoint_index):
    plt.scatter(x_n, y_m, color='r', label='Points with Noise')
    plt.scatter(x_n[outpoint_index], y_m[outpoint_index], color='black', label='OutPoints')
    plt.plot(x, y, 'b', label='Original Line')
    plt.xlabel('x')
    plt.ylabel('y')

# 随机抽样数据
def generate_data(a, b, c, num_points, noise):
    x_oringal = np.linspace(-10, 10, num_points)
    y_oringal = -(a * x_oringal + c) / b
    # 添加噪声
    noise_n = np.random.normal(0, noise, num_points)
    noise_m = np.random.normal(0, noise, num_points)
    x_n = x_oringal + noise_n
    y_m = y_oringal + noise_m

    # 加入外点
    outpoint_index = np.random.choice(range(num_points), size=15, replace=False)
    outpoint_values = np.random.uniform(low=20, high=30, size=len(outpoint_index))
    y_m[outpoint_index] = outpoint_values

    return x_oringal,y_oringal,x_n,y_m,outpoint_index


num_points = 100

def main():
    a_given, b_given, c_given, noise = 2.0, -1.0, -6.0, 1
    x, y, x_n, y_m, outpoint_index = generate_data(a_given, b_given, c_given, num_points, noise)

    y_right = a_given * x_n + c_given

    basic_visualize(x, y, x_n, y_m, outpoint_index)
    y_predict = least_sqaure(x_n, y_m)
    print(f"RMS误差评价为：{rms(y_right, y_predict)}")
    print("")
    basic_visualize(x, y, x_n, y_m, outpoint_index)
    y_predict = ransac(x_n, y_m)
    print(f"RMS误差评价为：{rms(y_right, y_predict)}")


if __name__ == "__main__":
    main()



