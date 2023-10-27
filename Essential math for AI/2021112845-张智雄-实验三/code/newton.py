import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np

# 自由选择修改函数
def func(x):
    return np.sin(x) + np.sin(2*x)
'''return 0.5 * x ** 3 + 4 * x ** 2 + 3 * x - 1'''

def func_2(x):
    f = x[0] ** 4 + 25 * x[1] ** 4 + x[0] ** 2 * x[1] ** 2

    df = np.array([4 * x[0] ** 3 + 2 * x[0] * x[1] ** 2, 100 * x[1] ** 3 + 2 * x[0] ** 2 * x[1]])

    d2f = np.array([[2 * x[0] ** 2 + 2 * x[1] ** 2, 4 * x[0] * x[1]],
                    [4 * x[0] * x[1], 300 * x[1] ** 2 + 2 * x[0] ** 2]])
    return f, df, d2f

# 一元求解
def newton_method_1(f, x0, tol=1e-6, max_iter=2000):
    x = x0
    x_vals = [x]

    for _ in range(max_iter):
        f_val = f(x)
        f_prime = (f(x + tol) - f(x - tol)) / (2 * tol)
        f_second = (f(x + tol) - 2 * f_val + f(x - tol)) / (tol ** 2)

        x_next = x - 0.01 * f_prime / f_second

        if abs(x_next - x) < tol:
            x_vals.append(x_next)
            return x_next, x_vals

        x = x_next
        x_vals.append(x)

    return None, x_vals

# 二元求解
def newton_method_2(func_2, x00, tol=1e-6, max_iter=2000):
    x = x00
    x_vals = [x]
    for _ in range(max_iter):
        f0, g1, g2 = func_2(x)
        next = np.linalg.inv(g2) @ g1
        x_next = x - next

        if abs(np.linalg.norm(g1)) < tol:
            x_vals.append(x_next)
            return x_next, x_vals

        x = x_next
        x_vals.append(x)

def solve_1():
    x = np.linspace(-8, 2, 1000)
    y = func(x)

    # 使用牛顿法求解方程并展示解点的迭代变化
    initial_guess = -6
    solution, iterations = newton_method_1(func, initial_guess)
    print(f'极值点x={solution}，y={func(solution)}')
    plt.plot(x, y, label='Curve')
    plt.xlabel('x')
    plt.ylabel('f(x)')
    plt.title('Curve with Multiple Bends')
    plt.grid(True)

    # 绘制解点的迭代变化
    for i in range(len(iterations)):
        plt.plot(iterations[i], func(iterations[i]), 'bo', alpha=(i+1)/len(iterations))
    plt.plot(solution, func(solution), 'ro', label='Solution')
    plt.legend()
    plt.show()

def solve_2():
    initial_guess = np.array([3, 3])

    solution, iterations = newton_method_2(func_2, initial_guess)
    print(f'极值点为x1={solution[0]}，x2={solution[1]}')
    iterations = np.array(iterations)

    # 绘制三维图形
    x = np.linspace(-3, 3, 1000)
    y = np.linspace(-3, 3, 1000)
    x_grid, y_grid = np.meshgrid(x, y)
    z_grid = x_grid ** 4 + 25 * y_grid ** 4 - x_grid ** 2 * y_grid ** 2
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    ax.plot_surface(x_grid, y_grid, z_grid, color='lightblue')
    ax.set_xlabel('x')
    ax.set_ylabel('y')
    ax.set_zlabel('f(x, y)')
    # 绘制轨迹
    x_vals = np.array(iterations)[:, 0]
    y_vals = np.array(iterations)[:, 1]
    z_vals = x_vals ** 4 + 25 * y_vals ** 4 - x_vals ** 2 * y_vals ** 2

    ax.plot(x_vals, y_vals, z_vals, 'bo-', label='Iteration', alpha = 0.7)
    ax.plot([x_vals[0]], [y_vals[0]], [z_vals[0]], 'go', label='Initial Guess')
    ax.plot([x_vals[-1]], [y_vals[-1]], [z_vals[-1]], 'ro', label='Solution')

    plt.show()


def main():
    solve_1()
    solve_2()

if __name__ == '__main__':
    main()