#include <iostream>
//#include <vector> // vector
//#include <utility> // pair
//#include <algorithm> // copy, sort(예정)
//#include <map> // multimap 참고 : https://blockdmask.tistory.com/88
//#include <io.h> // 파일 존재 확인 참고 : https://tw0226.tistory.com/121
//#include <string> // to_string
#include <ctime>
#include "../header/compl_system.h"
using namespace std;

string currentDateTime() {
	time_t t = time(nullptr);
	tm now;
	errno_t is_error = localtime_s(&now, &t);

	if (is_error == 0) {
		char buffer[256];
		strftime(buffer, sizeof(buffer), "%Y-%m-%d %X", &now);
		return buffer;
	}
	else {
		return "현재 시간을 얻을 수 없음";
	}
}

int main() {
	// 빠른 입출력
	ios::sync_with_stdio(false); input.tie(NULL); cout.tie(NULL);

	// 코드 실행 시간 측정 : https://scarlettb.tistory.com/5
	clock_t clock_start, clock_finish;
	double clock_duration;

	output << "main.cpp 실행 시각 : " << currentDateTime() << "\n\n";

	clock_start = clock();

	//// 참고 : https://cplusplus.com/reference/ctime/mktime/
	//time_t rawtime = time(&rawtime);
	//tm sometime;
	//errno_t is_error = localtime_s(&sometime, &rawtime);
	//sometime.tm_year = 2021 - 1900;
	//sometime.tm_mon = 10 - 1;
	//sometime.tm_mday = 11;
	//mktime(&sometime);
	//if (is_error == 0) {
	//	char buffer[256];
	//	strftime(buffer, sizeof(buffer), "%Y-%m-%d %X", &sometime);
	//	output << buffer << "\n\n";
	//}

	compl_system system;
	system.system_on(); // 정상 실행 확인함

	// system.receive_compl(); // 정상 실행 확인함

	// system.save_task(); // 정상 실행 확인함, 실행 후 데이터 파일이 변경되기 때문에 주석 처리

	system.view_all(); // 정상 실행 확인함
	system.view_all(); // 정상 실행 확인함
	system.view_all(); // 정상 실행 확인함
	system.view_all(); // 정상 실행 확인함
	system.view_all(); // 정상 실행 확인함

	system.clear_compls(); // 코드 1 : 누적 민원 있음, 처리 수량 미달, 강제 처리
	system.clear_compls(); // 코드 3 : 누적 민원 없음, 오류 처리 테스트

	system.view_all();

	system.auto_receive_compl(5000);

	system.clear_compls();

	system.view_all();

	clock_finish = clock();

	// clock_duration = (double)(clock_finish - clock_start) / CLOCKS_PER_SEC; // 초 단위로 환산
	clock_duration = (double)(clock_finish - clock_start);
	output << "\n프로그램 실행 시간 : " << clock_duration << "ms\n";

	input.close();
	output.close();

	return 0;
}