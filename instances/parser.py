"""
Transform .gr to .txt for Dijkstra
"""

import os
cwd = os.getcwd()

values = []
with open(f'{cwd}/NY/original/USA-road-d.NY.gr', 'r') as f:
    
    for line in f:
        if 'graph contains' in line:
            header = [s for s in line.split() if s.isdigit()]
        
        if line.startswith('a'):
            values.append(line.split()[1:])


with open(f'{cwd}/SF/original/NY-road-d.txt', 'w') as f:
    f.write(f'{header[0]} {header[1]}\n')
    for val in values:
        f.write('\t'.join(val))
        f.write('\n')