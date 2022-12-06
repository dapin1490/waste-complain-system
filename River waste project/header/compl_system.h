#pragma once

#include <iostream>
#include <vector> // vector
#include <utility> // pair
#include <algorithm> // copy, equal(미사용)
#include <map> // multimap 참고 : https://blockdmask.tistory.com/88
#include <io.h> // 파일 존재 확인 참고 : https://tw0226.tistory.com/121
#include <string> // to_string
#include <fstream> // 파일 입출력
#include <ctime> // 시간 데이터 관리
#include <sstream> // 문자열 파싱
#include <climits> // INF
#include <queue> // 우선순위 큐
#include <stack>
using namespace std;

enum class _error : int{shut_down, ValueErrorInt, ValueErrorChar, UnknownError};

const string intxt = "res/input.txt";
const string outtxt = "res/output.txt";

extern ifstream input{ intxt };
extern ofstream output{ outtxt };

/*
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
*/

// C++ 에러 메시지 참고 : https://learn.microsoft.com/ko-kr/cpp/error-messages/compiler-errors-1/c-cpp-build-errors?view=msvc-170
void error(_error code, string message="") {
	if (message.length() > 1)
		output << "\nerror message: " << message << "\n";
	else
		output << "\n";

	switch (code) {
	case _error::shut_down:
		output << "프로그램 비정상 종료\n";
		break;
	case _error::ValueErrorInt: // 잘못된 입력 - int
		output << "ValueErrorInt: int 값이 입력되어야 합니다.\n";
		break;
	case _error::ValueErrorChar: // 잘못된 입력 - char
		output << "ValueErrorChar: char 값이 입력되어야 합니다.\n";
		break;
	default:
		output << "UnknownError: 알 수 없는 오류\n";
	}
	exit(1); // 프로그램 비정상 종료
}

int date_to_int(time_t d) {
	int dint;
	tm dtm;
	errno_t is_valid = localtime_s(&dtm, &d);
	dint = (dtm.tm_year + 1900) * 10000 + (dtm.tm_mon + 1) * 100 + dtm.tm_mday;
	return dint;
}

double squared_d(pair<double, double> a, pair<double, double> b) {
	return pow(b.first - a.first, 2) + pow(b.second - a.second, 2);
}

class complain { // 민원 클래스
private:
	unsigned id; // 민원을 구분하기 위한 id
	string pic_name; // 사진 이름(필요시 절대/상대 파일 경로 포함, 사진 크기를 비롯해 사진 파일 자체에 대한 각종 정보는 원본 파일의 정보에 포함된다고 본다)
	time_t rawtime; // 민원 신고 날짜(time_t)
	tm comp_date; // 민원 신고 날짜(tm)
	errno_t is_valid_date; // 민원 신고 날짜가 오류 없이 저장돼있는지
	pair<double, double> coordinates; // 사진 좌표
	int waste_cnt; // 포함된 쓰레기의 종류 수
public:
	int wastes[5]; // 멤버 변수지만 어차피 getter를 써도 포인터로 전달되어 원본 수정이 가능하니 public 변수로 사용

public:
	complain() { pic_name = "None"; }
	complain(unsigned id, string pn, int cdate, double x, double y, int wcnt, int* ws);
	complain(unsigned id, string pn, int cdate, double x, double y, int wcnt, string ws);

	unsigned get_id() { return id; } // 민원 id 반환
	string get_name() { return pic_name; } // 사진 이름 반환
	time_t get_date() { return rawtime; } // 민원 신고 날짜 반환
	pair<double, double> get_codi() { return coordinates; } // 사진 좌표 반환(pair)
	int get_wcnt() { return waste_cnt; } // 포함 쓰레기 종류 수 반환

	void rename(string new_name) { pic_name = new_name; } // 사진 이름 변경
	void update_wcnt(int num) { waste_cnt += num; } // 포함 쓰레기 종류 수 변경
	// 민원 정보 출력
	void print();
};

bool is_wcnt_zero(complain c) {
	if (c.get_wcnt() == 0)
		return true;
	return false;
}

class compl_system { // 하천 쓰레기 민원 처리 시스템
private:
	unsigned latest_id; // 구역의 시작점을 id 0으로 하기 위해 1부터 시작
	pair<int, int> area_code; // 지역 코드(위도, 경도 정수 부분)
	unsigned thresh; // 민원 처리 최소 단위 : 누적된 민원의 수가 이 수보다 클 때 처리. 기본값은 20
	vector<vector<complain>> accumed_compls_list; // 쓰레기 분류별 누적 민원 배열
	unsigned file_checkpoint; // 전체 민원 파일 어디까지 읽었는지 표시

	vector<complain> all_compls; // 전체 민원 벡터
	multimap<string, complain> map_comp; // 사진 이름 기준 전체 민원 멀티맵
	multimap<double, complain> map_latitude; // 위도 기준 전체 민원 멀티맵
	multimap<double, complain> map_longitude; // 경도 기준 전체 민원 멀티맵
	multimap<time_t, complain, greater<time_t>> map_cdate_front; // 민원 접수 날짜 기준(최근순) 전체 민원 멀티맵
	multimap<time_t, complain> map_cdate_back; // 민원 접수 날짜 기준(오래된순) 전체 민원 멀티맵
	// 맵 내림차순(greater) 참고 : https://0xd00d00.github.io/2021/08/22/map_value_reverse.html

