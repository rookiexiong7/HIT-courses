import torch
from ltp import LTP

ltp = LTP("./ltp_small")  

# 将模型移动到 GPU 上
if torch.cuda.is_available():
    ltp.to("cuda")

# 分词 cws、词性 pos、命名实体标注 ner、语义角色标注 srl、依存句法分析 dep、语义依存分析树 sdp、语义依存分析图 sdpg
output = ltp.pipeline(["刘挺老师出席了在北京举行的国际会议。"], tasks=["cws", "pos", "ner", "srl", "dep", "sdp", "sdpg"])

print(output.ner)
