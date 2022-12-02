import pandas as pd
import random
import time


# 서로 다른 두 날짜 사이에 임의의 날짜 생성 : http://daplus.net/python-%EB%8B%A4%EB%A5%B8-%EB%91%90-%EB%82%A0%EC%A7%9C-%EC%82%AC%EC%9D%B4%EC%97%90-%EC%9E%84%EC%9D%98%EC%9D%98-%EB%82%A0%EC%A7%9C%EB%A5%BC-%EC%83%9D%EC%84%B1/
def str_time_prop(start, end, format, prop):
    """Get a time at a proportion of a range of two formatted times.

    start and end should be strings specifying times formated in the
    given format (strftime-style), giving an interval [start, end].
    prop specifies how a proportion of the interval to be taken after
    start.  The returned time will be in the specified format.
    """

    stime = time.mktime(time.strptime(start, format))
    etime = time.mktime(time.strptime(end, format))

    ptime = stime + prop * (etime - stime)

    return time.strftime(format, time.localtime(ptime))


def random_date(start, end, prop):
    return str_time_prop(start, end, '%Y%m%d', prop)


def df_random_date(x):
    return random_date("20220901", "20221001", random.random())


def is_waste_True(x):
    return 0 if x == 0 else 1


# print(random_date("20220901", "20221001", random.random()))  # Check OK

data = pd.read_csv(r"Prepare data\processed data\outlier_processed_data.csv", index_col=0, header=0)

# print(data.info())  # Load check OK

"""
할 일
- 사진 크기 column 제거 (O)
- 랜덤 날짜 8자리 숫자로 추가 : 문자열로 추가 후 타입 변경 (O)
- 쓰레기 종류 수 추가 (O)
- column 순서 변경 (O)
- 날짜순 정렬 (O)
- 인덱스, 제목 열 제거 : 저장할 때 header=None, index=None / 불러올 때 header=None (O)
    - 주의 : 날짜를 랜덤으로 부여하기 때문에 실행할 때마다 파일이 달라짐
"""

# print(data.columns)  # ['Name', 'Widths', 'Hights', 'Latitude', 'Longitude', 'Rubbish', 'Plastics', 'Cans', 'Glass', 'Papers']

# 사진 크기 column 제거
data.drop(['Widths', 'Hights'], axis=1, inplace=True)
# print(data.info())  # Check OK

# 랜덤 날짜 8자리 숫자로 추가
data['Date'] = None
data['Date'] = data['Date'].apply(df_random_date)  # apply : https://koreadatascientist.tistory.com/115
data['Date'] = data['Date'].astype('int64')
# Check OK
# print(data.info(), end="\n\n")
# print(data.head().T)

# 쓰레기 종류 수 추가하기 전에 쓰레기 개수 표기를 0/1로 통일
for col in ['Rubbish', 'Plastics', 'Cans', 'Glass', 'Papers']:
    data[col] = data[col].apply(is_waste_True)
# Check OK
# print(data.info(), end="\n\n")
# print(data.head().T)

# 쓰레기 종류 수 추가
data['waste_cnt'] = data['Rubbish'] + data['Plastics'] + data['Cans'] + data['Glass'] + data['Papers']
# Check OK
# print(data.info(), end="\n\n")
# print(data.head().T)
# print(data.describe().T)

# column 순서 변경 : https://hanawithdata.tistory.com/entry/pandas-column-%EC%88%9C%EC%84%9C-%EC%9E%AC%EB%B0%B0%EC%B9%98%ED%95%98%EB%8A%94-2%EA%B0%80%EC%A7%80-%EB%B0%A9%EB%B2%95
# print(data.columns)  # ['Name', 'Latitude', 'Longitude', 'Rubbish', 'Plastics', 'Cans', 'Glass', 'Papers', 'Date', 'waste_cnt']
data = data[['Name', 'Date', 'Latitude', 'Longitude', 'waste_cnt', 'Rubbish', 'Plastics', 'Cans', 'Glass', 'Papers']]
# Check OK
# print(data.info(), end="\n\n")
# print(data.head().T)

# 날짜순 정렬
data.sort_values(by='Date', inplace=True)
# Check OK
# print(data.info(), end="\n\n")
# print(data.head().T)
# print(data.tail().T)

print("with header")
print(data.info(), end="\n\n")
print(data.describe(), end="\n\n")
print(data.head(), end="\n\n")

# 인덱스, 제목 열 제거
data.to_csv(r"Prepare data\processed data\my_total_data.csv", header=None, index=None)
data = pd.read_csv(r"Prepare data\processed data\my_total_data.csv", header=None)
# Check OK
print("without header")
print(data.info(), end="\n\n")
print(data.describe(), end="\n\n")
print(data.head(), end="\n\n")