	// 두 민원이 서로 같은지 확인 : 사진 이름과 민원 날짜를 기본키로 사용하기로 함
	bool is_same(complain& a, complain& b);

	// 정상 실행 확인
	// 특정 분류의 쓰레기 민원이 충분히 많아 처리해도 될만한지 확인
	bool is_enough(int waste_code);
	
	// 사진 이름순 모든 민원 조회
	void view_all(multimap<string, complain>& mc);
	// 위도, 경도순 모든 민원 조회
	void view_all(multimap<double, complain> ml, int code);
	// 민원 날짜 오름차순 조회
	void view_all(multimap<time_t, complain, greater<time_t>> mcf);
	// 민원 날짜 내림차순 조회
	void view_all(multimap<time_t, complain> mcb);

	// 누적 민원 처리
	void clear_compls(int waste_code);

	// 누적 민원 물리적 처리
	void call_drone(vector<complain>& c_list);
public:
	compl_system();
	compl_system(int x, int y);

	pair<int, int> get_acode() { return area_code; }
	void set_thresh(int n) { thresh = n; }

	// 정상 실행 확인
	// 시스템 시작 : 사용자에게 지역 코드를 입력받고 기존 데이터 유무 확인, 새 로그 생성 등
	void system_on();

	// 일부 정상 실행 확인
	// 민원 접수
	void receive_compl();

	// 자동 민원 접수
	void auto_receive_compl(int cnt = 0);

	// 기본 실행 확인
	// 정렬 기준(sort_by)에 따른 전체 민원 조회(출력하게 할 것이므로 반환값 없음)
	void view_all();

	/*// 우선순위 최하위. 구현하지 않을 가능성 높음
	// 미확인
	// 검색 기준(search_by)에 따른 특정 민원 검색
	void search_compl() {
		// 가능한 검색 기준 : 사진 이름(오타 불허), 위도, 경도, 접수 날짜
		// 벡터 정렬 후 순회
		return;
	}
	*/

	// 정상 실행 확인
	// 특정 분류의 쓰레기 관련 민원 일괄 처리 : 처리 후 전체 민원 벡터나 다른 맵 등에서 NULL을 제거하는 과정이 필요함
	void clear_compls();

	// 정상 실행 확인
	// 파일에 기록된 데이터를 통한 이전 업무 기록 로드. 매번 새 시스템을 생성할 수는 없으니까.
	void load_save();

	// 정상 실행 확인
	// 업무 진행 상황을 파일로 기록. 업무를 종료하거나 중간 저장이 필요할 때 실행.
	void save_task();

	// 미구현
	// 승인 대기 목록을 확인하고 관할 구역에 속하는 민원을 추가 접수함
	void check_waiting();
};

class Edge {
public:
	double w; // 거리
	unsigned from_id; // 시작점
	unsigned to_id; // 종점
	pair<double, double> from; // 시작점 좌표
	pair<double, double> to; // 종점 좌표

	Edge(complain a, complain b);
	Edge(unsigned fid, unsigned tid, pair<double, double> f, pair<double, double> t);
};

struct cmp { // 다익스트라 우선순위 큐 비교 연산자 : 가중치가 적고 정점 번호가 적은 것을 우선으로 함
	bool operator()(Edge a, Edge b) {
		return a.w > b.w;
	}
};

class drone_graph {
private:
	unsigned v; // 정점 수 : id로 사용
	vector<Edge> edges; // 그래프가 갖는 간선들
	vector<bool> visited; // 방문 여부
	vector<Edge> mst; // 최소 신장 트리
	unsigned w_code; // 쓰레기 코드

public:
	// 생성자
	drone_graph() { this->v = 0; }
	drone_graph(unsigned v) { this->v = v; visited.resize(v); }

	// 함수 정의에 쓰인 const : 이 함수 안에서 쓰는 값들을 변경할 수 없다
	unsigned size() const { return v; } // 그래프가 갖는 정점의 수를 반환
	auto& edges_from() const { return this->edges; } // 그래프가 갖는 간선들을 반환
	// 특정 정점에 연결된 간선들만 반환
	auto edges_from(unsigned i) const;
	// mst 트리에서 특정 정점에 연결된 간선들만 반환
	auto mst_edges_from(unsigned i) const;

	// 방향 간선 추가
	void add(Edge& e);

	// 무방향 간선 추가
	void add_undir(Edge& e);

	// 최소 신장 트리 출력
	void print_mst(vector<complain>& c_list);

	// 프림 알고리즘으로 최소 신장 트리 생성 : https://yabmoons.tistory.com/363
	void prim_tree();

	// DFS로 최소 신장 트리 전부 방문
	void fly_dfs_drone(vector<complain>& c_list);
};

/* class complain */

