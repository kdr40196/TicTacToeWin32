#include "stdafx.h"
#include "GameController.hpp"

const int CELL_SIZE = 100;
GameController* GameController::instance = nullptr;


GameController::~GameController() {
	delete player1, player2, currentPlayer;
	instance = nullptr;
	player1 = player2 = currentPlayer = nullptr;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			board[i][j] = nullptr;
		}
	}
}

GameController* GameController::getInstance() {
	if (instance == nullptr) {
		instance = new GameController();
	}
	return instance;
}

void GameController::resetInstance() {
	delete instance;
	instance = nullptr;
}

BOOL GameController::getBoard(HWND& hWnd, RECT& boardRect) {
	RECT clientRect;
	if (GetClientRect(hWnd, &clientRect)) {
		int w = clientRect.right - clientRect.left;
		int h = clientRect.bottom - clientRect.top;

		boardRect.left = (w - CELL_SIZE * 3) / 2;
		boardRect.top = (h - CELL_SIZE * 3) / 2;
		boardRect.right = boardRect.left + CELL_SIZE * 3;
		boardRect.bottom = boardRect.top + CELL_SIZE * 3;

		return TRUE;
	}
	return FALSE;
}

void GameController::drawLine(HDC& hdc, int x1, int y1, int x2, int y2) {
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}

void GameController::showTurn(HWND & hWnd, HDC & hdc, std::wstring* result) {
	RECT rect;
	std::wstring turnText = L"";
	currentPlayer->id == 1 ? SetTextColor(hdc, RGB(200, 0, 0)) : SetTextColor(hdc, RGB(0, 0, 200));
	if (GetClientRect(hWnd, &rect)) {
		if (bWin) {
			turnText = L"Player " + std::to_wstring(currentPlayer->id) + L" Wins!!";
			if (result != nullptr)
				*result = turnText;
		}
		else if (bTie) {
			SetTextColor(hdc, RGB(0, 0, 0));
			turnText = L"TIE!!";
			if (result != nullptr)
				*result = turnText;
		}
		else turnText = L"Turn: Player " + std::to_wstring(currentPlayer->id);
		SetBkMode(hdc, TRANSPARENT);
		rect.top = rect.bottom - 50;
		FillRect(hdc, &rect, (HBRUSH)GetStockObject(GRAY_BRUSH));
		DrawText(hdc, turnText.c_str(), turnText.length(), &rect, DT_CENTER);
	}
}

int GameController::getCell(HWND& hwnd, int xPos, int yPos) {
	RECT boardRect;
	if (getBoard(hwnd, boardRect)) {
		if (PtInRect(&boardRect, { xPos, yPos })) {
			int x = (xPos - boardRect.left) / CELL_SIZE;
			int y = (yPos - boardRect.top) / CELL_SIZE;
			return x + y * 3;
		}
	}
	return -1;
}

BOOL GameController::getCellRect(HWND& hwnd, int cellNo, RECT& cellRect) {
	RECT boardRect;
	if (getBoard(hwnd, boardRect)) {

		int x = cellNo % 3;			//column
		int y = cellNo / 3;			//row
		cellRect.left = boardRect.left + CELL_SIZE * x + 1;
		cellRect.right = cellRect.left + CELL_SIZE - 2;
		cellRect.top = boardRect.top + CELL_SIZE * y + 1;
		cellRect.bottom = cellRect.top + CELL_SIZE - 2;
		return TRUE;
	}

	return FALSE;
}

void GameController::reset() {
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			board[i][j] = nullptr;
	winningMoves[0] = winningMoves[1] = winningMoves[1] = -1;
	currentPlayer = player1;
	bWin = bTie = false;
}

bool GameController::full() {
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			if (board[i][j] == nullptr)
				return false;
	return true;
}

bool GameController::win() {
	//row-wise
	for (int i = 0; i < 3; i++) {
		if (board[i][0] && board[i][0] == board[i][1] && board[i][1] == board[i][2]) {
			winningMoves[0] = i * 3;
			winningMoves[1] = 1 + i * 3;
			winningMoves[2] = 2 + i * 3;
			return true;
		}
	}

	//col-wise
	for (int j = 0; j < 3; j++) {
		if (board[0][j] && board[0][j] == board[1][j] && board[1][j] == board[2][j]) {
			winningMoves[0] = j;
			winningMoves[1] = j + 3;
			winningMoves[2] = j + 6;
			return true;
		}
	}

	//diagonally
	if (board[0][0] && board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
		winningMoves[0] = 0;
		winningMoves[1] = 4;
		winningMoves[2] = 8;
		return true;
	}

	else if (board[0][2] && board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
		winningMoves[0] = 2;
		winningMoves[1] = 4;
		winningMoves[2] = 6;
		return true;
	}

	return false;
}


//return win or tie
int GameController::mark(HDC& hdc, RECT& cellRect, int cellNo) {
	if (bWin || bTie) return 0;

	int row = cellNo / 3, col = cellNo % 3;

	if (board[row][col] == nullptr) {
		board[row][col] = currentPlayer;
		FillRect(hdc, &cellRect, currentPlayer->brush);

		//DrawIcon(...)

		if (win()) {
			bWin = true;
			return 1;
		}
		else if (full()) {
			bTie = true;
			return -1;
		}
		else if (currentPlayer == player1) {
			currentPlayer = player2;
		}
		else {
			currentPlayer = player1;
		}
	}

	return 0;
}
