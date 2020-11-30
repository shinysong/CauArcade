#include <iostream>
#include <bangtal>
#include <cstdio>
#include <time.h>
#include <Windows.h>
#include<cstdlib>
#include<ctime>

using namespace bangtal;
using namespace std;

extern void lobby_main();

ObjectPtr board[12][12]; // 게임보드
ScenePtr scene; // 배경
ObjectPtr monsters[7]; // 몬스터 7마리
TimerPtr mob_timer[7]; // 몬스터마다 움직이는 타이머
ObjectPtr heart[2]; //하트
SoundPtr h_minus;
const int board_x1 = 110; //움직일수 있는 범위
const int board_x2 = 900;
const int board_y1 = 100;
const int board_y2 = 580;
int heart_num = 2; //0-2 세개

class Player {
public:
	int px;
	int py;
	Player(int x, int y) :px(x), py(y) {}
};

class Mons {
public:
	int x;
	int y;
	int speed;
	bool isshow;
	bool islive;
	Mons(int x, int y, int s, bool isshow) :x(x), y(y), speed(s), isshow(true), islive(true) {}

public:
	Mons() { // 초기화
		x = board_x1;
		y = board_y1;
		speed = 10;
		isshow = true;
		islive = true;
	}
};

class bomb {
public:
	int bx;
	int by;
	bool isbomb;//터졌을때만 몬스터 죽게
	bomb(int x, int y, bool isbomb) :bx(x), by(y), isbomb(false) {}
};

bool heart_touch(int mx, int my, int bx, int by, int radius) { //범위안에 있는지 체크
	return((mx <= bx + radius && mx >= bx - radius && my <= by + radius && my >= by - radius));
}


bool board_check(int x, int y) { //게임보드 안에 있는 지 체크
	return ((board_x1 <= x && x <= board_x2 && board_y1 <= y && y <= board_y2));
}



bool t_monster; // 몬스터랑 닿았는지

int b_radius = 73;
bool touch(int mx, int my, int bx, int by, int radius) { //범위안에 있는지 체크
	return((mx <= bx + b_radius + radius && mx >= bx + b_radius - radius && my <= by + b_radius + radius && my >= by + b_radius - radius));
} //이미지 위치와 실제 위치와의 거리감이 있어서, 조정해줌.

int m_die = 0; // 죽은 몬스터 수 ->메인함수에서 체크하는 타이머 있음.

//move 위 아래 왼 오른  만들기
Mons right_move(ScenePtr scene, ObjectPtr monster, ObjectPtr player, Mons m, Player p, bomb b, bool t_monster) {
	t_monster = touch(m.x, m.y, b.bx, b.by, 100); // 물풍선과 닿았는지 체크

	if (t_monster == true && b.isbomb == true) {
		monster->hide();
		m.islive = false;
		m_die++;
		t_monster = false;
	}

	if (board_check(m.x, m.y) == true && heart_touch(m.x, m.y, p.px, p.py, 35) == false) {
		monster->locate(scene, m.x, m.y);
		m.x = m.x + m.speed;
	}
	else if (heart_touch(m.x, m.y, p.px, p.py, 35)) { //캐릭터와 몬스터가 닿으면 하트 줄어듬.
		h_minus->play();
		heart[heart_num]->hide();
		heart_num--;
		m.x = m.x + 70;
		monster->locate(scene, m.x, m.y);
		cout << "heart-1" << endl;
	}

	if (board_check(m.x, m.y) == false) { //벽에 닿으면 시계방향으로 움직임.
		monster->locate(scene, board_x2, m.y - m.speed);
		m.x = board_x2;
		m.y = m.y - m.speed;
		if (m.y < board_y1) {
			monster->locate(scene, m.x - m.speed, board_y1);
			m.x = m.x - m.speed;
			m.y = board_y1;
		}
	}

	return m;
}

Mons left_move(ScenePtr scene, ObjectPtr monster, ObjectPtr player, Mons m, Player p, bomb b, bool t_monster) {
	t_monster = touch(m.x, m.y, b.bx, b.by, 100); // 물풍선과 닿았는지 체크
	if (t_monster == true && b.isbomb == true) {
		monster->hide();
		m.islive = false;
		m_die++;
		t_monster = false;
	}

	if (board_check(m.x, m.y) == true && heart_touch(m.x, m.y, p.px, p.py, 35) == false) {

		monster->locate(scene, m.x, m.y);
		m.x = m.x - m.speed;
	}
	else if (heart_touch(m.x, m.y, p.px, p.py, 35)) {
		h_minus->play();
		heart[heart_num]->hide();
		heart_num--;
		m.x = m.x - 70;
		monster->locate(scene, m.x, m.y);
		cout << "heart-1" << endl;
	}

	if (board_check(m.x, m.y) == false) {
		monster->locate(scene, board_x1, m.y + m.speed);
		m.x = board_x1;
		m.y = m.y + m.speed;
		if (m.y > board_y2) {
			monster->locate(scene, m.x + m.speed, board_y2);
			m.x = m.x + m.speed;
			m.y = board_y2;
		}
	}
	return m;
}

