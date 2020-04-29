#!/usr/bin/env python3
import argparse
import matplotlib.pyplot as plt

pser = argparse.ArgumentParser()
pser.add_argument('dmesg', help='the dmesg.txt file')
args = pser.parse_args()
dmesg = args.dmesg


width = []
left = []
tick_label = []
pid2name = {}

lines = open(dmesg.replace('dmesg', 'stdout')).read().strip().rstrip().split('\n')
for line in lines:
    name, pid = line.split()
    pid2name[pid] = name

lines = open(dmesg).read().strip().rstrip().split('\n')
for line in lines:
    line = line.split(' ')
    pid, start, stop = line[-3:]
    start, stop = float(start), float(stop)
    width.append(stop - start)
    left.append(start)
    tick_label.append(pid2name[pid])

fig, ax = plt.subplots()
ax.barh(y=range(len(lines)), width=width, left=left, tick_label=tick_label)
ax.set_title(dmesg)
ax.grid()
ax.set_xlabel('time')
ax.set_ylabel('process name')
plt.show()
