#ifndef _SORTALGVISTYPES_
#define _SORTALGVISTYPES_

#define LISTSIZE 300

#include <vector>
#include <tuple>

using namespace std;

struct aniState
{
    int phase = 0; //Selects which phase of generation the software is currently in
    int frame = 0;

    //Extra values to store in order to store variables for the sorting algorithms
    int frameValInvisibleA = -1;
    int frameValAnimated = 0;
    int frameValInvisibleB = -1;
    bool frameValToggle = false;
    int secondaryList[LISTSIZE] = {}; //Used for storing single value items
    vector<tuple<int,int,int,bool>> frameValVector = {};

    int sortableList[LISTSIZE] = {}; //List to be sorted

    //Values to store state within the sorting algorithm
    int sortState = -1;

};

typedef struct aniState AnimationState;

#endif