#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/VertexArray.hpp>

class blockG;

class matchS
{
public:
    matchS(std::vector<int> idv_in);
    void add(int id_in1, int id_in2); // add new match pair to set
    void reduce();                    // remove duplicate id's
    void combine(std::vector<matchS*>& MatchSv); // combine overlapping sets
    void merge(std::vector<blockG*>& BlockGv, std::vector<int>& grid); // merge set into one promoted block

    std::vector<int> idv;     // vector of id's of blockG's contained
    int set_id = 0;           // set's identifier
    bool set_overlap = false; // for combining sets
};