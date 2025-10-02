#include <SFML/Graphics.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "blockQ.h"

blockQ::blockQ(int colori1, int colori2) : cli1{ colori1 }, cli2{ colori2 }
{
    pair_init();    // draws two squares in queue with colors
}

void blockQ::Qmove(int key) // move left or right within bounds, left: 2, right: 0
{
    vta.clear();

    if (key == 2 and pos > 1)                                 pos -= 1;
    else if (key == 0 and (cfg == 0 or cfg == 2) and pos < 7) pos += 1;
    else if (key == 0 and (cfg == 1 or cfg == 3) and pos < 8) pos += 1;
    // (furthest right pos allowed changes with cfg)

    pair_init();
}

void blockQ::Qrotate() // cycle to next cfg
{
    vta.clear();

    // if vertical and at right end don't rotate
    if ((cfg == 1 or cfg == 3) and pos == 8);

    else if (cfg < 3) cfg += 1; // advance
    else              cfg = 0;  // reset

    pair_init();
}

void blockQ::pair_init() // makes block pair in queue, row is 2 cells from screen top
{
    float x1 = pos + cfgv[cfg].x,     y1 = cfgv[cfg].y,
          x2 = pos + cfgv[cfg + 2].x, y2 = cfgv[cfg + 2].y;
    // (indexing cfgv with +2 lines up with the cycle of block positions in the 4 cfgs)

    sqrMake(vta, sf::Vector2f{ x1, y1 }, cli1);
    sqrMake(vta, sf::Vector2f{ x2, y2 }, cli2);
}