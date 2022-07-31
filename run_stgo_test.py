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



instances = { 5: 'a', 
             10: 'b',
             25: 'c',
             50: 'd',
             100: 'e'
            }

time_prefix = '.nodes'
rr_prefix = '.profiles'

big_path = 'InstanciasGrandes'
small_path = 'InstanciasChicas'

path = f'{cwd}/instances/STGO/'
n_inst= 1
for cities, inst in instances.items():
    for i in range(10):
        f_path = ''
        if cities < 11:
            f_path = f'{path}{small_path}/{inst}-Instancia{n_inst+i}'
        else:
            f_path = f'{path}{big_path}/{inst}-Instancia{n_inst+i}'
        print(cities,cities, f_path+time_prefix,  f_path+rr_prefix)
        try:
           proc = subprocess.run(args=[f'{getcwd()}/astar',
                                                str(cities),
                                                str(cities),
                                                f_path+time_prefix,
                                                f_path+rr_prefix],
                                                stdout=PIPE, 
                                                stderr=PIPE,
                                                check=True) 
        except subprocess.CalledProcessError as e: 
           
            print('SIGKILL 9!!')
            print(e)
            frontiers.append(-1)
            times.append(600)
            generated_a.append(0)
            expanded_a.append(0)
            tests.append(f'test#{inst}{n_inst}')
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
            tests.append(f'test#{inst}{n_inst}')
            cities_a.append(cities)
    n_inst+=i+1
        
    
    
        

df = pd.DataFrame.from_dict({'#test':tests,'#cities':cities_a,'frontier':frontiers,
                            'time':times,'generated':generated_a,'expanded':expanded_a})
print(df.to_string())


"""
       
"""