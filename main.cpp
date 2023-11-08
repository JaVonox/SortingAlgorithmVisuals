#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <math.h>
#include <SDL2/SDL_ttf.h>
using namespace std;

struct
{
    int phase = 9; //Selects which phase of generation the software is currently in
    int frame = 0;

    //Extra values to store in order to store variables for the sorting algorithms
    int frameValA = -1;
    int frameValB = -1;
    bool frameValBool = false;
    vector<int> secondaryList = {}; //Used for storing single value items
    vector<tuple<int,int,int,bool>> frameValVector = {};

    //Values to store state within the sorting algorithm
    int sortState = -1;

} animationState;

vector<int> sortableList = {}; //List to be sorted

//Text Rendering variables
SDL_Texture* textTexture;
SDL_Surface* textSurface;
SDL_Rect textRect;

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
    cout << "Passed check" << endl;
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

    int swapVal = sortableList[swapIndex0];
    sortableList[swapIndex0] = sortableList[swapIndex1];
    sortableList[swapIndex1] = swapVal;
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
                int swapVal = sortableList[animationState.frameValB];
                sortableList[animationState.frameValB] = sortableList[animationState.frameValB-1];
                sortableList[animationState.frameValB-1] = swapVal;
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
                    int swapVal = sortableList[animationState.frameValB];
                    sortableList[animationState.frameValB] = sortableList[animationState.frameValB-1];
                    sortableList[animationState.frameValB-1] = swapVal;
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
            animationState.frameValB = animationState.frameValA < 1 ? 1 : animationState.frameValA;
            animationState.frameValA++;
            animationState.sortState = 1;
            break;
        }
        case(1): //First loop
        {
            
            if(animationState.frameValB <= (listSize - animationState.frameValA))
            {
                if(sortableList[animationState.frameValB - 1] > sortableList[animationState.frameValB])
                {
                    int swapVal = sortableList[animationState.frameValB];
                    sortableList[animationState.frameValB] = sortableList[animationState.frameValB-1];
                    sortableList[animationState.frameValB-1] = swapVal;
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
                    int swapVal = sortableList[animationState.frameValB];
                    sortableList[animationState.frameValB] = sortableList[animationState.frameValB-1];
                    sortableList[animationState.frameValB-1] = swapVal;
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

int QuickSortPartition(int low, int high) //returns the new pivot index
{
    if(animationState.frameValA == -1 && animationState.frameValB == -1) //On any first run of this, initialise the frame values
    {
        animationState.frameValBool = false; //Indication that the sorting has not yet concluded
        animationState.frameValA = low - 1;
        animationState.frameValB = low;
    }

    if(animationState.frameValB < high) //Loop through until frame b becomes greater than or equal to the high value
    {
        if(sortableList[animationState.frameValB] <= sortableList[high])
        {
            animationState.frameValA++;

            //swap values

            int swapVal = sortableList[animationState.frameValA];
            sortableList[animationState.frameValA] = sortableList[animationState.frameValB];
            sortableList[animationState.frameValB] = swapVal;
        }

        animationState.frameValB++;
        return -1;
    }
    else
    {
        int swapVal = sortableList[animationState.frameValA+1];
        sortableList[animationState.frameValA+1] = sortableList[high];
        sortableList[high] = swapVal;

        animationState.frameValBool = true;
        return animationState.frameValA + 1;
    }

}

void QuickSort(int listSize)
{

    switch(animationState.sortState)
    {
        case(0): //Initialisation
        {
            int partIndex = QuickSortPartition(0,listSize-1);
            if(animationState.frameValBool) //When the partition has finished
            {
                animationState.frameValVector.push_back({0,partIndex-1,0,0}); //Push in left pivot
                animationState.frameValVector.push_back({partIndex+1,listSize-1,0,0}); //Push in right pivot
                animationState.sortState = 1;
            }
            break;
        }
        case(1): //Iteration
        {
            //Reset some important values
            animationState.frameValA = -1;
            animationState.frameValB = -1;
            animationState.frameValBool = false;

            if(size(animationState.frameValVector) > 0)
            {
                //Get values out of vector tuple and them remove it from the vector
                tuple<int,int,int,bool> iteratedTuple = animationState.frameValVector.back();
                int nLow = get<0>(iteratedTuple);
                int nHigh = get<1>(iteratedTuple);
 
                if(nLow < nHigh)
                {
                    animationState.sortState = 2; 
                    //There is intentionally no break here, allowing the program to fall into the case(2) of the switch statement immediately
                }
                else
                {
                    animationState.frameValVector.pop_back(); //Removes the back element
                    break;
                }
            }
            else
            {
                animationState.sortState = 3; //The set should now be sorted - setting sort state to 3 signifies that its done
                break;
            }
        }
        case(2): //Partitioning
        {
            //Gets the values from the vector
            tuple<int,int,int,bool> iteratedTuple = animationState.frameValVector.back();
            int nLow = get<0>(iteratedTuple);
            int nHigh = get<1>(iteratedTuple);

            int nPartIndex = QuickSortPartition(nLow,nHigh);

            if(animationState.frameValBool) //When the code for the partition ends, this will run and push new values back into the list
            {
                animationState.frameValA = -1;
                animationState.frameValB = -1;
                animationState.frameValVector.pop_back(); //Removes the back element when it is no longer needed
                animationState.frameValVector.push_back({nLow,nPartIndex-1,0,0}); //Push in left pivot
                animationState.frameValVector.push_back({nPartIndex+1,nHigh,0,0}); //Push in right pivot

                animationState.sortState = 1;
            }
            break;
        }
    }
}

void IterateMerge(int begin, int middle, int end, bool isSwapped)
{
    int i = begin;
    int j = middle;

    if(isSwapped)
    {
        for(int k = begin; k< end;k++)
        {
            if(i < middle && (j >= end || sortableList[i] <= sortableList[j]))
            {
                animationState.secondaryList.at(k) = sortableList[i];
                i++;
            }
            else
            {
                animationState.secondaryList.at(k) = sortableList[j];
                j++;
            }
        }
        cout << "Swapped loop ends" << endl;
    }
    else
    {
        for(int k = begin; k< end;k++)
        {
            if(i < middle && (j >= end || animationState.secondaryList.at(i) <= animationState.secondaryList.at(j)))
            {
                sortableList[k] = animationState.secondaryList.at(i);
                i++;
            }
            else
            {
                sortableList[k] = animationState.secondaryList.at(j);
                j++;
            }
        }

        cout << "Not swapped loop ends" << endl;
    }
}

void CopyArray(int begin, int end)
{
    for(int i = begin; i<end;i++)
    {
        animationState.secondaryList.at(i) = sortableList[i];
    }
}

void MergeSplit(int begin, int end, bool isSwapped) //is Swapped tells the code if the arrays should be swapped in the next function call. null is just so it can be used in the queue
{
    if(end - begin <= 1)
    {
        return;
    }

    int middle = (end + begin) /2;

    //Append the next item to the action stack. 
    animationState.frameValVector.push_back(tuple<int,int,int,bool>{begin,middle,end,isSwapped});

    //The -1 as the end value represents this as a merge split call
    animationState.frameValVector.push_back(tuple<int,int,int,bool>{middle,end,-1,!isSwapped});
    animationState.frameValVector.push_back(tuple<int,int,int,bool>{begin,middle,-1,!isSwapped});

    //MergeSplit(begin,middle,!isSwapped);
    //MergeSplit(middle,end,!isSwapped);
    //IterateMerge(begin, middle, end, isSwapped);

}

void GenerateVector(int listSize)
{
    for(int i = 0; i<listSize;i++)
    {
        animationState.secondaryList.push_back(0);
    }
}

void MergeSort(int listSize, int n)
{
    switch(animationState.sortState)
    {
        case(0): //Initialisation
        {
            CopyArray(0,n);
            MergeSplit(0,n,false);
            animationState.sortState = 1;
            cout << "end init" << endl;
            break;
        }
        case(1): //Action queue processing
        {
            //For the purposes of this sort, the frameValVector will serve as a stack that determines what action will be taken next.
            //In the future this might be replaced with a delegate structure

            if(size(animationState.frameValVector) == 0) //If there are no more queued actions
            {
                animationState.sortState = 3; //ends the iteration
                break;
            }
            else
            {
                tuple<int,int,int,bool> actionQueue = animationState.frameValVector.back(); //Get most recent element in stack

                if(get<2>(actionQueue) == -1) //If the third int in the action queue is a negative one, then we are dealing with a merge split - generating more actions
                {
                    animationState.frameValVector.pop_back(); //We can now safely delete the top of the stack
                    MergeSplit(get<0>(actionQueue),get<1>(actionQueue),get<3>(actionQueue));
                    break;
                }
                else //If the stack contains a value in the third int that isnt a -1, a merge iteration call must be done. this is handled in its own sort state
                {
                    //The top of the queue is not removed so it can be used in the sort state
                    animationState.sortState = 2;
                    //fall through into case 2
                }
            }
        }
        case(2):
        {
            tuple<int,int,int,bool> actionQueue = animationState.frameValVector.back(); //Get most recent element in stack
            animationState.frameValVector.pop_back();

            IterateMerge(get<0>(actionQueue),get<1>(actionQueue),get<2>(actionQueue),get<3>(actionQueue)); //Run the merge iteration

            animationState.sortState = 1;
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
                case(2): //Insertion sort - both A and B are white - currently defaults to only B white to do appearance testing
                //{
                //    redVal = 1.0f;
                //    greenVal = 1.0f;
                //    blueVal = 1.0f;
                //    mVal = 0.0f;
                //    break;
                //}
                case(4): //Bubble sort - only B is white - falls through
                case(6): //Cocktail shaker sort - only B is white
                case(8): //Quicksort - only B is white
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

void InitialiseText(SDL_Renderer *renderer, SDL_Rect *rect, char* text)
{ 
    if(textTexture != NULL)
    {
        SDL_DestroyTexture(textTexture);
        textTexture = NULL;
        SDL_FreeSurface(textSurface);
        textSurface = NULL;
    }

    TTF_Font* Sans = TTF_OpenFont("OpenSans.ttf",24);
    SDL_Color White = {255,255,255};
    textSurface = TTF_RenderText_Solid(Sans, text,White);
    textTexture = SDL_CreateTextureFromSurface(renderer,textSurface);

    int tWidth =0;
    int tHeight = 0;
    SDL_QueryTexture(textTexture,NULL,NULL,&tWidth,&tHeight);
    textRect.x = 10;
    textRect.y = 10;
    textRect.w = tWidth;
    textRect.h = tHeight;

    SDL_RenderCopy(renderer,textTexture,NULL,rect);
    TTF_CloseFont(Sans);
}

void DrawText(SDL_Renderer *renderer, SDL_Rect *rect)
{
    SDL_RenderCopy(renderer,textTexture,NULL,rect);
}

int main(int argc,char *argv[])
{
    srand(time(NULL));

    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    SDL_Window *window = SDL_CreateWindow("Sorting Algorithm Visualiser",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,1206,600,SDL_WINDOW_ALLOW_HIGHDPI);

    SDL_Renderer *renderer = SDL_CreateRenderer(window,-1,0);

    textRect.x = 0;
    textRect.y = 0;
    textRect.w = 500;
    textRect.h = 100;

    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_RenderClear(renderer);

    InitialiseText(renderer,&textRect,"Initialization");
    DrawText(renderer,&textRect);

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
                    InitialiseText(renderer,&textRect,"Shuffling");
                    animationState.phase = 1;
                    animationState.frame = 0;
                    animationState.frameValA = 1;
                    animationState.frameValB = 0;
                    animationState.sortState = 0;
                    animationState.frameValVector.clear();
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
                    InitialiseText(renderer,&textRect,"Insertion Sort");
                    animationState.phase = 2;
                    animationState.frame = 0;
                    animationState.frameValA = 0;
                    animationState.frameValB = 0;
                    animationState.sortState = 0;
                    animationState.frameValVector.clear();
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
                    InitialiseText(renderer,&textRect,"Shuffle");
                    animationState.phase = 3;
                    animationState.frame = 0;                    
                    animationState.frameValA = 0;
                    animationState.frameValB = 0;
                    animationState.sortState = 0;
                    animationState.frameValVector.clear();
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
                    InitialiseText(renderer,&textRect,"Bubble Sort");
                    animationState.phase = 4;
                    animationState.frame = 0;
                    animationState.frameValA = -1;
                    animationState.frameValB = -1;
                    animationState.sortState = 0;
                    animationState.frameValVector.clear();
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
                    InitialiseText(renderer,&textRect,"Shuffle");
                    animationState.phase = 5;
                    animationState.frame = 0;                    
                    animationState.frameValA = 0;
                    animationState.frameValB = 0;
                    animationState.sortState = 0;
                    animationState.frameValVector.clear();
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
                    InitialiseText(renderer,&textRect,"Cocktail Shaker Sort");
                    animationState.phase = 6;
                    animationState.frame = 0;
                    animationState.frameValA = 0;
                    animationState.frameValB = -1;
                    animationState.sortState = 0;
                    animationState.frameValVector.clear();
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
                    CheckListIsSorted(listSize);
                    InitialiseText(renderer,&textRect,"Shuffle");
                    animationState.phase = 7;
                    animationState.frame = 0;                    
                    animationState.frameValA = 0;
                    animationState.frameValB = 0;
                    animationState.sortState = 0;
                    animationState.frameValVector.clear();
                    animationState.frameValBool = false;
                }

                if(animationState.sortState != 0)
                {
                    SDL_Delay(1);
                }

                break;
            }
            case(7):
            {
                ShuffleList(listSize);
                animationState.frame++;

                if(animationState.frame >= 500)
                {
                    InitialiseText(renderer,&textRect,"Quick Sort");
                    animationState.phase = 8;
                    animationState.frame = 0;
                    animationState.frameValA = -1;
                    animationState.frameValB = -1;
                    animationState.sortState = 0;
                    animationState.frameValVector.clear();
                    animationState.frameValBool = false;
                }
                SDL_Delay(5);
                break;
            }
            case(8):
            {

                QuickSort(listSize);
                animationState.frame++;

                if(animationState.sortState == 3) //Run the sort and check if it is finished
                {
                    CheckListIsSorted(listSize);
                    InitialiseText(renderer,&textRect,"Shuffle");
                    animationState.phase = 9;
                    animationState.frame = 0;                    
                    animationState.frameValA = 0;
                    animationState.frameValB = 0;
                    animationState.sortState = 0;
                    animationState.frameValVector.clear();
                    animationState.frameValBool = false;                
                }

                SDL_Delay(1);

                break;
            }
            case(9):
            {
                ShuffleList(listSize);
                animationState.frame++;

                if(animationState.frame >= 500)
                {
                    InitialiseText(renderer,&textRect,"Merge Sort");
                    animationState.phase = 10;
                    animationState.frame = 0;
                    animationState.frameValA = -1;
                    animationState.frameValB = -1;
                    animationState.sortState = 0;
                    animationState.frameValVector.clear();
                    animationState.frameValBool = false;
                }
                SDL_Delay(5);
                break;
            }
            case(10):
            {
                if(animationState.frame == 0)
                {
                    GenerateVector(listSize);
                    cout << "vector generated" << endl;
                }

                MergeSort(listSize,listSize);
                animationState.frame++;

                if(animationState.sortState == 3) //Run the sort and check if it is finished
                {
                    CheckListIsSorted(listSize);
                    InitialiseText(renderer,&textRect,"Shuffle");
                    animationState.phase = -1;
                    animationState.frame = 0;                    
                    animationState.frameValA = 0;
                    animationState.frameValB = 0;
                    animationState.sortState = 0;
                    animationState.frameValVector.clear();
                    animationState.frameValBool = false;                
                }

                SDL_Delay(1);

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
        DrawText(renderer,&textRect);

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