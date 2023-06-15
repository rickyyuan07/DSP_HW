# coding=Big5
#!/usr/bin/python3
import sys
# with open("Big5-ZhuYin.map", "r", encoding='big5hkscs') as f:
with open(sys.argv[1], "r", encoding='big5hkscs') as f:
    data = f.readlines()

mapping = {}

for line in data:
    line = line.split(' ')
    chinese = line[0]
    zhu_yin = line[1].strip().split('/')
    # single word
    if chinese not in mapping:
        mapping[chinese] = set()
        mapping[chinese].add(chinese)
    # ZhuYin
    for x in zhu_yin:
        if x[0] not in mapping:
            mapping[x[0]] = set()
            mapping[x[0]].add(chinese)
        else:
            mapping[x[0]].add(chinese)

# with open("ZhuYin-Big5.map", "w", encoding='big5hkscs') as f:
with open(sys.argv[2], "w", encoding='big5hkscs') as f:
    for zhuyin in mapping:
        s = " ".join(str(x) for x in mapping[zhuyin])
        print(f"{zhuyin}\t{s}", file=f)
