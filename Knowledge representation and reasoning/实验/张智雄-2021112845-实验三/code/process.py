# -*- coding: utf-8 -*-
import os
import re
import json
import codecs
import random
import codecs
from tqdm import tqdm
from collections import defaultdict


class ProcessDuieData:
    def __init__(self):

        ## todo 定义相关文件初始地址
        self.data_path = "./data/duie_data/"
        self.train_file = self.data_path + "train.json"
        self.dev_file = self.data_path + "dev.json"
        self.test_file = self.data_path + "test.json"
        self.schema_file = self.data_path + "duie_schema.json"

    def get_ents(self):
        ents = set()
        rels = defaultdict(list)
        with open(self.schema_file, 'r', encoding="utf-8") as fp:
            lines = fp.readlines()
            for line in lines:
                data = eval(line)
                ## todo 基于模版文件 获取subject_type
                subject_type = data["subject_type"]
                ## todo 基于模版文件 获取object_type
                object_type = data["object_type"]["@value"]
                if "人物" in subject_type:
                    subject_type = "人物"
                if "人物" in object_type:
                    object_type = "人物"
                ents.add(subject_type)
                ents.add(object_type)
                predicate = data["predicate"]
                rels[subject_type + "_" + object_type].append(predicate)

        with open(self.data_path + "ner_data/labels.txt", "w", encoding="utf-8") as fp:
            fp.write("\n".join(list(ents)))

        with open(self.data_path + "re_data/rels.txt", "w", encoding="utf-8") as fp:
            json.dump(rels, fp, ensure_ascii=False, indent=2)

    def get_ner_data(self, input_file, output_file):
        res = []
        with codecs.open(input_file, 'r', encoding="utf-8", errors="replace") as fp:
            lines = fp.read().strip().split("\n")
            for i, line in enumerate(tqdm(lines)):
                try:
                    line = eval(line)
                except Exception as e:
                    continue
                tmp = {}
                text = line['text']
                tmp['text'] = [i for i in text]
                tmp["labels"] = ["O"] * len(text)
                tmp['id'] = i
                spo_list = line['spo_list']
                for j, spo in enumerate(spo_list):
                    # 从句子里面找到实体的开始位置、结束位置
                    if spo['subject'] == "" or spo['object']['@value'] == "":
                        continue
                    try:
                        # todo 找到subject在文本中的位置
                        subject_re_res = list(re.finditer(re.escape(spo['subject']), text))
                        subject_type = spo["subject_type"]
                        if "人物" in subject_type:
                            subject_type = "人物"
                    except Exception as e:
                        print(e)
                        print(spo['subject'].replace('+', '\+'), line['text'])
                        import sys
                        sys.exit(0)
                    for sbj in subject_re_res:
                        sbj_span = sbj.span()
                        sbj_start = sbj_span[0]
                        sbj_end = sbj_span[1]
                        tmp["labels"][sbj_start] = f"B-{subject_type}"
                        for j in range(sbj_start + 1, sbj_end):
                            tmp["labels"][j] = f"I-{subject_type}"
                    try:
                        # todo 找到object在文本中的位置
                        object_re_res = list(re.finditer(re.escape(spo['object']['@value']), text))
                        object_type = spo['object_type']['@value']
                        if "人物" in object_type:
                            object_type = "人物"
                    except Exception as e:
                        print(e)
                        print(line)
                        print(spo['object']['@value'].replace('+', '\+').replace('(', ''), line['text'])
                        import sys
                        sys.exit(0)
                    for obj in object_re_res:
                        obj_span = obj.span()
                        obj_start = obj_span[0]
                        obj_end = obj_span[1]
                        tmp["labels"][obj_start] = f"B-{object_type}"
                        for j in range(obj_start + 1, obj_end):
                            tmp["labels"][j] = f"I-{object_type}"
                res.append(tmp)

        with open(output_file, 'w', encoding="utf-8") as fp:
            fp.write("\n".join([json.dumps(i, ensure_ascii=False) for i in res]))


if __name__ == "__main__":
    processDuieData = ProcessDuieData()
    processDuieData.get_ents()

    ## todo 补全训练集和验证集的处理结果保存地址
    processDuieData.get_ner_data(processDuieData.train_file,
                                os.path.join(processDuieData.data_path, 'ner_data/train.txt'))
    processDuieData.get_ner_data(processDuieData.dev_file, 
                                os.path.join(processDuieData.data_path, 'ner_data/dev.txt'))
