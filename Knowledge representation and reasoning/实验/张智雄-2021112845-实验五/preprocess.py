def load_data(file_path):
    with open(file_path, 'r') as f:
        data = [line.strip().split() for line in f]

    return data

def load_2id(file_path):
    with open(file_path, 'r') as f:
        data = [line.strip().split('\t') for line in f]
    data2id = {ent: int(idx) for ent, idx in data}
    return data2id

def to_indexed_triplets(triplets, ent2idx, rel2idx):
    indexed_triplets = []
    for h, r, t in triplets:
        if h in ent2idx and t in rel2idx and r in ent2idx:
            indexed_triplets.append([int(ent2idx[h]), int(rel2idx[t]), int(ent2idx[r])])
    return indexed_triplets

def triplets(data):
    index_triplets = []
    for h in data:
        index_triplets.append([int(h[0]), int(h[2]), int(h[1])])
    return index_triplets



def load_dataset(dataset_name):
    base_path = f'{dataset_name}/'
    train_data = load_data(base_path + 'train.txt')
    valid_data = load_data(base_path + 'valid.txt')
    test_data = load_data(base_path + 'test.txt')
    n_n_data = load_data(base_path + 'n-n.txt')
    n_n_data = triplets(n_n_data[1:])

    ent2idx = load_2id(base_path + 'entity2id.txt')
    rel2idx = load_2id(base_path + 'relation2id.txt')

    train_data = to_indexed_triplets(train_data, ent2idx, rel2idx)
    valid_data = to_indexed_triplets(valid_data, ent2idx, rel2idx)
    test_data = to_indexed_triplets(test_data, ent2idx, rel2idx)

    return train_data, valid_data, test_data, n_n_data, ent2idx, rel2idx




if __name__ == '__main__':
    train_data, valid_data, test_data, n_n_data, ent2idx, rel2idx = load_dataset('FB15k')
    print(train_data)