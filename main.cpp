#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <math.h>
#include <SDL2/SDL_ttf.h>
#include <VSSynth/VSSynth.h>

using namespace std;
using namespace VSSynth;
using namespace Generators;

struct
{
    int phase = 0; //Selects which phase of generation the software is currently in
    int frame = 0;

    //Extra values to store in order to store variables for the sorting algorithms
    int frameValInvisibleA = -1;
    int frameValAnimated = 0;
    int frameValInvisibleB = -1;
    bool frameValToggle = false;
    vector<int> secondaryList = {}; //Used for storing single value items
    vector<tuple<int,int,int,bool>> frameValVector = {};

    //Values to store state within the sorting algorithm
    int sortState = -1;

} animationState;

class ToneItem{
    public:
        double freq;
        int timeRemaining;

        ToneItem(double nFreq, int nTime)
        {
            freq = nFreq;
            timeRemaining = nTime;
        }
};

vector<ToneItem> audioKeys = {}; //Audio freq by time remaining and isplaying

vector<int> sortableList = {}; //List to be sorted

//Text Rendering variables
SDL_Texture* textTexture;
SDL_Surface* textSurface;
SDL_Rect textRect;

//Audio variable
Tone tone(
[](double frequency, double time) {
    return Waveforms::triangle(frequency,time);
});

void AppendAudio(int freq, int framesLeft)
{
    audioKeys.push_back(ToneItem(freq,framesLeft));
    tone.playNote(freq);
}

void PollAudio()
{
    int audioLen = size(audioKeys);
    for(int i=0;i<audioLen;i++) //This should serve as both an if statement and a loop
    {
        if(audioKeys[i].timeRemaining <= 0)
        {
            tone.stopNote(audioKeys[i].freq);

            audioKeys.erase(audioKeys.begin()+i); //Remove the index from the vector
            //Reset the properties for the loop
            audioLen--;
            i--;
        }
        else
        {
            audioKeys[i].timeRemaining--;
        }
    }
}

void ClearAudio()
{
    int audioLen = size(audioKeys);

    for(int i=0;i<audioLen;i++) //This should serve as both an if statement and a loop
    {
        tone.stopNote(audioKeys[i].freq);
    }
    audioKeys.clear();
}

double GetFreq(int inVal)
{
    return abs(100+((float)(inVal)*1.9f));
}

void CheckListIsSorted(int size)
{
    if(sortableList[animationState.frameValAnimated] != animationState.frameValAnimated+1)
    {
        cout << "Check failed at value " << sortableList[animationState.frameValAnimated] << " position " << animationState.frameValAnimated << " expected " << animationState.frameValAnimated + 1 << endl;
    }
    animationState.frameValAnimated++;
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

    AppendAudio(GetFreq(swapVal),0);
}

