import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split

from regression import LogisticRegression

def main():
    # beans的测试，按需注释切换
    acc = []
    data = pd.read_csv('./data/Dry_Bean.csv')
    data["Class"] = data["Class"].map({"SEKER": 1, "BARBUNYA": 0})
    data = np.array(data[:3349])
    characters = data[:, 2:4].astype(float)
    labels = data[:, -1].astype(float)
    X_train, X_test, y_train, y_test = train_test_split(characters, labels, test_size=0.2, random_state=177)
    for i in range(0, 1501, 50):
        model = LogisticRegression(learning_rate=0.00001, num_epochs=i)
        model.gredient_decent(X_train, y_train)
        y_pred_test = model.predict(X_test)
        accuracy = np.mean(y_pred_test == y_test)
        print(f"梯度下降法准确率为{accuracy * 100}%")
        acc.append(accuracy)
    x_ticks = range(0, 1501, 50)

    plt.plot(x_ticks, acc, label="Gradient Descent")
    plt.xlabel('Epoch')
    plt.ylabel('Accuracy')

    # 设置x轴的刻度范围
    plt.xlim(0, 1500)
    plt.show()

    '''# iris的测试
    acc_1 = []
    acc_2 = []
    data = pd.read_csv('./data/Iris.csv')
    data["Species"] = data["Species"].map({"Iris-setosa": 1, "Iris-versicolor": 0})
    data = np.array(data[:100].values)
    characters = data[:, 1:-1].astype(float)
    labels = data[:, -1].astype(float)
    X_train, X_test, y_train, y_test = train_test_split(characters, labels, test_size=0.2, random_state=10)


    for i in range(0, 1501, 50):
        model_1 = LogisticRegression(learning_rate=0.001, num_epochs=i)
        model_1.gredient_decent(X_train, y_train)
        y_pred_test = model_1.predict(X_test)
        accuracy = np.mean(y_pred_test == y_test)
        print(f"梯度下降法准确率为{accuracy * 100}%")
        acc_1.append(accuracy)

        model_2 = LogisticRegression(learning_rate=0.001, num_epochs=i)
        model_2.newton(X_train, y_train)
        y_pred_test = model_2.predict(X_test)
        accuracy = np.mean(y_pred_test == y_test)
        print(f"梯度下降法准确率为{accuracy * 100}%")
        acc_2.append(accuracy)

    x_ticks = range(0, 1501, 50)

    plt.plot(x_ticks, acc_1, label="Gradient Descent")
    plt.plot(x_ticks, acc_2, label="Newton's Method")
    plt.xlabel('Epoch')
    plt.ylabel('Accuracy')

    # 设置x轴的刻度范围
    plt.xlim(0, 1500)
    plt.show()'''

if __name__ == '__main__':
    main()