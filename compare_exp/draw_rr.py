import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.font_manager import FontProperties

font_prop = FontProperties(family='sans-serif', size=16)

# 定义Excel文件名
excel_files = ['data_bf.xlsx', 'data_norm.xlsx', 'data_omni.xlsx', 'data_sliding.xlsx', 'data_cm.xlsx']
name = ['MicroSketch_with_bf','MicroSketch','Omni Window','Sliding Sketch','CM sketch']
# 存储每个文件的mem和avrg_aae数据
data = {}

# 为每个文件创建DataFrame并提取所需数据
for file in excel_files:
    df = pd.read_excel(file)
    # 选择第1到第7行
    selected_data = df.iloc[-7:, :]
    # 提取mem和avrg_aae列
    #mem = np.log2(selected_data['mem'].values)
    mem = selected_data['mem'].values
    avrg_aae = np.log10(selected_data['avrg_aae'].values)
    avrg_are = np.log10(selected_data['avrg_are'].values)
    avrg_pr = selected_data['avrg_pr'].values
    avrg_rr = selected_data['avrg_rr'].values
    data[file] = (mem, avrg_rr)

# 设置绘图
plt.figure(figsize=(10, 6))
i = 0
# 为每个文件绘制曲线
for file, (mem, avrg_rr) in data.items():
    plt.plot(mem, avrg_rr, label=name[i], marker='o')  # 'o' 是标记的类型
    i += 1

# 添加图例
plt.legend()

# 添加标题和轴标签
plt.xlabel('Memory(KB)',fontproperties=font_prop)
plt.ylabel('RR',fontproperties=font_prop)

# 显示图形
plt.savefig('./result/tmp_rr.png',dpi=350)