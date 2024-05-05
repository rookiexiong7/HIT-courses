import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import itertools
import random

def random_seed(seed): 
    np.random.seed(seed)
    random.seed(seed)

def iris():
    data = pd.read_csv('./Iris.data')
    data["Species"] = data["Species"].map({"Iris-setosa": 1, "Iris-versicolor": 0, "Iris-virginica": 2})
    data = np.array(data[:].values)

    return data

def generate_data(k, count):
    means = np.zeros((k, 2))                   # 各个高斯分布的均值
    data = np.zeros((count, 3))                 # 待分类的数据
    means = [[2, 2], [-2, -2], [2, -2], [-2, 2]]
    cov = np.array([[1, 0.36], [0.36, 1]])            # 协方差矩阵
    classes = []
    index = 0
    for i in range(k):
        for j in range(int(count / k)):
            data[index, 0:2] = np.random.multivariate_normal(means[i], cov)
            data[index, 2] = i
            index += 1
            if i not in classes:
                classes.append(i)
    return means, data, classes

class K_means:
    def __init__(self, data, k):
        self.k = k
        self.data_x = data[:, 0:-1]
        self.data_y = data[:, -1]
    
    # 初始化中心点
    def init_center(self):
        self.center = np.zeros((self.k, self.data_x.shape[1]))
        for i in range(self.k):
            random = np.random.randint(0, self.data_x.shape[0])
            #random = np.random.randint(i*(self.data_x.shape[0]/self.k), (i+1)*(self.data_x.shape[0]/self.k))
            self.center[i] = (self.data_x[random])
    
    def distance(self, x, y):
        return np.sqrt(np.sum(np.square(x - y)))
    
    def update_center(self):
        self.clusters = [[] for i in range(self.k)]
        for i in range(self.data_x.shape[0]):
            min_dis = 100000
            min_index = 0
            for j in range(self.k):
                dis = self.distance(self.data_x[i], self.center[j])
                if dis < min_dis:
                    min_dis = dis
                    min_index = j
            self.clusters[min_index].append(self.data_x[i])
        for i in range(self.k):
            self.center_old = self.center
            self.center[i] = np.mean(np.array(self.clusters[i]), axis=0)
    
    def train(self, limit=100000):
        self.init_center()
        self.update_center()
        for i in range(limit):
            self.update_center()
            if np.sum(np.abs(self.center - self.center_old)) < 0.1:
                break
            self.center_old = self.center
        return self.clusters, self.center
    
    def accuary(self, classes): 
        classes = list(itertools.permutations(classes, len(classes)))
        acc = []
        for cls in classes:
            count = 0
            for i in range(self.data_x.shape[0]):
                min_dis = 100000
                min_index = 0
                for j in range(self.k):
                    dis = self.distance(self.data_x[i], self.center[j])
                    if dis < min_dis:
                        min_dis = dis
                        min_index = j
                if cls[int(self.data_y[i])] == min_index:
                    count += 1
            acc.append(count / self.data_x.shape[0])
        #print(acc)
        return max(acc)

    def draw(self):
        plt.figure()
        for i in range(self.k):
            plt.scatter(np.array(self.clusters[i])[:, 0], np.array(self.clusters[i])[:, 1])
        plt.show()

