// Screen.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <conio.h>
#include <Windows.h>

using namespace std;

void draw(char* loc, const char* face)
{
	strncpy(loc, face, strlen(face));
}

void gotoxy(int x, int y)
{
	COORD pos = { x,y };

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

}

class Screen {
	int size;
	char* screen;

public:
	Screen(int sz) : size(sz), screen(new char[sz + 1]) {}
	~Screen() { delete[] screen; }

	void draw(int pos, const char* face) 
	{
		if (face == nullptr) return;
		if (pos < 0 || pos >= size) return;
		strncpy(&screen[pos], face, strlen(face));
	}

	void render() 
	{
		printf("%s\r", screen);
	}

	void clear()
	{
		memset(screen, ' ', size);
		screen[size] = '\0';
	}

	int length()
	{
		return size;
	}

};

class GameObject {
	int pos;
	char face[20];
	Screen* screen;

public:
	GameObject() { };
	GameObject(int pos, const char* face, Screen* screen)
		: pos(pos), screen(screen)
	{
		strcpy(this->face, face);
	}
	char* getFace()
	{
		return face;
	}

	void setFace(const char* face)
	{
		strcpy(this->face, face);
	}
	
	int getPosition()
	{
		return pos;
	}
	void setPosition(int pos)
	{
		this->pos = pos;
	}
	void draw()
	{
		screen->draw(pos, face);
	}
};

class Player : public GameObject {
	
	bool right;
	int hp;
	int count;
	bool IsPoison;

public:
	Player(int pos, const char* face, Screen* screen)
		: GameObject(pos, face, screen), hp(10), count(0), IsPoison(false)
	{	
		this->right = true;
	}
	
	void moveLeft()
	{
		setPosition(getPosition() - 1);
		right = false;
	}

	void moveRight()
	{
		setPosition(getPosition() + 1);
		right = true;
	}

	void setHp()
	{
		hp = hp - 1;
	}

	int getHp()
	{
		return hp;
	}

	void getPoison()
	{
		IsPoison = true;
	}

	void attack_by_enemy(int enemy_pos)
	{
		if (getPosition() - 4 >= enemy_pos && getPosition() <= enemy_pos + 8)
		{
			getPoison();
		}

	}

	void setCount()
	{
		count++;
	}

	bool getway()
	{
		return right;
	}

	void update(int enemy_pos)
	{
		attack_by_enemy(enemy_pos);
		if (IsPoison)
		{
			setCount();
			if (count % 20 == 0)
			{
				setHp();
				if (count == 60)
				{
					count = 0;
					IsPoison = false;
				}
			}
		}
	}

};

class Enemy : public GameObject {
	
	int hp;
	int damage;
	int count;

public:
	Enemy(int pos, const char* face, Screen* screen)
		: GameObject(pos, face, screen), hp(5), damage(1), count(0)
	{
	}

	/*void moveRandom()
	{
		setPosition( getPosition() + rand() % 3 - 1);
	}*/

	void move(int player_pos)
	{
		if (getPosition() > player_pos)
			setPosition(getPosition() - 1);

		else if (getPosition() < player_pos)
			setPosition(getPosition() + 1);
		else
			setPosition(getPosition());
	}

	void setHp()
	{
		hp--;
	}


	int getHp()
	{
		return hp;
	}

	int count_move()
	{
		count++;
		return count;
	}

	void update(int player_pos)
	{
		//moveRandom();
		count_move();
		if (count == 10)
		{
			count = 0;
			move(player_pos);
		}
	}
};

class Bullet : public GameObject {
	bool isFiring;
	bool isMod;
	bool wayFiring;
	int count;
	bool Isreload;
	bool IsAttack;

public:
	Bullet(){}
	Bullet(int pos, const char* face, Screen* screen)
		: GameObject(pos, face, screen), isFiring(false), wayFiring(false), isMod(true), count(0), Isreload(false),IsAttack(false)
	{
	}

	void moveLeft()
	{
		setPosition(getPosition() - 1);
	}

	void moveRight()
	{
		setPosition(getPosition() + 1);
	}

	bool getMod()
	{
		return isMod;
	}

	void checkMod()
	{
		if (isMod == true)
		{
			isMod = false;
		}
		else if (isMod == false)
		{
			isMod = true;
		}
	}

	void draw()
	{
		if (isFiring == false) return;
		GameObject::draw();
	}

	void fire(int player_pos , bool get_way)
	{
		if (isMod == false) return;
		isFiring = true;
		wayFiring = get_way;
		if (wayFiring)
		{
			setPosition(player_pos + 8);
		}

		else if (wayFiring == false)
		{
			setPosition(player_pos - 1);
		}

	}

	bool getReload()
	{
		return Isreload;
	}

	void setAttack()
	{
		IsAttack = true;
	}

	bool getAttack()
	{
		return IsAttack;
	}

	void reloadBullet()
	{
		setPosition(-1);
		Isreload = false;
		isFiring = false;
	}

