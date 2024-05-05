import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import math
from sklearn.model_selection import train_test_split

def sigmoid(x):
    limit_x = np.clip(x, -500, 500)
    return 1 / (1 + np.exp(-limit_x))

def loss_func(X, y, theta, lamda):
    m = len(y)
    epsilon = 1e-10
    y_pred = sigmoid(np.dot(X, theta))
    y_pred = np.clip(y_pred, epsilon, 1 - epsilon)
    loss = -np.sum(y * np.log(y_pred) + (1 - y) * np.log(1 - y_pred)) / m + lamda * np.sum((theta) ** 2) / (2 * m)
    return loss

# 定义二项逻辑回归模型
class LogisticRegression:
    def __init__(self, learning_rate, num_epochs, lamda=0):
        self.learning_rate = learning_rate
        self.num_epochs = num_epochs
        self.theta = None
        self.loss_history = []
        self.lamda = lamda

    # 梯度下降
    def gredient_decent(self, X, y):
        X = np.hstack((np.ones((X.shape[0], 1)), X))
        self.theta = np.ones(X.shape[1])

        for epoch in range(self.num_epochs):
            y_pred = sigmoid(np.dot(X, self.theta))
            # 计算梯度
            gradient = (np.dot(X.T, (y_pred - y)) + self.lamda * self.theta) / len(X)
            # 更新权重
            self.theta -= self.learning_rate * gradient
            loss = loss_func(X, y, self.theta, self.lamda)
            print(f"第{epoch+1}次迭代，损失函数为{loss}")
            self.loss_history.append(loss)

    # 牛顿法
    def newton(self, X, y):
        X = np.hstack((np.ones((X.shape[0], 1)), X))
        self.theta = np.ones(X.shape[1])

        for epoch in range(self.num_epochs):
            y_pred = sigmoid(np.dot(X, self.theta))
            # 计算梯度和hessian矩阵
            gradient = (np.dot(X.T, (y_pred - y)) + self.lamda * self.theta) / len(X) 
            Hessian = (np.dot(X.T, np.dot(np.diagflat(y_pred * (1 - y_pred)), X))  + self.lamda * np.eye(X.shape[1])) / len(X) 
            # 更新权重
            self.theta -= np.linalg.pinv(Hessian).dot(gradient)
            loss = loss_func(X, y, self.theta, self.lamda)
            print(f"第{epoch+1}次迭代，损失函数为{loss}")
            self.loss_history.append(loss)

    def predict(self, X):
        X = np.hstack((np.ones((X.shape[0], 1)), X))
        y_pred = sigmoid(np.dot(X, self.theta))
        y_pred = np.round(y_pred)
        return y_pred

def draw_loss(model, str):
    plt.plot(model.loss_history)
    plt.xlabel('Epoch')
    plt.ylabel('Loss')
    plt.title(str)
    plt.show()

def draw_result(X, y, theta, str):
    plt.scatter(X[:, 0], X[:, 1], c=y, cmap='bwr')

    plot_x = np.array([min(X[:, 0]) - 2, max(X[:, 0]) + 2])  
    plot_y = - (theta[0] + theta[1] * plot_x) / theta[2] 
    plt.plot(plot_x, plot_y, label='Decision Boundary', color='green')

    plt.xlabel('Feature 1')
    plt.ylabel('Feature 2')
    plt.title(f'Classification Results - {str}')
    plt.show()

def draw_compare(model_1, model_2):
    plt.plot(model_1.loss_history, label="Gradient Descent")
    plt.plot(model_2.loss_history, label="Newton's Method")
    plt.xlabel('Epoch')
    plt.ylabel('Loss')
    plt.title('Loss Function Comparison')
    plt.legend()
    plt.show()

def generate_data(train_sample, navie = True):
    np.random.seed(177)
    pos_mean = [1, 1] #正例的两维度均值
    neg_mean = [-1, -1] #反例的两维度均值
    X = np.zeros((2 * train_sample, 2))
    Y = np.zeros((2 * train_sample))
    # 满足朴素贝叶斯假设
    if navie:
        cov = np.mat([[0.49, 0], [0, 0.49]])
        X[:train_sample, :] = np.random.multivariate_normal(pos_mean, cov, train_sample)
        X[train_sample:, :] = np.random.multivariate_normal(neg_mean, cov, train_sample)
        Y[:train_sample] = 1
        Y[train_sample:] = 0
    # 不满足朴素贝叶斯假设
    else:
        cov = np.mat([[0.49, 0.25], [0.25, 0.49]])
        X[:train_sample, :] = np.random.multivariate_normal(pos_mean, cov, train_sample)
        X[train_sample:, :] = np.random.multivariate_normal(neg_mean, cov, train_sample)
        Y[:train_sample] = 1
        Y[train_sample:] = 0

    return X, Y

