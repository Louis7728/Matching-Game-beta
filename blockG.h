#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/VertexArray.hpp>

void sqrMake(sf::VertexArray& vta, sf::Vector2f pos, int colori);

class matchP;

class blockG
{
public:

	blockG(float Gx, float Gy, int colori, std::vector<int>& grid, bool vt_cfg);
	void fall(std::vector<int>& grid); // tries to fall one cell
	void match_find(std::vector<blockG*>& BlockGv, std::vector<matchP*>& MatchPv);
	void promote(); // raise to next color

	sf::VertexArray vta;  // vertexes for one square

	bool space = false,   // is there space in column for a blockQ to drop
	     settled = false; // can still fall

	sf::Vector2f pos{ 0,0 }; // position in cells, x: 1-8, y: 3-9
	int cli = 0,             // color int
	    id = 0,   // id of blockG's set
	    age = 0;  // for comparing how recent blockG's are
};
