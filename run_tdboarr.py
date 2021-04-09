import subprocess
from os import getcwd
import pandas as pd
import pickle 

def get_cities(inst):
    with open(f'test/test{inst}') as f:
        cities = f.readline().strip()
        return cities
'''
def parse_output(out):
    if 'No solution found!' in output:
        _, data = out.split('!')
        return data.strip().split(',')
    else:
        return out.strip().split(',') 
'''



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
for inst in range(30):
#for inst in range(1):
    print(f'test #{inst}')
    max_cities = get_cities(inst)
    for cities in ([c for c in range(5,int(max_cities),5)]+[int(max_cities)]):
    #for cities in [24]:
        print(f'testing with {cities} cities')
        
        try:
            proc = subprocess.run(args=[f'{getcwd()}/tdboarr',
                                            max_cities,
                                            str(cities),
                                            str(inst)],
                                            capture_output=True,
                                            check=True)
            
        except subprocess.CalledProcessError as e: 

            print('SIGKILL 9!!')
            frontiers.append(-1)
            times.append(600)
            generated_a.append(0)
            expanded_a.append(0)
            tests.append(f'test#{inst}')
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
            tests.append(f'test#{inst}')
            cities_a.append(cities)

df = pd.DataFrame.from_dict({'#test':tests,'#cities':cities_a,'frontier':frontiers,
                            'time':times,'generated':generated_a,'expanded':expanded_a})

#df.to_csv('results.csv', index = False)
pickle.dump(df, open(f'df.pickle','wb'))



max_iter = 100
selected_inst = '12'
frontiers = []
times = []
expanded_a = []
generated_a = []
tests = []
#cities_a = [] 
for inst in range(30):
    max_cities = get_cities(inst)
    print(f'test #{inst}')
    for i in range(max_iter):  

        try:
            proc = subprocess.run(args=[f'{getcwd()}/tdboarr',
                                            max_cities,
                                            selected_inst,
                                            str(inst)],
                                            capture_output=True,
                                            check=True)
            
        except subprocess.CalledProcessError as e: 

            print('SIGKILL 9!!')
            print(e)
            frontiers.append(-1)
            times.append(600)
            generated_a.append(0)
            expanded_a.append(0)
            tests.append(f'test#{inst}')
            #cities_a.append(cities)
        else:
            print('success')
            output = proc.stdout.decode()
        
            frontier, time, expanded, generated = parse_output(output,False) 
            print(frontier, time, generated, expanded)
            frontiers.append(frontier)
            times.append(time)
            generated_a.append(generated)
            expanded_a.append(expanded)
            tests.append(f'test#{inst}')
            #cities_a.append(cities)

df2 = pd.DataFrame.from_dict({'#test':tests,'frontier':frontiers,
                            'time':times,'generated':generated_a,'expanded':expanded_a})

pickle.dump(df2, open(f'df2.pickle','wb'))
