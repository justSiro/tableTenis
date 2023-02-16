#include <iostream>
#include <set>
#include <windows.h>
#include <cassert>
#include <conio.h>
using namespace std;

#define SCREEN_WIDTH 70
#define SCREEN_HEIGHT 25
#define ref_rate 50 //Refresh frame every 100ms

enum dir { UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT, STABLE }; //Direction of balls
//short GetAsynKeyState(int key);

unsigned int PlayerSize = 4;
unsigned int PlayerP2 = (SCREEN_HEIGHT - PlayerSize) / 2 - 1; //Y coordinate is not needed-player is always at the bottom
unsigned int PlayerP1 = (SCREEN_HEIGHT - PlayerSize) / 2 - 1; //Y coordinate is not needed-player is always at the bottom
unsigned int Player_move_space = 2; //Player can move -+7 blocks per refresh
unsigned int score[2] = { 0, 0 };
bool winner = false;
bool defeat = false;

class ball {
public:
	inline bool operator<(const ball& b) const {
		if (coordY != b.coordY) return coordY < b.coordY;
		return coordX < b.coordX;
	}

	unsigned int getX() const { return coordX; }

	unsigned int getY() const { return coordY; }

	void SetCoords(int x, int y) {
		assert(x >= 0 && x < SCREEN_WIDTH&& y >= 0 && y < SCREEN_HEIGHT); //Assert position is in the map
		coordX = x;
		coordY = y;
	}

	dir GetDir()const { return direction; } //Returns direction of ball

	void setDir(dir new_dir) { direction = new_dir; } //Changes direction of ball

private:
	unsigned int coordX = 0, coordY = 0;
	dir direction = DOWN_RIGHT; //Ball starts moving towards the lower right angle of the frame
};

void new_ball(set<ball>& vec, int coordx = SCREEN_WIDTH / 2, int coordy = SCREEN_HEIGHT / 2) {
	ball b;
	if (coordx || coordy) b.SetCoords(coordx, coordy);
	//Set new coordinates only if they have been provided and are not default (0,0)

	vec.insert(b);
}

void draw(set<ball> vec, unsigned int PlayerP2, unsigned int PlayerSize) { //Draw the map
	auto it = vec.begin(); //Access first ball in the set
	bool left_ballz = vec.size(); //There are balls to be drawn(if size is not 0)

	for (unsigned int y = 0; y <= SCREEN_HEIGHT; y++) {
		for (unsigned int x = 0; x < SCREEN_WIDTH; x++) {
			if (left_ballz && x == it->getX() && y == it->getY()) { //If the coordinates match the current ball's coordinates
				cout << "8";
				if (++it == vec.end()) left_ballz = false; //Set next ball as available
			}
			else if (y == 0 || y == SCREEN_HEIGHT - 1) //If the first or last line is being drawn
				cout << "E";
			else if (x == 0 && y < SCREEN_HEIGHT) //If the first column is being drawn
				cout << ">";
			else if (x == SCREEN_WIDTH - 1 && y < SCREEN_HEIGHT) //If the last column is being drawn
				cout << "<";
			else if (x == (SCREEN_WIDTH - 1) / 2 && (y == 1 || y == SCREEN_HEIGHT - 2)) //If the first or last column is being drawn
				cout << "I";

			else if ((x == 1 || x == 2) && y >= PlayerP1 && y < PlayerP1 + PlayerSize && y < SCREEN_HEIGHT - 1) {
				cout << "H"; //Draw player
			}
			else if ((x == SCREEN_WIDTH - 2 || x == SCREEN_WIDTH - 3) && y >= PlayerP2 && y < PlayerP2 + PlayerSize) {
				cout << "H"; //Draw player
			}
			else cout << " ";
		}
		cout << "\n";
	}
	cout << "			SCORE: " << score[0] << '-' << score[1] << "\n" << "\n";
}

void drawEnd() {
	if (winner == false) cout << "player 2 win";
	else cout << "player 1 win" << "\n" << ' ';
	system("PAUSE");
}

