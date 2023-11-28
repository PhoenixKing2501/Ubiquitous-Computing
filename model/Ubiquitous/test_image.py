from torchvision import transforms
from model import ConvNeuralNet
import torch
from PIL import Image

checkpoint_path = 'checkpoint/checkpoint.pth'
classes = [
    'Asphalt Good',
    'Asphalt Regular',
    'Asphalt Bad',
    'Paved Good',
    'Paved Bad',
    'Unpaved Good',
    'Unpaved Bad',
]
device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
transform = transforms.Compose([
    transforms.Resize((256, 256)),
    transforms.ToTensor(),
    transforms.Normalize(mean=[0.4515, 0.4711, 0.4367],
                         std=[0.2504, 0.2435, 0.2782])
])

model = ConvNeuralNet(len(classes))
model.load_state_dict(torch.load(checkpoint_path))
model = model.to(device)
model.eval()


def test(image_path: str):
    img = Image.open(image_path)
    img = transform(img).float()  # type: ignore
    img = img.unsqueeze(0)
    img = img.to(device)

    pred = ""

    with torch.no_grad():
        output = model(img)
        label = torch.argmax(output)

        print(f'Prediction: {classes[label]}')
        pred = classes[label]

    return pred

# END main()


if __name__ == '__main__':
    image_path = r"D:\Code\Python\Ubiquitous\Test\06_paved(Bad)\000000000.png"
    test(image_path=image_path)
