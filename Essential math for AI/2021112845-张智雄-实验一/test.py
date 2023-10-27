import numpy as np

from linear import least_sqaure as ls, ransac as ra, generate_data as g_line
from robust_test_linear import generate_data as g_line_with_outpoints
from function_tools import root_mean_square as rms
from nonlinear import least_squares, ransac, neural_network, generate_data
from robust_test_nonlinear import generate_data as g_data_with_outpoints
from optimize import ridge, curve


# 验证直线-无外点
def linear_line(x_n, y_m, y_right):
    y_predict = ls(x_n, y_m)
    print(f"最小二乘直线误差评价为：{rms(y_right, y_predict)}")

    y_predict = ra(x_n, y_m)
    print(f"RMS直线误差评价为：{rms(y_right, y_predict)}")

# 验证直线-有外点
def robust_line(x_n, y_m, y_right):
    y_predict = ls(x_n, y_m)
    print(f"最小二乘直线误差（有外点）评价为：{rms(y_right, y_predict)}")

    y_predict = ra(x_n, y_m)
    print(f"RMS直线误差（有外点）评价为：{rms(y_right, y_predict)}")

# 验证多项式最高项次数的影响
def degree_test(t, y, y_n):
    # 最小二乘法的验证
    print("最小二乘法曲线的验证")
    for degree in range(1,15):
        X, model = least_squares(t, y_n, degree)
        print(f"最高项次数为{degree}的误差：{rms(y, model.predict(X))}")

    # RANSAC法的验证
    print("RANSAC法曲线的验证")
    for degree in range(1,15):
        X, model = ransac(t, y_n, degree)
        print(f"最高项次数为{degree}的误差：{rms(y, model.predict(X))}")

# 比较在有外点下，各方法的性能
def outpoints_compare(t, y, y_n):
    degree = 10
    # 最小二乘
    X1, model_1 = least_squares(t, y_n, degree)
    print(f"最小二乘法（有外点）：{rms(y, model_1.predict(X1))}")

    # RANSAC
    X2, model_2 = ransac(t, y_n, degree)
    print(f"RANSAC法（有外点）：{rms(y, model_2.predict(X2))}")

    # 神经网络
    model_3 = neural_network(t, y_n)
    print(f"神经网络法（有外点）：{rms(y, model_3.predict(t.reshape(-1, 1)))}")

    # 岭回归
    X4, model_4 = ridge(t, y_n)
    print(f"岭回归法（有外点）：{rms(y, model_4.predict(X4))}")

    # 曲线拟合
    y_fit = curve(t, y_n)
    print(f"曲线拟合法（有外点）：{rms(y, y_fit)}")

num_points = 100
noise = 0.1

def main():
    # 直线
    a_given, b_given, c_given, noise_line = 2.0, -1.0, -6.0, 1

    x, y, x_n, y_m = g_line(a_given, b_given, c_given, num_points, noise_line)
    y_right = a_given * x_n + c_given
    linear_line(x_n, y_m, y_right)

    x1, y1, x_n1, y_m1, outpoint_index = g_line_with_outpoints(a_given, b_given, c_given, num_points, noise_line)
    y_right1 = a_given * x_n1 + c_given
    robust_line(x_n1, y_m1, y_right1)

    print("================================")
    print("对多项式最高项次数的检验：")
    # 曲线
    a, w1, b, w2 = 1.0, 1.0, 0.5, 2.0
    # 不含外点
    t, y, y_n = generate_data(a, w1, b, w2, num_points, noise)
    degree_test(t, y, y_n)

    print("================================")
    print("对各曲线拟合方法的检验：")
    # 含外点
    a, w1, b, w2 = 1.0, 1.0, 0.5, 2.0
    t1, y1, y_n1, outpoint_index = g_data_with_outpoints(a, w1, b, w2, num_points, noise)
    outpoints_compare(t1, y1, y_n1)

if __name__ == "__main__":
    main()