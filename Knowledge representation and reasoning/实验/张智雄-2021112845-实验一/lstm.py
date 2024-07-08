import jieba
import torch
import torch.nn as nn
from tqdm import tqdm
from torch.utils.data import Dataset, DataLoader
from collections import Counter
import torch.optim as optim

jieba.initialize()

device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

# Language Modelding 自回归训练
class TextDataset(Dataset):
    def __init__(self, text_ids, seq_length):
        self.text_ids = text_ids
        self.seq_length = seq_length

    def __len__(self):
        return len(self.text_ids) - self.seq_length

    def __getitem__(self, idx):
        return (torch.tensor(self.text_ids[idx: idx + self.seq_length]),
                torch.tensor(self.text_ids[idx + 1: idx + 1 + self.seq_length]))
    
class LSTMModel(nn.Module):
    def __init__(self, vocab_size, embedding_dim, hidden_dim):
        super(LSTMModel,self).__init__()
        self.hidden_dim = hidden_dim
        self.embedding = nn.Embedding(vocab_size, embedding_dim)         # 嵌入层将词索引转换为密集的嵌入向量
        self.lstm = nn.LSTM(embedding_dim, hidden_dim, batch_first=True) # LSTM层
        self.lm_head = nn.Linear(hidden_dim, vocab_size)                 # 线性层，将输出映射回词表大小向量
        
    def forward(self, x, hidden):
        embeds = self.embedding(x)
        lstm_out, hidden = self.lstm(embeds, hidden)
        output = self.lm_head(lstm_out)
        return output, hidden
    
    def init_hidden(self, batch_size):
        return (torch.zeros(1, batch_size, self.hidden_dim).to(device),
                torch.zeros(1, batch_size, self.hidden_dim).to(device))

def preprocess(file_path, vocab_size):
    with open(file_path, 'r', encoding='gb2312',errors='ignore') as f:
        text = f.read()
    words = list(jieba.cut(text))
    vocab = dict(Counter(words).most_common(vocab_size - 1))
    # Add <unk> token to the vocabulary for OOV words as 0 index
    vocab['<unk>'] = len(words) - sum(vocab.values())
    idx2word = list(vocab.keys())
    word2idx = {word: idx for idx, word in enumerate(idx2word)}
    text_ids = [word2idx.get(word, word2idx['<unk>']) for word in words]

    return text_ids, idx2word, word2idx

def train(model, data_loader, criterion, optimizer, num_epochs):
    model.train()
    losses = []
    for epoch in range(num_epochs):
        model.train()
        with tqdm(enumerate(data_loader), desc=f"Epoch {epoch+1}", total=len(data_loader)) as pbar:
            for i, (x, y) in pbar:
                batch_size = x.size(0)
                hidden = model.init_hidden(batch_size)    
                x = x.to(device)
                y = y.to(device)
                hidden = tuple([each.data for each in hidden])
                optimizer.zero_grad()
                output, hidden = model(x, hidden)
                loss = criterion(output.transpose(1, 2), y)
                loss.backward()
                optimizer.step()
                losses.append(loss.item())
                pbar.set_postfix(loss=loss.item())  

        print(f'Epoch {epoch + 1}, Loss: {loss.item()}')

        if loss.item() < 3:
            break
    torch.save(model.state_dict(), 'ckpt/rnn/lstm_model.pth')
    return losses

def draw_loss(losses):
    import matplotlib.pyplot as plt
    plt.plot(losses)
    plt.xlabel('Steps')
    plt.ylabel('Loss')
    plt.title('LSTM Loss Curve')
    plt.show()

file_path = './data/text.txt'
num_epochs = 10
vocab_size = 30000
seq_length = 30
embedding_dim = 64
hidden_dim = 256
batch_size = 64

if __name__ == '__main__':
    vectorized_text, id2word, word2idx = preprocess(file_path, vocab_size)
    dataset = TextDataset(vectorized_text, seq_length)
    data_loader = DataLoader(dataset, batch_size=batch_size, shuffle=True)

    model = LSTMModel(vocab_size, embedding_dim, hidden_dim).to(device)
    criterion = nn.CrossEntropyLoss()
    optimizer = optim.Adam(model.parameters())
    losses = train(model, data_loader, criterion, optimizer, num_epochs)
    draw_loss(losses)
