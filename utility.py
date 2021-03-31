import pandas as pd
import numpy as np

modes = {'Subsampling': ["1.csv", "2.csv", "5.csv", "10.csv", "20.csv"],
         'Resolution': ["1_0.csv", "1_2.csv", "1_5.csv", "2_0.csv", "3_0.csv", "4_0.csv"],
         'SpatialThreading': ["0.csv", "1.csv", "2.csv", "4.csv", "8.csv", "12.csv"],
         'TemporalThreading': ["0.csv", "1.csv", "2.csv", "4.csv", "8.csv", "12.csv"]}

for (mode, files) in modes.items():
    base = pd.read_csv(mode + '/' + files[0])
    base_execution = float(base.values[-1][0].split(':')[1])
    with open(mode + "/res.csv", 'w+') as f:
        f.write("Parameter,Penalty function,Execution time\n")
        for file in files[1:]:
            csv = pd.read_csv(mode + '/' + file)
            t = float(csv.values[-1][0].split(':')[1])
            execution = 1 + (base_execution - t) / base_execution
            err = 1 + np.square(10000 * np.subtract(
                base.values[:, 1][:-1], csv.values[:, 1][:-1])).mean()
            f.write(','.join([file, str(err / execution), str(t)]))
            f.write('\n')
