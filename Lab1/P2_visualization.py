import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# 加载之前保存的CSV文件
# df = pd.read_csv("time_10_2000.csv")
df = pd.read_csv("Problem2/data/Linux_P2_M=1e4_n=10_1e4.csv")


# 设置绘图风格
plt.style.use('seaborn-v0_8-darkgrid')

# 创建一个颜色的列表
palette = plt.get_cmap('Set1')

# 绘制多条折线图，每种优化策略一条线
plt.figure(figsize=(10, 8))

# 绘制折线图
plt.plot('n', 'naive', data=df, marker='', color=palette(1), linewidth=2, label="Naive")
plt.plot('n', 'optimize_multi_link', data=df, marker='', color=palette(2), linewidth=2, label="Optimize_multi_link")
plt.plot('n', 'optimize_recursion_func', data=df, marker='', color=palette(3), linewidth=2, label="Optimize_recursion_func")
plt.plot('n', 'optimize_recursion_nested_loop', data=df, marker='', color=palette(3), linewidth=2, label="Optimize_recursion_nested_loop")


# 添加标题和标签
plt.title('Performance Comparison of Different Optimization Strategies', loc='left', fontsize=12, fontweight=0, color='orange')
plt.xlabel("Array Size (n)")
plt.ylabel("Execution Time (ms)")

# 添加图例
plt.legend()

# 展示图表
plt.show()

# 绘制箱线图比较三种方法的分布
plt.figure(figsize=(10, 6))
sns.boxplot(data=df[['naive', 'optimize_multi_link', 'optimize_recursion_func', 'optimize_recursion_nested_loop']])
plt.title('Distribution of Execution Times for Different Strategies')
plt.ylabel('Execution Time (ms)')
plt.show()

# 绘制条形图比较不同n值下的执行时间
plt.figure(figsize=(12, 8))
df_melt = df.melt('n', var_name='Method', value_name='Execution Time')
sns.barplot(x='n', y='Execution Time', hue='Method', data=df_melt)
plt.title('Execution Time Comparison Across Different n Values')
plt.xticks(rotation=45)
plt.show()


# 尝试使用热力图来展示每种策略随矩阵大小变化的执行时间

# 首先创建一个以'n'为索引，策略为列的新DataFrame，用于热力图的数据源
heatmap_df = df.set_index('n')[['naive', 'optimize_multi_link', 'optimize_recursion_func', 'optimize_recursion_nested_loop']]

# 使用seaborn绘制热力图
plt.figure(figsize=(10, 8))
sns.heatmap(heatmap_df, cmap='coolwarm', annot=True, fmt=".4f", linewidths=.5, cbar_kws={'label': 'Execution Time (ms)'})
plt.title('Execution Time Heatmap by Matrix/Vector Size and Strategy')
plt.ylabel('Array Size (n)')
plt.xlabel('Optimization Strategy')
plt.show()

