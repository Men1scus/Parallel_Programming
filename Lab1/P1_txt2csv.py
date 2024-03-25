import pandas as pd

def convert_txt_to_csv(txt_file_path, csv_file_path):
    with open(txt_file_path, 'r') as file:
        lines = file.readlines()

    data = {'n': [], 'naive': [], 'optimize': [], 'unroll': []}

    for line in lines:
        if line.startswith('n = '):
            n_value = int(line.strip().split(' = ')[1])
            data['n'].append(n_value)
        elif line.startswith('naive:'):
            naive_time = float(line.strip().split(' ')[1][:-2])
            data['naive'].append(naive_time)
        elif line.startswith('optimize:'):
            optimize_time = float(line.strip().split(' ')[1][:-2])
            data['optimize'].append(optimize_time)
        elif line.startswith('unroll:'):
            unroll_time = float(line.strip().split(' ')[1][:-2])
            data['unroll'].append(unroll_time)

    df = pd.DataFrame(data)

    df.to_csv(csv_file_path, index=False)

    print(f"Data has been successfully converted to {csv_file_path}")


txt_file_path = 'Problem1/data/txt/Linux_P1_M=1e3_n=10_1e4.txt'
csv_file_path = 'Problem1/data/Linux_P1_M=1e3_n=10_1e4.csv'

convert_txt_to_csv(txt_file_path, csv_file_path)