complain::complain(unsigned id, string pn, int cdate, double x, double y, int wcnt, int* ws) {
	// 참고 : https://cplusplus.com/reference/ctime/mktime/
	// 참고 : https://www.it-note.kr/143
	rawtime = time(&rawtime);
	is_valid_date = localtime_s(&comp_date, &rawtime);
	comp_date.tm_year = cdate / 10000 - 1900;
	comp_date.tm_mon = cdate % 10000 / 100 - 1;
	comp_date.tm_mday = cdate % 100;
	rawtime = mktime(&comp_date);

	this->id = id;
	pic_name = pn;
	coordinates = make_pair(x, y);
	waste_cnt = wcnt;
	copy(ws, ws + 5, wastes); // 배열 복사 참고 : https://terrorjang.tistory.com/98

	// print();
}
complain::complain(unsigned id, string pn, int cdate, double x, double y, int wcnt, string ws) {
	// 참고 : https://cplusplus.com/reference/ctime/mktime/
	// 참고 : https://www.it-note.kr/143
	time_t rawtime = time(&rawtime);
	is_valid_date = localtime_s(&comp_date, &rawtime);
	comp_date.tm_year = cdate / 10000 - 1900;
	comp_date.tm_mon = (cdate % 10000) / 100 - 1;
	comp_date.tm_mday = cdate % 100;
	rawtime = mktime(&comp_date);

	this->id = id;
	pic_name = pn;
	coordinates = make_pair(x, y);
	waste_cnt = wcnt;

	for (int i = 0; i < 5; i++) {
		switch (ws.at(i)) {
		case '0':
			wastes[i] = 0;
			break;
		case '1':
			wastes[i] = 1;
			break;
		default:
			wastes[i] = NULL;
		}
	}

	// print();
}

// 민원 정보 출력
void complain::print() {
	output << "\n민원 id : " << id << "\n민원 파일명 : \"" << pic_name << "\"\n";
	if (is_valid_date == 0) {
		char buffer[256];
		//strftime(buffer, sizeof(buffer), "%Y-%m-%d %X", &comp_date);
		strftime(buffer, sizeof(buffer), "%Y-%m-%d", &comp_date);
		output << "신고 일자 : " << buffer << "\n";
	}
	else {
		output << "신고 일자 : 얻을 수 없음\n";
	}
	output << "신고 좌표 : (" << coordinates.first << ", " << coordinates.second << ")\n";
	output << "포함 쓰레기 종류 수 : " << waste_cnt << "\n";
	output << "일반 " << (wastes[0] == 0 ? 'X' : 'O')
		<< "   플라스틱 " << (wastes[1] == 0 ? 'X' : 'O')
		<< "   캔 " << (wastes[2] == 0 ? 'X' : 'O')
		<< "   유리 " << (wastes[3] == 0 ? 'X' : 'O')
		<< "   종이 " << (wastes[4] == 0 ? 'X' : 'O') << "\n";
}

/* class compl_system */

// 두 민원이 서로 같은지 확인
bool compl_system::is_same(complain& a, complain& b) {
	if (a.get_id() == b.get_id())
		return true;
	return false;
}

// 특정 분류의 쓰레기 민원이 충분히 많아 처리해도 될만한지 확인
bool compl_system::is_enough(int waste_code) {
	if (accumed_compls_list[waste_code].size() >= thresh)
		return true;
	return false;
}

