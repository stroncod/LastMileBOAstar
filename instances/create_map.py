import argparse
import random
from itertools import permutations
import subprocess
import os
cwd = os.getcwd()

if __name__ == '__main__':
    
    
    parser = argparse.ArgumentParser(description='Create an instance for BOA')
    parser.add_argument('-n', dest='nodes')
    parser.add_argument('-c', dest='city')
    args = parser.parse_args()
    random.seed(42)
    nodes = {} 
    

    nodes = [i for i in range(1,int(args.nodes))]
    sample = sorted(random.sample(nodes,100))
    
    results = {}
    city = args.city
    print(city)
    
    for p in permutations(sample,2):
        #print(p)
        proc = subprocess.Popen(f'{cwd}/Dijkstra/testall {cwd}/Dijkstra/{city}-road-d.txt {p[0]} {p[1]}', shell=True, stdout=subprocess.PIPE)
        subprocess_return = proc.stdout.read()
        #print(subprocess_return.decode("utf-8").split('.')[0])
        results[p] = subprocess_return.decode("utf-8").split('.')[0]
    
    
    with open(f'{cwd}/{city}/BOA/{city}-d.boa', 'w+') as f:
        for t, val in results.items():
            f.write(f'{t[0]}\t {t[1]}\t {val}\n')
    
    

    

    