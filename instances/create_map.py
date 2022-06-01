import argparse
import random
from itertools import permutations
                


if __name__ == '__main__':
    
    
    parser = argparse.ArgumentParser(description='Create an instance for BOA')
    parser.add_argument('-n', dest='nodes')
    parser.add_argument('-c', dest='city')
    args = parser.parse_args()
    random.seed(42)
    nodes = {} 
    

    nodes = [i for i in range(int(args.nodes))]
    sample = random.sample(nodes,100)
    
    print(args.city)
    
    for p in permutations(sample,2):
        print(p)
    
    
    