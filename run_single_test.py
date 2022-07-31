import subprocess
from os import getcwd
import pandas as pd
import pickle
from subprocess import PIPE

cwd = getcwd()

def parse_output(out, flag):
    last = out.split('!')[-1]
    if flag:
        aux = last.strip().split(',')
        return aux[:1]+[600]+aux[1:]  

    return last.strip().split(',')

frontiers = []
times = []
expanded_a = []
generated_a = []
tests = []
cities_a = [] 


road = 'NY'
inst1 = f'{cwd}/instances/{road}/BOA/{road}-t.boa'
inst2 = f'{cwd}/instances/{road}/BOA/{road}-s.boa'
max_cities = 100
for cities in ([c for c in range(5,max_cities,5)]+[max_cities]):
    print(f'testing with {cities} cities')
    try:
       proc = subprocess.run(args=[f'{getcwd()}/astar',
                                            str(max_cities),
                                            str(cities),
                                            inst1,
                                            inst2],
                                            stdout=PIPE, 
                                            stderr=PIPE,
                                            check=True) 
    except subprocess.CalledProcessError as e: 
           
        print('SIGKILL 9!!')
        frontiers.append(-1)
        times.append(600)
        generated_a.append(0)
        expanded_a.append(0)
        tests.append(f'test#{road}')
        cities_a.append(cities)
    else:
        print('success')
        output = proc.stdout.decode()
        
        frontier, time, generated, expanded = parse_output(output,False) 
            #pyprint(frontier, time, generated, expanded)
        frontiers.append(frontier)
        times.append(time)
        generated_a.append(generated)
        expanded_a.append(expanded)
        tests.append(f'test#{road}')
        cities_a.append(cities)

df = pd.DataFrame.from_dict({'#test':tests,'#cities':cities_a,'frontier':frontiers,
                            'time':times,'generated':generated_a,'expanded':expanded_a})
print(df.to_string())
