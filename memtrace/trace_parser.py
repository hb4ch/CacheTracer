# -*- coding: UTF-8 -*-
import sys

# path = "C://Users//chen//Downloads//memory_trace.clean"
# log_path = "./log"
path = sys.argv[1]
log_path = sys.argv[2]
print("input log path: {}".format(path))
print("output log path: {}".format(log_path))

_log = open(path)
log = open(log_path, "w")

thread_mapping = {}
thread_mapping_counter = 0

line_index_config = {"thread_uid": 0, "op": 2, "addr": 3, "size": 4, "2_offset": 3}

mem_ins_counter = 0

_line = _log.readline()
while _line:
    _line = _line.strip().split("\t")
    if len(_line) not in [5, 8]:
        _line = _log.readline()
        continue

    thread_uid = _line[line_index_config["thread_uid"]]

    if thread_uid not in thread_mapping:
        thread_mapping[thread_uid] = thread_mapping_counter
        thread_mapping_counter += 1

    thread_id = thread_mapping[thread_uid]

    try:
        op = _line[line_index_config["op"]]
        addr = _line[line_index_config["addr"]]
        size = _line[line_index_config["size"]]
        size = size if int(size) < 64 else 64  # temp
        line = "{} {} {} {}\n".format(thread_id, op, addr, size)
        log.write(line)
        mem_ins_counter += 1

        if len(_line) > 5:
            op = _line[line_index_config["op"] + line_index_config["2_offset"]]
            addr = _line[line_index_config["addr"] + line_index_config["2_offset"]]
            size = _line[line_index_config["size"] + line_index_config["2_offset"]]
            size = size if int(size) < 64 else 64
            line = "{} {} {} {}\n".format(thread_id, op, addr, size)
            log.write(line)
    except Exception:
        print("error line: {}".format(_line))

    _line = _log.readline()

_log.close()
log.close()

print("mem instruction count: {}".format(mem_ins_counter))
