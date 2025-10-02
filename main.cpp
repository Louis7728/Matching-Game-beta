#include <iostream> 
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "blockQ.h"
#include "blockG.h"
#include "matchP.h"
#include "matchS.h"

// remake of a flash game called 'Digital Upgrade'
// block pairs spawn in a queue, can be moved left and right, rotated, and dropped
// matching 3 blocks of the same color merges the blocks into one of the next color
//
// current state: all basic mechanics working but only drops a set color pair
//
// to do: 
// 
// make rng
// organize program
// 
// check for game over or win state
// variable grid size (cell number)
// 
// draw match connectors
// make in-between frames
// 
// determine what makes easier or harder
// scale difficulty over time
// 


// global values for grid
unsigned int GW = 800;
unsigned int GH = 600;
float gb_ceL = 50;  // 10x12 grid in 500x600px
float gb_pdg = 150; // 100px padding on sides


sf::VertexArray Bdr_init()
{
    // grid:    (100,0)   <--> (700,0) in 12 segments (50px)
    //          ~~~
    //          (100,600) <--> (700,600)
    // 
    // cell:    (0,0) <-> (24,0)-(25,0) <-> (50,0)
    //          (0,24)   
    //          (0,25)
    //          (0,50)


    // generates outline of game grid

    float ceL = gb_ceL, pdg = gb_pdg,
          hfL = 24, hfU = 25;

    std::vector<sf::Vector2f> vta{

        // border lines (2px wide, middle of cells)
        {pdg + hfL,         1*ceL + hfL}, {pdg + hfL,         2*ceL + hfU},
        {pdg + hfU,         1*ceL + hfL}, {pdg + hfU,         2*ceL + hfU},
        {pdg + 9*ceL + hfL, 1*ceL + hfL}, {pdg + 9*ceL + hfL, 2*ceL + hfU},
        {pdg + 9*ceL + hfU, 1*ceL + hfL}, {pdg + 9*ceL + hfU, 2*ceL + hfU},

        {pdg + hfL,         3*ceL + hfL}, {pdg + hfL,         11*ceL + hfU},
        {pdg + hfU,         3*ceL + hfL}, {pdg + hfU,         11*ceL + hfU},
        {pdg + 9*ceL + hfL, 3*ceL + hfL}, {pdg + 9*ceL + hfL, 11*ceL + hfU},
        {pdg + 9*ceL + hfU, 3*ceL + hfL}, {pdg + 9*ceL + hfU, 11*ceL + hfU},

        {pdg + hfL,        11*ceL + hfL}, {pdg + 9*ceL + hfU, 11*ceL + hfL},
        {pdg + hfL,        11*ceL + hfU}, {pdg + 9*ceL + hfU, 11*ceL + hfU},
    };

    sf::VertexArray Bdr(sf::PrimitiveType::Lines);
    for (const sf::Vector2f& vt : vta)
    {
        Bdr.append(sf::Vertex{ vt , { 128,128,128 } }); // color: gray 50%
    }
    return Bdr;
}

sf::Color int_to_sfcolor(int colori)
{
    // maps an object's int color to an sf::Color

    std::vector<sf::Color> cmap{ sf::Color::Red,       sf::Color{255,128,0}, sf::Color::Yellow,
                                 sf::Color{0,192,0},   sf::Color{0,192,192}, sf::Color::Blue,
                                 sf::Color{255,0,192}, sf::Color::Black };
    // 2nd element is orange, 4th is green, 5th is cyan, 7th is magenta

    return cmap[colori];
}

void sqrMake(sf::VertexArray& vta, sf::Vector2f pos, int colori)
{
    // given position and color, populates vertex array to make a square

    float ceL = gb_ceL, pdg = gb_pdg;

    float sqr_pdg = 5;                        // padding around squares
    sf::Color color = int_to_sfcolor(colori); // get color from int

    // square in given cell positon with 5px padding on each side
    sf::Vertex v1{ {pdg + ceL*pos.x +     sqr_pdg, ceL*pos.y +     sqr_pdg }, color };
    sf::Vertex v2{ {pdg + ceL*(pos.x+1) - sqr_pdg, ceL*pos.y +     sqr_pdg }, color };
    sf::Vertex v3{ {pdg + ceL*pos.x +     sqr_pdg, ceL*(pos.y+1) - sqr_pdg }, color };
    sf::Vertex v4{ {pdg + ceL*(pos.x+1) - sqr_pdg, ceL*(pos.y+1) - sqr_pdg }, color };
    
    vta.setPrimitiveType(sf::PrimitiveType::Triangles);
    std::vector<sf::Vertex> vtv{ v1,v2,v3,  v2,v3,v4 }; // makes two triangles
    for (const sf::Vertex& vt : vtv) vta.append(vt);
}