Mons up_move(ScenePtr scene, ObjectPtr monster, ObjectPtr player, Mons m, Player p, bomb b, bool t_monster) {
	t_monster = touch(m.x, m.y, b.bx, b.by, 100); // 물풍선과 닿았는지 체크
	if (t_monster == true && b.isbomb == true) {
		monster->hide();
		m.islive = false;
		m_die++;
		t_monster = false;
	}

	if (board_check(m.x, m.y) == true && heart_touch(m.x, m.y, p.px, p.py, 35) == false) {
		monster->locate(scene, m.x, m.y);
		m.y = m.y + m.speed;

	}
	else if (heart_touch(m.x, m.y, p.px, p.py, 35)) {
		h_minus->play();
		heart[heart_num]->hide();
		heart_num--;
		m.y = m.y + 80;
		monster->locate(scene, m.x, m.y);
		cout << "heart-1" << endl;
	}

	if (board_check(m.x, m.y) == false) {
		monster->locate(scene, m.x + m.speed, board_y2);
		m.x = m.x + m.speed;
		m.y = board_y2;
		if (m.x > board_x2) {
			monster->locate(scene, board_x2, m.y - m.speed);
			m.x = board_x2;
			m.y = m.y - m.speed;
		}
	}

	return m;
}

Mons down_move(ScenePtr scene, ObjectPtr monster, ObjectPtr player, Mons m, Player p, bomb b, bool t_monster) {
	t_monster = touch(m.x, m.y, b.bx, b.by, 100); // 물풍선과 닿았는지 체크
	if (t_monster == true && b.isbomb == true) {
		monster->hide();
		m.islive = false;
		m_die++;
		t_monster = false;
	}

	if (board_check(m.x, m.y) == true && heart_touch(m.x, m.y, p.px, p.py, 35) == false) {
		monster->locate(scene, m.x, m.y);
		m.y = m.y - m.speed;

	}
	else if (heart_touch(m.x, m.y, p.px, p.py, 35)) {
		h_minus->play();
		heart[heart_num]->hide();
		heart_num--;
		m.y = m.y - 80;
		monster->locate(scene, m.x, m.y);
		cout << "heart-1" << endl;
	}

	if (board_check(m.x, m.y) == false) {
		monster->locate(scene, m.x - m.speed, board_y1);
		m.x = m.x - m.speed;
		m.y = board_y1;
		if (m.x < board_x1) {
			monster->locate(scene, board_x1, m.y + m.speed);
			m.x = board_x1;
			m.y = m.y + m.speed;
		}
	}

	return m;
}



