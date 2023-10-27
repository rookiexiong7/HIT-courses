import numpy as np
import matplotlib.pyplot as plt
import os

def show():
    # 在此处修改文件路径，以展示不同的结果
    image = np.load('./output/noise_train_0.npy')
    low_rank_uint8 = np.clip(image, 0, 255).astype(np.uint8)
    _, axes = plt.subplots(2, 5, figsize=(10, 4))

    for i, ax in enumerate(axes.flat):
        ax.imshow(low_rank_uint8[i].reshape(28, 28), cmap='gray')

    plt.suptitle('MNIST Images With Labels after PCA', fontsize=16)
    plt.show()

# 照片输出为png文件
def output():
    output_dir = './output/images'
    os.makedirs(output_dir, exist_ok=True)
    for str in ['train_images_0','low_rank_train_0', 'noise_train_0','train_images_1','low_rank_train_1', 'noise_train_1', 'train_images_2', 'low_rank_train_2', 'noise_train_2']:
        image = np.load('./output/'+str+'.npy')
        for i in range(8):
            plt.imshow(image[i].reshape(28, 28), cmap='gray')  # 使用灰度颜色映射
            plt.axis('off')  # 隐藏坐标轴
            # 构建保存路径
            image_path = os.path.join(output_dir, f"{str}_{i + 1}.png")
            # 保存图像
            plt.savefig(image_path)
            # 清除当前图像
            plt.clf()

def main():
    #output()
    show()

if __name__ == '__main__':
    main()