class GMM:
    def __init__(self, data, k, center=None):
        self.k = k
        self.data_x = data[:, 0:-1]
        self.data_y = data[:, -1]
        self.center = center

    def init_gauss(self):
        self.mean = []
        self.cov = []
        self.alpha = []
        # 使用k-means的结果初始化
        if self.center is not None:
            for i in range(self.k):
                self.mean.append(self.center[i])
                self.cov.append(np.eye(self.data_x.shape[1]))
                self.alpha.append(1 / self.k)
        # 随机初始化
        else:
            for i in range(self.k):
                random = np.random.randint(0, self.data_x.shape[0])
                #random = np.random.randint(i*(self.data_x.shape[0]/self.k), (i+1)*(self.data_x.shape[0]/self.k))
                self.mean.append(self.data_x[random])
                self.cov.append(np.eye(self.data_x.shape[1]))
                self.alpha.append(1 / self.k)

    def gaussian(self, x, mean, cov):
        return np.exp(-0.5 * np.dot(np.dot((x - mean), np.linalg.inv(cov)), (x - mean).T)) / (np.sqrt(np.linalg.det(cov)) * np.power(2 * np.pi, self.data_x.shape[1] / 2))
    
    def likelihood(self):
        sum = 0.0
        for k in range(self.k):
            sum += self.alpha[k] * self.gaussian(self.data_x, self.mean[k], self.cov[k])
        return np.sum(np.log(sum))
        #return np.sum(np.log(np.sum(self.alpha[k] * self.gaussian(self.data_x, self.mean[k], self.cov[k]) for k in range(self.k))))

    def E_step(self):
        self.gamma = np.zeros((self.data_x.shape[0], self.k))
        for i in range(self.data_x.shape[0]):
            for j in range(self.k):
                self.gamma[i, j] = self.alpha[j] * self.gaussian(self.data_x[i], self.mean[j], self.cov[j])
            self.gamma[i] /= np.sum(self.gamma[i])
    
    def M_step(self):
        for j in range(self.k):
            Nk = np.sum(self.gamma[:, j])
            self.mean[j] = np.sum(self.gamma[:, j].reshape(-1, 1) * self.data_x, axis=0) / Nk
            self.cov[j] = np.dot((self.data_x - self.mean[j]).T, 
                                 (self.data_x - self.mean[j]) * self.gamma[:, j].reshape(-1, 1)) / Nk
            self.alpha[j] = Nk / self.data_x.shape[0]
    
    def train(self, limit=100000):
        self.init_gauss()
        likelihood = []
        likelihood.append(self.likelihood())
        for i in range(limit):
            self.E_step()
            self.M_step()
            likelihood.append(self.likelihood())
            if np.abs(likelihood[i + 1] - likelihood[i]) < 0.1:
                break
        plt.figure()
        plt.plot(likelihood)
        plt.show()
        return self.mean, self.cov, self.alpha
    
    def accuary(self, classes):
        z = np.argmax(self.gamma, axis=1)
        classes = list(itertools.permutations(classes, len(classes)))
        acc = []
        for cls in classes:
            count = 0
            for i in range(self.data_x.shape[0]):
                if z[i] == cls[int(self.data_y[i])]:
                    count += 1
            acc.append(count / self.data_x.shape[0]) 
        #print(acc)
        return max(acc)
    
    def draw(self):
        z = np.argmax(self.gamma, axis=1)
        plt.figure()
        for i in range(self.k):
            plt.scatter(self.data_x[z == i, 0], self.data_x[z == i, 1])
        plt.show()


if __name__ == '__main__':
    
    # random_seed(177)
    # k = 4
    # count = 1000
    # means, data, classes = generate_data(k, count)
    # print(classes)

    # k_means = K_means(data, k)
    # clusters, center = k_means.train()
    # k_means.draw()
    # print("K-means正确率： ", k_means.accuary(classes))
    # print()
    # gmm = GMM(data, k, center)
    # mean, cov, alpha = gmm.train()
    # print(mean, cov, alpha)
    # gmm.draw()
    # print("GMM正确率： ", gmm.accuary(classes))

    random_seed(6)
    data = iris()
    k_means = K_means(data, 3)
    clusters, center = k_means.train()
    k_means.draw()
    print("K-means正确率： ", k_means.accuary([0, 1, 2]))
    print(center)
    gmm = GMM(data, 3, center)
    mean, cov, alpha = gmm.train()
    print(mean, cov, alpha)
    gmm.draw()
    print("GMM正确率： ", gmm.accuary([0, 1, 2]))





