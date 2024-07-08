import torch
import torch.nn as nn
import torch.nn.functional as F


class TransH(nn.Module):
    def __init__(self, ent_num, rel_num, device, dim=100, norm=1, margin=4.0, alpha=0.01):
        super(TransH, self).__init__()
        self.ent_num = ent_num
        self.rel_num = rel_num
        self.device = device
        self.dim = dim
        self.norm = norm  # 使用L1范数还是L2范数
        self.margin = margin
        self.alpha = alpha

        # 初始化实体和关系表示向量
        self.ent_embeddings = nn.Embedding(self.ent_num, self.dim)
        torch.nn.init.xavier_uniform_(self.ent_embeddings.weight.data)
        self.ent_embeddings.weight.data = F.normalize(self.ent_embeddings.weight.data, p=2, dim=1)

        self.rel_embeddings = nn.Embedding(self.rel_num, self.dim)
        torch.nn.init.xavier_uniform_(self.rel_embeddings.weight.data)
        self.rel_embeddings.weight.data = F.normalize(self.rel_embeddings.weight.data, p=2, dim=1)

        # 初始化关系超平面的法向量
        self.norm_vectors = nn.Embedding(self.rel_num, self.dim)
        torch.nn.init.xavier_uniform_(self.norm_vectors.weight.data)
        self.norm_vectors.weight.data = F.normalize(self.norm_vectors.weight.data, p=2, dim=1)

        # 损失函数
        self.criterion = nn.MarginRankingLoss(margin=self.margin)

    def get_ent_resps(self, ent_idx):  # [batch]
        return self.ent_embeddings(ent_idx)  # [batch, emb]

    def transfer(self, ent_embs, norm_vecs):
        # 将实体嵌入投影到关系的超平面
        return ent_embs - torch.sum(ent_embs * norm_vecs, dim=1, keepdim=True) * norm_vecs

    def distance(self, h_idx, r_idx, t_idx):
        h_embs = self.ent_embeddings(h_idx)  # [batch, emb]
        r_embs = self.rel_embeddings(r_idx)  # [batch, emb]
        t_embs = self.ent_embeddings(t_idx)  # [batch, emb]
        norm_vecs = self.norm_vectors(r_idx)  # [batch, emb]

        # 将实体投影到关系的超平面
        h_embs_proj = self.transfer(h_embs, norm_vecs)  # [batch, emb]
        t_embs_proj = self.transfer(t_embs, norm_vecs)  # [batch, emb]

        scores = h_embs_proj + r_embs - t_embs_proj
        norms = (torch.mean(h_embs.norm(p=self.norm, dim=1) - 1.0)
                 + torch.mean(r_embs ** 2)
                 + torch.mean(t_embs.norm(p=self.norm, dim=1) - 1.0)) / 3
        return scores.norm(p=self.norm, dim=1), norms

    def loss(self, positive_distances, negative_distances):
        target = torch.tensor([-1], dtype=torch.float, device=self.device)
        return self.criterion(positive_distances, negative_distances, target)

    def forward(self, pos_triplets, neg_triplets):
        ph_idx, pr_idx, pt_idx = pos_triplets[:, 0], pos_triplets[:, 1], pos_triplets[:, 2]
        nh_idx, nr_idx, nt_idx = neg_triplets[:, 0], neg_triplets[:, 1], neg_triplets[:, 2]
        pos_distances, pos_norms = self.distance(ph_idx, pr_idx, pt_idx)
        neg_distances, neg_norms = self.distance(nh_idx, nr_idx, nt_idx)

        tmp_loss = self.loss(pos_distances, neg_distances)
        tmp_loss += self.alpha * pos_norms  # 正则化项
        tmp_loss += self.alpha * neg_norms  # 正则化项

        return tmp_loss
    
    def predict(self, h_idx, r_idx, t_idx):
        return self.distance(h_idx, r_idx, t_idx)
