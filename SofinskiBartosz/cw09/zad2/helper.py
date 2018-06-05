#!/bin/env python

with open("test_txt.txt") as f:
    s = f.read().split('\n')

for line in s:
    print(len(line))