// 사진 이름순 모든 민원 조회
void compl_system::view_all(multimap<string, complain>& mc) {
	output << "\n사진 이름순으로 모든 민원을 조회합니다.\n";

	char ans = 'Y';
	auto iter = mc.begin();
	int total = 1;
	size_t mc_len = mc.size();

	output << "총 민원 수 : " << mc_len << "\n\n";

	while (iter != mc.end() && ans != 'N') {
		int cnt = 1;
		for (iter; iter != mc.end() && cnt < 20; iter++, cnt++, total++) {
			output << total << "/" << mc_len << "\n";
			iter->second.print();
		}

		if (mc_len >= total) {
			output << "남은 민원 " << int(mc_len) - total << "개\n";
			output << "더 보기 Y / N\n";
			input >> ans;
		}
		else {
			output << "남은 민원 0개\n";
			output << "모든 민원을 조회하였습니다. ";
			break;
		}
	}

	output << "조회를 종료합니다.\n";
}
// 위도, 경도순 모든 민원 조회
void compl_system::view_all(multimap<double, complain> ml, int code) {
	char ans = 'Y';
	auto iter = ml.begin();
	int total = 1;
	size_t ml_len = ml.size();

	if (code == 1) {
		output << "\n위도순으로 모든 민원을 조회합니다.\n";
	}
	else if (code == 2) {
		output << "\n경도순으로 모든 민원을 조회합니다.\n";
	}
	else {
		output << "경도/위도순 정렬을 호출했지만 코드가 잘못되었습니다. 조회를 종료합니다.\n";
		return;
	}

	output << "총 민원 수 : " << ml_len << "\n\n";

	while (iter != ml.end() && ans != 'N') {
		int cnt = 1;
		for (iter; iter != ml.end() && cnt < 20; iter++, cnt++, total++) {
			output << total << "/" << ml_len << "\n";
			iter->second.print();
		}

		if (ml_len >= total) {
			output << "남은 민원 " << int(ml_len) - total << "개\n";
			output << "더 보기 Y / N\n";
			input >> ans;
		}
		else {
			output << "남은 민원 0개\n";
			output << "모든 민원을 조회하였습니다. ";
			break;
		}
	}

	output << "조회를 종료합니다.\n";
}
// 민원 날짜 오름차순 조회
void compl_system::view_all(multimap<time_t, complain, greater<time_t>> mcf) {
	output << "\n민원 날짜 오름차순으로 모든 민원을 조회합니다.\n";

	char ans = 'Y';
	auto iter = mcf.begin();
	int total = 1;
	size_t mcf_len = mcf.size();

	output << "총 민원 수 : " << mcf_len << "\n\n";

	while (iter != mcf.end() && ans != 'N') {
		int cnt = 1;
		for (iter; iter != mcf.end() && cnt < 20; iter++, cnt++, total++) {
			output << total << "/" << mcf_len << "\n";
			iter->second.print();
		}

		if (mcf_len >= total) {
			output << "남은 민원 " << int(mcf_len) - total << "개\n";
			output << "더 보기 Y / N\n";
			input >> ans;
		}
		else {
			output << "남은 민원 0개\n";
			output << "모든 민원을 조회하였습니다. ";
			break;
		}
	}

	output << "조회를 종료합니다.\n";
}
// 민원 날짜 내림차순 조회
void compl_system::view_all(multimap<time_t, complain> mcb) {
	output << "\n민원 날짜 내림차순으로 모든 민원을 조회합니다.\n";

	char ans = 'Y';
	auto iter = mcb.begin();
	int total = 1;
	size_t mcb_len = mcb.size();

	output << "총 민원 수 : " << mcb_len << "\n\n";

	while (iter != mcb.end() && ans != 'N') {
		int cnt = 1;
		for (iter; iter != mcb.end() && cnt < 20; iter++, cnt++, total++) {
			output << total << "/" << mcb_len << "\n";
			iter->second.print();
		}

		if (mcb_len >= total) {
			output << "남은 민원 " << int(mcb_len) - total << "개\n";
			output << "더 보기 Y / N\n";
			input >> ans;
		}
		else {
			output << "남은 민원 0개\n";
			output << "모든 민원을 조회하였습니다. ";
			break;
		}
	}

	output << "조회를 종료합니다.\n";
}

// 누적 민원 처리
void compl_system::clear_compls(int waste_code) {
	output << "\n처리 요청 쓰레기 코드 : " << waste_code << "\n";
	if (!is_enough(waste_code)) {
		output << "누적된 민원의 수가 최소 처리 단위보다 적습니다. 그래도 처리하시겠습니까? Y / N\n";
		char ans;
		input >> ans;
		if (ans == 'N') {
			output << "민원을 처리하지 않습니다. 처리 요청을 종료합니다.\n";
			return;
		}
	}

	output << "처리를 시작합니다...\n";

	call_drone(accumed_compls_list[waste_code]);
	accumed_compls_list[waste_code].clear();

	for (complain e : all_compls) {
		bool updated = false;
		if (e.wastes[waste_code] > 0) {
			e.update_wcnt(-1);
			e.wastes[waste_code] = 0;
			updated = true;
		}

		// 연관 컨테이너 반복자 이용해 삭제할 때 주의 : https://wordbe.tistory.com/entry/STL-erase
		auto mc = map_comp.find(e.get_name());
		for (mc; mc != map_comp.end();) {
			if (compl_system::is_same(e, mc->second)) {
				if (e.get_wcnt() == 0) {
					map_comp.erase(mc++);
				}
				else {
					if (updated)
						mc->second = e;
					mc++;
				}
			}
			else
				mc++;
		}

		auto ml = map_latitude.find(e.get_codi().first);
		for (ml; ml != map_latitude.end();) {
			if (compl_system::is_same(e, ml->second))
				if (e.get_wcnt() == 0) {
					map_latitude.erase(ml++);
				}
				else {
					if (updated)
						ml->second = e;
					ml++;
				}
			else
				ml++;
		}

		auto ml2 = map_longitude.find(e.get_codi().second);
		for (ml2; ml2 != map_longitude.end();) {
			if (compl_system::is_same(e, ml2->second))
				if (e.get_wcnt() == 0) {
					map_longitude.erase(ml2++);
				}
				else {
					if (updated)
						ml2->second = e;
					ml2++;
				}
			else
				ml2++;
		}

		auto mcf = map_cdate_front.find(e.get_date());
		for (mcf; mcf != map_cdate_front.end();) {
			if (compl_system::is_same(e, mcf->second))
				if (e.get_wcnt() == 0) {
					map_cdate_front.erase(mcf++);
				}
				else {
					if (updated)
						mcf->second = e;
					mcf++;
				}
			else
				mcf++;
		}

		auto mcb = map_cdate_back.find(e.get_date());
		for (mcb; mcb != map_cdate_back.end();) {
			if (compl_system::is_same(e, mcb->second))
				if (e.get_wcnt() == 0) {
					map_cdate_back.erase(mcb++);
				}
				else {
					if (updated)
						mcb->second = e;
					mcb++;
				}
			else
				mcb++;
		}
	}

	// 참고 : https://cho001.tistory.com/164
	all_compls.erase(remove_if(all_compls.begin(), all_compls.end(), ::is_wcnt_zero), all_compls.end());

	output << "처리가 완료되었습니다.\n";

	return;
}

