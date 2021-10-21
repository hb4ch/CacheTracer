# -*- coding: UTF-8 -*-
import sys
import os

program_path = sys.argv[1]
program = program_path.strip().split("/")[-1]
print("program: {}".format(program))

trace_command = "./pin -t source/tools/trace/obj-intel64/trace.so -- " + program_path
print("trace_command: {}".format(trace_command))
os.system(trace_command)

clean_command = "sort -k 1 -n memory_trace.out | sed '/thread_sync/d' | awk 'BEGIN {OFS=\"\\t\"}; {$1=\"\";sub('\\t\\t',\"\")}1' > memory_trace.clean"
print("clean_command: {}".format(clean_command))
os.system(clean_command)

os.system("python3 trace_parser.py memory_trace.clean " + program + ".log")

os.system("rm -rf memory_trace.out memory_trace.clean")