#!/usr/bin/python3
import sys
import random

if len(sys.argv) != 3:
    print("Expecting exactly 2 args: nbits size")
    sys.exit(1)

nbits = int(sys.argv[1])
size = int(sys.argv[2])
if nbits < 1 or nbits > 31:
    print("nbits must be between 1 and 31")
    sys.exit(1)

if nbits == 1:
    lo = 0
    hi = 1
else:
    lo = 2**(nbits-1)
    hi = 2**nbits - 1
count = 0
for x in range(size):
    print('%d ' % random.randint(lo, hi), end='')
    count = count + 1
    if count > 10:
        print('')
        count = 0
print('')
