import torch
import torch.nn as nn

# Creating a CNN class
class ConvNeuralNet(nn.Module):
    def __init__(self, num_classes):
        super(ConvNeuralNet, self).__init__()
        self.relu = nn.ReLU()
        self.conv_layer1 = nn.Conv2d(3, 32, kernel_size=3, padding=1)
        self.max_pool1 = nn.MaxPool2d(kernel_size=2, stride=2)
        self.conv_layer2 = nn.Conv2d(32, 64, kernel_size=3, padding=1)
        self.max_pool2 = nn.MaxPool2d(kernel_size=2, stride=2)
        self.conv_layer3 = nn.Conv2d(64, 128, kernel_size=3, padding=1)
        self.max_pool3 = nn.MaxPool2d(kernel_size=2, stride=2)
        self.fc1 = nn.Linear(128*32*32, 256)
        self.fc2 = nn.Linear(256, num_classes)
        # self.softmax = nn.Softmax(dim=1)

    def forward(self, x):
        out = self.conv_layer1(x)
        out = self.relu(out)
        out = self.max_pool1(out)
        out = self.conv_layer2(out)
        out = self.relu(out)
        out = self.max_pool2(out)
        out = self.conv_layer3(out)
        out = self.relu(out)
        out = self.max_pool3(out)
        out = out.reshape(out.size(0), -1)
        out = self.fc1(out)
        out = self.relu(out)
        out = self.fc2(out)
        # out = self.softmax(out)

        return out


if __name__ == "__main__":
    model = ConvNeuralNet(10)

    arr = torch.rand(1, 3, 256, 256)
    out = model.forward(arr)

    print(out.shape)