void two_main() {
	srand((unsigned int)time(NULL));
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
	SoundPtr bgm = Sound::create("Music/2.mp3");
	SoundPtr bomb_set = Sound::create("Music/bomb_set.mp3");
	SoundPtr bomb_pop = Sound::create("Music/bomb_pop.mp3");
	h_minus = Sound::create("Music/player_die.mp3");	

	scene = Scene::create("스테이지 2", "images/2탄.png"); //배경

	Player player_class(275, 265);
	auto player = Object::create("images/down_01.png", scene, 275, 265);
	auto water = Object::create("images/water.png", scene, player_class.px, player_class.py, false);
	bomb waterbomb(player_class.px, player_class.py, false);
	auto water_bomb = Object::create("images/bomb.png", scene, player_class.px, player_class.py, false);
	auto watertimer1 = Timer::create(1); //물풍선이 터지는 시간
	auto watertimer2 = Timer::create(0.5f); //터지는 순간
	auto gameoverTimer = Timer::create(60); //터지는 순간

	//하트 위치
	for (int h = 0; h <= heart_num; h++) {
		heart[h] = Object::create("images/heart.png", scene, 25 + h * 47, 5, true); //하트
		heart[h]->setScale(0.04f);
	}



	//몬스터 랜덤으로 움직이게
	int stage = 2;
	Mons mobs[7];
	int mob_int[7] = { 0, 45, 24, 70, 15, 62, 32 }; //랜덤 위치
	Mons m1(board_x1, 300, 10, true);
	for (int i = 0; i < 7; i++) {
		if (i < 3) {
			mobs[i].x = board_x1 + (rand() % 100);
			mobs[i].y = 100 + (rand() % 500);
			mobs[i].speed = 10 + (rand() % 5);
			monsters[i] = Object::create("images/몬스터.png", scene, mobs[i].x, mobs[i].y);
			mob_timer[i] = Timer::create(0.1f);
		}
		else {
			mobs[i].x = board_x2 - (rand() % 100);
			mobs[i].y = board_y2 - (rand() % 500);
			mobs[i].speed = 10 + (rand() % 10);
			monsters[i] = Object::create("images/몬스터.png", scene, mobs[i].x, mobs[i].y);
			mob_timer[i] = Timer::create(0.1f);
		}
	}

	for (int i = 0; i < 7; i++) {
		mob_timer[i]->setOnTimerCallback([&, i](TimerPtr t)->bool {
			if (mob_int[i] <= 30) { // 위
				mobs[i] = up_move(scene, monsters[i], player, mobs[i], player_class, waterbomb, t_monster);

			}
			else if (30 < mob_int[i] && mob_int[i] <= 40) { // 아래
				mobs[i] = down_move(scene, monsters[i], player, mobs[i], player_class, waterbomb, t_monster);
			}
			else if (40 < mob_int[i] && mob_int[i] <= 60) { // 오른
				mobs[i] = right_move(scene, monsters[i], player, mobs[i], player_class, waterbomb, t_monster);
			}
			else if (60 < mob_int[i] && mob_int[i] <= 80) { //왼
				mobs[i] = left_move(scene, monsters[i], player, mobs[i], player_class, waterbomb, t_monster);
			}
			if (mob_int[i] == 81) { // 각 몬스터마다 다른 랜덤으로 움직이는 숫자 부여
				mob_int[0] = rand() % 80;
				mob_int[1] = rand() % 80;
				mob_int[2] = rand() % 80;
				mob_int[3] = rand() % 80;
				mob_int[4] = rand() % 80;
				mob_int[5] = rand() % 80;
				mob_int[6] = rand() % 80;
			}
			if (stage == 2 && mobs[i].islive == true) {
				mob_timer[i]->set(0.1f);
				mob_timer[i]->start();
				mob_int[i]++;
			}

			if (m_die == 7) {
				//몬스터 죽었는지
				auto winSound = Sound::create("music/Win.mp3");

				showMessage("WIN!\n다음 스테이지로 이동!");

				winSound->play();
				bgm->stop();
				gameoverTimer->stop();
				hideTimer();
				Sleep(1000);
				lobby_main();
			}

			if (heart_num == -1) { //캐릭터 하트 체크
				for (int i = 0; i < 7; i++) {
					mob_timer[i]->stop();
				}
				auto LoseSound = Sound::create("music/Lose.mp3");
				showMessage("다시 도전해보세요!");
				LoseSound->play();
				bgm->stop();
				gameoverTimer->stop();
				hideTimer();
				Sleep(1000);
				lobby_main();

				//로비 버튼. 리스타트 버튼 필요
			}
			return 0;
			});

	}

	bool water_use = false;
	scene->setOnKeyboardCallback([&](ScenePtr scene, int key, bool pressed)->bool {

		if ((player_class.py < board_y2 - 10 && key == 84)) {
			player_class.py = player_class.py + 10;
			player->setImage("images/up_01.png");
			player->locate(scene, player_class.px, player_class.py);
			return 0;
		}
		else if (player_class.py > board_y1 + 10 && key == 85)
		{
			player_class.py = player_class.py - 10;
			player->setImage("images/down_01.png");
			player->locate(scene, player_class.px, player_class.py);
			return 0;
		}
		else if (player_class.px > board_x1 + 10 && key == 82)
		{
			player_class.px = player_class.px - 10;
			player->setImage("images/left_01.png");
			player->locate(scene, player_class.px, player_class.py);
			return 0;
		}
		else if (player_class.px < board_x2 - 10 && key == 83)
		{
			player_class.px = player_class.px + 10;
			player->setImage("images/right_01.png");
			player->locate(scene, player_class.px, player_class.py);
			return 0;
		}
		else if (board_check(player_class.px, player_class.py) == true && key == 75 && water_use == false)
		{
			cout << "space" << endl;
			water->locate(scene, player_class.px, player_class.py);
			waterbomb.bx = player_class.px - 68; // 실제 위치와 보이는 위치를 같게 조정
			waterbomb.by = player_class.py - 68;
			water_bomb->locate(scene, waterbomb.bx, waterbomb.by);
			water->show();
			bomb_set->play();
			watertimer1->setOnTimerCallback([&](TimerPtr t)->bool {
				waterbomb.isbomb = true;//물풍선이 터져있을때만 몬스터 죽음.
				cout << "water" << endl;
				water->hide();
				bomb_pop->play();
				water_bomb->show();
				watertimer2->start();
				return 0;
				});
			watertimer2->setOnTimerCallback([&](TimerPtr t)->bool {
				cout << "waterbomb" << endl;
				waterbomb.isbomb = false;
				water_bomb->hide();
				water_use = false;
				return 0;
				});
			watertimer1->start();
			water_use = true;
			watertimer1->set(1);
			watertimer2->set(0.5f);
		}
		});

	for (int i = 0; i < 7; i++) {
		mob_timer[i]->start();
	}

	//시간 초과시 실패
	gameoverTimer->setOnTimerCallback([&](TimerPtr)-> bool {
		for (int i = 0; i < 7; i++) {
			monsters[i]->hide();
			mob_timer[i]->stop();
		}
		auto LoseSound = Sound::create("music/Lose.mp3");
		showMessage("시간초과 ㅠㅠ 다시 도전해보세요!");
		LoseSound->play();
		bgm->stop();
		gameoverTimer->stop();
		hideTimer();
		Sleep(1000);
		lobby_main();

		return true;
		});


	gameoverTimer->start();
	showTimer(gameoverTimer);
	bgm->play(true);
	startGame(scene);
}