各源代码内容如下：
	file_operations.py为读取数据的一些函数；
	RPCA.py为Robust-PCA的封装函数；
	test_of_pca.py为PCA在不同k取值下的正确率对比
	show_of_rpca.py为rpca得到矩阵的文件输出函数以及展示；
	main.py为主函数，包含pca和rpca的分类使用函数；
	注：pca默认保留90%的信息，rpca分别执行无噪声、掩膜噪声、高斯噪声三种情况
其他文件包括：（在此精简版内不包含数据集和下述npy文件，另附在完整版压缩包内）
	data文件夹内为mnist数据集
	output文件夹内包含rpca输出的png文件夹images，以及M,L,S三个矩阵的npy文件
	k.npy是test_of_pca.py运行结果
	/output/images内为实际rpca运行结果。
