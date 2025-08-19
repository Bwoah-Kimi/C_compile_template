# Author:           Zhantong Zhu
# Acknowledgement:  VSCode GitHub Copilot

shell rm -f /home/bwoah/tools/C_compile_template/results/log.txt
shell touch /home/bwoah/tools/C_compile_template/results/log.txt
set logging file /home/bwoah/tools/C_compile_template/results/log.txt
set logging redirect
set logging enabled on

x/5600xg 0x8000A000

set logging enabled off