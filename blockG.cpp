#include <SFML/Graphics.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "blockG.h"
#include "matchP.h"

blockG::blockG(float Gx, float Gy, int colori, std::vector<int>& grid, bool vt_cfg) : cli{ colori }
{
    pos.x = Gx;
    pos.y = Gy;

    id = static_cast<int>(10 * pos.x + pos.y);

    // check for collision in starting cell
    if (grid[id] == -1)
    {
        // either not vertical or  top of pair or  is vertical with space for both blocks
        if (!vt_cfg or Gy == 2 or (vt_cfg and grid[id + 1] == -1))
        {
            sqrMake(vta, pos, cli); // make block in vertex array
            grid[id] = cli;         // set grid cell to color
            space = true;           // confirms block creation
        }
    }
    else space = false;
}

void blockG::fall(std::vector<int>& grid) // tries to fall one cell
{
    // check for empty cell below and if in vertical bound
    if (grid[id + 1] == -1 and pos.y < 10)
    {
        vta.clear();
        grid[id] = -1;  // reset cell

        pos.y += 1;
        id += 1;
        grid[id] = cli; // fill new cell
        sqrMake(vta, pos, cli);

        settled = false;
    }
    else settled = true;
}

void blockG::match_find(std::vector<blockG*>& BlockGv, std::vector<matchP*>& MatchPv)
{
    // look for adjacent block with matching color 
    for (blockG* bG_adj : BlockGv)
    {
        float bGx = bG_adj->pos.x,   // values for adjacent block checks
              bGy = bG_adj->pos.y;
        int  bGci = bG_adj->cli;

        bool rt = pos.y == bGy and pos.x + 1 == bGx,
             dn = pos.x == bGx and pos.y + 1 == bGy; // ...this could be done more simply using bG's id

        if ((rt or dn) and cli == bGci) // if adjacent and same color then add new match to vector
        {
            matchP* mP = new matchP(id, bG_adj->id);
            MatchPv.push_back(mP);
        }
    }
}

void blockG::promote() 
{
    // raise to next color, called when a set merges
    vta.clear();
    cli += 1;
    sqrMake(vta, pos, cli);
}