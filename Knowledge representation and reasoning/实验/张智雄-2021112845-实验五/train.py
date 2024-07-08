import torch
import numpy as np
from tqdm import tqdm

from TransE import TransE
from TransH import TransH
from TransR import TransR
from preprocess import *

def generate_negative_samples(triplets, ent_num):
    neg_triplets = []
    for triplet in triplets:
        neg_triplet = triplet.copy()
        if np.random.uniform() > 0.5:
            neg_triplet[0] = np.random.randint(ent_num)
        else:
            neg_triplet[2] = np.random.randint(ent_num)
        neg_triplets.append(neg_triplet)
    return neg_triplets

def train_model(model, train_data, valid_data, epochs=500, batch_size=128 , learning_rate=0.001):
    optimizer = torch.optim.Adam(model.parameters(), lr=learning_rate)
    scheduler = torch.optim.lr_scheduler.StepLR(optimizer, step_size=50, gamma=0.1)
    train_data = torch.tensor(train_data, dtype=torch.long, device=model.device)
    valid_data = torch.tensor(valid_data, dtype=torch.long, device=model.device)
    with tqdm(total=epochs, desc=f'Training', unit='epoch') as pbar_epochs:
        for epoch in range(epochs):
            model.train()
            total_loss = 0.0
            for i in range(0, len(train_data), batch_size):
                pos_triplets = train_data[i:i + batch_size]
                neg_triplets = generate_negative_samples(pos_triplets.cpu().numpy(), model.ent_num)
                neg_triplets = torch.tensor(np.array(neg_triplets), dtype=torch.long, device=model.device)
                loss = model(pos_triplets, neg_triplets)

                optimizer.zero_grad()
                loss.backward()
                optimizer.step()
                total_loss += loss.item()

            # print(f'Epoch {epoch + 1} / {epochs}, Loss: {total_loss / len(train_data)}')
            if (epoch + 1) % 50 == 0:
                evaluate_model(model, valid_data)
            scheduler.step()
            pbar_epochs.update(1)
            pbar_epochs.set_postfix({'Average Loss': total_loss / len(train_data)})

def evaluate_model(model, test_data):
    model.eval()
    if not isinstance(test_data, torch.Tensor):
        test_data = torch.tensor(test_data, dtype=torch.long, device=model.device)
    else:
        test_data = test_data.clone().detach().to(device=model.device).to(torch.long)

    hits_at_10 = 0
    hits_at_10_head = 0
    total_rank = 0
    total_head_rank = 0
    total_tail_rank = 0
    with torch.no_grad():
        with tqdm(total=len(test_data), desc='Testing', unit='triplet') as pbar:
            for triplet in test_data:
                head, rel, tail = triplet[0], triplet[1], triplet[2]
                
                if isinstance(model, TransE):
                    head_scores = model.predict(torch.stack([torch.arange(model.ent_num, device=model.device), 
                                                            rel.repeat(model.ent_num), 
                                                            tail.repeat(model.ent_num)], dim=1))
                    tail_scores = model.predict(torch.stack([head.repeat(model.ent_num), 
                                                            rel.repeat(model.ent_num), 
                                                            torch.arange(model.ent_num, device=model.device)], dim=1))
                else:
                    head_scores = model.predict(torch.arange(model.ent_num, device=model.device), 
                                                rel.repeat(model.ent_num), 
                                                tail.repeat(model.ent_num))[0]
                    tail_scores = model.predict(head.repeat(model.ent_num), 
                                                rel.repeat(model.ent_num), 
                                                torch.arange(model.ent_num, device=model.device))[0]

                head_rank = (head_scores < head_scores[head.item()]).sum().item() + 1
                tail_rank = (tail_scores < tail_scores[tail.item()]).sum().item() + 1

                total_rank += head_rank + tail_rank
                total_head_rank += head_rank
                total_tail_rank += tail_rank
                if head_rank <= 10:
                    hits_at_10_head += 1
                if tail_rank <= 10:
                    hits_at_10 += 1
                pbar.update(1)

    hits_at_10 /= len(test_data)
    hits_at_10_head /= len(test_data)
    mean_rank = total_rank / (2 * len(test_data))
    mean_head_rank = total_head_rank / len(test_data)
    mean_tail_rank = total_tail_rank / len(test_data)
    print(f'\nHits@10:{(hits_at_10 + hits_at_10_head)/2}, Hits@10_Head: {hits_at_10}, Hits@10_Tail: {hits_at_10_head}')
    print(f'Mean Rank: {mean_rank}, Mean Head Rank: {mean_head_rank}, Mean Tail Rank: {mean_tail_rank}')
    return hits_at_10, mean_rank



if __name__ == "__main__":
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    dataset_name = 'FB15k'  # 'WN18' or 'FB15k'
    train_data, valid_data, test_data, n_n_data, ent2idx, rel2idx = load_dataset(dataset_name)
    
    model_class = TransH    # Change this to TransE, TransH, or TransR
    print(f'Using Model: {str(model_class)}')
    print(f'Using dataset: {dataset_name}')
    model = model_class(ent_num=len(ent2idx), rel_num=len(rel2idx), device=device).to(device)
    # FB15K -> epochs=100, batch_size=8192
    # WN18  -> epochs>=400, batch_size=128
    train_model(model, train_data, valid_data, epochs=100, batch_size=8192, learning_rate=0.001)
    print('\nTesting on test dataset...')
    evaluate_model(model, test_data)
    if str(dataset_name) == 'FB15k':
        evaluate_model(model, n_n_data)
