import pandas as pd
import random
import matplotlib.pyplot as plt
import scipy.stats as stats

# 이상치 제거 함수 : https://lifelong-education-dr-kim.tistory.com/16
def remove_out(dataframe, remove_col, rev_range=3):
    dff = dataframe
    for k in remove_col:
        if dff[k].dtype == "object":
            continue
        level_1q = dff[k].quantile(0.25)
        level_3q = dff[k].quantile(0.75)
        IQR = level_3q - level_1q
        # rev_range = 3  # 제거 범위 조절 변수
        dff = dff[(dff[k] <= level_3q + (rev_range * IQR)) & (dff[k] >= level_1q - (rev_range * IQR))]
        dff = dff.reset_index(drop=True)
    return dff

# concat한 데이터 불러오기
pre_data = pd.read_csv(r"Prepare data\processed data\concat_all_data.csv", header=0, encoding='utf-8')
print(pre_data.head())
print(pre_data.tail())

# print(pre_data.columns)  # ['Name', 'Widths', 'Hights', 'Latitude', 'Longitude', 'Rubbish', 'Plastics', 'Cans', 'Glass', 'Papers', 'Data_Name', 'TrashName', 'TrashType', 'TrashWeight', 'TrashPrice', 'TrashLocation', 'TrashTransparency', 'fabric', 'Rubish', 'Heights', 'Longtitude']

column_list = list(pre_data.columns)
del_cols = column_list[10:]  # 제거할 column
print(del_cols)

# 지정된 데이터 이외의 데이터, nan 제거
pre_data.drop(del_cols, axis=1, inplace=True)
pre_data.dropna(inplace=True)

# 특수문자(공백) 제거 : https://seong6496.tistory.com/242
pre_data['Latitude'] = pre_data["Latitude"].str.replace(pat=' ', repl='')
# pre_data['Latitude'] = pre_data["Latitude"].str.replace(pat=r'[^\w]', repl='', regex=True)
pre_data['Latitude'] = pre_data['Latitude'].astype('float64')

# int여야 하는 값 타입 변경
column_list = list(pre_data.columns)
for col in column_list[5:]:
    pre_data[col] = pre_data[col].astype('int64')

print(pre_data.info())
print(pre_data.describe().T)

# 1차 처리 데이터 저장
pre_data.to_csv(r"Prepare data\processed data\pre_processed_data.csv")

# 이상치 제거
pre_data = remove_out(pre_data, list(pre_data.columns)[:5], 45)
pre_data.dropna(inplace=True)

print(pre_data.info())
print(pre_data.describe().T)

# 2차 처리 데이터 저장
pre_data.to_csv(r"Prepare data\processed data\outlier_processed_data.csv")
