import numpy as np
from sklearn.decomposition import PCA
from sklearn.svm import SVC
from sklearn.metrics import accuracy_score
import matplotlib.pyplot as plt

from file_opertions import data_prepare


def accuracy(test_labels, predicted_labels, k):
    accuracy = accuracy_score(test_labels, predicted_labels)
    print(f"PCA降为{k}维后，准确率为：{accuracy}")

    return accuracy
def pca_classify(train_images, train_labels, test_images, test_labels, k):
    pca = PCA(n_components=k)
    train_images_pca = pca.fit_transform(train_images)
    test_images_pca = pca.transform(test_images)

    # svm分类
    svm_classifier = SVC()
    svm_classifier.fit(train_images_pca, train_labels)
    predicted_labels = svm_classifier.predict(test_images_pca)

    # 正确率评估
    a = accuracy(test_labels, predicted_labels, k)
    return a

def main():
    x = np.load('k.npy')
    print(x)
    train_images, train_labels, test_images, test_labels = data_prepare()
    x = np.zeros((50,2))
    for k in range(1,50):
        acc = pca_classify(train_images, train_labels, test_images, test_labels, k)
        x[k,0] = k
        x[k,1] = acc
    np.save('k.npy',x)

if __name__ == '__main__':
    main()
