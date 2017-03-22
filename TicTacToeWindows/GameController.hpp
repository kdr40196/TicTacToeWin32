#pragma once

#include<Windows.h>
#include<string>
#include"Player.hpp"

extern const int CELL_SIZE;

class GameController {
	static GameController* instance;

	Player *player1, *player2;
	bool bWin, bTie;
	GameController() {
		player1 = new Player(1, CreateSolidBrush(RGB(200, 0, 0)));
		player2 = new Player(2, CreateSolidBrush(RGB(0, 0, 200)));
	}
	GameController(const GameController&) {}
	GameController operator = (const GameController) {}
public:
	~GameController();

	Player* board[3][3];
	Player* currentPlayer;
	
	int winningMoves[3];

	static GameController* getInstance();
	void GameController::resetInstance();
	BOOL getBoard(HWND& hWnd, RECT& boardRect);
	void drawLine(HDC& hdc, int x1, int y1, int x2, int y2);
	///<param name = "result">Text printed below the board. Pass nullptr if not needed.</param>
	void showTurn(HWND & hWnd, HDC & hdc, std::wstring* result);
	int getCell(HWND& hwnd, int xPos, int yPos);
	BOOL getCellRect(HWND& hwnd, int cellNo, RECT& cellRect);
	void reset();
	bool full();
	bool win();
	///<summary>Returns 1 if the player wins, -1 if tie, 0 otherwise</summary>
	int mark(HDC& hdc, RECT& cellRect, int cellNo);
};
