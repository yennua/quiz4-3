﻿#include <bangtal>
using namespace bangtal;

#include <iostream>
using namespace std;

#include <cstdlib>
#include <ctime>

int puzzleXY[10][2] = { { 320, 469 }, { 534, 469 }, { 748, 469 }, { 320, 255 }, { 534, 255 }, { 748, 255 }, { 320, 41 }, { 534, 41 }, { 748, 41 },};
int now, cnt;
int blank = 3;
ObjectPtr puzzle[10];
ObjectPtr origin_p[10]; //원본
int game = 0; //초기화 작업 중일때와 게임 진행 중일때를 구분하기 위한 변수

int puzzle_i(ObjectPtr object) {
	for (int i = 0; i < 9; i++) {
		if (puzzle[i] == object) return i+1;
	}
	return -1;
}

bool clearCheck() {
	for (int i = 0; i < 9;i++) {
		if (puzzle[i] != origin_p[i]) return false;
	}
	return true;
}


void checkSwap(ScenePtr back) {
	int temp;
	if ((now != blank - 3 && now != blank - 1 && now != blank + 1 && now != blank + 3) || (now % 3 == 0 && blank == now + 1) || (now % 3 == 1 && blank == now - 1)) {
	}
	else{
		ObjectPtr tempObj = puzzle[now-1];
		puzzle[now - 1] = puzzle[blank - 1];
		puzzle[now - 1]->locate(back, puzzleXY[now - 1][0], puzzleXY[now - 1][1]);
		puzzle[blank - 1] = tempObj;
		puzzle[blank - 1]->locate(back, puzzleXY[blank - 1][0], puzzleXY[blank - 1][1]);

		temp = now;
		now = blank;
		blank = temp;

		cout << blank << "<-" << now << endl;

		if(game==1)	cnt++; //게임 진행중이면 횟수 증가
	}
}

void randMove(ScenePtr back) {
	int near[4] = { -3, -1, 1, 3 };
	int noww;
	srand((unsigned int)time(NULL));
	cout << cnt << endl;

	do {
		noww = blank + near[rand() % 4];
	} while (noww < 1 || noww > 9 || now==noww);
	now = noww;
	checkSwap(back);
	/*
	clock_t delay = 100;
	clock_t start = clock();
	while (clock() - start < delay);
	*/
	if (cnt == 1 && clearCheck()) cnt = 500;
}

int main()
{
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

	auto back_G = Scene::create("라이언 슬라이딩 퍼즐", "Images/배경.png");

	auto timer = Timer::create(0.f);

	auto soundT = Timer::create(21.f);
	SoundPtr sound = Sound::create("Sounds/폭탄소리.mp3");
	SoundPtr sound2 = Sound::create("Sounds/시작.mp3");

	soundT->setOnTimerCallback([&](TimerPtr timer)->bool {
		if (game == 1) sound->play(false);
		timer->stop();
		return true;
		});

	int challnge = 1;
	timer->setOnTimerCallback([&](TimerPtr timer)->bool {
		if (game==1) challnge = 0;
		else {
			randMove(back_G);
			cnt--;
			if (cnt > 0) {
				timer->set(0.f);
				timer->start();
			}
			else {
				game = 1;
				cnt = 1;
				timer->stop();
				timer->set(40.f);
				timer->start();
				soundT->set(21.f);
				sound2->play(false);
				soundT->start();
			}
		}
		return true;
		});


	char path[20];
	for (int i = 0; i < 9; i++) {
		sprintf_s(path, "Images/%d.png", i + 1);
		//printf("%s", path);
		puzzle[i] = Object::create(path, back_G, puzzleXY[i][0], puzzleXY[i][1]);
		origin_p[i] = puzzle[i];

		puzzle[i]->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
			now = puzzle_i(object);
			checkSwap(back_G);

			if (clearCheck()) {
				timer->stop();
				char clear[100];
				if (challnge == 1) {
					sprintf_s(clear, "Clear! 슬라이드 횟수: %d 챌린지: Success!", cnt);
				}
				else sprintf_s(clear, "Clear! 슬라이드 횟수: %d 챌린지: Fail...", cnt);
				showMessage(clear);
			}

			return true;
			});
	}


	auto origin = Object::create("Images/원본.png", back_G, 0, 0);

	auto startButton = Object::create("Images/start.png", back_G, 980, 40);
	auto restartButton = Object::create("Images/restart.png", back_G, 980, 40);

	restartButton->hide();

	startButton->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		//게임 시작
		origin->hide();
		puzzle[blank-1]->hide();

		showTimer(timer);

		cnt = 500;

		startButton->hide();
		restartButton->show();
		timer->start();


		return true;
		});


	restartButton->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		//재시작
		blank = 3;

		for (int i = 0; i < 9; i++) 	puzzle[i] = origin_p[i];
		restartButton->hide();
		startButton->show();
		timer->stop();
		game = 0;
		timer->set(0.f);
		return true;
		});

	startGame(back_G);
}