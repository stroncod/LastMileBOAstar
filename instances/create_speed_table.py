
# factors = {'21': 0.1, '41':0.1, '15':01}
import random
import os
import math
cwd = os.getcwd()

with open(f'{cwd}/NY/BOA/NY-d.boa', 'r') as distance, open(f'{cwd}/NY/BOA/NY-t.boa', 'r') as time:
    count = 0
    nodes = 100
    factor = 5
    all_vel = []
    output = open('NY/BOA/NY-s.boa', 'w')
    for line1, line2 in zip(distance, time):
        if count == nodes-2:
            assert  dn1 == tn1
            vel_avg = sum(all_vel)/nodes
            output.write(dn1)
            for i in range(25):
                final_vel = math.ceil(vel_avg +random.uniform(20,100))
                output.write(f' {final_vel}')
            output.write('\n')
            all_vel = []
            count = 0
        dn1, dn2, d = line1.split()
        tn1, tn2, t = line2.split()
        vel = int(d)/int(t)
        all_vel.append(vel)
        count+=1
        
            
        