#pragma once

class matchP
{
public:
    matchP(int bG_id1, int bG_id2);

    int id1 = 0, id2 = 0, // id's storing blockG coordinates
        set_id = -1;      // which set is it in
};