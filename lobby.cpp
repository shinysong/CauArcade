#include <iostream>
#include <bangtal>
#include <ctime>

using namespace bangtal;
using namespace std;

extern void one_main();
extern void two_main();
ScenePtr lobbyScene;
SoundPtr lobbySound;

void lobby_main() {
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

	lobbyScene = Scene::create("", "Images/대기실.png");
	lobbySound = Sound::create("music/lobby.mp3");

	lobbySound->play();

	auto ob = Object::create("Images/pan.png", lobbyScene, 0, 0);

	ob->setOnMouseCallback([&](ObjectPtr, int x, int y, MouseAction) -> bool {
		// cout << x << " " << y << endl;
		if (x > 380 && x < 800 && y > 475 && y < 575) {
			lobbySound->stop();
			one_main();
		}
		else if (x > 815 && x < 1235 && y > 475 && y < 575) {
			showMessage("3단계");
		}
		else if (x > 380 && x < 800 && y > 370 && y < 470) {
			lobbySound->stop();
			two_main();
		} 
		return true;
	});

	enterScene(lobbyScene->ID());
}