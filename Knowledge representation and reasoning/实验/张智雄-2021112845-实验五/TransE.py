import torch
import torch.nn as nn
import torch.nn.functional as F

class TransE(nn.Module):
    def __init__(self, ent_num, rel_num, device, dim=100, norm=1, margin=2.0, alpha=0.01):
        super(TransE, self).__init__()
        self.ent_num = ent_num
        self.rel_num = rel_num
        self.device = device
        self.dim = dim
        self.norm = norm
        self.margin = margin
        self.alpha = alpha
        
        # 初始化实体和关系表示向量
        self.ent_embeddings = nn.Embedding(self.ent_num, self.dim)
        torch.nn.init.xavier_uniform_(self.ent_embeddings.weight.data)
        self.ent_embeddings.weight.data = F.normalize(self.ent_embeddings.weight.data, 2, 1)
        
        self.rel_embeddings = nn.Embedding(self.rel_num, self.dim)
        torch.nn.init.xavier_uniform_(self.rel_embeddings.weight.data)
        self.rel_embeddings.weight.data = F.normalize(self.rel_embeddings.weight.data, 2, 1)
        
        # 损失函数
        self.criterion = nn.MarginRankingLoss(margin=self.margin)

    def forward(self, pos_triplets, neg_triplets):
        pos_distances, pos_norms = self._distance(pos_triplets)
        neg_distances, neg_norms = self._distance(neg_triplets)
        
        target = torch.tensor([-1], dtype=torch.float, device=self.device)
        loss = self.criterion(pos_distances, neg_distances, target)
        loss += self.alpha * (pos_norms + neg_norms)
        return loss

    def _distance(self, triplets):
        h_embs = self.ent_embeddings(triplets[:, 0])  # [batch, emb]
        r_embs = self.rel_embeddings(triplets[:, 1])  # [batch, emb]
        t_embs = self.ent_embeddings(triplets[:, 2])  # [batch, emb]
        
        scores = h_embs + r_embs - t_embs
        
        norms = (torch.mean(h_embs.norm(p=self.norm, dim=1) - 1.0) +
                 torch.mean(r_embs ** 2) +
                 torch.mean(t_embs.norm(p=self.norm, dim=1) - 1.0)) / 3
        
        return scores.norm(p=self.norm, dim=1), norms

    def predict(self, triplets):
        with torch.no_grad():
            distances, _ = self._distance(triplets)
        return distances.cpu().numpy()
