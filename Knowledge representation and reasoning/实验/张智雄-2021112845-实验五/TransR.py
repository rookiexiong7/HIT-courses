import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.nn.init as init

class TransR(nn.Module):
    def __init__(self, ent_num, rel_num, device, dim_e=200, dim_r=200, norm=1, margin=4.0, alpha=0.01):
        super(TransR, self).__init__()
        self.ent_num = ent_num
        self.rel_num = rel_num
        self.device = device
        self.dim_e = dim_e  # 实体嵌入维度
        self.dim_r = dim_r  # 关系嵌入维度
        self.norm = norm  # 使用L1范数还是L2范数
        self.margin = margin
        self.alpha = alpha

        # 初始化实体和关系表示向量
        self.ent_embeddings = nn.Embedding(self.ent_num, self.dim_e)
        init.xavier_uniform_(self.ent_embeddings.weight.data)
        self.rel_embeddings = nn.Embedding(self.rel_num, self.dim_r)
        init.xavier_uniform_(self.rel_embeddings.weight.data)
        # 初始化关系特定投影矩阵
        self.transfer_matrix = nn.Embedding(self.rel_num, self.dim_r * self.dim_e)
        init.xavier_uniform_(self.transfer_matrix.weight.data)

        self.dist_fn = nn.PairwiseDistance(p=self.norm) # the function for calculating the distance 
        self.criterion = nn.MarginRankingLoss(margin=self.margin)

    def _transfer(self, ent_embs, proj_matrix):        
        batch = ent_embs.size(0)
        rels_transfer = proj_matrix.view(batch, self.dim_e, self.dim_r)  # [batch, ent_dim, rel_dim]
        ent_embs = ent_embs.view(batch, 1, self.dim_e)  # [batch, 1, ent_dim]
        ent_proj = torch.matmul(ent_embs, rels_transfer)  # [batch, 1, rel_dim]
        return ent_proj.view(batch, self.dim_r)  # [batch, rel_dim]
    
    def get_ent_resps(self, ent_idx):
        return self.ent_embeddings(ent_idx)

    # score
    def distance(self, h_idx, r_idx, t_idx):
        h_embs = self.ent_embeddings(h_idx)
        r_embs = self.rel_embeddings(r_idx)
        t_embs = self.ent_embeddings(t_idx)
        proj_matrices = self.transfer_matrix(r_idx)

        h_embs_proj = self._transfer(h_embs, proj_matrices)
        t_embs_proj = self._transfer(t_embs, proj_matrices)
        scores = h_embs_proj + r_embs - t_embs_proj 
        
        norms = (torch.mean(h_embs.norm(p=self.norm, dim=1) - 1.0)
                 + torch.mean(r_embs ** 2)
                 + torch.mean(t_embs.norm(p=self.norm, dim=1) - 1.0)) / 3
        return scores.norm(p=self.norm, dim=1), norms

    def _cal_margin_base_loss(self, pos_distances, neg_distances):
        ones = torch.tensor([-1], dtype=torch.float, device=self.device)
        return self.criterion(pos_distances, neg_distances, ones)
    
    # 正则项
    def _cal_scale_loss(self, embedding: nn.Embedding):
        norm = torch.norm(embedding.weight, p=2, dim=1)  # the L2 norm of entity embedding, size: [ent_num, ]
        scale_loss = torch.sum(F.relu(norm - 1))
        return scale_loss
    
    def loss(self, positive_distances, negative_distances):
        margin_based_loss = self._cal_margin_base_loss(positive_distances, negative_distances)
        ent_scale_loss = self._cal_scale_loss(self.ent_embeddings)
        rel_scale_loss = self._cal_scale_loss(self.rel_embeddings)
        return margin_based_loss + self.alpha * ((ent_scale_loss + rel_scale_loss) / (self.ent_num + self.rel_num))
    
    def forward(self, pos_triplets, neg_triplets):
        ph_idx, pr_idx, pt_idx = pos_triplets[:, 0], pos_triplets[:, 1], pos_triplets[:, 2]
        nh_idx, nr_idx, nt_idx = neg_triplets[:, 0], neg_triplets[:, 1], neg_triplets[:, 2]
        pos_distances, pos_norms = self.distance(ph_idx, pr_idx, pt_idx)
        neg_distances, neg_norms = self.distance(nh_idx, nr_idx, nt_idx)
        tmp_loss = self.loss(pos_distances, neg_distances)
        return tmp_loss
    
    def predict(self, h_idx, r_idx, t_idx):
        return self.distance(h_idx, r_idx, t_idx)