void bounce(set<ball>& vec, ball a, bool& defeat) { //Logic function for ball movement
	switch (a.getX()) {
	case 3: //Collides with left wall
		if ((PlayerP1 > a.getY()) || (PlayerP1 + PlayerSize - 1 < a.getY())) {
			score[1]++;
			if (score[1] == 5) {
				winner = false;
				defeat = !true;
			}
			a.SetCoords((rand() % 9 + 10), (rand() % 9 + 10));
		}
		switch (a.GetDir()) {
		case DOWN_LEFT:
			a.setDir(DOWN_RIGHT);
			break;
		case UP_LEFT:
			a.setDir(UP_RIGHT);
			break;
		}
		break;
	case SCREEN_WIDTH - 4: //Collides with right wall
		if ((PlayerP2 > a.getY()) || (PlayerP2 + PlayerSize - 1 < a.getY())) {
			score[0]++;
			if (score[0] == 5) {
				winner = true;
				defeat = !true;
			}
			a.SetCoords((rand() % 9 +10), (rand() % 9 + 10));
		}
		switch (a.GetDir()) {
		case DOWN_RIGHT:
			a.setDir(DOWN_LEFT);
			break;
		case UP_RIGHT:
			a.setDir(UP_LEFT);
			break;
		}
		break;
	}

	switch (a.getY()) {
	case 1: //Collides with upper wall
		switch (a.GetDir()) {
		case UP_RIGHT:
			a.setDir(DOWN_RIGHT);
			break;
		case UP_LEFT:
			a.setDir(DOWN_LEFT);
			break;
		}
		break;
	case SCREEN_HEIGHT - 2: //Collides with lower wall(player territory)
		switch (a.GetDir()) {
		case DOWN_RIGHT:
			a.setDir(UP_RIGHT);
			break;
		case DOWN_LEFT:
			a.setDir(UP_LEFT);
			break;
		}
		break;
	}

	vec.insert(a); //Insert the ball back into the set after processing
}

void move_balls(set<ball>& vec, bool& defeat) {
	set<ball> sup_vec = vec;
	vec.clear();

	for (auto x : sup_vec) {
		ball a = x;
		switch (a.GetDir()) {
		case UP_LEFT:
			a.SetCoords(a.getX() - 1, a.getY() - 1);
			break;
		case UP_RIGHT:
			a.SetCoords(a.getX() + 1, a.getY() - 1);
			break;
		case DOWN_LEFT:
			a.SetCoords(a.getX() - 1, a.getY() + 1);
			break;
		case DOWN_RIGHT:
			a.SetCoords(a.getX() + 1, a.getY() + 1);
			break;
		}
		bounce(vec, a, defeat); //Logic for moving balls
	}
}

void move_player1() {
	if (_kbhit()) {
		switch (toupper(_getch())) { //Get single character input
		case 'W': //Move up
			if (Player_move_space >= PlayerP1) PlayerP1 = 1; //Do not allow player to exceed the limits of the map
			else PlayerP1 -= Player_move_space;
			break;
		case 'S': //Move down
			PlayerP1 += Player_move_space;
			if (PlayerP1 + PlayerSize >= SCREEN_HEIGHT - 1) PlayerP1 = SCREEN_HEIGHT - PlayerSize - 1;
			break;
		default:
			break;
		}
	}
}
void move_player2() {
	if (GetAsyncKeyState(VK_UP)) {
		if (Player_move_space >= PlayerP2) PlayerP2 = 1; //Do not allow player to exceed the limits of the map
		else PlayerP2 -= Player_move_space;
	}
	if (GetAsyncKeyState(VK_DOWN)) {
		PlayerP2 += Player_move_space;
		if (PlayerP2 + PlayerSize >= SCREEN_HEIGHT - 1) PlayerP2 = SCREEN_HEIGHT - PlayerSize - 1;
	}
}

void startGame() {
	cout << "To start the game, PRESS: [space]" << "\n";
	cout << "INTRODUCTION" << "\n";
	cout << "Player 1: [W] to go up, [S] to go down" << "\n";
	cout << "Player 2: [arrow up] to go up, [arrow down] to go down" << "\n";
	while (1) {
		if (GetAsyncKeyState(VK_SPACE)) {
			system("cls"); //Erase frame
			defeat = true;
			set<ball> ball_vec;
			new_ball(ball_vec, SCREEN_WIDTH / 2 + 3, SCREEN_HEIGHT / 2 - 1); //Get 1 ball at the beginning

			while (defeat) {
				move_balls(ball_vec, defeat);
				draw(ball_vec, PlayerP2, PlayerSize);
				Sleep(ref_rate);
				move_player1();
				move_player2();
				system("cls"); //Erase frame
			}
			drawEnd();
			break;
		}
		
	}
}

int main() {
	startGame();
}

