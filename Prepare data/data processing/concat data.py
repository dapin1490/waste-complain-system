# os 모듈에서 특정 폴더 내 파일명을 다 불러와서 리스트로 만들고 판다스로 로드 병합
import pandas as pd
import os

file_routes = []
error_list = []
fout = open(r"Prepare data\processed data\error_list.txt", 'w')

for dirname, _, filenames in os.walk(r'Prepare data\shared data'):
	for filename in filenames:
		file_routes.append(os.path.join(dirname, filename))

# print(file_routes)
total_data = pd.read_csv(r"Prepare data\shared data\01.csv", header=0, index_col=0, encoding='utf-8')
print(total_data.info())

for i in range(1, len(file_routes)):
	print(i)
	try:
		this_data = pd.read_csv(file_routes[i], header=0, index_col=0, encoding='utf-8')
		total_data = pd.concat([total_data, this_data], ignore_index=True)
	except UnicodeDecodeError:
		error_list.append(file_routes[i])
		print(f"{file_routes[i]}\n인코딩 던짐")
		fout.write(f"{file_routes[i]}\n인코딩 던짐\n\n")
	except pd.errors.ParserError:
		error_list.append(file_routes[i])
		print(f"{file_routes[i]}\n줄이 이상하다고 함")
		fout.write(f"{file_routes[i]}\n줄이 이상하다고 함\n\n")
	print(total_data.info())

print(total_data.describe().T)
total_data.to_csv(r"Prepare data\processed data\concat_all_data.csv", index=False)

check_data = pd.read_csv(r"Prepare data\processed data\concat_all_data.csv")
print(check_data.head())
print(check_data.describe().T)

fout.close()
