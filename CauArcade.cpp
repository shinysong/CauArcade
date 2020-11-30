#include <iostream>
#include <bangtal>
#include <ctime>

using namespace bangtal;
using namespace std;

extern void lobby_main();

ScenePtr openScene;
SoundPtr openSound;

int main() {
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);

	openScene = Scene::create("", "Images/시작화면.png");
	openSound = Sound::create("music/opening.mp3");

	openSound->play();

	auto ob = Object::create("Images/pan.png", openScene, 0, 0);

	ob->setOnMouseCallback([&](ObjectPtr, int x, int y, MouseAction) -> bool {
		//cout << x << " " << y << endl;

		if (x > 930 && x < 1210 && y > 40 && y < 105) {
			openSound->stop();
			lobby_main();
		}
		return true;
		});

	startGame(openScene);
	return 0;
}