void bQ_to_bG(blockQ* BlockQ, std::vector<blockG*>& BlockGv, std::vector<int>& Grid, bool& success)
{
    // converts blocks dropped from queue to grid
    // adds two blockG to storage vector but does not delete blockQ

    float Gx1 = BlockQ->pos, Gx2 = BlockQ->pos, // x position of blockQ's
          Gy1 = 3,           Gy2 = 3;           // y pos of 1st grid row
    bool  vt_cfg = false; // is blockQ in a vertical config

    if      (BlockQ->cfg == 0)   // horizantal cfg's
        Gx2 += 1;

    else if (BlockQ->cfg == 2)
        Gx1 += 1;

    else if (BlockQ->cfg == 1) { // vertical cfg's
        Gy2 -= 1;
        vt_cfg = true;
    }
    else if (BlockQ->cfg == 3) {
        Gy1 -= 1;
        vt_cfg = true;
    }

    // initialize
    blockG* BG1 = nullptr;  blockG* BG2 = nullptr;
    success = false;

    if (Gy1 >= Gy2) // create lower block first 
    {
        BG1 = new blockG(Gx1, Gy1, BlockQ->cli1, Grid, vt_cfg); // sets BG1->space
        if (BG1->space) // if succeeded try to create second block
        {
            BG2 = new blockG(Gx2, Gy2, BlockQ->cli2, Grid, vt_cfg);
            if (BG2->space)
            {
                success = true;  // both succeeded
                BlockGv.push_back(BG1); BlockGv.push_back(BG2);
            }
            else { delete BG1; delete BG2; } // delete if failed
        }
        else delete BG1; 
    }
    else // same sequence if Gy2 > Gy1
    {
        BG2 = new blockG(Gx2, Gy2, BlockQ->cli2, Grid, vt_cfg);
        if (BG2->space)
        {
            BG1 = new blockG(Gx1, Gy1, BlockQ->cli1, Grid, vt_cfg);
            if (BG1->space)
            {
                success = true;
                BlockGv.push_back(BG2); BlockGv.push_back(BG1);
            }
            else { delete BG2; delete BG1; }
        }
    }
}

void mP_to_mS(std::vector<matchP*>& MatchPv, std::vector<matchS*>& MatchSv)
{
    for (int i = 0; i < MatchPv.size(); i++) {
        for (int j = 0; j < MatchPv.size(); j++) {

            matchP* MPA = MatchPv[i];
            matchP* MPB = MatchPv[j];

            bool match_overlap = MPA->id1 == MPB->id1 or MPA->id1 == MPB->id2 or
                MPA->id2 == MPB->id1 or MPA->id2 == MPB->id2;

            if (i != j and match_overlap)
            {
                // if neither in set, make new set
                if (MPA->set_id == -1 and MPB->set_id == -1)
                {
                    std::vector<int> idv{ MPA->id1,MPA->id2,MPB->id1,MPB->id2 };
                    matchS* MatchS = new matchS(idv);
                    MatchSv.push_back(MatchS);

                    MPA->set_id = MatchS->set_id; MPB->set_id = MatchS->set_id;
                }

                // if A in set but not B, add B
                else if (MPA->set_id != -1 and MPB->set_id == -1)
                {
                    for (matchS* MatchS : MatchSv)            // find A's set
                    {
                        if (MatchS->set_id == MPA->set_id)
                        {
                            MatchS->add(MPB->id1, MPB->id2); // add B
                        }
                        MPB->set_id = MatchS->set_id;
                    }
                }

                // if B in set but not A, add A
                else if (MPA->set_id == -1 and MPB->set_id != -1)
                {
                    for (matchS* MatchS : MatchSv)
                    {
                        if (MatchS->set_id == MPB->set_id)
                        {
                            MatchS->add(MPA->id1, MPA->id2);
                        }
                        MPA->set_id = MatchS->set_id;
                    }
                }

                // if both have sets then do nothing
            }
        }
    }
}

void grid_test_init(std::vector<blockG*>& BlockGv, std::vector<int>& Grid, int t_num)
{
    // for testing different grid block configurations

    std::vector<sf::Vector3f> input;

    if (t_num == 1) input =
    {
        {2,6,1},
        {2,8,1},
        //{2,10,1},
        {3,4,1}
    };

    if (t_num == 2) input =
    {
        {2,4,1},{3,4,1},        {5,4,1},{6,4,1},
        {2,6,4},{3,6,3},{4,6,1},{5,6,3},{6,6,4},
        {2,8,3},{3,8,4},{4,8,1},{5,8,4},{6,8,3},
    };

    for (sf::Vector3f in : input)
    {
        int cli_in = static_cast<int>(in.z);
        BlockGv.push_back(new blockG(in.x, in.y, cli_in, Grid, false));
    }
}


