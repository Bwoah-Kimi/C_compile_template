# Auto-generated GDB script for set_dco
# Author:           Mingxuan Li
# Acknowledgement:  VSCode GitHub Copilot

# 加载ELF文件
file bin/set_dco.elf

# 加载程序到目标
load

# 在loop处设置断点
break loop

# 运行程序
continue

# 显示完整信息
echo SET_DCO testing completed\n
