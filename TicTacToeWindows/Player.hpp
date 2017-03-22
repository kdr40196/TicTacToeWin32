#pragma once

#include<Windows.h>

class Player {
public:
	//HICON icon;
	HBRUSH brush;
	int id;
	Player(int id, HBRUSH brush);
	Player& operator = (Player& other);
	~Player();
	bool operator == (Player& other);
};