// 누적 민원 물리적 처리
void compl_system::call_drone(vector<complain>& c_list) {
	drone_graph navi{unsigned(c_list.size() + 1)};
	pair<double, double> station = make_pair(double(area_code.first), double(area_code.second));

	for (int i = 0; i < c_list.size(); i++) {
		Edge edge = Edge(0, i + 1, station, c_list[i].get_codi());
		navi.add_undir(edge);
	}

	for (int i = 0; i < c_list.size(); i++) {
		for (int j = 0; j < c_list.size(); j++) {
			if (i == j)
				continue;
			Edge edge = Edge(i + 1, j + 1, c_list[i].get_codi(), c_list[j].get_codi());
			navi.add_undir(edge);
		}
	}

	output << "경로를 구성합니다...\n";
	navi.prim_tree();

	output << "경로가 생성되었습니다.\n";
	navi.print_mst(c_list);

	navi.fly_dfs_drone(c_list);

	output << "탐색된 경로로 수거 드론을 요청합니다.\n";
	return;
}

compl_system::compl_system() { // 기본 생성자
	latest_id = 1;
	area_code = make_pair(NULL, NULL);
	thresh = 20;
	accumed_compls_list.resize(5);
	file_checkpoint = 0;
}
compl_system::compl_system(int x, int y) { // 생성자
	latest_id = 1;
	area_code = make_pair(x, y);
	thresh = 20;
	accumed_compls_list.resize(5);
	file_checkpoint = 0;
}

// 시스템 시작 : 사용자에게 지역 코드를 입력받고 기존 데이터 유무 확인, 새 로그 생성 등
void compl_system::system_on() {
	pair<int, int> acode;
	string file_route;
	bool is_file_valuable;
	char is_load_save;

	output << "\n하천 쓰레기 민원 처리 시스템을 시작합니다. 관할 구역의 위도와 경도 정수 부분을 공백으로 구분하여 입력해 주세요.\n";
	input >> acode.first >> acode.second;
	output << "입력된 코드는 (" << acode.first << ", " << acode.second << ")입니다.\n";

	this->area_code = acode;
	file_route = "res/" + to_string(acode.first) + "_" + to_string(acode.second) + ".csv";
	is_file_valuable = _access(file_route.c_str(), 0) != -1;

	if (is_file_valuable) { // 파일 존재 확인 참고 : https://tw0226.tistory.com/121
		// 기존 데이터 파일 존재함
		output << "기존 데이터가 존재합니다. 불러오시겠습니까? Y / N\n";
		input >> is_load_save;
		output << "입력된 값은 " << is_load_save << "입니다.\n";

		if (is_load_save == 'N') { // 기존 데이터 불러오지 않기
			output << "계속하기를 선택하면 기존의 데이터가 모두 소실됩니다. 계속하시겠습니까? Y / N\n";
			input >> is_load_save;
			output << "입력된 값은 " << is_load_save << "입니다.\n";

			switch (is_load_save) {
			case 'Y': // 기존 데이터 무시하고 새 데이터 생성 진행
				is_load_save = 'N';
				break;
			case 'N': // 기존 데이터 사용하기로 함
				is_load_save = 'Y';
				break;
			default: // 이상한 대답 하면 나도 안 해 던져
				error(_error::shut_down, "잘못된 입력: 'Y' 또는 'N'만 입력할 수 있습니다.");
			}
		}

		if (is_load_save == 'Y') { // 기존 데이터 불러오기
			this->load_save();
		}
	}
	else if (!is_file_valuable) { // 기존 데이터 파일 존재하지 않음
		// 새 세이브 파일 만들고 메소드 종료
		// 참고 : https://homzzang.com/b/cpp-34
		ofstream new_save(file_route);
		new_save.close();
		return;
	}

	if (is_file_valuable && is_load_save == 'N') {
		// 기존 데이터 파일 존재하지만 사용자가 기존의 데이터를 불러오지 않기로 했음
		// 세이브 파일은 있으나 로드할 세이브가 없으므로 메소드 종료.
		return;
	}
}

