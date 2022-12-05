# waste-complain-system

## 관련 블로그 글 보기
- <a href="https://dapin1490.github.io/satinbower/posts/it-data-structure-training-project/" target="_blank">[하천 쓰레기 프로젝트] 프로젝트 개요</a>
- <a href="https://dapin1490.github.io/satinbower/posts/it-dst-project-plan/" target="_blank">[하천 쓰레기 프로젝트] 프로젝트 구현 계획</a>
- <a href="https://dapin1490.github.io/satinbower/posts/it-dst-project-end/" target="_blank">[하천 쓰레기 프로젝트] 프로젝트 마무리</a>

## 주제
하천 쓰레기 민원 처리 시스템 만들기

## 주요 기능
접수 : 새 민원 등록, 지역에 따라 구분하여 처리  
조회 : 특정 데이터를 기준으로 정렬하여 조회  
민원 처리 : 일정량 이상 민원이 누적되면 한번에 처리. 초안에 더해 드론을 이용하여 쓰레기를 수거한다는 설정 추가함.  

## 구현
개별 민원 클래스와 민원 시스템 클래스 생성  
**개별 민원 클래스** : 민원에 대한 각종 정보 저장. 민원에 대한 정보를 반환하거나 일부 조작할 수 있고, 민원을 사용자가 알아볼 수 있게 출력하는 함수 정의.  
**민원 시스템 클래스** : 민원 클래스를 벡터와 맵에 저장하고 처리하는 클래스. 전체 민원 벡터와 유형별 누적 민원 벡터, 정렬 기준별 멀티맵을 멤버로 갖고, 정렬 기준에 따른 조회 함수 구현. 프로그램 종료와 상관없이 민원 처리를 지속할 수 있도록 업무 진행 상황을 저장하고 로드하는 기능이 있음. 민원 처리는 데이터로 저장된 민원 처리와 드론을 이용한 물리적 처리가 구분됨. 물리적 처리는 드론을 이용한다는 전제로 특정 처리장에서 출발해 각 쓰레기까지의 직선 거리와 쓰레기 사이의 거리를 고려해 프림 알고리즘으로 경로를 탐색하고, 데이터 처리는 민원을 저장했던 벡터와 맵에서 처리한 내용을 업데이트하고 완전히 처리된 민원의 경우 삭제함.

## 데이터
**전체 수강생 데이터(csv) 59개 중** 비공개 링크, 파일 존재하지 않음 문제로 얻지 못한 2개와 인코딩 문제 또는 파싱 문제로 합치지 못한 5개를 제외한 **52개 데이터 사용함**.  
  
[shared data](https://github.com/dapin1490/waste-complain-system/tree/main/Prepare%20data/shared%20data) : 인코딩과 파싱 문제로 합치지 못한 5개를 포함한 57개의 csv 파일이 저장된 폴더  
  
[error_list.txt](https://github.com/dapin1490/waste-complain-system/blob/main/Prepare%20data/processed%20data/error_list.txt) : 파일은 존재하지만 오류로 합치지 못한 파일과 오류 이유 목록  
  
[concat_all_data.csv](https://github.com/dapin1490/waste-complain-system/blob/main/Prepare%20data/processed%20data/concat_all_data.csv) : 52개의 csv 파일을 하나로 concat한 파일  
  
[pre_processed_data.csv](https://github.com/dapin1490/waste-complain-system/blob/main/Prepare%20data/processed%20data/pre_processed_data.csv) : 지정된 column 이외의 데이터, NaN, 불필요한 공백 등 제거하고 데이터타입 지정한 파일. 오타 등의 이상치 포함됨.  
  
[outlier_processed_data.csv](https://github.com/dapin1490/waste-complain-system/blob/main/Prepare%20data/processed%20data/outlier_processed_data.csv) : 위의 [pre_processed_data.csv](https://github.com/dapin1490/waste-complain-system/blob/main/Prepare%20data/processed%20data/pre_processed_data.csv)에서 이상치 제거한 파일. 전체 샘플 중 이상치는 극히 일부일 것으로 예상되기 때문에 최대한 많은 샘플을 남기려고 했음. 이에 대한 문의가 있다면 issue로 올려주시기 바람.  
  
[my_total_data.csv](https://github.com/dapin1490/waste-complain-system/blob/main/Prepare%20data/processed%20data/my_total_data.csv) : 내 프로젝트에 필요한 형식으로 처리한 데이터. 헤더와 인덱스가 없고 column은 &#91;`파일명, 날짜(8자리 숫자), 경도, 위도, 포함된 쓰레기 종류 수, 각 쓰레기 유형별 유무`&#93; 순서로 구성되어 있다.  

## 결과 및 효과
결과 및 효과를 서술하기에 앞서, 본 프로젝트에서 의미하는 결과 및 효과에 대해 정의한다. 본 프로젝트의 결과 및 효과는 본 프로젝트를 이용함으로써 얻을 수 있는 이득을 말하는 것으로 크게 유형 효과와 무형 효과로 나뉘며, 유형 효과는 설득력 있는 수치적 근거를 들어 계산할 수 있는 이득을 의미하고 무형 효과는 수치로 계산할 수 없는 이득을 의미한다.  

- 유형 효과  
  - 드론의 경로를 먼저 입력한 후 자동으로 운행하게 함으로써 드론 조종 인력의 인건비를 절감할 수 있다.
- 무형 효과  
  - 하천 쓰레기에 대한 민원 처리 과정 대부분을 프로그램이 자동으로 수행하게 함으로써 민원 처리 인력이 다른 업무에 더 집중할 수 있게 한다.

## 부족한 점
- 검색 기능 미구현
- 승인 대기 목록에 저장하는 기능은 구현했지만 시간 문제로 승인 대기 목록을 다시 불러오는 기능은 구현하지 않았다.
- 누적된 쓰레기 수가 지정치를 넘어서면 자동으로 쓰레기 처리를 권장하는 기능을 구현하고 싶었지만 시간이 부족했다.
