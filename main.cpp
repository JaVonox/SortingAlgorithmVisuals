#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <math.h>

using namespace std;

struct
{
    int phase = 0; //Selects which phase of generation the software is currently in
    //Phase 0 = initial list, phase 1 = shuffling, phase 2 insertionsort, phase 3 shuffling
    int frame = 0;

    //Extra values to store in order to store variables for the sorting algorithms
    int frameValA = -1;
    int frameValB = -1;

} animationState;

vector<int> sortableList = {}; //List to be sorted

bool CheckListIsSorted(int size)
{
    for(int i =0;i<size;i++)
    {
        if(sortableList[i] != i)
        {
            return false;
        }
    }
    return true;
}
void PopulateList(int size)
{
    for(int i =0;i<size;i++)
    {
        sortableList.push_back(i);
    }
}

void ShuffleList(int listSize)
{
    int swapIndex0 = rand() % (listSize + 1);
    int swapIndex1 = rand() % (listSize + 1);

    int index0Item = sortableList[swapIndex0];
    sortableList[swapIndex0] = sortableList[swapIndex1];
    sortableList[swapIndex1] = index0Item;
}

void InsertionSort(int listSize)
{
    int j = animationState.frameValA;
    while(j > 0 && sortableList[j-1] > sortableList[j])
    {
        int index0Item = sortableList[j];
        sortableList[j] = sortableList[j-1];
        sortableList[j-1] = index0Item;
        j = j - 1;
    }
    animationState.frameValA = animationState.frameValA + 1;
}

void DrawList(int listSize, SDL_Renderer *renderer)
{
    for(int i;i<listSize;i++)
    {
        float hVal = sortableList[i]; //The 0-360 value that represents the value and the hue of a column.
        SDL_Rect rect;
        rect.x = 3 + (i * 4);
        rect.w = 3;
        rect.y = 500 - hVal;
        rect.h = hVal;

        //HSL to RGB
        //Lightness and saturation are constant for this specific program. Leaving them in there for future reference
        float lightness = 0.5f;
        float saturation = 1.0f;
        float cVal = (1.0f - abs(2.0f * lightness - 1.0f)) * saturation;
        float xVal = cVal * (1.0f - (abs((fmod(hVal/60.0f,2.0f)) - 1.0f)));
        float mVal = lightness - cVal / 2;

        float redVal = 0;
        float greenVal = 0;
        float blueVal = 0;


        if(hVal < 60)
        {
            redVal = cVal;
            greenVal = xVal;
            blueVal = 0;
        }
        else if(hVal < 120)
        {
            redVal = xVal;
            greenVal = cVal;
            blueVal = 0;
        }
        else if(hVal < 180)
        {
            redVal = 0;
            greenVal = cVal;
            blueVal = xVal;
        }
        else if(hVal < 240)
        {
            redVal = 0;
            greenVal = xVal;
            blueVal = cVal;
        }
        else if(hVal < 300)
        {
            redVal = xVal;
            greenVal = 0;
            blueVal = cVal;
        }
        else
        {
            redVal = cVal;
            greenVal = 0;
            blueVal = xVal;
        }

        SDL_SetRenderDrawColor(renderer,(redVal + mVal) * 255,(greenVal + mVal) * 255,(blueVal + mVal) * 255,255);
        SDL_RenderFillRect(renderer,&rect);
    }
}

int main(int argc,char *argv[])
{
    srand(time(NULL));

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("Sorting",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,1206,600,SDL_WINDOW_ALLOW_HIGHDPI);

    SDL_Renderer *renderer = SDL_CreateRenderer(window,-1,0);
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    int listSize = 300;
    PopulateList(listSize);
    cout << "List populated" << endl;
    DrawList(listSize,renderer);

    SDL_RenderPresent(renderer);

    SDL_Event event;

    while(true)
    {
        switch(animationState.phase)
        {
            case(0):
            {
                animationState.frame++;
                if(animationState.frame >= 500)
                {
                    animationState.phase = 1;
                    animationState.frame = 0;
                }
                _sleep(5);
                break;
            }
            case(1):
            {
                ShuffleList(listSize);
                animationState.frame++;

                if(animationState.frame >= 500)
                {
                    animationState.phase = 2;
                    animationState.frame = 0;
                }
                _sleep(5);
                break;
            }
            case(2):
            {
                if(animationState.frame == 0)
                {
                    animationState.frameValA = 0; //This will store the i value for the sorting algorithm
                }
                
                InsertionSort(listSize);
                _sleep(5);
                animationState.frame++;

                if(animationState.frameValA >= listSize) //Run the sort and check if it is finished
                {
                    animationState.phase = 3;
                    animationState.frame = 0;
                }
                break;
            }
            default:
            {
                break;
            }
        }

        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);
        DrawList(listSize,renderer);
        SDL_RenderPresent(renderer);

        if(SDL_PollEvent(&event))
        {
            if(SDL_QUIT == event.type)
            {
                break;
            }
        }
    }
    return 0;
}