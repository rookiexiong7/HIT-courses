import numpy as np
import matplotlib.pyplot as plt
import math

# 计算RMS误差函数    
def root_mean_square(given_function, fitted_function):

    return np.sqrt(np.mean((given_function - fitted_function) ** 2))

# 绘制结果并计算RMS误差
def draw(x, y_noisy, degree, w):
    x_t = np.arange(0, 1, 0.01)
    y_t = np.sin(2 * np.pi * x_t)
    plt.scatter(x, y_noisy, c='b', label='data point')
    plt.title('polynomial fitting') 
    plt.plot(x_t, y_t, label='True Function')

    test_sample = 100
    test_x = np.linspace(0, 1, test_sample)
    test_x_feature = np.column_stack([test_x ** i for i in range(degree + 1)])
    test_y = np.dot(test_x_feature, w)
    ture_y = np.sin(2 * np.pi * test_x)
    rms = root_mean_square(test_y, ture_y)
    plt.plot(test_x, test_y, label='Fitting Function')
    plt.legend()
    plt.show() 
    return rms

# 绘制多个图像并计算RMS误差
def draw_10(fig, axs, x, y_noisy, degree, w, size, lamda, string, lr, n):
    i = int((n - 1) / 3)
    j = int((n - 1) - i * 3)

    x_t = np.arange(0, 1, 0.01)
    y_t = np.sin(2 * np.pi * x_t)
    axs[i,j].scatter(x, y_noisy, c='b', label='data point')

    axs[i,j].plot(x_t, y_t, label='True Function')

    test_sample = 100
    test_x = np.linspace(0, 1, test_sample)
    test_x_feature = np.column_stack([test_x ** i for i in range(degree + 1)])
    test_y = np.dot(test_x_feature, w)
    ture_y = np.sin(2 * np.pi * test_x)
    rms = root_mean_square(test_y, ture_y)
    axs[i,j].plot(test_x, test_y, label='Fitting Function')
    if string == 'degree':
        axs[i,j].set_title(f'{string} = ' + str(degree))
    elif string == 'size':
        axs[i,j].set_title(f'{string} = ' + str(size))
    elif string == 'lamda':
        axs[i,j].set_title(f'{string} = ' + str(lamda))
    elif string == 'lr':
        axs[i,j].set_title(f'{string} = ' + str(lr))
    axs[i,j].legend()
    return rms

# 生成数据
def data_generate(mean, var, size, degree, seed=None):
    if seed is not None:
        np.random.seed(seed)
    
    x = np.linspace(0, 1, size)  
    y_true = np.sin(2 * np.pi * x)  
    y_noisy = np.sin(2 * np.pi * x) + np.random.normal(mean, var, len(x))  
    x_feature = np.column_stack([x ** i for i in range(degree + 1)])
    return x, x_feature, y_true, y_noisy

# 最小二乘法
def least_square(X, Y, lamda, degree):
    A = np.linalg.inv(np.dot(X.T, X) + lamda * np.eye(degree+1)).dot(X.T).dot(Y)
    return A

# 共轭梯度法
def conjugate_gradient(x_feature, y_noisy, degree, alpha, accept_gradient, lamda):
    A = np.zeros((degree + 1, ))
    Q = np.dot(x_feature.T, x_feature)  + lamda * np.eye(degree + 1, dtype=float)
    b = np.dot(x_feature.T, y_noisy)
    r = b - np.dot(Q, A)
    p = r
    count = 0
    while True:
        a = (np.dot(r.T, r) / np.dot(np.dot(p.T, Q), p))
        A = A + np.dot(a, p)
        r1 = r - np.dot(np.dot(a, Q), p)
        if np.dot(r1.T, r1) <= accept_gradient:
            break
        else:
            beta = (np.dot(r1.T, r1) / np.dot(r.T, r))
            p = r1 + np.dot(beta, p)
            count = count + 1
            r = r1
    print("Iterations:", count)
    return A


#梯度下降法
def gradient_descent(X, Y, degree, alpha, accept_gradient, lamda, limit=100000):
    iteration = 0
    A = np.zeros(degree + 1)
    while iteration < limit:    
        gradient_w = np.dot(X.T, X).dot(A) - np.dot(X.T, Y) + lamda * A
        norm_gradient_w = np.linalg.norm(gradient_w)
        if norm_gradient_w < accept_gradient:
            break
        iteration += 1
        A -= alpha * gradient_w
    print("Iterations:", iteration)
    return A

# 测试不同的多项式阶数，比较实验效果
def test_degree():
    fig, axs = plt.subplots(2, 3, figsize=(15, 10))
    rms_test = []
    rms_train = []
    n = 1
    list = [1, 3, 5, 7, 9, 11]
    for degree in list:
        lamda = 0
        mean = 0
        size = 10
        var = 0.36
        alpha = 0.01
        accept_gradient = 1e-20
        seed = 177
        x, x_feature, y_true, y_noisy = data_generate(mean, var, size, degree, seed)
        A = conjugate_gradient(x_feature, y_noisy, degree, alpha, accept_gradient, lamda)
        y_pred = np.dot(x_feature, A)
        rms_train.append(root_mean_square(y_noisy, y_pred))

        x = x = draw_10(fig, axs, x, y_noisy, degree, A, size, lamda, 'degree', alpha, n)
        n += 1
        rms_test.append(x)
        print(A)

    plt.suptitle('polynomial fitting(degree test)')
    plt.tight_layout()
    plt.show()

    plt.plot(list, rms_test, label='Test RMS')
    plt.plot(list, rms_train, label='Train RMS')
    plt.legend()
    plt.show()

