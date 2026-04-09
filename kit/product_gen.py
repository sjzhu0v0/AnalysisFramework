#!/usr/bin/env python3
import itertools
import sys
import argparse

def read_clean_lines(filepath):
    """读取文件并返回去除空白符的非空行列表"""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            return [line.strip() for line in f if line.strip()]
    except FileNotFoundError:
        print(f"错误: 找不到文件 '{filepath}'", file=sys.stderr)
        sys.exit(1)

def main():
    # 1. 命令行参数解析
    parser = argparse.ArgumentParser(
        description="计算多个文本文件的笛卡尔积（直积）。",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="示例:\n  python product_gen.py list1.txt list2.txt\n  python product_gen.py -c '_' list1.txt list2.txt"
    )
    
    # 定义 -c 参数，默认值为逗号
    parser.add_argument('-c', '--connector', default=',', help='连接符，默认为逗号 (,)')
    
    # 接收剩余的所有参数作为文件列表
    parser.add_argument('files', nargs='+', help='需要处理的文本文件')
    
    args = parser.parse_args()
    
    # 2. 读取所有文件内容
    all_data = []
    for path in args.files:
        data = read_clean_lines(path)
        if not data:
            print(f"错误: 文件 '{path}' 为空", file=sys.stderr)
            sys.exit(1)
        all_data.append(data)

    # 3. 计算直积并输出
    # *all_data 解包列表，支持任意数量文件的直积
    for combination in itertools.product(*all_data):
        print(args.connector.join(combination))

if __name__ == "__main__":
    main()
