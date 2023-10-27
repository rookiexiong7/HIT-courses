import torch
import torch.nn as nn
import torch.optim as optim
from torchvision import datasets, transforms
import matplotlib.pyplot as plt

class Net(nn.Module):
    def __init__(self):
        super(Net, self).__init__()
        self.fc = nn.Linear(784, 10)

    def forward(self, x):
        x = x.view(-1, 784)
        x = self.fc(x)
        return x

# 重置模型参数
def weight_reset(m):
    if isinstance(m, nn.Linear):
        m.reset_parameters()

transform = transforms.Compose([transforms.ToTensor(), transforms.Normalize((0.1307,), (0.3081,))])
train_dataset = datasets.MNIST('./data/', train=True, download=False, transform=transform)
train_loader = torch.utils.data.DataLoader(train_dataset, batch_size=64, shuffle=True)


model = Net()
optimizers = [optim.SGD(model.parameters(), lr=0.01),
              optim.Adam(model.parameters(), lr=0.01),
              optim.RMSprop(model.parameters(), lr=0.01),
              optim.AdamW(model.parameters(), lr=0.01),
              optim.ASGD(model.parameters(), lr=0.01),
              optim.Adagrad(model.parameters(), lr=0.01),
              optim.Adadelta(model.parameters(), lr=0.01),
              ]
num_epochs = 10
losses = []

for optimizer in optimizers:
    optimizer_losses = []
    # 计算未处理损失
    model.apply(weight_reset)
    with torch.no_grad():
        for data, target in train_loader:
            output = model(data)
            loss = nn.CrossEntropyLoss()(output, target)
            initial_loss = loss.item()
            break
    print(initial_loss)
    optimizer_losses.append(initial_loss)

    for epoch in range(num_epochs):
        epoch_loss = 0.0
        for batch_idx, (data, target) in enumerate(train_loader):
            optimizer.zero_grad()
            output = model(data)
            loss = nn.CrossEntropyLoss()(output, target)
            loss.backward()
            optimizer.step()
            epoch_loss += loss.item()

        epoch_loss /= len(train_loader)
        optimizer_losses.append(epoch_loss)
        print(f'Epoch {epoch + 1}/{num_epochs}, Loss: {epoch_loss}')

    losses.append(optimizer_losses)

# 绘制损失曲线
colors = ['red', 'blue', 'green', 'orange', 'brown', 'pink', 'gray']
optim_names = ['SGD', 'Adam', 'RMSprop', 'AdamW', 'ASGD', 'Adagrad', 'Adadelta']

plt.figure()
for i, loss_values in enumerate(losses):
    plt.plot(loss_values, color=colors[i], label=optim_names[i])

plt.xlabel('Epochs')
plt.ylabel('Loss')
plt.title('Training Loss')
plt.legend()
plt.show()
