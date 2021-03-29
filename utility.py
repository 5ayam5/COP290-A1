import pandas as pd
import numpy as np

modes = {'Resolution': ["1_0.csv", "1_2.csv", "1_5.csv", "2_0.csv", "3_0.csv", "4_0.csv"],
         'SpatialThreading': ["1.csv", "2.csv", "4.csv", "8.csv", "12.csv"],
         'TemporalThreading': ["1.csv", "2.csv", "4.csv", "8.csv", "12.csv"],
         'Subsampling': ["1.csv", "2.csv", "5.csv", "10.csv", "20.csv"]}

for (mode, files) in modes.items():
	print(mode + ":")
	base = pd.read_csv(mode + '/' + files[0])
	for file in files[1:]:
		print(file, np.square(100 * np.subtract(base.values[:,1][:-1], pd.read_csv(mode + '/' + file).values[:,1][:-1])).mean())
	print()