// 민원 접수
void compl_system::receive_compl() {
	complain new_comp;
	unsigned c_id;
	string word;
	string pn; // 사진 이름(필요시 절대/상대 파일 경로 포함, 사진 크기를 비롯해 사진 파일 자체에 대한 각종 정보는 원본 파일의 정보에 포함된다고 본다)
	int cdate; // 민원 신고 날짜
	double x, y; // 사진 좌표
	int wcnt; // 포함된 쓰레기의 종류 수
	string ws;
	char ans = 'N';
	string waiting_file_route = "res/waiting list.csv";

	while (ans != 'Y') {
		output << "\n새 민원을 입력합니다. 민원명(파일명)을 입력해 주세요.\n";
		input >> pn;

		output << "민원 신고 날짜를 8자리 숫자로 입력해 주세요.\n";
		input >> word;
		cdate = stoi(word);

		output << "신고 위치(위도, 경도)를 공백으로 구분하여 입력해 주세요.\n";
		input >> word;
		x = stod(word);
		input >> word;
		y = stod(word);

		output << "신고할 쓰레기의 종류 수를 입력해 주세요.\n";
		input >> word;
		wcnt = stoi(word);

		output << "분류별 쓰레기 포함 여부를 공백 없이 다섯 자리 숫자로 입력해 주세요.\n";
		input >> ws;

		new_comp = complain(latest_id, pn, cdate, x, y, wcnt, ws);

		output << "민원 정보 입력이 완료되었습니다. 입력한 정보를 다시 확인해 주세요.\n\n";
		new_comp.print();

		output << "이 정보로 계속하시겠습니까? Y / N\n";
		input >> ans;
	}

	if (int(x) != this->area_code.first || int(y) != this->area_code.second) {
		output << "해당 민원은 관할 구역에 위치하지 않습니다. 승인 대기 목록에 추가합니다.\n";

		// 참고 : https://blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=sea5727&logNo=220978963342
		fstream file;
		file.open(waiting_file_route, ios::app);
		// ofstream file{ waiting_file_route };

		file << pn << "," << cdate << "," << x << "," << y << "," << wcnt << ",";
		for (int i = 0; i < 5; i++) {
			file << ws.at(i) << (i < 4 ? "," : "\n");
		}

		file.close();

		return;
	}

	output << "민원을 접수 중입니다...\n";
	latest_id += 1;
	this->all_compls.push_back(new_comp);
	this->map_comp.insert(make_pair(new_comp.get_name(), new_comp));
	this->map_latitude.emplace(new_comp.get_codi().first, new_comp);
	this->map_longitude.emplace(new_comp.get_codi().second, new_comp);
	this->map_cdate_front.emplace(new_comp.get_date(), new_comp);
	this->map_cdate_back.emplace(new_comp.get_date(), new_comp);

	for (int i = 0; i < 5; i++) {
		switch (new_comp.wastes[i]) {
		case 0:
			continue;
		default:
			accumed_compls_list[i].push_back(new_comp);
		}
	}

	output << "민원이 접수되었습니다.\n";

	return;
}

// 자동 민원 접수
void compl_system::auto_receive_compl(int cnt) {
	ifstream file_in{ "..\\Prepare data\\processed data\\my_total_data.csv" };
	string waiting_file_route = "res/waiting list.csv";

	string pic_name;
	int comp_date;
	pair<double, double> coordinates;
	int waste_cnt;
	int wastes[5];

	string line;
	istringstream word; // 참고 : https://chbuljumeok1997.tistory.com/42

	output << "파일로 작성된 민원을 접수합니다. 접수 범위는 " << file_checkpoint + 1 << "번째부터 " << cnt << "개입니다.\n";

	for (int i = 0; i < file_checkpoint; i++) // start 이전 부분은 아무것도 안 하고 넘김
		getline(file_in, line);

	for (int i = 0; i < cnt; i++) {
		getline(file_in, line);

		if (line.length() <= 1) {
			break;
		}

		word = istringstream(line);

		// 참고 : https://myprivatestudy.tistory.com/48
		getline(word, line, ','); // 참고 : https://myprivatestudy.tistory.com/48
		pic_name = line;

		getline(word, line, ',');
		comp_date = stoi(line);

		getline(word, line, ',');
		coordinates.first = stod(line);
		getline(word, line, ',');
		coordinates.second = stod(line);

		getline(word, line, ',');
		waste_cnt = stoi(line);

		for (int i = 0; i < 5; i++) {
			getline(word, line, ',');
			wastes[i] = stoi(line);
		}

		complain cp{ latest_id, pic_name, comp_date, coordinates.first, coordinates.second, waste_cnt, wastes };

		output << "No. " << i << "\n";
		cp.print();

		if (int(coordinates.first) != this->area_code.first || int(coordinates.second) != this->area_code.second) {
			output << "이 민원은 관할 구역에 위치하지 않습니다. 승인 대기 목록에 추가합니다.\n\n";

			// 참고 : https://blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=sea5727&logNo=220978963342
			fstream file;
			file.open(waiting_file_route, ios::app);

			// ofstream file{ waiting_file_route };

			file << cp.get_name() << "," << date_to_int(cp.get_date()) << "," << cp.get_codi().first << "," << cp.get_codi().second << "," << cp.get_wcnt() << ",";
			for (int i = 0; i < 5; i++) {
				file << cp.wastes[i] << (i < 4 ? "," : "\n");
			}

			file.close();

			continue;
		}

		latest_id += 1;
		this->all_compls.push_back(cp);
		this->map_comp.emplace(cp.get_name(), cp);
		this->map_latitude.emplace(cp.get_codi().first, cp);
		this->map_longitude.emplace(cp.get_codi().second, cp);
		this->map_cdate_front.emplace(cp.get_date(), cp);
		this->map_cdate_back.emplace(cp.get_date(), cp);

		for (int i = 0; i < 5; i++) {
			switch (cp.wastes[i]) {
			case 0:
				continue;
			default:
				accumed_compls_list[i].push_back(cp);
			}
		}
	}

	file_checkpoint += cnt;

	output << "접수를 완료했습니다.\n";

	return;
}

