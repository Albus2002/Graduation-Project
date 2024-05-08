import pandas as pd

# 假设文本文件名为data.txt
filenames = ['output_bf.txt','output.txt','output_omni.txt','output_cm.txt','output_sliding.txt']
unit = [7,6,5,5,5]
excel_filenames = ['data_bf.xlsx','data_norm.xlsx','data_omni.xlsx','data_cm.xlsx','data_sliding.xlsx']

for i in range(0,5):
    # 初始化一个空列表来存储解析后的数据
    data_list = []

    # 读取文本文件并解析每一行
    with open(filenames[i], 'r') as file:
        record = {}  # 初始化一个字典来存储一个记录的所有属性和值
        t = 0
        for line in file:
            t += 1
            line = line.strip()  # 去除每行的前后空白字符
            line = line.rstrip('\n')
            if line:  # 确保不处理空行
                # 分割属性和值，属性前的冒号需要处理掉
                for item in line.split(','):
                    key, value = item.split(': ')
                    # 转换数值属性为相应的数据类型
                    if key in ['mem', 'WIN', 'T', 'size_k', 'H', 'log_base', 'bf_num', 'bf_rate','l','w','d']:
                        value = int(value)
                    else:
                        value = float(value) if '.' in value else value  # 检查是否有小数点来决定转换为float还是保持字符串
                    record[key] = value
                    print(key)
                    print(value)
            if t == unit[i]:
                # 当遇到空行时，表示一个记录结束，将其添加到列表中
                if record:
                    data_list.append(record)
                    record = {}  # 重置字典为下一个记录做准备
                t = 0

    # 将解析后的数据转换为DataFrame
    df = pd.DataFrame(data_list)
    print(df)

    # 确保列的顺序，可以手动指定列的顺序
    columns_order = ['mem', 'WIN', 'T', 'size_k', 'H', 'log_base', 'bf_num', 'bf_rate', 'avrg_aae', 'avrg_are', 'avrg_pr', 'avrg_rr']
    df = df[columns_order]

    # 将DataFrame导出为Excel文件

    df.to_excel(excel_filenames[i], index=False)

    print(f'Excel file {excel_filenames[i]} has been created.')