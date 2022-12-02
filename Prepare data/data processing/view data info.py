import pandas as pd

data = pd.read_csv(r"Prepare data\processed data\my_total_data.csv", header=None, names=['Name', 'Date', 'Latitude', 'Longitude', 'waste_cnt', 'Rubbish', 'Plastics', 'Cans', 'Glass', 'Papers'])

print(data.info(), end="\n\n")
print(data.describe(), end="\n\n")
print(data.head(), end="\n\n")
