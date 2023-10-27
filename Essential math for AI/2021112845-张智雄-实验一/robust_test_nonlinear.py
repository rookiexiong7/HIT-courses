import numpy as np
import matplotlib.pyplot as plt

from nonlinear import least_squares, ransac, neural_network

# 绘制结果
def basic_visualize(t, y, y_n, outpoint_index):
    plt.scatter(t, y_n, color='r', label='Points with Noise')
    plt.scatter(t[outpoint_index], y_n[outpoint_index], color='black', label='OutPoints')
    plt.plot(t, y, 'b', label='Original Function')
    plt.xlabel('t')
    plt.ylabel('y')

# 随机抽样数据
def generate_data(b1, w1, b2, w2, num_points, noise):
    t = np.linspace(0, 10, num_points)
    y_oringal = b1 * np.sin(w1 * t) + b2 * np.cos(w2 * t)
    # 添加噪声
    noise = np.random.normal(0, noise, num_points)
    y_n = y_oringal + noise

    # 加入外点
    outpoint_index = np.random.choice(range(num_points), size=15, replace=False)
    outpoint_values = np.random.uniform(low=-5, high=5, size=len( outpoint_index))
    y_n[ outpoint_index] =  outpoint_values

    return t, y_oringal, y_n, outpoint_index


num_points = 100  # 数据点数量
noise = 0.1 # 噪声水平

def main():
    a, w1, b, w2 = 1.0, 1.0, 0.5, 2.0
    t, y, y_n, outpoint_index = generate_data(a, w1, b, w2, num_points, noise)
    degree = 10
    basic_visualize(t, y, y_n, outpoint_index)
    least_squares(t, y_n, degree)
    basic_visualize(t, y, y_n, outpoint_index)
    ransac(t, y_n, degree+4)
    basic_visualize(t, y, y_n, outpoint_index)
    neural_network(t, y_n)


if __name__ == "__main__":
    main()

