#!/bin/bash

# 指定项目的头文件目录和源文件目录
INCLUDE_DIR="./include"
SRC_DIR="./src"

# 创建一个临时文件来存储头文件列表
TMP_HEADERS_LIST=$(mktemp)

# 遍历include目录和src目录下的所有.h和.c文件
find $INCLUDE_DIR $SRC_DIR -type f \( -name "*.h" -o -name "*.c" \) | while read -r file; do
    # 提取每个文件中所有的#include <xxx>引用，并追加到临时文件中
    grep -o '#include <[^>]*>' "$file" >> "$TMP_HEADERS_LIST"
done

# 处理临时文件，提取并排序所有唯一的头文件名，排除重复
awk -F'[<>]' '{print $2}' "$TMP_HEADERS_LIST" | sort -u

# 删除临时文件
rm "$TMP_HEADERS_LIST"
