#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <math.h>

using namespace std;

struct
{
    int phase = 0; //Selects which phase of generation the software is currently in
    int frame = 0;

    //Extra values to store in order to store variables for the sorting algorithms
    int frameValA = -1;
    int frameValB = -1;
    bool frameValBool = false;

    //Values to store state within the sorting algorithm
    int sortState = -1;

} animationState;

vector<int> sortableList = {}; //List to be sorted

bool CheckListIsSorted(int size)
{
    for(int i =0;i<size;i++)
    {
        if(sortableList[i] != i+1)
        {
            cout << "Check failed at value " << sortableList[i] << " position " << i << " expected " << i + 1 << endl;
            return false;
        }
    }
    cout << "Passed check";
    return true;
}
void PopulateList(int size)
{
    for(int i =0;i<size;i++)
    {
        sortableList.push_back(i+1);
    }
}

void ShuffleList(int listSize)
{
    int swapIndex0 = rand() % listSize;
    int swapIndex1 = rand() % listSize;

    int index0Item = sortableList[swapIndex0];
    sortableList[swapIndex0] = sortableList[swapIndex1];
    sortableList[swapIndex1] = index0Item;
}

void InsertionSort(int listSize)
{
    switch(animationState.sortState)
    {
        case(0): //sortState 0 is the state when looking for the next value to pass backwards
        {
            if(animationState.frameValA > 0 && sortableList[animationState.frameValA-1] > sortableList[animationState.frameValA]) 
            {

                animationState.frameValB = animationState.frameValA;
                animationState.sortState = 1;
            }
            else
            {
                animationState.frameValA = animationState.frameValA + 1;
            }
            break;
        }
        case(1): //sortState 1 is when the value is being passed backwards
        {
            if(animationState.frameValB > 0 && sortableList[animationState.frameValB-1] > sortableList[animationState.frameValB]) 
            {
                int index0Item = sortableList[animationState.frameValB];
                sortableList[animationState.frameValB] = sortableList[animationState.frameValB-1];
                sortableList[animationState.frameValB-1] = index0Item;
                animationState.frameValB--;
            }
            else
            {
                animationState.frameValB = -1;
                animationState.sortState = 0;
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

void BubbleSort(int listSize)
{
    switch(animationState.sortState)
    {
        case(0): //State 0 is before the system loops
        {   
            animationState.frameValBool = false;
            animationState.sortState = 1;
            animationState.frameValB = 1;
            animationState.frameValA++;
            break;
        }
        case(1): //State 1 is when the loop occurs
        {
            
            if(animationState.frameValB <= (listSize - animationState.frameValA) - 1)
            {
                if(sortableList[animationState.frameValB - 1] > sortableList[animationState.frameValB])
                {
                    int index0Item = sortableList[animationState.frameValB];
                    sortableList[animationState.frameValB] = sortableList[animationState.frameValB-1];
                    sortableList[animationState.frameValB-1] = index0Item;
                    animationState.frameValBool = true;
                }
                animationState.frameValB++;
            }
            else
            {
                animationState.sortState = 0;
            }
            break;
        }
    }
}

void CocktailShakerSort(int listSize)
{
    switch(animationState.sortState)
    {
        case(0): //Initialisation
        {   
            animationState.frameValBool = false;
            animationState.sortState = 1;
            animationState.frameValB = 1;
            animationState.frameValA++;
            break;
        }
        case(1): //First loop
        {
            
            if(animationState.frameValB <= (listSize - animationState.frameValA))
            {
                if(sortableList[animationState.frameValB - 1] > sortableList[animationState.frameValB])
                {
                    int index0Item = sortableList[animationState.frameValB];
                    sortableList[animationState.frameValB] = sortableList[animationState.frameValB-1];
                    sortableList[animationState.frameValB-1] = index0Item;
                    animationState.frameValBool = true;
                }
                animationState.frameValB++;
            }
            else
            {
                if(animationState.frameValBool) //Enter backwards loop
                {
                    animationState.frameValB = (listSize - animationState.frameValA);
                    animationState.sortState = 2;
                }
                else
                {
                    animationState.sortState = 0;
                }
            }
            break;
        }
        case(2): //Second loop
        {
            if(animationState.frameValB > animationState.frameValA)
            {
                if(sortableList[animationState.frameValB - 1] > sortableList[animationState.frameValB])
                {
                    int index0Item = sortableList[animationState.frameValB];
                    sortableList[animationState.frameValB] = sortableList[animationState.frameValB-1];
                    sortableList[animationState.frameValB-1] = index0Item;
                    animationState.frameValBool = true;
                }
                animationState.frameValB--;
            }
            else
            {
                animationState.sortState = 0;
            }
            break;
        }
    }
}

void DrawList(int listSize, SDL_Renderer *renderer)
{
    for(int i=0;i<listSize;i++)
    {
        float hVal = sortableList[i]; //The 1-360 value that represents the value and the hue of a column.
        SDL_Rect rect;
        rect.x = 3 + (i * 4);
        rect.w = 3;
        rect.y = 500 - hVal;
        rect.h = hVal + 10;

        //HSL to RGB
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

        if(i == animationState.frameValA || i == animationState.frameValB)
        {
            switch(animationState.phase)
            {
                case(2): //Insertion sort - both A and B are white
                {
                    redVal = 1.0f;
                    greenVal = 1.0f;
                    blueVal = 1.0f;
                    mVal = 0.0f;
                    break;
                }
                case(4): //Bubble sort - only B is white - falls into case 6
                case(6): //Cocktail shaker sort - only B is white
                {
                    if(i==animationState.frameValB)
                    {
                        redVal = 1.0f;
                        greenVal = 1.0f;
                        blueVal = 1.0f;
                        mVal = 0.0f;
                    }
                    break;
                }
                default:
                {
                    break;
                }
            }
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
        if(animationState.frame == 0 && animationState.phase != -1)
        {
            SDL_Delay(1500);
        }

        switch(animationState.phase)
        {

            case(0):
            {
                animationState.frame++;
                if(animationState.frame >= 500)
                {
                    cout << "Shuffle" << endl;
                    animationState.phase = 1;
                    animationState.frame = 0;
                    animationState.frameValA = 1;
                    animationState.frameValB = 0;
                    animationState.sortState = 0;
                    animationState.frameValBool = false;
                }
                SDL_Delay(5);
                break;
            }
            case(1):
            {
                ShuffleList(listSize);
                animationState.frame++;

                if(animationState.frame >= 500)
                {
                    cout << "Insertion Sort" << endl;
                    animationState.phase = 2;
                    animationState.frame = 0;
                    animationState.frameValA = 0;
                    animationState.frameValB = 0;
                    animationState.sortState = 0;
                    animationState.frameValBool = false;
                }
                SDL_Delay(5);
                break;
            }
            case(2):
            {
                InsertionSort(listSize);
                animationState.frame++;

                if(animationState.frameValA >= listSize) //Run the sort and check if it is finished
                {
                    CheckListIsSorted(listSize);
                    cout << "Shuffle" << endl;
                    animationState.phase = 3;
                    animationState.frame = 0;                    
                    animationState.frameValA = 0;
                    animationState.frameValB = 0;
                    animationState.sortState = 0;
                    animationState.frameValBool = false;                
                }
                SDL_Delay(1);
                break;
            }
            case(3):
            {
                ShuffleList(listSize);
                animationState.frame++;

                if(animationState.frame >= 500)
                {
                    cout << "Bubble Sort" << endl;
                    animationState.phase = 4;
                    animationState.frame = 0;
                    animationState.frameValA = -1;
                    animationState.frameValB = -1;
                    animationState.sortState = 0;
                    animationState.frameValBool = false;
                }
                SDL_Delay(5);
                break;
            }
            case(4):
            {
                BubbleSort(listSize);
                animationState.frame++;

                if(animationState.frameValBool == false && animationState.sortState == 0) //Run the sort and check if it is finished
                {
                    CheckListIsSorted(listSize);
                    cout << "Shuffle" << endl;
                    animationState.phase = 5;
                    animationState.frame = 0;                    
                    animationState.frameValA = 0;
                    animationState.frameValB = 0;
                    animationState.sortState = 0;
                    animationState.frameValBool = false;
                }

                if(animationState.sortState == 1)
                {
                    SDL_Delay(1);
                }
                break;
            }
            case(5):
            {
                ShuffleList(listSize);
                animationState.frame++;

                if(animationState.frame >= 500)
                {
                    "Cocktail Shaker Sort";
                    animationState.phase = 6;
                    animationState.frame = 0;
                    animationState.frameValA = 0;
                    animationState.frameValB = 0;
                    animationState.sortState = 0;
                    animationState.frameValBool = false;
                }
                SDL_Delay(5);
                break;
            }
            case(6):
            {
                CocktailShakerSort(listSize); 
                animationState.frame++;

                if(animationState.frameValBool == false && animationState.sortState == 0) //Run the sort and check if it is finished
                {
                    cout << CheckListIsSorted(listSize) << endl;
                    cout << "Shuffle" << endl;
                    animationState.phase = -1;
                    animationState.frame = 0;                    
                    animationState.frameValA = 0;
                    animationState.frameValB = 0;
                    animationState.sortState = 0;
                    animationState.frameValBool = false;
                }

                SDL_Delay(1);
                //if(animationState.sortState != 0)
                //{
                //    SDL_Delay(1);
                //}

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