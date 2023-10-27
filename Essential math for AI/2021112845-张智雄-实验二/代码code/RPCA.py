import numpy as np
from numpy.linalg import norm, svd

def robust_pca(matrix, lambda_=None, mu=None, p=None, max_iter=1000, tol=1e-7):

    # 获取矩阵的维度
    m, n = matrix.shape

    # 初始化变量
    low_rank = np.zeros((m, n)).astype('uint8')
    sparse = np.zeros((m, n)).astype('uint8')
    Y = matrix
    norm_two = norm(Y.ravel(), 2)

    # 如果未提供 lambda_、mu和p，则设置默认值
    if lambda_ is None:
        lambda_ = 1 / np.sqrt(max(m, n))
    if mu is None:
        mu = 1.25 / norm_two
    if p is None:
        p = 1.5

    norm_inf = norm(Y.ravel(), np.inf) / lambda_
    dual_norm = np.max([norm_two, norm_inf])
    dnorm = norm(matrix, 'fro')
    Y = Y / dual_norm

    # 奇异值阈值（SVT）运算符
    def svt(x, epsilon):
        u, s, v = svd(x, full_matrices=False)
        s = soft_threshold(s, epsilon)  # 使用软阈值函数处理奇异值向量
        return u @ np.diag(s) @ v

    # 循环迭代执行交替方向乘子法（ADMM）
    for iteration in range(max_iter):
        print(f"第{iteration}次迭代")

        # 更新低秩成分，使用奇异值分解（SVD）
        low_rank = svt(matrix - sparse + (1 / mu) * Y, 1 / mu)

        # 更新稀疏成分，使用软阈值处理
        sparse = soft_threshold(matrix - low_rank + (1 / mu) * Y, lambda_ / mu)

        # 更新拉格朗日乘子矩阵
        Y += mu * (matrix - low_rank - sparse)
        mu *= p

        # 检查收敛性

        if (norm(matrix - low_rank - sparse, 'fro') / dnorm) < tol:
            break

    return low_rank, sparse

# 软阈值处理函数
def soft_threshold(x, epsilon):

    return np.maximum(x - epsilon, 0) - np.maximum(-x - epsilon, 0)