# 测试不同的数据量，比较实验效果
def test_size():
    fig, axs = plt.subplots(2, 3, figsize=(15, 10))
    rms_test = []
    rms_train = []
    list = [10, 30, 50, 70, 90, 110]
    n = 1
    for size in list:
        degree = 10
        lamda = 0
        mean = 0
        var = 0.36
        alpha = 0.01
        accept_gradient = 1e-20
        seed = 177
        x, x_feature, y_true, y_noisy = data_generate(mean, var, size, degree, seed)
        A = conjugate_gradient(x_feature, y_noisy, degree, alpha, accept_gradient, lamda)
        y_pred = np.dot(x_feature, A)
        rms_train.append(root_mean_square(y_noisy, y_pred))

        x = draw_10(fig, axs, x, y_noisy, degree, A, size, lamda, 'size', alpha, n)
        n += 1
        print(n)
        rms_test.append(x)
        print(A)

    plt.suptitle('polynomial fitting(size test)')
    plt.tight_layout()
    plt.show()

    plt.plot(list, rms_test, label='Test RMS')
    plt.plot(list, rms_train, label='Train RMS')
    plt.legend()
    plt.show()

# 测试不同学习率，比较实验效果
def test_lr():
    fig, axs = plt.subplots(2, 3, figsize=(15, 10))
    rms_test = []
    rms_train = []
    i = 1
    list = [0.1, 0.01, 0.001, 0.0001, 1e-5, 1e-6]
    for alpha in list:
        degree = 5
        lamda = 0
        mean = 0
        size = 10
        var = 0.1
        accept_gradient = 0.01
        seed = 177
        x, x_feature, y_true, y_noisy = data_generate(mean, var, size, degree, seed)
        A = gradient_descent(x_feature, y_noisy, degree, alpha, accept_gradient, lamda)
        y_pred = np.dot(x_feature, A)
        rms_train.append(root_mean_square(y_noisy, y_pred))

        x = draw_10(fig, axs, x, y_noisy, degree, A, size, lamda, 'lr', alpha, i)
        i += 1
        rms_test.append(x)
        print(A)

    plt.suptitle('polynomial fitting(learning rate test)')
    plt.tight_layout()
    plt.show()


# 测试不同lamda，比较实验效果
def test_lamda():
    fig, axs = plt.subplots(2, 3, figsize=(15, 10))
    rms_test = []
    rms_train = []
    i = 1
    for lamda in [0, math.exp(-18), math.exp(-4), 5e-6, 0.01, 1]:
        degree = 10
        alpha = 0.01
        mean = 0
        size = 10
        var = 0.36
        accept_gradient = 1e-20
        seed = 177
        x, x_feature, y_true, y_noisy = data_generate(mean, var, size, degree, seed)
        A = conjugate_gradient(x_feature, y_noisy, degree, alpha, accept_gradient, lamda)
        y_pred = np.dot(x_feature, A)
        rms_train.append(root_mean_square(y_noisy, y_pred))

        x = draw_10(fig, axs, x, y_noisy, degree, A, size, lamda, 'lamda', alpha, i)
        i += 1
        rms_test.append(x)
        print(A)

    plt.suptitle('polynomial fitting(lamda test)')
    plt.tight_layout()
    plt.show()

#固定情况
def main():
    degree = 5 #阶数
    lamda = 0 #惩罚项的lamda
    mean = 0
    size = 10 #训练样本个数
    var = 0.1 #高斯噪声的方差
    alpha = 0.1 #梯度下降法的步长
    accept_gradient = 0.0001 #梯度下降法阈值
    seed = 177
    x, x_feature, y_true, y_noisy = data_generate(mean, var, size, degree, seed) #生成数据
    A = gradient_descent(x_feature, y_noisy, degree, alpha, accept_gradient, lamda) #梯度下降法
    draw(x, y_noisy, degree, A)

    print(A)

# 过拟合情况
def overfitting_test():
    degree = 10 #阶数
    lamda =  0.00#惩罚项的lamda
    mean = 0
    size = 10 #训练样本个数
    var = 0.36 #高斯噪声的方差
    alpha = 0.1 #梯度下降法的步长
    accept_gradient = 1e-20 #梯度下降法阈值
    seed = 1314
    x, x_feature, y_true, y_noisy = data_generate(mean, var, size, degree, seed) #生成数据
    A = conjugate_gradient(x_feature, y_noisy, degree, alpha, accept_gradient, lamda) #梯度下降法
    draw(x, y_noisy, degree, A)
    print(A)


if __name__ == '__main__':
    overfitting_test()
    main()
    test_lamda()
    test_degree()
    test_size()
    test_lr()
    