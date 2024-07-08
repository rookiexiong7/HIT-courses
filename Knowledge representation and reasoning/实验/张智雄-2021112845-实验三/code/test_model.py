import os
import json
import torch
import numpy as np
from config import NerConfig
from torch.utils.data import DataLoader
from transformers import BertTokenizer
from data_loader import NerDataset
from model import BertNer
from tqdm import tqdm
from seqeval.metrics import classification_report

def create_data_loader(data, args, tokenizer):
    dataset = NerDataset(data, args, tokenizer)
    return DataLoader(dataset, shuffle=False, batch_size=args.dev_batch_size, num_workers=2)


def load_model(args, device):

    model = BertNer(args)
    model.load_state_dict(torch.load(os.path.join(args.output_dir, "pytorch_model_ner.bin")))
    model.to(device)
    model.eval()
    return model


def predict(model, data_loader, id2label, device):
    
    predictions = []
    for batch in tqdm(data_loader):
        batch = {key: value.to(device) for key, value in batch.items()}
        input_ids = batch["input_ids"]
        attention_mask = batch["attention_mask"]

        with torch.no_grad():
            output = model(input_ids, attention_mask)
            logits = output.logits

        attention_mask = attention_mask.detach().cpu().numpy()
        for i in range(input_ids.size(0)):
            length = sum(attention_mask[i])
            logit = [id2label[id_] for id_ in logits[i][1:length]]
            predictions.append(logit)
    return predictions


def extract_entities(predictions, test_data):
    """Extract named entities from predictions."""
    all_entities, all_labels = [], []
    for prediction, data in zip(predictions, test_data):
        entities, labels = [], []
        entity, label = "", ""
        for char, tag in zip(data['text'], prediction):
            if tag == 'O':
                if entity:
                    entities.append(entity)
                    labels.append(label)
                    entity, label = "", ""
            elif tag.startswith('B'):
                if entity:
                    entities.append(entity)
                    labels.append(label)
                entity, label = char, tag[2:]
            elif tag.startswith('I') and label == tag[2:]:
                entity += char
        if entity:
            entities.append(entity)
            labels.append(label)
        all_entities.append(entities)
        all_labels.append(labels)
    return all_entities, all_labels


def print_entities(entities, labels):
    """Print the extracted entities and their labels."""
    for entity_list, label_list in zip(entities, labels):
        for entity, label in zip(entity_list, label_list):
            print(f"{entity}: {label}", end='\t')
        print()


if __name__ == "__main__":
    data_name = "duie_data"
    args = NerConfig(data_name)

    with open(os.path.join(args.output_dir, "ner_args.json"), "w") as fp:
        json.dump(vars(args), fp, ensure_ascii=False, indent=2)

    tokenizer = BertTokenizer.from_pretrained(args.bert_dir)
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

    text_list = [
        '《民航客运服务会话》是1995年中国民航出版社出版的图书，作者是周石田',
        '再有之后的《半生缘》，蒋勤勤饰演的顾曼璐完全把林心如的曼桢衬得像是涉世未深的小姑娘，毫无半点风情',
        '裴友生，男，汉族，湖北蕲春人，1957年12月出生，大专学历',
        '吴君如演的周吉是电影《花田喜事》，在周吉大婚之夜，其夫林嘉声逃走失踪，后来其夫新科状元高中回来，周吉急往城楼相识，但林嘉声却言夫妻情断，覆水难收'
    ]
    
    test_data = [{'text': [char for char in text], 'labels': ['O' for _ in text], 'id': 0} for text in text_list]
    test_loader = create_data_loader(test_data, args, tokenizer)
    model = load_model(args, device)
    predictions = predict(model, test_loader, args.id2label, device)
    entities, labels = extract_entities(predictions, test_data)
    for text, prediction in zip(text_list, predictions):
        print("原文本：" + text)
        print("预测结果："+ str(prediction))
    print("\n实体抽取结果：")
    print_entities(entities, labels)

    # # test model on test data
    # test_data_path = os.path.join(os.path.join(args.data_dir, data_name), "test.json")
    # import json
    # test_data = []
    # with open(test_data_path, "r", encoding="utf-8") as fp:
    #     for line in fp:
    #         data = json.loads(line)
    #         data['text'] = [char for char in data['text']]
    #         data['labels'] = ['O' for _ in data['text']]
    #         data['id'] = 0
    #         test_data.append(data)

    # test_loader = create_data_loader(test_data, args, tokenizer)
    # model = load_model(args, device)
    # predictions = predict(model, test_loader, args.id2label, device)
    # entities, labels = extract_entities(predictions, test_data)
    # print_entities(entities, labels)
    # # 输出为txt文件
    # with open("test_entities.txt", "w", encoding="utf-8") as fp:
    #     for entity_list, label_list in zip(entities, labels):
    #         for entity, label in zip(entity_list, label_list):
    #             fp.write(f"{entity}: {label}\t")
    #         fp.write("\n")