void InsertionSort(int listSize)
{
    switch(animationState.sortState)
    {
        case(0): //sortState 0 is the state when looking for the next value to pass backwards
        {
            if(animationState.frameValInvisibleA > 0 && sortableList[animationState.frameValInvisibleA-1] > sortableList[animationState.frameValInvisibleA]) 
            {

                animationState.frameValAnimated = animationState.frameValInvisibleA;
                animationState.sortState = 1;
            }
            else
            {
                animationState.frameValInvisibleA = animationState.frameValInvisibleA + 1;
            }
            break;
        }
        case(1): //sortState 1 is when the value is being passed backwards
        {
            if(animationState.frameValAnimated > 0 && sortableList[animationState.frameValAnimated-1] > sortableList[animationState.frameValAnimated]) 
            {
                AppendAudio(GetFreq(sortableList[animationState.frameValAnimated-1]),0); 

                int swapVal = sortableList[animationState.frameValAnimated];
                sortableList[animationState.frameValAnimated] = sortableList[animationState.frameValAnimated-1];
                sortableList[animationState.frameValAnimated-1] = swapVal;
                animationState.frameValAnimated--;
            }
            else
            {
                animationState.frameValAnimated = -1;
                animationState.sortState = 0;
            }
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
            animationState.frameValToggle = false;
            animationState.sortState = 1;
            animationState.frameValAnimated = 1;
            animationState.frameValInvisibleA++;
            break;
        }
        case(1): //State 1 is when the loop occurs
        {
            
            if(animationState.frameValAnimated <= (listSize - animationState.frameValInvisibleA) - 1)
            {

                AppendAudio(GetFreq(sortableList[animationState.frameValAnimated]),0); 

                if(sortableList[animationState.frameValAnimated - 1] > sortableList[animationState.frameValAnimated])
                {

                    int swapVal = sortableList[animationState.frameValAnimated];

                    sortableList[animationState.frameValAnimated] = sortableList[animationState.frameValAnimated-1];
                    sortableList[animationState.frameValAnimated-1] = swapVal;
                    animationState.frameValToggle = true;
                }
                animationState.frameValAnimated++;
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
            animationState.frameValToggle = false;
            animationState.frameValAnimated = animationState.frameValInvisibleA < 1 ? 1 : animationState.frameValInvisibleA;
            animationState.frameValInvisibleA++;
            animationState.sortState = 1;
            break;
        }
        case(1): //First loop
        {
            
            if(animationState.frameValAnimated <= (listSize - animationState.frameValInvisibleA))
            {
                AppendAudio(GetFreq(sortableList[animationState.frameValAnimated]),0);

                if(sortableList[animationState.frameValAnimated - 1] > sortableList[animationState.frameValAnimated])
                {
                    int swapVal = sortableList[animationState.frameValAnimated];
                    sortableList[animationState.frameValAnimated] = sortableList[animationState.frameValAnimated-1];
                    sortableList[animationState.frameValAnimated-1] = swapVal;
                    animationState.frameValToggle = true;
                }
                animationState.frameValAnimated++;
            }
            else
            {
                if(animationState.frameValToggle) //Enter backwards loop
                {
                    animationState.frameValAnimated = (listSize - animationState.frameValInvisibleA);
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
            if(animationState.frameValAnimated > animationState.frameValInvisibleA)
            {
                AppendAudio(GetFreq(sortableList[animationState.frameValAnimated - 1]),0); 

                if(sortableList[animationState.frameValAnimated - 1] > sortableList[animationState.frameValAnimated])
                {
                    int swapVal = sortableList[animationState.frameValAnimated];
                    sortableList[animationState.frameValAnimated] = sortableList[animationState.frameValAnimated-1];
                    sortableList[animationState.frameValAnimated-1] = swapVal;
                    animationState.frameValToggle = true;
                }
                animationState.frameValAnimated--;
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
    if(animationState.frameValInvisibleA == -1 && animationState.frameValAnimated == -1) //On any first run of this, initialise the frame values
    {
        animationState.frameValToggle = false; //Indication that the sorting has not yet concluded
        animationState.frameValInvisibleA = low - 1;
        animationState.frameValAnimated = low;
    }

    AppendAudio(GetFreq(sortableList[animationState.frameValAnimated]),0); 

    if(animationState.frameValAnimated < high) //Loop through until frame b becomes greater than or equal to the high value
    {

        if(sortableList[animationState.frameValAnimated] <= sortableList[high])
        {
            animationState.frameValInvisibleA++;

            //swap values

            int swapVal = sortableList[animationState.frameValInvisibleA];
            sortableList[animationState.frameValInvisibleA] = sortableList[animationState.frameValAnimated];
            sortableList[animationState.frameValAnimated] = swapVal;
        }

        animationState.frameValAnimated++;
        return -1;
    }
    else
    {
        int swapVal = sortableList[animationState.frameValInvisibleA+1];
        sortableList[animationState.frameValInvisibleA+1] = sortableList[high];
        sortableList[high] = swapVal;

        animationState.frameValToggle = true;
        return animationState.frameValInvisibleA + 1;
    }

}

void QuickSort(int listSize)
{

    switch(animationState.sortState)
    {
        case(0): //Initialisation
        {
            int partIndex = QuickSortPartition(0,listSize-1);
            if(animationState.frameValToggle) //When the partition has finished
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
            animationState.frameValInvisibleA = -1;
            animationState.frameValAnimated = -1;
            animationState.frameValToggle = false;

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

            if(animationState.frameValToggle) //When the code for the partition ends, this will run and push new values back into the list
            {
                animationState.frameValInvisibleA = -1;
                animationState.frameValAnimated = -1;
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
    if(animationState.frameValToggle == false) //If list is being initialised
    {
        animationState.frameValInvisibleA = begin; //i
        animationState.frameValAnimated = middle; //j
        animationState.frameValInvisibleB = begin; //k
        animationState.frameValToggle = true; //Reset frameValToggle
    }

    AppendAudio(GetFreq(sortableList[animationState.frameValAnimated]),0); 

    if(animationState.frameValInvisibleB< end)
    {
        if(isSwapped)
        {
            if(animationState.frameValInvisibleA < middle && (animationState.frameValAnimated >= end || sortableList[animationState.frameValInvisibleA] <= sortableList[animationState.frameValAnimated]))
            {
                animationState.secondaryList.at(animationState.frameValInvisibleB) = sortableList[animationState.frameValInvisibleA];
                animationState.frameValInvisibleA++;
            }
            else
            {
                animationState.secondaryList.at(animationState.frameValInvisibleB) = sortableList[animationState.frameValAnimated];
                animationState.frameValAnimated++;
            }

            animationState.frameValInvisibleB++;
        }
        else
        {
            if(animationState.frameValInvisibleA < middle && (animationState.frameValAnimated >= end || animationState.secondaryList.at(animationState.frameValInvisibleA) <= animationState.secondaryList.at(animationState.frameValAnimated)))
            {
                sortableList[animationState.frameValInvisibleB] = animationState.secondaryList.at(animationState.frameValInvisibleA);
                animationState.frameValInvisibleA++;
            }
            else
            {
                sortableList[animationState.frameValInvisibleB] = animationState.secondaryList.at(animationState.frameValAnimated);
                animationState.frameValAnimated++;
            }

            animationState.frameValInvisibleB++;
        }
    }
    else
    {
        animationState.sortState = 1;
        animationState.frameValInvisibleA = -1;
        animationState.frameValAnimated = -1;
        animationState.frameValInvisibleB = -1;
        animationState.frameValToggle = false;
        animationState.frameValVector.pop_back();
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
                    animationState.frameValToggle = false;
                    //fall through into case 2
                }
            }
        }
        case(2):
        {
            tuple<int,int,int,bool> actionQueue = animationState.frameValVector.back(); //Get most recent element in stack

            IterateMerge(get<0>(actionQueue),get<1>(actionQueue),get<2>(actionQueue),get<3>(actionQueue)); //Run the merge iteration
            break;
        }
    }
}

void RadixSort(int listSize)
{
    switch(animationState.sortState)
    {
        case(0): //initialisation
        {
            for(int i=0;i<listSize;i++)
            {
                animationState.secondaryList.push_back(sortableList[i]); //Make a temporary copy of the sortable list
            }

            for(int i =0;i<10;i++)
            {
                animationState.frameValVector.push_back(tuple<int,int,int,bool>(0,0,0,0));
            }
            animationState.sortState = 1;
            animationState.frameValAnimated = 0;
            animationState.frameValInvisibleA = 1; //Invisible A stores the value of the current exponent
            animationState.frameValInvisibleB = 3; //Invisible B stores the current digit count
            break;
        }
        case(1): //Reset the count of each digit list
        {
            for(int i =0;i<10;i++)
            {
                animationState.frameValVector[i] = tuple<int,int,int,bool>(0,0,0,0);
            }

            animationState.sortState = 2;
            //Allow fall through into the next process
        }
        case(2): //Count up the digit value for each item on the current exponent
        {
            if(animationState.frameValAnimated < listSize)
            {
                int index = floor((float)(animationState.secondaryList[animationState.frameValAnimated])/ (float)(animationState.frameValInvisibleA)); 

                //Increment the count of each digit
                int currentCount = get<0>(animationState.frameValVector[index%10]);
                animationState.frameValVector[index%10] = tuple<int,int,int,bool>(currentCount+1,0,0,0);
                
                AppendAudio(GetFreq(sortableList[animationState.frameValAnimated]),0); 
                animationState.frameValAnimated++;

            }
            else
            {
                for(int i=1;i<10;i++) //Reprocess framevalvector array
                {
                    int currentCount = get<0>(animationState.frameValVector[i]);
                    int prevCount = get<0>(animationState.frameValVector[i-1]);

                    animationState.frameValVector[i] = tuple<int,int,int,bool>(currentCount+prevCount,0,0,0);
                }

                animationState.frameValAnimated = listSize-1;
                animationState.sortState =3;
            }

            
            break;
        }
        case(3):
        {
            if(animationState.frameValAnimated >= 0)
            {
                int index = floor((float)(animationState.secondaryList[animationState.frameValAnimated])/ (float)(animationState.frameValInvisibleA)); 

                int indexToModify = (get<0>(animationState.frameValVector[index % 10]));
                sortableList[indexToModify - 1] = animationState.secondaryList[animationState.frameValAnimated];
                animationState.frameValVector[index % 10] = tuple<int,int,int,bool>(indexToModify-1,0,0,0);

                AppendAudio(GetFreq(sortableList[indexToModify - 1]),1); //Only instance so far where more than 1 frame of audio is used - allows for simultaneous audio

                animationState.frameValAnimated -=1;
            }
            else
            {
                animationState.frameValAnimated =0;
                animationState.sortState = 4;
            }
            break;
        }
        case(4):
        {
            if(animationState.frameValAnimated < listSize)
            {
                animationState.secondaryList[animationState.frameValAnimated] = sortableList[animationState.frameValAnimated];
                AppendAudio(GetFreq(sortableList[animationState.frameValAnimated]),0); 
                animationState.frameValAnimated++;
            }
            else
            {
                animationState.frameValAnimated = 0;
                animationState.sortState = 1;
                animationState.frameValInvisibleB--;
                animationState.frameValInvisibleA *=10;
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

        if(i==animationState.frameValAnimated)
        {
            redVal = 1.0f;
            greenVal = 1.0f;
            blueVal = 1.0f;
            mVal = 0.0f;
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

    Synthesizer synth;

    tone.setVolume(20);

    synth.open();
    synth.addSoundGenerator(&tone);
    synth.unpause();

    SDL_Event event;

    while(true)
    {
        
        if(animationState.frame == 0 && animationState.phase != -1)
        {
            SDL_Delay(100);
            ClearAudio();
            if(animationState.phase == 0 || ((animationState.phase - 2) % 3) != 1)
            {
                SDL_Delay(1400);
            }

        }
        else
        {
            PollAudio();
        }

        if(!((animationState.phase - 2) % 3==0)) //If not in a sorting animationstate. each sorting phase occurs on (x-2) % 3 == 0
        {
            //only valid inputs should be 1 and 2

            if(animationState.phase == 0 || ((animationState.phase - 2) % 3) == 1) //Check phase
            {
                if(animationState.frame == 0)
                {
                    InitialiseText(renderer,&textRect,"Check");
                }

                CheckListIsSorted(listSize);
                animationState.frame++;

                if(animationState.frameValAnimated >= listSize)
                {
                    animationState.phase++;
                    animationState.frame = 0;                    
                    animationState.frameValInvisibleA = 0;
                    animationState.frameValAnimated = -1;
                    animationState.frameValInvisibleB = 0;
                }
                else
                {
                    AppendAudio(100 + GetFreq(animationState.frameValAnimated),0);
                }

                SDL_Delay(5);

            }
            else //Shuffle phase
            {
                if(animationState.frame == 0)
                {
                    InitialiseText(renderer,&textRect,"Shuffle");
                }

                ShuffleList(listSize);
                animationState.frame++;

                if(animationState.frame >= 500)
                {
                    animationState.phase++;
                    animationState.frame = 0;
                    animationState.sortState = 0;
                    animationState.frameValVector.clear();
                    animationState.secondaryList.clear();
                }
                SDL_Delay(5);
            }
        }
        else
        {
            switch(animationState.phase / 3)
            {
                case(0):
                {
                    if(animationState.frame == 0)
                    {
                        InitialiseText(renderer,&textRect,"Insertion Sort");
                        animationState.frameValInvisibleA = 0;
                        animationState.frameValAnimated = 0;
                        animationState.frameValInvisibleB = 0;
                        animationState.sortState = 0;
                        animationState.frameValToggle = false;
                    }

                    InsertionSort(listSize);
                    animationState.frame++;

                    if(animationState.frameValInvisibleA >= listSize) //Run the sort and check if it is finished
                    {
                        animationState.phase++;
                        animationState.frame = 0;                    
                        animationState.frameValAnimated = 0;              
                    }
                    SDL_Delay(3);
                    break;
                }
                case(1):
                {
                    if(animationState.frame == 0)
                    {
                        InitialiseText(renderer,&textRect,"Bubble Sort");
                        animationState.frameValInvisibleA = -1;
                        animationState.frameValAnimated = -1;
                        animationState.frameValInvisibleB = -1;
                        animationState.sortState = 0;
                        animationState.frameValToggle = false;
                    }

                    BubbleSort(listSize);
                    animationState.frame++;

                    if(animationState.frameValToggle == false && animationState.sortState == 0) //Run the sort and check if it is finished
                    {
                        animationState.phase++;
                        animationState.frame = 0;                    
                        animationState.frameValAnimated = 0;   
                    }

                    if(animationState.sortState == 1)
                    {
                        SDL_Delay(3);
                    }
                    break;
                }
                case(2):
                {
                    if(animationState.frame == 0)
                    {
                        InitialiseText(renderer,&textRect,"Cocktail Shaker Sort");
                        animationState.frameValInvisibleA = 0;
                        animationState.frameValAnimated = -1;
                        animationState.frameValInvisibleB = -1;
                        animationState.sortState = 0;
                        animationState.frameValToggle = false;
                    }

                    CocktailShakerSort(listSize); 
                    animationState.frame++;

                    if(animationState.frameValToggle == false && animationState.sortState == 0) //Run the sort and check if it is finished
                    {
                        animationState.phase = 9;
                        animationState.frame = 0;                    
                        animationState.frameValAnimated = 0;   

                    }

                    if(animationState.sortState != 0)
                    {
                        SDL_Delay(3);
                    }

                    break;
                }
                case(3):
                {
                    if(animationState.frame == 0)
                    {
                        InitialiseText(renderer,&textRect,"Quick Sort");
                        animationState.frameValInvisibleA = -1;
                        animationState.frameValAnimated = -1;
                        animationState.frameValInvisibleB = -1;
                        animationState.sortState = 0;
                        animationState.frameValToggle = false;
                    }

                    QuickSort(listSize);
                    animationState.frame++;

                    if(animationState.sortState == 3) //Run the sort and check if it is finished
                    {
                        animationState.phase++;
                        animationState.frame = 0;                    
                        animationState.frameValAnimated = 0;                
                    }

                    SDL_Delay(3);

                    break;
                }
                case(4):
                {
                    if(animationState.frame == 0)
                    {
                        InitialiseText(renderer,&textRect,"Merge Sort");
                        animationState.frameValInvisibleA = -1;
                        animationState.frameValAnimated = -1;
                        animationState.frameValInvisibleB = -1;
                        animationState.sortState = 0;
                        animationState.frameValToggle = false;
                        GenerateVector(listSize);
                    }

                    MergeSort(listSize,listSize);
                    animationState.frame++;

                    if(animationState.sortState == 3) //Run the sort and check if it is finished
                    {
                        animationState.phase++;
                        animationState.frame = 0;                    
                        animationState.frameValAnimated = 0;                
                    }

                    SDL_Delay(3);

                    break;
                }
                case(5):
                {                       
                    if(animationState.frame == 0)
                    {
                        InitialiseText(renderer,&textRect,"Radix Sort (LSD)");
                        animationState.frameValInvisibleA = -1;
                        animationState.frameValAnimated = -1;
                        animationState.frameValInvisibleB = -1;
                        animationState.sortState = 0;
                        animationState.frameValToggle = false;
                    }
                    RadixSort(listSize);
                    animationState.frame++;

                    if(animationState.frameValInvisibleB <= 0)
                    {
                        animationState.phase++;
                        animationState.frame = 0;                    
                        animationState.frameValAnimated = 0;                
                    }

                    SDL_Delay(4);
                    break;
                }
                default:
                {
                    if(animationState.frame == 0)
                    {
                        InitialiseText(renderer,&textRect,"End");
                    }
                    animationState.frame = 1;
                    break;
                }
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

    synth.pause();
    synth.close();

    return 0;
}