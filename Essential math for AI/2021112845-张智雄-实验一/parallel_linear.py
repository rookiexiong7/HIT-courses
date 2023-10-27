import numpy as np
import matplotlib.pyplot as plt
from sklearn.linear_model import LinearRegression
from sklearn.linear_model import RANSACRegressor

# 生成模拟数据
def generate_data(a, b, c, num_points, noise):
    lines = []
    for ci in c:
        line_points = []
        for j in range(num_points):
            x = -10 + 20 * np.random.rand()
            y = (-a * x - ci) / b + noise * np.random.randn()
            line_points.append((x, y))
        lines.append(np.array(line_points))
    return lines

# RANSAC拟合
def fit_ransac(lines):
    models = []
    for line_points in lines:
        X = line_points[:, 0].reshape(-1, 1)
        y = line_points[:, 1]
        ransac = RANSACRegressor()
        ransac.fit(X, y)
        models.append(ransac)
    return models

# 可视化结果
def visualize(lines, models):
    plt.figure()
    for line_points, ransac in zip(lines, models):
        X = line_points[:, 0].reshape(-1, 1)
        y = line_points[:, 1]
        plt.scatter(X, y)
        y_pred = ransac.predict(X)
        plt.plot(X, y_pred, color='red')
    plt.show()

# 主函数
def main():
    num_points = 100
    noise = 1
    a, b = 2, 3
    c = [-10, 0, 10]
    lines = generate_data(a, b, c, num_points, noise)
    models = fit_ransac(lines)
    visualize(lines, models)

if __name__ == "__main__":
    main()
