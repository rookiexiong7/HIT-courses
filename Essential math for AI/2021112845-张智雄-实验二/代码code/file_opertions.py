import numpy as np
import struct

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