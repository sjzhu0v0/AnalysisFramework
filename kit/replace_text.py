#!/usr/bin/env python3
import sys

def main():
    if len(sys.argv) != 4:
        print(f"用法: {sys.argv[0]} <文件名> <旧字符串> <新字符串>")
        sys.exit(1)

    file_path = sys.argv[1]
    old_str = sys.argv[2]
    new_str = sys.argv[3]

    try:
        # 以文本方式读取
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()

        # 检查是否包含旧字符串（可选）
        if old_str not in content:
            print("警告：未找到匹配的旧字符串，文件未改动。")
            return

        # 核心：纯文本替换，无视所有特殊字符
        new_content = content.replace(old_str, new_str)

        # 写回文件
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(new_content)

        print("替换成功！")

    except FileNotFoundError:
        print(f"错误：文件 '{file_path}' 不存在。")
        sys.exit(1)

if __name__ == "__main__":
    main()
