interfacegan执行脚本
python code/interfacegan/edit.py
editing执行脚本
python code/editing/scripts/inference.py --images_dir=./input --save_dir=./output --edit=smile.pt ./e4e_ffhq_encode.pt 

对于interfacegan模型放置在code/interfacegan/models/pretrain目录下
对于editing模型请放置inversion模型于code/editing/目录下，同时可以选择更换不同的生成器模型，在inference.py内修改即可。

预训练模型链接
https://jht213-my.sharepoint.com/:f:/g/personal/hit_zzx_jht213_onmicrosoft_com/EvaqvEWMsQlCkot-hTs5b74B0pH2kCj2P_NEZFIBFZe1TQ?e=w3PiaX

小组成员
2021112845张智雄、2021112887李佳阳、2021111542王佳依、2021111849王诗雅