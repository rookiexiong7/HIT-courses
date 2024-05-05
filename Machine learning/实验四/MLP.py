# 不使用Pytorch构造一个多层感知机，完成对某类型样本数据的分类

import numpy as np
import matplotlib.pyplot as plt
import random
import pandas as pd
from sklearn.model_selection import train_test_split
import struct

def random_seed(seed):
    np.random.seed(seed)
    random.seed(seed)

def load_mnist_images(filename):
    with open(filename, 'rb') as file:
        _, num_images, rows, cols = struct.unpack('>IIII', file.read(16))
        images = np.fromfile(file, dtype=np.uint8).reshape(num_images, rows * cols)
    return images

def load_mnist_labels(filename):
    with open(filename, 'rb') as file:
        _, num_labels = struct.unpack('>II', file.read(8))
        labels = np.fromfile(file, dtype=np.uint8)
    return labels

def data_prepare():
    train_images = load_mnist_images('./data/train-images-idx3-ubyte')
    train_labels = load_mnist_labels('./data/train-labels-idx1-ubyte')
    test_images = load_mnist_images('./data/t10k-images-idx3-ubyte')
    test_labels = load_mnist_labels('./data/t10k-labels-idx1-ubyte')

    return train_images, train_labels, test_images, test_labels

def iris():
    data = pd.read_csv('./Iris.data')
    data["Species"] = data["Species"].map({"Iris-setosa": 1, "Iris-versicolor": 0, "Iris-virginica": 2})
    data = np.array(data[:].values)

    characters = data[:, :-1].astype(float)
    labels = data[:, -1].astype(float)


    X_train, X_test, y_train, y_test = train_test_split(characters, labels, test_size=0.2, random_state=10)
    return X_train, X_test, y_train, y_test

def visualize_images_with_labels(images, true_labels, predicted_labels):
    image = images[:10]
    true_label = true_labels[:10]
    predicted_label = predicted_labels[:10]

    _, axes = plt.subplots(2, 5, figsize=(10, 4))

    for i, ax in enumerate(axes.flat):
        ax.imshow(image[i].reshape(28, 28), cmap='gray')
        ax.axis('off')
        ax.text(0.5, -0.1, f'True: {true_label[i].argmax()}', transform=ax.transAxes,
                horizontalalignment='center', verticalalignment='center', color='black')
        ax.text(0.5, -0.2, f'Predicted: {predicted_label[i]}', transform=ax.transAxes,
                horizontalalignment='center', verticalalignment='center', color='red')
    plt.suptitle('MNIST Images With Labels after MLP', fontsize=16)
    plt.show()

