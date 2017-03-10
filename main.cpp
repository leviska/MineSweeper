#include <bits/stdc++.h>
#include <SFML/Graphics.hpp>
using namespace std;

const int N = 15;
const int M = 20;
const int SPRITE_SIZE = 16;
const int BOMB_COUNT = 50;

int field[M + 2][N + 2];
int output[M][N];
bool generating;
int curbomb;
bool gameOver;

void openCell(sf::Vector2i pos) {
	queue<pair<int, int>> bfs;
	auto add = [&bfs](int i, int j) {
		if(i >= 0 && j >= 0 && i < M && j < N && output[i][j] == 11) {
			if(field[i + 1][j + 1] == 0) {
				output[i][j] = field[i + 1][j + 1];
				bfs.push({i, j});
			}
			else if(field[i + 1][j + 1] < 9) {
				output[i][j] = field[i + 1][j + 1];
			}
		}
	};
	if(output[pos.y][pos.x] != 10) {
		output[pos.y][pos.x] = field[pos.y + 1][pos.x + 1];
		if(output[pos.y][pos.x] == 9) {
			gameOver = true;
		}
		if(output[pos.y][pos.x] == 0) {
			bfs.push({pos.y, pos.x});
			while(!bfs.empty()) {
				auto v = bfs.front();
				bfs.pop();
				add(v.first - 1, v.second - 1);
				add(v.first - 1, v.second + 1);
				add(v.first + 1, v.second - 1);
				add(v.first + 1, v.second + 1);
				add(v.first - 1, v.second);
				add(v.first, v.second - 1);
				add(v.first + 1, v.second);
				add(v.first, v.second + 1);
			}
		}
	}
}

void leftClick(sf::Vector2i pos) {
	openCell(pos);
}

void rightClick(sf::Vector2i pos) {
	if(output[pos.y][pos.x] >= 9) {
		if(output[pos.y][pos.x] != 10) {
			output[pos.y][pos.x] = 10;
		}
		else {
			output[pos.y][pos.x] = 11;
		}
	}
	else if(output[pos.y][pos.x] > 0) {
		int bombs = 0;
		auto count = [&bombs](int i, int j, bool did) {
			if(i >= 0 && j >= 0 && i < M && j < N) {
				if(output[i][j] == 10) {
					bombs++;
				}
				else if(did) {
					openCell(sf::Vector2i(j, i));
				}
			}
		};
		int i = pos.y;
		int j = pos.x;
		count(i - 1, j - 1, 0);
		count(i + 1, j + 1, 0);
		count(i - 1, j + 1, 0);
		count(i + 1, j - 1, 0);
		count(i - 1, j, 0);
		count(i + 1, j, 0);
		count(i, j + 1, 0);
		count(i, j - 1, 0);
		if(bombs >= output[i][j]) {
			count(i - 1, j - 1, 1);
			count(i + 1, j + 1, 1);
			count(i - 1, j + 1, 1);
			count(i + 1, j - 1, 1);
			count(i - 1, j, 1);
			count(i + 1, j, 1);
			count(i, j + 1, 1);
			count(i, j - 1, 1);
		}
	}
}

int main() {
	srand(time(0));
	sf::RenderWindow window(sf::VideoMode(N * SPRITE_SIZE, M * SPRITE_SIZE), "Minesweeper");
	sf::Texture text;
	text.loadFromFile("res/sprite.png");
	generating = true;
	curbomb = 0;
	gameOver = false;

	while(window.isOpen()) {
		if(generating) {
			for(int i = 0; i < M; i++) {
				for(int j = 0; j < N; j++) {
					output[i][j] = 11;
				}
			}
			for(int i = 0; i <= M + 1; i++) {
				for(int j = 0; j <= N + 1; j++) {
					field[i][j] = 0;
				}
			}
			curbomb = 0;
			while(curbomb < BOMB_COUNT) {
				int i = rand() % M + 1;
				int j = rand() % N + 1;
				if(field[i][j] != 9) {
					field[i][j] = 9;
					auto inc = [](int i, int j) mutable { 
						if(field[i][j] < 9) field[i][j]++;
					};
					inc(i - 1, j - 1);
					inc(i - 1, j);
					inc(i - 1, j + 1);
					inc(i, j - 1);
					inc(i, j + 1);
					inc(i + 1, j - 1);
					inc(i + 1, j);
					inc(i + 1, j + 1);
					curbomb++;
				}
			}
			gameOver = false;
			generating = false;
		}
		
		sf::Event event;
		while (window.pollEvent(event)) {
			if(event.type == sf::Event::Closed) {
				window.close();
			}
			if(event.type == sf::Event::MouseButtonPressed) {
				if(gameOver) {
					generating = true;
					continue;
				}
				if(event.mouseButton.button == sf::Mouse::Left) {
					sf::Vector2i pos = sf::Mouse::getPosition(window);
					pos /= SPRITE_SIZE;
					leftClick(pos);
				}
				else if(event.mouseButton.button == sf::Mouse::Right) {
					sf::Vector2i pos = sf::Mouse::getPosition(window);
					pos /= SPRITE_SIZE;
					rightClick(pos);
				}
			}
		}

		window.clear(sf::Color::Black);
		for(int i = 0; i < M; i++) {
			for(int j = 0; j < N; j++) {
				if(gameOver) {
					output[i][j] = field[i + 1][j + 1];
				}
				sf::Sprite sprite;
				sprite.setTexture(text);
				sf::IntRect rect;
				rect.width = SPRITE_SIZE;
				rect.height = SPRITE_SIZE;

				int val = output[i][j];
				if(val <= 8) {
					rect.top = 0;
					rect.left = SPRITE_SIZE * val;
				}
				else {
					rect.top = SPRITE_SIZE;
					rect.left = 0;
					if(val > 9) {
						rect.left = val - 6;
					}
					rect.left *= SPRITE_SIZE;
				}
				sprite.setTextureRect(rect);
				sprite.setPosition(sf::Vector2f(j * SPRITE_SIZE, i * SPRITE_SIZE));
				window.draw(sprite);
			}
		}
		window.display();
	}
	return 0;
}
