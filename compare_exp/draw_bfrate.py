import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.font_manager import FontProperties

font_prop = FontProperties(family='sans-serif', size=16)

# 定义Excel文件名
file = 'data_bf.xlsx'
name = 'MicroSketch_with_bf'

# 存储每个文件的mem和avrg_aae数据
data = {}

# 为每个文件创建DataFrame并提取所需数据

df = pd.read_excel(file)
# 选择第1到第7行
selected_data = df.iloc[-7:, :]
# 提取mem和avrg_aae列
#mem = np.log2(selected_data['mem'].values)
bf_rate = selected_data['bf_rate'].values
avrg_aae = selected_data['avrg_aae'].values
avrg_are = selected_data['avrg_are'].values
avrg_pr = selected_data['avrg_pr'].values
avrg_rr = selected_data['avrg_rr'].values
data[file] = (bf_rate, avrg_are)

# 设置绘图
plt.figure(figsize=(10, 6))
plt.plot(bf_rate, avrg_are, label=name, marker='o')  # 'o' 是标记的类型

# 添加图例
plt.legend()

# 添加标题和轴标签
plt.xlabel('bf_rate',fontproperties=font_prop)
plt.ylabel('ARE',fontproperties=font_prop)

# 显示图形
plt.savefig('./result/tmp_bf_rate.png',dpi=350)