class MLP(object):
    def __init__(self, arch, layers):
        self.arch = arch
        self.layers = layers

    def init_params(self):
        '''初始化权重和偏置'''
        self.weights = []
        self.biases = []
        self.h = []
        for i in range(self.layers - 1):
            self.weights.append(np.random.randn(self.arch[i+1], self.arch[i]))
            self.biases.append(np.random.randn(self.arch[i+1], 1))
            self.h.append(np.zeros((self.arch[i], 1)))

        self.h.append(np.zeros((self.arch[-1], 1)))
        for j in range(len(self.h)):
            print(f'第{j}层的神经元数目为',self.h[j].shape)
        
    # 激活函数
    def sigmoid(self, z):
        z = np.clip(z, -500, 500)
        return 1 / (1 + np.exp(-z))
    
    def sigmoid_prime(self, z):
        return self.sigmoid(z) * (1 - self.sigmoid(z))

    def softmax(self, z):
        z = np.clip(z, -500, 500)
        return np.exp(z) / np.sum(np.exp(z))

    
    def cross_entropy_loss(self, y_true, y_pred):
        '''计算多分类交叉熵损失'''
        y_pred = np.clip(y_pred, 1e-10, 1 - 1e-10)
        loss = -np.sum(y_true * np.log(y_pred))
        prime = y_pred - y_true
        return loss, prime
    
    def forward(self, x):
        '''前向传播,给定输入x,计算输出y'''
        self.h[0] = x
        for i in range(1, self.layers):
            self.h[i] = self.sigmoid(np.dot(self.weights[i-1], self.h[i-1]) + self.biases[i-1])  
        self.h[-1] = self.softmax(np.dot(self.weights[-1], self.h[-2]) + self.biases[-1])
        return self.h[-1]        
    
    def backward(self, y, learning_rate):
        '''反向传播,给定输入x和标签y,计算梯度'''
        loss, prime = self.cross_entropy_loss(y, self.h[-1])
        weights, biases = [], []
        for i in range(self.layers - 1):
            weights.append(np.zeros((self.arch[i+1], self.arch[i])))
            biases.append(np.zeros((self.arch[i+1], 1)))
        for t in range(self.layers - 1, 0, -1):
            grad_w = np.dot(prime, self.h[t-1].T)
            grad_b = prime
            # 更新参数
            weights[t-1] = grad_w
            biases[t-1] =  grad_b
            # 更新梯度
            prime = np.dot(self.weights[t-1].T, prime) * self.sigmoid_prime(self.h[t-1])
        
        return loss, weights, biases

    def SGD(self, training_data, batch_size, eta):
        '''随机梯度下降法训练神经网络'''
        random.shuffle(training_data)
        data = training_data[:batch_size]
        loss, weight, biase = [], [], []
        for img,label in data: 
            img = img.reshape((-1, 1))
            label = label.reshape((-1, 1))  
            y = self.forward(img)
            los, grad_w, grad_b = self.backward(label, eta)
            loss.append(los)
            weight.append(grad_w)
            biase.append(grad_b)
        # 更新参数
        for j in range(len(weight)):
            for i in range(self.layers - 1):
                self.weights[i] -= eta * weight[j][i] / batch_size
                self.biases[i] -= eta * biase[j][i] / batch_size
        return np.mean(loss)
        
    def train(self, data, epochs, batch_size, eta, test_data):
        '''训练神经网络'''
        self.init_params() 
        loss = []
        acc = []
        random.shuffle(data)
        for i in range(epochs):
            # train_data = data[:batch_size]
            if (i*batch_size) % len(data) > ((i+1)*batch_size) % len(data):
                train_data = data[(i*batch_size) % len(data):] + data[:(i+1)*batch_size % len(data)]
            else:
                train_data = data[(i*batch_size) % len(data):((i+1)*batch_size) % len(data)]
            loss.append(self.SGD(train_data, batch_size, eta))
            acc.append(self.evaluate(test_data))
            print(f"Epoch {i}: {loss[i]}, Accuary: {acc[i]}")
        plt.figure()
        plt.plot(loss)
        plt.xlabel("Epoch")
        plt.ylabel("Loss")
        plt.show()

        plt.figure()
        plt.plot(acc, color='orange')
        plt.xlabel("Epoch")
        plt.ylabel("Accuracy")
        plt.show()

    def evaluate(self, test_data):
        '''评估神经网络的性能'''
        num = 0
        test_data = test_data[:100]
        test_img, test_label = zip(*test_data)
        pred_label = [] 
        for x,y in test_data:
            x = x.reshape((-1, 1))
            y = y.reshape((-1, 1)).argmax()
            y_pred = self.forward(x).argmax()
            pred_label.append(y_pred)
            if y_pred == y:
                num += 1
        #visualize_images_with_labels(test_img, test_label, pred_label)
        return num / len(test_data)
    
