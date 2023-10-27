import numpy as np
from sklearn.decomposition import PCA
from sklearn.svm import SVC
from sklearn.metrics import accuracy_score
from sklearn.metrics import confusion_matrix
import matplotlib.pyplot as plt
import seaborn as sns

from file_opertions import data_prepare
from RPCA import robust_pca as rpca

# 原图可视化
def image_visualize(images, labels):
    image = images[:10]
    label = labels[:10]

    _, axes = plt.subplots(nrows=2, ncols=5, figsize=(10, 4))

    for i, ax in enumerate(axes.flat):
        ax.imshow(image[i].reshape(28, 28), cmap='gray')
        ax.axis('off')
        ax.text(0.5, -0.15, "Label:"+str(label[i]), transform=ax.transAxes, color='black', fontsize=12, ha='center')

    plt.suptitle('MNIST Orginal Images With Labels', fontsize=16)
    plt.show()

# 降维后图片可视化
def visualize_images_with_labels(images, true_labels, predicted_labels):
    image = images[:10]
    true_label = true_labels[:10]
    predicted_label = predicted_labels[:10]

    _, axes = plt.subplots(2, 5, figsize=(10, 4))

    for i, ax in enumerate(axes.flat):
        ax.imshow(image[i].reshape(28, 28), cmap='gray')
        ax.axis('off')
        ax.text(0.5, -0.1, f'True: {true_label[i]}', transform=ax.transAxes,
                horizontalalignment='center', verticalalignment='center', color='black')
        ax.text(0.5, -0.2, f'Predicted: {predicted_label[i]}', transform=ax.transAxes,
                horizontalalignment='center', verticalalignment='center', color='red')
    plt.suptitle('MNIST Images With Labels after PCA', fontsize=16)
    plt.show()

# 二维散点图
def point_visualize_2(images, labels):
    plt.scatter(images[:, 0], images[:, 1], c=labels, cmap='rainbow')
    plt.xlabel('Principal Component 1')
    plt.ylabel('Principal Component 2')
    plt.colorbar(label='Digit')
    plt.title('PCA 2D Visualization')
    plt.show()

# 三维散点图
def point_visualize_3(images, labels):
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    ax.scatter(images[:, 0], images[:, 1], images[:, 2], c=labels, cmap='rainbow')
    ax.set_xlabel('Principal Component 1')
    ax.set_ylabel('Principal Component 2')
    ax.set_zlabel('Principal Component 3')
    ax.set_title('PCA 3D Visualization')
    plt.show()

# 混淆矩阵
def matrix_visualize(true_labels, predicted_labels):
    cm = confusion_matrix(true_labels, predicted_labels)

    plt.figure(figsize=(10, 8))
    sns.heatmap(cm, annot=True, cmap='Blues', fmt='d', square=True, xticklabels=range(10), yticklabels=range(10))
    plt.xlabel('Predicted Label')
    plt.ylabel('True Label')
    plt.title('Confusion Matrix')
    plt.show()

# 预测正确率计算
def accuracy(test_labels, predicted_labels, k):
    accuracy = accuracy_score(test_labels, predicted_labels)
    if k == -1:
        print(f"RPCA降维后，准确率为：{accuracy}")
    elif k < 1:
        print(f"PCA保留信息量为{k*100}%后，准确率为：{accuracy}")
    elif k > 1:
        print(f"PCA降至{k}维后，准确率为：{accuracy}")
    return accuracy

# PCA分类
def pca_classify(train_images, train_labels, test_images, test_labels, k):
    pca = PCA(n_components=k)
    train_images_pca = pca.fit_transform(train_images)
    if k < 1:
        print(f"当保留信息量为{k*100}%时，主成分维数是{train_images_pca.shape[1]}")
    test_images_pca = pca.transform(test_images)


    # 降维效果可视化
    if train_images_pca.shape[1] >= 2 :
        point_visualize_2(test_images_pca, test_labels)
    if train_images_pca.shape[1] >= 3 :
        point_visualize_3(test_images_pca, test_labels)

    # svm分类
    svm_classifier = SVC()
    svm_classifier.fit(train_images_pca, train_labels)
    predicted_labels = svm_classifier.predict(test_images_pca)

    # 保留主成分图像可视化
    components = pca.components_
    images_to_show = np.dot(test_images_pca, components)
    visualize_images_with_labels(images_to_show, test_labels, predicted_labels)

    # 正确率评估
    a = accuracy(test_labels, predicted_labels, k)
    matrix_visualize(test_labels, predicted_labels)
    return a

def rpca_classify(train_images, train_labels, test_images, test_labels, i):

    train_images_rpca, noise_train = rpca(train_images)
    np.save(f'./output/low_rank_train_{i}.npy', train_images_rpca)
    np.save(f'./output/noise_train_{i}.npy', noise_train)

    test_images_rpca, noise_test = rpca(test_images)
    np.save(f'./output/low_rank_test_{i}.npy', test_images_rpca)
    np.save(f'./output/noise_test_{i}.npy', noise_test)

    # svm分类
    svm_classifier = SVC()
    svm_classifier.fit(train_images_rpca, train_labels)
    predicted_labels = svm_classifier.predict(test_images_rpca)

    # 正确率评估
    accuracy(test_labels, predicted_labels, k=-1)
    matrix_visualize(test_labels, predicted_labels)

def show():
    image = np.load('low_rank_train.npy')
    low_rank_uint8 = np.clip(image, 0, 255).astype(np.uint8)
    _, axes = plt.subplots(2, 5, figsize=(10, 4))

    for i, ax in enumerate(axes.flat):
        ax.imshow(low_rank_uint8[i].reshape(28, 28), cmap='gray')

    plt.suptitle('MNIST Images With Labels after PCA', fontsize=16)
    plt.show()

# 掩膜噪声
def noise_add_1(images):
    mask = np.random.choice([0, 1], size=images.shape, p=[0.9, 0.1])

    # 将掩膜应用于图像
    noisy_image = images.copy()
    noisy_image[mask == 1] = 255  # 将掩膜为1的像素设为白色

    return noisy_image

def noise_add_2(images):
    mean = 0
    stddev = 30
    noisy_image = images + np.random.normal(mean, stddev, images.shape)
    noisy_image = np.clip(noisy_image, 0, 255).astype(np.uint8)
    return noisy_image

def main():
    # 数据准备
    train_images, train_labels, test_images, test_labels = data_prepare()
    k = 0.9
    pca_classify(train_images, train_labels, test_images, test_labels, k)

    # 不加噪声
    np.save(f'./output/train_images_0.npy', train_images)
    np.save(f'./output/test_images_0.npy', test_images)
    rpca_classify(train_images, train_labels, test_images, test_labels, 0)
    # 掩膜噪声
    train_images_n1 = noise_add_1(train_images)
    test_images_n1 = noise_add_1(test_images)
    np.save(f'./output/train_images_1.npy', train_images_n1)
    np.save(f'./output/test_images_1.npy', test_images_n1)
    rpca_classify(train_images_n1, train_labels, test_images_n1, test_labels, 1)
    # 高斯噪声
    train_images_n2 = noise_add_2(train_images)
    test_images_n2 = noise_add_2(test_images)
    np.save(f'./output/train_images_2.npy', train_images_n2)
    np.save(f'./output/test_images_2.npy', test_images_n2)
    rpca_classify(train_images_n2, train_labels, test_images_n2, test_labels, 2)


if __name__ == '__main__':
    main()