# 自己生成的数据集进行二项分布回归
def main():
    train_sample = 500
    X, Y = generate_data(train_sample, navie = True)
    X_train, X_test, y_train, y_test = train_test_split(X, Y, test_size=0.2, random_state=177)

    print("梯度下降法")
    print("======================================")
    model_1 = LogisticRegression(learning_rate=0.1, num_epochs=1000, lamda=math.exp(-18))
    model_1.gredient_decent(X_train, y_train)
    draw_loss(model_1,'Gredient Decent')
    # 在测试集上验证正确率
    y_pred_test_1 = model_1.predict(X_test)
    accuracy_1 = np.mean(y_pred_test_1 == y_test)
    print(f"梯度下降法准确率为{accuracy_1 * 100}%")
    draw_result(X_train, y_train, model_1.theta, 'Gradient Descent - train')
    draw_result(X_test, y_test, model_1.theta, 'Gradient Descent')

    # ============================================================= #
    print("牛顿法")
    print("======================================")
    model_2 = LogisticRegression(learning_rate=0.001, num_epochs=1000, lamda=math.exp(-18))
    model_2.newton(X_train, y_train)
    draw_loss(model_2, 'NewTons method')
    # 在测试集上验证正确率
    y_pred_test_2 = model_2.predict(X_test)
    accuracy_2 = np.mean(y_pred_test_2 == y_test)
    print(f"牛顿法准确率为{accuracy_2 * 100}%")
    draw_result(X_train, y_train, model_2.theta, 'Newton - train')
    draw_result(X_test, y_test, model_2.theta, 'Newton')

    # 对比
    draw_compare(model_1, model_2)

# dry-beans数据集上的二项回归（梯度下降）
def dry_beans():
    data = pd.read_excel('./Dry_Bean_Dataset.xlsx')
    data["Class"] = data["Class"].map({"SEKER": 1, "BARBUNYA": 0})
    data = np.array(data[:3349])
    characters = data[:, 2:4].astype(float)
    labels = data[:, -1].astype(float)
    X_train, X_test, y_train, y_test = train_test_split(characters, labels, test_size=0.2, random_state=10)

    print("梯度下降法")
    print("======================================")
    model_1 = LogisticRegression(learning_rate=0.01, num_epochs=1000, lamda=0.01)
    model_1.gredient_decent(X_train, y_train)
    draw_loss(model_1,'Gredient Decent')
    # 在测试集上验证正确率
    y_pred_test_1 = model_1.predict(X_test)
    accuracy_1 = np.mean(y_pred_test_1 == y_test)
    print(f"梯度下降法准确率为{accuracy_1 * 100}%")

    draw_result(X_train, y_train, model_1.theta, 'Gradient Descent - train')
    draw_result(X_test, y_test, model_1.theta, 'Gradient Descent')

# iris数据集上的二项回归（包含牛顿法和梯度下降法）
def iris():
    data = pd.read_csv('./Iris.data')
    data["Species"] = data["Species"].map({"Iris-setosa": 1, "Iris-versicolor": 0})
    data = np.array(data[:100].values)

    characters = data[:, :-1].astype(float)
    labels = data[:, -1].astype(float)

    X_train, X_test, y_train, y_test = train_test_split(characters, labels, test_size=0.2, random_state=10)

    print("梯度下降法")
    print("======================================")
    model_1 = LogisticRegression(learning_rate=0.001, num_epochs=1000)
    model_1.gredient_decent(X_train, y_train)
    # 在测试集上验证正确率
    y_pred_test_1 = model_1.predict(X_test)
    accuracy_1 = np.mean(y_pred_test_1 == y_test)
    print(f"梯度下降法准确率为{accuracy_1 * 100}%")
    draw_result(X_train, model_1.predict(X_train), model_1.theta, 'Gradient Descent - train')
    draw_result(X_test, y_pred_test_1, model_1.theta, 'Gradient Descent')

    # ============================================================= #
    print("牛顿法")
    print("======================================")
    # 牛顿法lr无意义，方便写程序就没单独创个类
    model_2 = LogisticRegression(learning_rate=0.001, num_epochs=100, lamda=0)
    model_2.newton(X_train, y_train)
    print('\n')
    # 在测试集上验证正确率
    y_pred_test_2 = model_2.predict(X_test)
    accuracy_2 = np.mean(y_pred_test_2 == y_test)
    print(f"牛顿法准确率为{accuracy_2 * 100}%")
    draw_result(X_train,  model_2.predict(X_train), model_2.theta, 'Newton - train') 
    draw_result(X_test, y_pred_test_2, model_2.theta, 'Newton')

    # 对比
    draw_compare(model_1, model_2)

if __name__ == '__main__':
    iris()
    dry_beans()
    main()