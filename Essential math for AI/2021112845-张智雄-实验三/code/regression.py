import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split

def sigmoid(x):
    limit_x = np.clip(x, -500, 500)
    return 1 / (1 + np.exp(-limit_x))

def loss_func(X, y, theta):
    m = len(y)
    epsilon = 1e-10
    y_pred = sigmoid(np.dot(X, theta))
    y_pred = np.clip(y_pred, epsilon, 1 - epsilon)
    loss = -np.sum(y * np.log(y_pred) + (1 - y) * np.log(1 - y_pred)) / m
    return loss

# 定义二项逻辑回归模型
class LogisticRegression:
    def __init__(self, learning_rate, num_epochs):
        self.learning_rate = learning_rate
        self.num_epochs = num_epochs
        self.theta = None
        self.loss_history = []

    # 梯度下降
    def gredient_decent(self, X, y):
        X = np.hstack((np.ones((X.shape[0], 1)), X))
        self.theta = np.ones(X.shape[1])

        for epoch in range(self.num_epochs):
            y_pred = sigmoid(np.dot(X, self.theta))
            # 计算梯度
            gradient = np.dot(X.T, (y_pred - y)) / len(X)
            # 更新权重
            self.theta -= self.learning_rate * gradient
            loss = loss_func(X, y, self.theta)
            print(f"第{epoch+1}次迭代，损失函数为{loss}")
            self.loss_history.append(loss)

    # 牛顿法
    def newton(self, X, y):
        X = np.hstack((np.ones((X.shape[0], 1)), X))
        self.theta = np.ones(X.shape[1])

        for epoch in range(self.num_epochs):
            y_pred = sigmoid(np.dot(X, self.theta))
            # 计算梯度和hessian矩阵
            gradient = np.dot(X.T, (y_pred - y)) / len(X)
            Hessian = np.dot(X.T, np.dot(np.diagflat(y_pred * (1 - y_pred)), X))
            # 更新权重
            self.theta -= np.linalg.inv(Hessian).dot(gradient)
            loss = loss_func(X, y, self.theta)
            print(f"第{epoch+1}次迭代，损失函数为{loss}")
            self.loss_history.append(loss)

    def predict(self, X):
        X = np.hstack((np.ones((X.shape[0], 1)), X))
        y_pred = sigmoid(np.dot(X, self.theta))
        y_pred = np.round(y_pred)
        return y_pred

def draw(model, str):
    plt.plot(model.loss_history)
    plt.xlabel('Epoch')
    plt.ylabel('Loss')
    plt.title(str)
    plt.show()

def main():
    '''使用时加/删除对应的注释'''

    '''    # dry-beans数据集上的二项回归（梯度下降）
    data = pd.read_csv('./data/Dry_Bean.csv')
    data["Class"] = data["Class"].map({"SEKER": 1, "BARBUNYA": 0})
    data = np.array(data[:3349])
    characters = data[:, 2:4].astype(float)
    labels = data[:, -1].astype(float)
    X_train, X_test, y_train, y_test = train_test_split(characters, labels, test_size=0.2, random_state=177)

    print("梯度下降法")
    print("======================================")
    model_1 = LogisticRegression(learning_rate=0.00001, num_epochs=1000)
    model_1.gredient_decent(X_train, y_train)
    draw(model_1,'Gredient Decent')
    # 在测试集上验证正确率
    y_pred_test_1 = model_1.predict(X_test)
    accuracy_1 = np.mean(y_pred_test_1 == y_test)
    print(f"梯度下降法准确率为{accuracy_1 * 100}%")

    plt.scatter(X_test[:, 0], X_test[:, 1], c=y_pred_test_1, cmap='bwr')
    plt.xlabel('Feature 1')
    plt.ylabel('Feature 2')
    plt.title('Classification Results - Gradient Descent')
    plt.show()'''

    # iris数据集上的二项回归（包含牛顿法和梯度下降法）
    data = pd.read_csv('./data/Iris.csv')
    data["Species"] = data["Species"].map({"Iris-setosa": 1, "Iris-versicolor": 0})
    data = np.array(data[:100].values)

    characters = data[:, 1:-1].astype(float)
    labels = data[:, -1].astype(float)

    X_train, X_test, y_train, y_test = train_test_split(characters, labels, test_size=0.2, random_state=10)

    print("梯度下降法")
    print("======================================")
    model_1 = LogisticRegression(learning_rate=0.001, num_epochs=1000)
    model_1.gredient_decent(X_train, y_train)
    draw(model_1,'Gredient Decent')
    # 在测试集上验证正确率
    y_pred_test_1 = model_1.predict(X_test)
    accuracy_1 = np.mean(y_pred_test_1 == y_test)
    print(f"梯度下降法准确率为{accuracy_1 * 100}%")

    plt.scatter(X_test[:, 0], X_test[:, 1], c=y_pred_test_1, cmap='bwr')
    plt.xlabel('Feature 1')
    plt.ylabel('Feature 2')
    plt.title('Classification Results - Gradient Descent')
    plt.show()

    # ============================================================= #
    print("牛顿法")
    print("======================================")
    # 牛顿法lr无意义，方便写程序就没单独创个类
    model_2 = LogisticRegression(learning_rate=0.001, num_epochs=1000)
    model_2.newton(X_train, y_train)
    print('\n')
    draw(model_2, 'NewTons method')
    # 在测试集上验证正确率
    y_pred_test_2 = model_2.predict(X_test)
    accuracy_2 = np.mean(y_pred_test_2 == y_test)
    print(f"牛顿法准确率为{accuracy_2 * 100}%")

    plt.scatter(X_test[:, 0], X_test[:, 1], c=y_pred_test_2, cmap='bwr')
    plt.xlabel('Feature 1')
    plt.ylabel('Feature 2')
    plt.title('Classification Results - Newton')
    plt.show()

    # 对比
    plt.plot(model_1.loss_history, label="Gradient Descent")
    plt.plot(model_2.loss_history, label="Newton's Method")
    plt.xlabel('Epoch')
    plt.ylabel('Loss')
    plt.title('Loss Function Comparison')
    plt.legend()
    plt.show()

if __name__ == '__main__':
    main()