// 정렬 기준(sort_by)에 따른 전체 민원 조회(출력하게 할 것이므로 반환값 없음)
void compl_system::view_all() {
	// 가능한 정렬 기준 : 사진 이름 오름차순, 위도 오름차순, 경도 오름차순, 접수 날짜 오름차순/내림차순
	// 맵 사용
	int ans;
	output << "\n접수된 모든 민원을 조회합니다. 정렬 기준을 숫자로 선택해 주세요.\n1. 사진 이름\n2. 위도\n3. 경도\n4. 접수 일자 오름차순\n5. 접수 일자 내림차순\n";
	input >> ans;

	switch (ans) {
	case 1: output << "선택한 값은 '1. 사진 이름'입니다.\n"; view_all(map_comp); break;
	case 2: output << "선택한 값은 '2. 위도'입니다.\n"; view_all(map_latitude, 1); break;
	case 3: output << "선택한 값은 '3. 경도'입니다.\n"; view_all(map_longitude, 2); break;
	case 4: output << "선택한 값은 '4. 접수 일자 오름차순'입니다.\n"; view_all(map_cdate_front); break;
	case 5: output << "선택한 값은 '5. 접수 일자 내림차순'입니다.\n"; view_all(map_cdate_back); break;
	default: output << "잘못된 입력입니다. 조회를 종료합니다.\n";
	}

	return;
}

// 특정 분류의 쓰레기 관련 민원 일괄 처리 : 처리 후 전체 민원 벡터나 다른 맵 등에서 NULL을 제거하는 과정이 필요함
void compl_system::clear_compls() {
	output << "\n누적 민원 처리를 요청합니다. 처리할 쓰레기 코드를 숫자로 입력해 주세요.\n0. 일반\n1. 플라스틱\n2. 캔\n3. 유리\n4. 종이\n";
	int code;
	input >> code;
	clear_compls(code);
	return;
}

// 파일에 기록된 데이터를 통한 이전 업무 기록 로드. 매번 새 시스템을 생성할 수는 없으니까.
void compl_system::load_save() {
	string file_route = "res/" + to_string(int(area_code.first)) + "_" + to_string(int(area_code.second)) + ".csv";
	ifstream data_file{ file_route };

	string pic_name;
	int comp_date;
	pair<double, double> coordinates;
	int waste_cnt;
	int wastes[5];

	output << "\nstart loading...\n";

	while (!data_file.eof()) {
		string line;
		istringstream word; // 참고 : https://chbuljumeok1997.tistory.com/42

		getline(data_file, line);

		if (line.length() <= 1) {
			break;
		}

		word = istringstream(line);

		// 참고 : https://myprivatestudy.tistory.com/48
		getline(word, line, ','); // 참고 : https://myprivatestudy.tistory.com/48
		pic_name = line;

		getline(word, line, ',');
		comp_date = stoi(line);

		getline(word, line, ',');
		coordinates.first = stod(line);
		getline(word, line, ',');
		coordinates.second = stod(line);

		getline(word, line, ',');
		waste_cnt = stoi(line);

		for (int i = 0; i < 5; i++) {
			getline(word, line, ',');
			wastes[i] = stoi(line);
		}

		complain cp{ latest_id++, pic_name, comp_date, coordinates.first, coordinates.second, waste_cnt, wastes };
		this->all_compls.push_back(cp);
		this->map_comp.emplace(cp.get_name(), cp);
		this->map_latitude.emplace(cp.get_codi().first, cp);
		this->map_longitude.emplace(cp.get_codi().second, cp);
		this->map_cdate_front.emplace(cp.get_date(), cp);
		this->map_cdate_back.emplace(cp.get_date(), cp);

		for (int i = 0; i < 5; i++) {
			switch (cp.wastes[i]) {
			case 0:
				continue;
			default:
				accumed_compls_list[i].push_back(cp);
			}
		}
	}

	output << "load complete\n";
	data_file.close();
}

// 업무 진행 상황을 파일로 기록. 업무를 종료하거나 중간 저장이 필요할 때 실행.
void compl_system::save_task() {
	string file_route = "res/" + to_string(int(area_code.first)) + "_" + to_string(int(area_code.second)) + ".csv";
	ofstream save_file{ file_route };

	// 사진이름,날짜8자리,위도,경도,쓰레기수,각 쓰레기 여부

	output << "\n업무 기록을 저장합니다...\n";

	for (auto i : all_compls) {
		save_file << i.get_name() << ","; // 사진 이름 파일에 입력

		// 민원 날짜를 8자리 숫자로 변환
		time_t date = i.get_date();
		tm date_tm;
		errno_t err = localtime_s(&date_tm, &date);
		char date_cstr[128];
		strftime(date_cstr, sizeof(date_cstr), "%Y%m%d", &date_tm);
		string date_str = date_cstr;
		save_file << date_str << ","; // 민원 날짜 파일에 입력

		save_file << i.get_codi().first << "," << i.get_codi().second << ","; // 위도, 경도 파일에 입력
		save_file << i.get_wcnt() << ","; // 쓰레기 수 파일에 입력

		for (int j = 0; j < 5; j++)
			save_file << i.wastes[j] << (j < 4 ? "," : "\n");
	}

	save_file.close();

	output << "업무 기록이 저장되었습니다.\n";

	return;
}

