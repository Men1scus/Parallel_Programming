import pandas as pd

def convert_txt_to_csv(txt_file_path, csv_file_path):
    with open(txt_file_path, 'r') as file:
        lines = file.readlines()

    data = {'n': [], 'naive': [], 'optimize_multi_link': [], 'optimize_recursion_func': [], 'optimize_recursion_nested_loop': []}

    for line in lines:
        if line.startswith('n = '):
            n_value = int(line.strip().split(' = ')[1])
            data['n'].append(n_value)
        elif line.startswith('naive:'):
            naive_time = float(line.strip().split(' ')[1][:-2])
            data['naive'].append(naive_time)
        elif line.startswith('optimize_multi_link:'):
            optimize_time = float(line.strip().split(' ')[1][:-2])
            data['optimize_multi_link'].append(optimize_time)
        elif line.startswith('optimize_recursion_func:'):
            unroll_time = float(line.strip().split(' ')[1][:-2])
            data['optimize_recursion_func'].append(unroll_time)
        elif line.startswith('optimize_recursion_nested_loop:'):
            unroll_time = float(line.strip().split(' ')[1][:-2])
            data['optimize_recursion_nested_loop'].append(unroll_time)

    df = pd.DataFrame(data)

    df.to_csv(csv_file_path, index=False)

    print(f"Data has been successfully converted to {csv_file_path}")


txt_file_path = 'Problem2/data/txt/Linux_P2_M=1_n=10_1e6.txt'
csv_file_path = 'Problem2/data/Linux_P2_M=1_n=10_1e6.csv'

convert_txt_to_csv(txt_file_path, csv_file_path)