int main()
{
    // Border
    sf::VertexArray Border = Bdr_init(); // vertex array for borders

    // Queue
    blockQ* BlockQ = nullptr;     // queue block pointer
    std::vector<blockG*> BlockGv; // vector of grid blocks (ptr)

    // Grid
    std::vector<int> Grid(10 * 10); // grid of cells  incl bounds, ind: 10*x + y
    for (int& cli : Grid) cli = -1; // initialize grid to empty
    bool G_success = false;         // if successfully converted from queue to grid blocks

    // Match storage
    std::vector<matchP*> MatchPv;  // vector of matched pairs (ptr)
    std::vector<matchS*> MatchSv;  // vector of sets of >2 (ptr)

    // Matching cycle
    int fr_ct = 0;             // frame counter
    bool first_m_cycle = true, // set to false after 1st matching cycle
         all_stl = true,       // if all blockG's have fallen to bottom
         all_mch = false;      // after settled, if all matches are made

    grid_test_init(BlockGv, Grid, 1); // 0 for no test

    // Window and Game Cycle
    sf::RenderWindow window(sf::VideoMode({ GW, GH }), "Matching Game");
    window.setFramerateLimit(30);

    while (window.isOpen())
    {
        // Controls
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) window.close();

            // right,left - move, up - rotate, down - drop, esc - exit
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                bool Bk = (BlockQ != nullptr); // is there a block in the queue

                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) window.close();

                else if (Bk and keyPressed->scancode == sf::Keyboard::Scan::Right) // 0
                {
                    BlockQ->Qmove(0);
                }
                else if (Bk and keyPressed->scancode == sf::Keyboard::Scan::Up)    // 1
                {
                    BlockQ->Qrotate();
                }
                else if (Bk and keyPressed->scancode == sf::Keyboard::Scan::Left)  // 2
                {
                    BlockQ->Qmove(2);
                }
                else if (Bk and keyPressed->scancode == sf::Keyboard::Scan::Down)  // 3
                {
                    bQ_to_bG(BlockQ, BlockGv, Grid, G_success); // tries to make two blockG's

                    if (G_success)
                    {
                        // reset blockQ pointer and success flag after conversion
                        delete BlockQ; 
                        BlockQ = nullptr;
                        G_success = false;
                    }
                }
            }
        }


        // Game Cycle

        if (fr_ct == 4) fr_ct = 0; // switches from 30fps to 6ups
        else            fr_ct += 1;

        if (fr_ct == 0)
        {

            // let blockG's fall and check if all settled
            all_stl = true; // reset
            for (blockG* BlockG : BlockGv)
            {
                BlockG->fall(Grid); // moves blockG down and sets settled
                if (!BlockG->settled) all_stl = false;
                BlockG->age += 1;                    
            }
            if (!all_stl) all_mch = false;

            // all settled but unmatched: make matches and merge
            if (all_stl and !all_mch)
            {
                // make match pairs and add to matchP vector
                MatchPv.clear();
                for (blockG* BlockG : BlockGv) BlockG->match_find(BlockGv, MatchPv);

                // group overlapping pairs into sets
                mP_to_mS(MatchPv, MatchSv);
                
                // combine overlapping sets
                for (matchS* MatchS : MatchSv)
                {
                    while (MatchS->set_overlap)
                    {
                        MatchS->combine(MatchSv); // combine overlapping sets
                    }
                }

                // merge each set into a promoted block
                for (matchS* MatchS : MatchSv)
                {
                    MatchS->reduce();             // remove duplicate block id's in set
                    MatchS->merge(BlockGv, Grid); // merge and promote/delete blocks
                    all_mch = false;              // (need to rematch grid after any merges)
                }

                if (MatchSv.size() == 0) all_mch = true; // if no sets then grid is stable

                // reset for next cycle
                for (matchS* MatchS : MatchSv) delete MatchS;
                MatchSv.clear();
            }

            // all setlled and merged: make new block pair in queue
            if (all_stl and all_mch and BlockQ == nullptr)
            {
                // ...call rng function to decide colors
                BlockQ = new blockQ(0, 1);
            }
        }

        // Window Management
        window.clear();

        window.draw(Border);
        if (BlockQ != nullptr)         window.draw(BlockQ->vta);
        for (blockG* BlockG : BlockGv) window.draw(BlockG->vta);

        window.display();
    }
}