/* class Edge */

Edge::Edge(complain a, complain b) {
	from_id = a.get_id();
	to_id = b.get_id();

	from = a.get_codi();
	to = b.get_codi();

	w = squared_d(from, to);
}

Edge::Edge(unsigned fid, unsigned tid, pair<double, double> f, pair<double, double> t) {
	from_id = fid;
	to_id = tid;
	from = f;
	to = t;
	w = squared_d(from, to);
}

/* class drone_graph */

// 특정 정점에 연결된 간선들만 반환
auto drone_graph::edges_from(unsigned i) const { // 특정 정점에 연결된 간선들만 반환
	vector<Edge> edge_from_i;
	for (auto& e : edges) {
		if (e.from_id == i)
			edge_from_i.push_back(e);
	}
	/* // 이쪽 코드도 똑같은 기능을 함
	for (int idx = 0; idx < this->edges.size(); idx++) {
		if (this->edges[idx].from == i)
			edge_from_i.push_back(edges[idx]);
	}*/
	return edge_from_i;
}

// mst 트리에서 특정 정점에 연결된 간선들만 반환
auto drone_graph::mst_edges_from(unsigned i) const {
	vector<Edge> edge_from_i;
	for (int idx = 0; idx < this->mst.size(); idx++) {
		if (this->mst[idx].from_id == i)
			edge_from_i.push_back(mst[idx]);
	}
	return edge_from_i;
}

// 방향 간선 추가
void drone_graph::add(Edge& e) {
	if (e.from_id >= 0 && e.from_id <= v && e.to_id >= 0 && e.to_id <= v)
		this->edges.push_back(e);
	else
		error(_error::shut_down, "정점 범위 초과");

	return;
}

// 무방향 간선 추가
void drone_graph::add_undir(Edge& e) {
	if (e.from_id >= 0 && e.from_id <= v && e.to_id >= 0 && e.to_id <= v) {
		this->edges.push_back(e);
		this->edges.push_back(Edge(e.to_id, e.from_id, e.to, e.from));
	}
	else
		error(_error::shut_down, "정점 범위 초과");

	return;
}

// 최소 신장 트리 출력
void drone_graph::print_mst(vector<complain>& c_list) {
	for (int i = 0; i <= mst.size(); i++) {
		output << "# " << (i != 0 ? to_string(c_list[i - 1].get_id()) : "station") << " : "; // 정점 번호
		vector<Edge> edge = this->drone_graph::mst_edges_from(i); // 정점에 연결된 간선 가져오기
		for (auto& e : edge)
			output << "(" << (e.to_id != 0 ? to_string(c_list[e.to_id - 1].get_id()) : "station") << ", " << e.w << ")  "; // 정점에 연결된 간선 출력
		output << "\n";
	}
	return;
}

// 프림 알고리즘으로 최소 신장 트리 생성 : https://yabmoons.tistory.com/363
void drone_graph::prim_tree() {
	priority_queue<Edge, vector<Edge>, cmp> que;
	vector<Edge> edge_from_i = edges_from(0);

	mst.clear();
	visited.assign(v, false);

	for (Edge e : edge_from_i)
		que.push(e);

	visited[0] = true;

	while (!que.empty()) {
		Edge curr = que.top();
		que.pop();

		if (!visited[curr.to_id]) {
			visited[curr.to_id] = true;
			mst.push_back(curr);
			edge_from_i = edges_from(curr.to_id);
			for (Edge e : edge_from_i)
				if (!visited[e.to_id])
					que.push(e);
		}
	}

	// 탐색을 끝낸 후 방문 여부 표시를 모두 지움
	visited.assign(v, false);

	return;
}

// DFS로 최소 신장 트리 전부 방문
void drone_graph::fly_dfs_drone(vector<complain>& c_list) {
	string route = "";
	stack<pair<unsigned, unsigned>> s; // first는 방문해야 할 노드, second는 직전에 방문한 노드

	visited.assign(v, false);

	s.emplace(0, 0);

	output << "DFS 경로 탐색을 시작합니다.\n";

	while (!s.empty()) {
		pair<unsigned, unsigned> curr = s.top();
		s.pop();

		if (!visited[curr.first]) {
			visited[curr.first] = true;
			if (curr.first == 0)
				route += "station   ";
			else
				route += (to_string(c_list[curr.first - 1].get_id()) + "   ");

			auto connected_edges = mst_edges_from(curr.first);
			for (auto e : connected_edges) {
				s.emplace(e.to_id, curr.first);
			}
		}
	}

	// 탐색을 끝낸 후 방문 여부 표시를 모두 지움
	visited.assign(v, false);

	// 탐색 결과 출력
	output << "DFS route : " << route << "\n";
	output << "----------\n";
}
