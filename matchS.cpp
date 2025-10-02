
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "matchS.h"
#include "blockG.h"


matchS::matchS(std::vector<int> idv_in)
{
    for (int id : idv_in) idv.push_back(id);  // initial stored block id's
    set_id = idv_in[0] + 100 * (idv_in[1] + 100 * idv_in[2]); // id of set
}

void matchS::add(int id_in1, int id_in2) 
{
    // add new match pair to the set
    idv.push_back(id_in1);
    idv.push_back(id_in2);
}


void matchS::combine(std::vector<matchS*>& MatchSv)
{
    // combine overlapping sets
    set_overlap = false;

    for (matchS* mS : MatchSv)
    {
        // look for an overlapping set
        if (set_id != mS->set_id) {     // not comparing to itself
            for (int id_ths : idv)    { // check each id against each id of other set
            for (int id_mS : mS->idv) {

                    if (id_ths == id_mS) set_overlap = true;
            }
            }
        }
        // combine sets
        if (set_overlap)
        {
            for (int id_mS : mS->idv)
            {
                idv.push_back(id_mS);  // add all id's to this set
            }
            mS->set_id = -1;            // deactivate copied set
        }
        // remove secondary combined set from set vector
        std::vector<matchS*> MSv_tmp;
        for (matchS* mS : MatchSv)
        {
            if (mS->set_id != -1) MSv_tmp.push_back(mS);
            else                  delete mS;
        }
        MatchSv = MSv_tmp;
    }
}

void matchS::reduce()
{
    // remove duplicate block id's from overlaps ...should probably be rewritten
    // i to get id to check against, j to scan vector, k to index tmp vector

    //std::vector<int> idv_tmp;
    //for (int id : idv) idv_tmp.push_back(id);

    for (int i = 0; i < idv.size(); i++) {

        for (int j = 0; j < idv.size(); j++) {

            if (i != j and idv[i] == idv[j])
            {
                //std::cout << "i " << idv.size() << "  ";

                idv.erase(idv.begin() + j);
                j--; // offset index if element removed ...works with or without

                //std::cout << "f " << idv.size() << "\n";

            }
        }
    }
    //idv = idv_tmp;
}

void matchS::merge(std::vector<blockG*>& BlockGv, std::vector<int>& grid)
{
    // match stored id's to blockG's with same id's
    std::vector<blockG*> set_bGv;

    for (blockG* bG : BlockGv) {
        for (int id : idv) {

            if (bG->id == id)
            {
                set_bGv.push_back(bG);
            }
        }
    }

    // compare how recent blocks are
    int age_min = 1000;

    for (blockG* bG : set_bGv)
    {
        if (bG->age < age_min) // if newest, set min to own age
        {
            age_min = bG->age;
        }
        else if (bG->age == age_min) // if two with same low age, favor up left 
        {
            for (blockG* bG_B : set_bGv)
            {
                if (bG->pos.x < bG_B->pos.x or bG->pos.y < bG_B->pos.y)
                {
                    bG->age -= 1;
                    age_min = bG->age;
                }
            }
        }
    }

    // promote newest block, delete others, clean blockG vector
    for (blockG* bG : set_bGv)
    {
        if (bG->age == age_min) bG->promote();
        else
        {
            grid[bG->id] = -1; // sets cell to empty
            bG->id = -1;       // flag to be removed from BlockGv
        }
    }
    std::vector<blockG*> BGv_temp;
    for (blockG* bG : BlockGv)
    {
        if (bG->id != -1) BGv_temp.push_back(bG);
        else              delete bG;
    }
    BlockGv = BGv_temp;
}