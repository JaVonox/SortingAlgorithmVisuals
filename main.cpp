#include "sortingAlgorithms.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <VSSynth/VSSynth.h>

using namespace VSSynth;
using namespace Generators;

AnimationState animationState; //This struct stores most of the relevant data for the animation and sorting datasets

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

//Text Rendering variables
SDL_Texture* textTexture;
SDL_Surface* textSurface;
SDL_Rect textRect;

//Audio variable
Instrument *tone = new MonophonicInstrument(
    [](double freq, double time) {return 0.5 * Waveforms::square(freq, time);},
    Patches::BRASS_ENVELOPE
);


void AppendAudio(uint16_t freq, int framesLeft)
{
    if(freq > 0)
    {
        audioKeys.push_back(ToneItem(freq,framesLeft));
        tone->holdNote(freq);
    }
}

void PollAudio()
{
    int audioLen = size(audioKeys);
    for(uint16_t i=0;i<audioLen;i++) //This should serve as both an if statement and a loop
    {
        if(audioKeys[i].timeRemaining <= 0)
        {
            tone->releaseNote(audioKeys[i].freq);

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

    for(uint16_t i=0;i<audioLen;i++) //This should serve as both an if statement and a loop
    {
        tone->releaseNote(audioKeys[i].freq);
    }
    audioKeys.clear();
}

void PopulateList()
{
    for(uint16_t i =0;i<LISTSIZE;i++)
    {
        animationState.sortableList[i] = i+1;
    }
}


void DrawList(SDL_Renderer *renderer)
{
    for(uint16_t i=0;i<LISTSIZE;i++)
    {
        float hVal = animationState.sortableList[i]; //The 1-360 value that represents the value and the hue of a column.
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

void ClearSecondaryList()
{
    for(uint16_t i = 0; i < LISTSIZE;i++)
    {
        animationState.secondaryList[i] = 0;
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

    PopulateList();
    _sleep(1);
    DrawList(renderer);

    SDL_RenderPresent(renderer);

    Synthesizer synth;

    tone->setVolume(20);

    synth.open();
    synth.addSoundGenerator(tone);
    synth.unpause();

    SDL_Event event;

    //_sleep(7000);

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

                CheckListIsSorted(&animationState);
                animationState.frame++;

                if(animationState.frameValAnimated >= LISTSIZE)
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

                AppendAudio(ShuffleList(&animationState),0);
                animationState.frame++;

                if(animationState.frame >= 500)
                {
                    animationState.phase++;
                    animationState.frame = 0;
                    animationState.sortState = 0;
                    animationState.frameValVector.clear();
                    ClearSecondaryList();
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

                    AppendAudio(InsertionSort(&animationState),0);
                    animationState.frame++;

                    if(animationState.frameValInvisibleA >= LISTSIZE) //Run the sort and check if it is finished
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

                    AppendAudio(BubbleSort(&animationState),0);
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

                    AppendAudio(CocktailShakerSort(&animationState),0); 
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

                    AppendAudio(QuickSort(&animationState),0);
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
                        GenerateVector(&animationState);
                    }

                    AppendAudio(MergeSort(LISTSIZE,&animationState),0);
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
                    std::tuple<int16_t,int16_t> radRet = RadixSort(&animationState);
                    AppendAudio(get<0>(radRet),get<1>(radRet));
                    animationState.frame++;

                    if(animationState.frameValInvisibleB <= 0)
                    {
                        animationState.phase++;
                        animationState.frame = 0;                    
                        animationState.frameValAnimated = 0;                
                    }

                    SDL_Delay(3);
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
        DrawList(renderer);
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