# 多项回归模型
class LogisticRegression:
    def __init__(self, learning_rate, bacth, num_epochs):
        self.learning_rate = learning_rate
        self.num_epochs = num_epochs
        self.theta = None
        self.batch = bacth
        self.loss_history = []
        self.acc_history = []

    def softmax(self, z):
        z = np.clip(z, -500, 500)
        for i in range(len(z)):
            z[i] = np.exp(z[i]) / np.sum(np.exp(z[i]))
        return z
    
    def loss_func(self, X, y, theta):
        '''计算损失函数'''
        y_pred = self.softmax(np.dot(X, theta))
        y_pred = np.clip(y_pred, 1e-10, 1 - 1e-10)
        loss = -np.sum(y * np.log(y_pred)) / len(X)
        return loss

    def gredient_decent(self, X, y, test_images, test_label, lamda=0):
        '''梯度下降法'''
        train_data = list(zip(X, y))
        self.theta = np.zeros((X.shape[1], y.shape[1]))
        random.shuffle(train_data)
        for epoch in range(self.num_epochs):
            if (epoch*self.batch) % len(train_data) > ((epoch+1)*self.batch) % len(train_data):
                data = train_data[(epoch*self.batch) % len(train_data):] + train_data[:(epoch+1)*self.batch % len(train_data)]
            else:
                data = train_data[(epoch*self.batch) % len(train_data):((epoch+1)*self.batch) % len(train_data)]
            X_batch, y_batch = zip(*data)
            X_batch = np.array(X_batch)
            y_batch = np.array(y_batch)
            y_pred = self.softmax(np.dot(X_batch, self.theta))
            # 计算梯度
            gradient = np.dot(X_batch.T, (y_pred - y_batch)) / len(X) + lamda * self.theta
            # 更新权重
            self.theta -= self.learning_rate * gradient
            loss = self.loss_func(X_batch, y_batch, self.theta)
            acc = self.evaluate(test_images, test_label)
            print(f"第{epoch+1}次迭代，损失函数为{loss}, 准确率为{acc}")
            self.loss_history.append(loss)  
            self.acc_history.append(acc)
    
    def predict(self, X):
        '''预测'''
        y_pred = self.softmax(np.dot(X, self.theta))
        y_pred = np.argmax(y_pred, axis=1)
        return y_pred   

    def evaluate(self, X, y):
        '''评估模型性能'''
        num = 0
        y_pred = self.predict(X)
        y_ture = np.argmax(y, axis=1)
        for i in range(len(y_pred)):
            if y_pred[i] == y_ture[i]:
                num += 1
        acc = num / len(y_pred)
        return acc
    
    def draw_loss(self):
        '''绘制损失函数变化曲线'''
        plt.plot(self.loss_history)
        plt.xlabel('Epoch')
        plt.ylabel('Loss')
        plt.show()

    def draw_acc(self):
        '''绘制准确率变化曲线'''
        plt.plot(self.acc_history, color='orange')
        plt.xlabel('Epoch')
        plt.ylabel('Accuracy')
        plt.show()

def regression():
    # X_train, X_test, y_train, y_test = iris()
    # train_label = np.zeros((X_train.shape[0], 3))
    # test_label = np.zeros((X_test.shape[0], 3))
    # for i in range(len(X_train)):
    #     train_label[i][int(y_train[i])] = 1
    # for i in range(len(X_test)):
    #     test_label[i][int(y_test[i])] = 1
    # Regression = LogisticRegression(0.01, 120, 2000)
    # Regression.gredient_decent(X_train, train_label, lamda=0)
    # Regression.draw_loss()
    # acc = Regression.evaluate(X_test, test_label)
    # print(f"Accuracy: {acc}")
    
    train_images, train_labels, test_images, test_labels = data_prepare()
    train_label = np.zeros((train_images.shape[0], 10))
    test_label = np.zeros((test_images.shape[0], 10))
    # random.shuffle(train_images)
    train_images = train_images[:60000]
    for i in range(len(train_images)):
        train_label[i][train_labels[i]] = 1
    for i in range(len(test_images)):
        test_label[i][test_labels[i]] = 1
    Regression = LogisticRegression(0.001, 256, 1000)
    Regression.gredient_decent(train_images, train_label, test_images, test_label, lamda=0)
    Regression.draw_loss()
    Regression.draw_acc()
    acc = Regression.evaluate(test_images, test_label)
    print(f"Accuracy: {acc}")

if __name__ == '__main__':
    # regression()
    
    random_seed(10)
    train_images, train_labels, test_images, test_labels = data_prepare()
    train_label = np.zeros((train_images.shape[0], 10))
    test_label = np.zeros((test_images.shape[0], 10))
    train_images = train_images[:60000]
    for i in range(len(train_images)):
        train_label[i][train_labels[i]] = 1
    for i in range(len(test_images)):
        test_label[i][test_labels[i]] = 1
    train_data = list(zip(train_images, train_label))
    test_data = list(zip(test_images, test_label))
    # 构建神经网络
    mlp = MLP([784, 100, 10],3)
    mlp.train(train_data, 100, 256, 0.1, test_data)
    acc = mlp.evaluate(test_data)
    print(f"Accuracy: {acc}")

    # X_train, X_test, y_train, y_test = iris()
    # train_label = np.zeros((X_train.shape[0], 3))
    # test_label = np.zeros((X_test.shape[0], 3))
    # for i in range(len(X_train)):
    #     train_label[i][int(y_train[i])] = 1
    # for i in range(len(X_test)):
    #     test_label[i][int(y_test[i])] = 1
    # train_data = list(zip(X_train, train_label))
    # test_data = list(zip(X_test, test_label))
    # # 构建神经网络
    # mlp = MLP([4, 50, 3], 3)
    # mlp.train(train_data, 100, 120, 0.01)
    # acc = mlp.evaluate(test_data)
    # print(f"Accuracy: {acc}")

