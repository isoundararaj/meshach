#!/usr/bin/python3
import sys

if len(sys.argv) != 3:
    print("Expecting exactly 2 args: filename nbits")
    sys.exit(1)

in_file = sys.argv[1]
nbits = int(sys.argv[2])
with open(in_file) as f:
    o = ''
    for l in f:
        l = l.strip()
        for w in l.split():
            oo = '{0:%db}' % (nbits)
            o += oo.format(int(w))
oo = [o[i:i+4] for i in range(0, len(o), 4)]
for i in range(0, len(oo), 4):
    s = ''
    for j in range(i, min(i+4, len(oo))):
        s += '{:x}'.format(int(oo[j], 2))
    s += ' '
    if (i%4 == 3):
        c = '\n'
    else:
        c = ''
    print(s, end=c)
print('')

    #s = '{:x}{:x}{:x}{:x} '.format(int(oo[i], 2), int(oo[i+1],2), int(oo[i+2],2), int(oo[i+3],2))
    #print(s)