	void update(int enemy_pos, Enemy* enemy)
	{
		if (getReload())
		{
			count++;
			if (count == 10)
			{
				reloadBullet();
				count = 0;
			}
		}

		if (isFiring == false) return;
		int pos = getPosition();

		if (wayFiring && ((enemy_pos - 2) <= pos && pos <= (enemy_pos + 3)) ) {
			pos = pos + 1;
		}

		else if (wayFiring == false && (enemy_pos) <= pos && pos <= (enemy_pos + 6 )&& enemy->getHp() > 0) {
			enemy->setHp();
			isFiring = false;
		}

		else if (wayFiring == false && pos != 0) {
			pos = pos - 1;
		}
		else if ((enemy_pos - 2) <= pos && pos <= (enemy_pos + 3) && enemy->getHp() > 0){
			enemy->setHp();
			isFiring = false;
		}
		else if (wayFiring && pos == 81)
		{
			isFiring = false;
		}

		else
		{
			isFiring = false;
		}
		setPosition(pos);
	}
};


class Laser : public GameObject {
	bool isFiring;
	bool isMod;
	bool wayFiring;

public:
	Laser(int pos, const char* face, Screen* screen)
		: GameObject(pos, face, screen), isFiring(false), wayFiring(false), isMod(false)
	{
	}

	void moveLeft()
	{
		setPosition(getPosition() - 1);
	}

	void moveRight()
	{
		setPosition(getPosition() + 1);
	}

	void checkMod()
	{
		if (isMod == true)
		{
			isMod = false;
		}
		else if (isMod == false)
		{
			isMod = true;
		}
	}

	void draw()
	{
		if (isFiring == false) return;
		GameObject::draw();
	}

	void fire(int player_pos, bool get_way)
	{
		if (isMod == false) return;
		isFiring = true;
		wayFiring = get_way;

		if (get_way)
		{
			setPosition(player_pos + 9);
		}

		else if (get_way == false)
		{
			setPosition(player_pos - 2);
		}

	}

	void update(int player_pos, int enemy_pos , bool get_way, Screen* screen)
	{

		if (isFiring == false) return;
		
		int num = enemy_pos - getPosition();
		int pos = getPosition();
		

		if (wayFiring && (enemy_pos - 2) >= pos && pos <= (enemy_pos + 3)) {
			pos = pos + 1;
			draw();

		}
		else if (wayFiring == false && pos != 0) {
			pos = pos - 1;
			draw();
		}
		else {
			isFiring = false;
		}
		
		setPosition(pos);
	}
};


int main()
{
	int bullet_count = 9;
	int max_bullets = 10;
	Screen screen{ 80 };
	Player player = { 60, "( ^_^)┌", &screen };
	Enemy enemy{ 10, "(*--*)", &screen };
	Bullet* bullets[10];
	Laser laser(-1, "=", &screen);

	for (int i = 0; i <= max_bullets; i++)
	{
		bullets[i] = new Bullet(-1, "+", &screen);
	}


	while (true)
	{
		screen.clear();

		if (_kbhit())
		{
			int c = _getch();
			switch (c) {
			case 'a':
			case 'A':
				player.setFace("┐(^_^ )");
				player.moveLeft();
				break;
			case 'd':
			case 'D':
				player.setFace("( ^_^)┌");
				player.moveRight();
				break;
			case 'm':
			case 'M':
				for (int i = 0; i <= max_bullets; i++)
				{
					bullets[i]->checkMod();
				}
				laser.checkMod();
				break;
			case ' ':
				laser.fire(player.getPosition(), player.getway());
				int j;
				for (j = 0; j <= max_bullets; j++)
					if (bullets[j]->getPosition() == -1)
						break; 
				if (j == max_bullets)
				{
					break;
				}
				bullets[j]->fire(player.getPosition() , player.getway());
				bullet_count = bullet_count - 1;
				break;
			}
		}
		player.draw();


		// 총알 리로드 조건? 10번째가 -1이 아니고 0번째가 -1이 아니다.
		if (bullets[max_bullets - 1]->getPosition() != -1)
		{
			for (int i = 0; i < max_bullets; i++)
			{
				bullets[i]->reloadBullet();
				bullet_count = 9;
			}
		}
		
		for (int i = 0; i <= max_bullets; i++)
		{
			bullets[i]->draw();
		};
		laser.draw();

		player.update(enemy.getPosition());
		for (int i = 0; i <= max_bullets; i++)
		{
			bullets[i]->update(enemy.getPosition(), &enemy);
		}
		
		enemy.update(player.getPosition());
		laser.update(player.getPosition(), enemy.getPosition(), player.getway(), &screen);
		
		if (enemy.getHp() > 0)
		{
			enemy.draw();
		}


		gotoxy(0, 6);
		printf("총알수 : \n %d ", bullet_count);
		gotoxy(0, 12);
		if (bullets[0]->getMod())
			printf("현재모드 : 총");
		else if (bullets[0]->getMod() == false)
			printf("현재모드 : 레이저건");
		gotoxy(0, 8);
		printf("적체력  :  \n %d ", enemy.getHp());
		gotoxy(0, 10);
		printf("플레이어 체력 : %d", player.getHp());
		gotoxy(0, 0);

		screen.render();
		Sleep(66);
	}

	return 0;
}