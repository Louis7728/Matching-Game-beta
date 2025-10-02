#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/VertexArray.hpp>

void sqrMake(sf::VertexArray& vta, sf::Vector2f pos, int colori);

class blockQ
{
public:
	blockQ(int colori1, int colori2);
	void Qmove(int key); // move left or right within bounds, left: 2, right: 0
	void Qrotate();      // cycle to next cfg

	float pos = 1;            // x position in # of cells 1-8
	int   cfg = 0;            // configuration/rotation 0-3
	int   cli1, cli2;     // int color values 0-6
	sf::VertexArray vta;  // contains vertexs of two squares

private:
	// config vector, indexed with cfg, +2 for second block as cycle offset
	std::vector<sf::Vector2f> cfgv{ {0,2}, {0,2}, {1,2}, {0,1}, {0,2}, {0,2} };
	
	void pair_init();  // makes block pair in queue, row is 2 cells from screen top
};
