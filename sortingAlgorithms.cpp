#include "sortingAlgorithms.h"

double GetFreq(int inVal)
{
    return 100+((float)(inVal)*1.9f);
}

void CheckListIsSorted(AnimationState* animationState)
{
    if(animationState->sortableList[animationState->frameValAnimated] != animationState->frameValAnimated+1)
    {
        cout << "Check failed at value " << animationState->sortableList[animationState->frameValAnimated] << " position " << animationState->frameValAnimated << " expected " << animationState->frameValAnimated + 1 << endl;
    }
    animationState->frameValAnimated++;
}

int ShuffleList(AnimationState* animationState)
{
    int swapIndex0 = rand() % LISTSIZE;
    int swapIndex1 = rand() % LISTSIZE;

    int swapVal = animationState->sortableList[swapIndex0];
    animationState->sortableList[swapIndex0] = animationState->sortableList[swapIndex1];
    animationState->sortableList[swapIndex1] = swapVal;

    return GetFreq(swapVal);
}

int InsertionSort(AnimationState* animationState)
{
    int soundOutput = 0;

    switch(animationState->sortState)
    {
        case(0): //sortState 0 is the state when looking for the next value to pass backwards
        {
            if(animationState->frameValInvisibleA > 0 && animationState->sortableList[animationState->frameValInvisibleA-1] > animationState->sortableList[animationState->frameValInvisibleA]) 
            {

                animationState->frameValAnimated = animationState->frameValInvisibleA;
                animationState->sortState = 1;
            }
            else
            {
                animationState->frameValInvisibleA = animationState->frameValInvisibleA + 1;
            }
            break;
        }
        case(1): //sortState 1 is when the value is being passed backwards
        {
            if(animationState->frameValAnimated > 0 && animationState->sortableList[animationState->frameValAnimated-1] > animationState->sortableList[animationState->frameValAnimated]) 
            {
                soundOutput = GetFreq(animationState->sortableList[animationState->frameValAnimated-1]); 

                int swapVal = animationState->sortableList[animationState->frameValAnimated];
                animationState->sortableList[animationState->frameValAnimated] = animationState->sortableList[animationState->frameValAnimated-1];
                animationState->sortableList[animationState->frameValAnimated-1] = swapVal;
                animationState->frameValAnimated--;
            }
            else
            {
                animationState->frameValAnimated = -1;
                animationState->sortState = 0;
            }
            break;
        }
    }

    return soundOutput;
}

int BubbleSort(AnimationState* animationState)
{
    int soundOutput = 0;

    switch(animationState->sortState)
    {
        case(0): //State 0 is before the system loops
        {   
            animationState->frameValToggle = false;
            animationState->sortState = 1;
            animationState->frameValAnimated = 1;
            animationState->frameValInvisibleA++;
            break;
        }
        case(1): //State 1 is when the loop occurs
        {
            
            if(animationState->frameValAnimated <= (LISTSIZE - animationState->frameValInvisibleA) - 1)
            {

                soundOutput = GetFreq(animationState->sortableList[animationState->frameValAnimated]); 

                if(animationState->sortableList[animationState->frameValAnimated - 1] > animationState->sortableList[animationState->frameValAnimated])
                {

                    int swapVal = animationState->sortableList[animationState->frameValAnimated];

                    animationState->sortableList[animationState->frameValAnimated] = animationState->sortableList[animationState->frameValAnimated-1];
                    animationState->sortableList[animationState->frameValAnimated-1] = swapVal;
                    animationState->frameValToggle = true;
                }
                animationState->frameValAnimated++;
            }
            else
            {
                animationState->sortState = 0;
            }
            break;
        }
    }

    return soundOutput;
}

int CocktailShakerSort(AnimationState* animationState)
{
    int soundOutput = 0;

    switch(animationState->sortState)
    {
        case(0): //Initialisation
        {   
            animationState->frameValToggle = false;
            animationState->frameValAnimated = animationState->frameValInvisibleA < 1 ? 1 : animationState->frameValInvisibleA;
            animationState->frameValInvisibleA++;
            animationState->sortState = 1;
            break;
        }
        case(1): //First loop
        {
            
            if(animationState->frameValAnimated <= (LISTSIZE - animationState->frameValInvisibleA))
            {
                soundOutput = (animationState->sortableList[animationState->frameValAnimated]);

                if(animationState->sortableList[animationState->frameValAnimated - 1] > animationState->sortableList[animationState->frameValAnimated])
                {
                    int swapVal = animationState->sortableList[animationState->frameValAnimated];
                    animationState->sortableList[animationState->frameValAnimated] = animationState->sortableList[animationState->frameValAnimated-1];
                    animationState->sortableList[animationState->frameValAnimated-1] = swapVal;
                    animationState->frameValToggle = true;
                }
                animationState->frameValAnimated++;
            }
            else
            {
                if(animationState->frameValToggle) //Enter backwards loop
                {
                    animationState->frameValAnimated = (LISTSIZE - animationState->frameValInvisibleA);
                    animationState->sortState = 2;
                }
                else
                {
                    animationState->sortState = 0;
                }
            }
            break;
        }
        case(2): //Second loop
        {
            if(animationState->frameValAnimated > animationState->frameValInvisibleA)
            {
                soundOutput = (animationState->sortableList[animationState->frameValAnimated - 1]); 

                if(animationState->sortableList[animationState->frameValAnimated - 1] > animationState->sortableList[animationState->frameValAnimated])
                {
                    int swapVal = animationState->sortableList[animationState->frameValAnimated];
                    animationState->sortableList[animationState->frameValAnimated] = animationState->sortableList[animationState->frameValAnimated-1];
                    animationState->sortableList[animationState->frameValAnimated-1] = swapVal;
                    animationState->frameValToggle = true;
                }
                animationState->frameValAnimated--;
            }
            else
            {
                animationState->sortState = 0;
            }
            break;
        }
    }

    return soundOutput;
}

tuple<int,int> QuickSortPartition(int low, int high, AnimationState* animationState) //returns the new pivot index
{
    int soundOutput = 0;

    if(animationState->frameValInvisibleA == -1 && animationState->frameValAnimated == -1) //On any first run of this, initialise the frame values
    {
        animationState->frameValToggle = false; //Indication that the sorting has not yet concluded
        animationState->frameValInvisibleA = low - 1;
        animationState->frameValAnimated = low;
    }

    soundOutput = GetFreq(animationState->sortableList[animationState->frameValAnimated]); 

    if(animationState->frameValAnimated < high) //Loop through until frame b becomes greater than or equal to the high value
    {

        if(animationState->sortableList[animationState->frameValAnimated] <= animationState->sortableList[high])
        {
            animationState->frameValInvisibleA++;

            //swap values

            int swapVal = animationState->sortableList[animationState->frameValInvisibleA];
            animationState->sortableList[animationState->frameValInvisibleA] = animationState->sortableList[animationState->frameValAnimated];
            animationState->sortableList[animationState->frameValAnimated] = swapVal;
        }

        animationState->frameValAnimated++;
        return tuple<int,int>(-1,soundOutput);
    }
    else
    {
        int swapVal = animationState->sortableList[animationState->frameValInvisibleA+1];
        animationState->sortableList[animationState->frameValInvisibleA+1] = animationState->sortableList[high];
        animationState->sortableList[high] = swapVal;

        animationState->frameValToggle = true;
        return tuple<int,int>(animationState->frameValInvisibleA + 1,soundOutput);
    }

}

int QuickSort(AnimationState* animationState)
{
    int audioOutput = 0;
    
    switch(animationState->sortState)
    {
        case(0): //Initialisation
        {
            tuple<int,int> retVal = QuickSortPartition(0,LISTSIZE-1,animationState);

            int partIndex = get<0>(retVal);
            audioOutput = get<1>(retVal);
            if(animationState->frameValToggle) //When the partition has finished
            {
                animationState->frameValVector.push_back({0,partIndex-1,0,0}); //Push in left pivot
                animationState->frameValVector.push_back({partIndex+1,LISTSIZE-1,0,0}); //Push in right pivot
                animationState->sortState = 1;
            }
            break;
        }
        case(1): //Iteration
        {
            //Reset some important values
            animationState->frameValInvisibleA = -1;
            animationState->frameValAnimated = -1;
            animationState->frameValToggle = false;

            if(size(animationState->frameValVector) > 0)
            {
                //Get values out of vector tuple and them remove it from the vector
                tuple<int,int,int,bool> iteratedTuple = animationState->frameValVector.back();
                int nLow = get<0>(iteratedTuple);
                int nHigh = get<1>(iteratedTuple);
 
                if(nLow < nHigh)
                {
                    animationState->sortState = 2; 
                    //There is intentionally no break here, allowing the program to fall into the case(2) of the switch statement immediately
                }
                else
                {
                    animationState->frameValVector.pop_back(); //Removes the back element
                    break;
                }
            }
            else
            {
                animationState->sortState = 3; //The set should now be sorted - setting sort state to 3 signifies that its done
                break;
            }
        }
        case(2): //Partitioning
        {
            //Gets the values from the vector
            tuple<int,int,int,bool> iteratedTuple = animationState->frameValVector.back();
            int nLow = get<0>(iteratedTuple);
            int nHigh = get<1>(iteratedTuple);

            tuple<int,int> retVal = QuickSortPartition(nLow,nHigh,animationState);
            int nPartIndex = get<0>(retVal);
            audioOutput = get<1>(retVal);


            if(animationState->frameValToggle) //When the code for the partition ends, this will run and push new values back into the list
            {
                animationState->frameValInvisibleA = -1;
                animationState->frameValAnimated = -1;
                animationState->frameValVector.pop_back(); //Removes the back element when it is no longer needed
                animationState->frameValVector.push_back({nLow,nPartIndex-1,0,0}); //Push in left pivot
                animationState->frameValVector.push_back({nPartIndex+1,nHigh,0,0}); //Push in right pivot

                animationState->sortState = 1;
            }
            break;
        }
    }

    return audioOutput;
}

int IterateMerge(int begin, int middle, int end, bool isSwapped, AnimationState* animationState)
{
    int audioOutput = 0;

    if(animationState->frameValToggle == false) //If list is being initialised
    {
        animationState->frameValInvisibleA = begin; //i
        animationState->frameValAnimated = middle; //j
        animationState->frameValInvisibleB = begin; //k
        animationState->frameValToggle = true; //Reset frameValToggle
    }

    if(animationState->frameValAnimated < LISTSIZE) //Animated frame can become 300 but the value is never accessed normally. if the index is accessed it'll respond with junk data usually though 
    {
        audioOutput = GetFreq(animationState->sortableList[animationState->frameValAnimated]); 
    }

    if(animationState->frameValInvisibleB< end)
    {
        if(isSwapped)
        {
            if(animationState->frameValInvisibleA < middle && (animationState->frameValAnimated >= end || animationState->sortableList[animationState->frameValInvisibleA] <= animationState->sortableList[animationState->frameValAnimated]))
            {
                animationState->secondaryList[animationState->frameValInvisibleB] = animationState->sortableList[animationState->frameValInvisibleA];
                animationState->frameValInvisibleA++;
            }
            else
            {
                animationState->secondaryList[animationState->frameValInvisibleB] = animationState->sortableList[animationState->frameValAnimated];
                animationState->frameValAnimated++;
            }

            animationState->frameValInvisibleB++;
        }
        else
        {
            if(animationState->frameValInvisibleA < middle && (animationState->frameValAnimated >= end || animationState->secondaryList[animationState->frameValInvisibleA] <= animationState->secondaryList[animationState->frameValAnimated]))
            {
                animationState->sortableList[animationState->frameValInvisibleB] = animationState->secondaryList[animationState->frameValInvisibleA];
                animationState->frameValInvisibleA++;
            }
            else
            {
                animationState->sortableList[animationState->frameValInvisibleB] = animationState->secondaryList[animationState->frameValAnimated];
                animationState->frameValAnimated++;
            }

            animationState->frameValInvisibleB++;
        }
    }
    else
    {
        animationState->sortState = 1;
        animationState->frameValInvisibleA = -1;
        animationState->frameValAnimated = -1;
        animationState->frameValInvisibleB = -1;
        animationState->frameValToggle = false;
        animationState->frameValVector.pop_back();
    }

    return audioOutput;

}

void CopyArray(int begin, int end, AnimationState* animationState)
{
    for(int i = begin; i<end;i++)
    {
        animationState->secondaryList[i] = animationState->sortableList[i];
    }
}

void MergeSplit(int begin, int end, bool isSwapped, AnimationState* animationState) //is Swapped tells the code if the arrays should be swapped in the next function call. null is just so it can be used in the queue
{
    if(end - begin <= 1)
    {
        return;
    }

    int middle = (end + begin) /2;

    //Append the next item to the action stack. 
    animationState->frameValVector.push_back(tuple<int,int,int,bool>{begin,middle,end,isSwapped});

    //The -1 as the end value represents this as a merge split call
    animationState->frameValVector.push_back(tuple<int,int,int,bool>{middle,end,-1,!isSwapped});
    animationState->frameValVector.push_back(tuple<int,int,int,bool>{begin,middle,-1,!isSwapped});

}

void GenerateVector(AnimationState* animationState)
{
    for(int i = 0; i<LISTSIZE;i++)
    {
        animationState->secondaryList[i] = 0;
    }
}

int MergeSort(int n, AnimationState* animationState)
{
    int audioOutput = 0;

    switch(animationState->sortState)
    {
        case(0): //Initialisation
        {
            CopyArray(0,n,animationState);
            MergeSplit(0,n,false,animationState);
            animationState->sortState = 1;
            break;
        }
        case(1): //Action queue processing
        {
            //For the purposes of this sort, the frameValVector will serve as a stack that determines what action will be taken next.
            //In the future this might be replaced with a delegate structure

            if(size(animationState->frameValVector) == 0) //If there are no more queued actions
            {
                animationState->sortState = 3; //ends the iteration
                break;
            }
            else
            {
                tuple<int,int,int,bool> actionQueue = animationState->frameValVector.back(); //Get most recent element in stack

                if(get<2>(actionQueue) == -1) //If the third int in the action queue is a negative one, then we are dealing with a merge split - generating more actions
                {
                    animationState->frameValVector.pop_back(); //We can now safely delete the top of the stack
                    MergeSplit(get<0>(actionQueue),get<1>(actionQueue),get<3>(actionQueue),animationState);
                    break;
                }
                else //If the stack contains a value in the third int that isnt a -1, a merge iteration call must be done. this is handled in its own sort state
                {
                    //The top of the queue is not removed so it can be used in the sort state
                    animationState->sortState = 2;
                    animationState->frameValToggle = false;
                    //fall through into case 2
                }
            }
        }
        case(2):
        {
            tuple<int,int,int,bool> actionQueue = animationState->frameValVector.back(); //Get most recent element in stack

            audioOutput = IterateMerge(get<0>(actionQueue),get<1>(actionQueue),get<2>(actionQueue),get<3>(actionQueue),animationState); //Run the merge iteration
            break;
        }
    }

    return audioOutput;
}

tuple<int,int> RadixSort(AnimationState* animationState)
{
    int audioOutput = 0;
    int audioFrames = 0;

    switch(animationState->sortState)
    {
        case(0): //initialisation
        {
            for(int i=0;i<LISTSIZE;i++)
            {
                animationState->secondaryList[i] = animationState->sortableList[i]; //Make a temporary copy of the sortable list
            }

            for(int i =0;i<10;i++)
            {
                animationState->frameValVector.push_back(tuple<int,int,int,bool>(0,0,0,0));
            }
            animationState->sortState = 1;
            animationState->frameValAnimated = 0;
            animationState->frameValInvisibleA = 1; //Invisible A stores the value of the current exponent
            animationState->frameValInvisibleB = log10(LISTSIZE)+1; //Invisible B stores the current digit count
            break;
        }
        case(1): //Reset the count of each digit list
        {
            for(int i =0;i<10;i++)
            {
                animationState->frameValVector[i] = tuple<int,int,int,bool>(0,0,0,0);
            }

            animationState->sortState = 2;
            //Allow fall through into the next process
        }
        case(2): //Count up the digit value for each item on the current exponent
        {
            if(animationState->frameValAnimated < LISTSIZE)
            {
                int index = floor((float)(animationState->secondaryList[animationState->frameValAnimated])/ (float)(animationState->frameValInvisibleA)); 

                //Increment the count of each digit
                int currentCount = get<0>(animationState->frameValVector[index%10]);
                animationState->frameValVector[index%10] = tuple<int,int,int,bool>(currentCount+1,0,0,0);
                
                audioOutput = GetFreq(animationState->sortableList[animationState->frameValAnimated]); 
                audioFrames = 0;

                animationState->frameValAnimated++;

            }
            else
            {
                for(int i=1;i<10;i++) //Reprocess framevalvector array
                {
                    int currentCount = get<0>(animationState->frameValVector[i]);
                    int prevCount = get<0>(animationState->frameValVector[i-1]);

                    animationState->frameValVector[i] = tuple<int,int,int,bool>(currentCount+prevCount,0,0,0);
                }

                animationState->frameValAnimated = LISTSIZE-1;
                animationState->sortState =3;
            }

            
            break;
        }
        case(3):
        {
            if(animationState->frameValAnimated >= 0)
            {
                int index = floor((float)(animationState->secondaryList[animationState->frameValAnimated])/ (float)(animationState->frameValInvisibleA)); 

                int indexToModify = (get<0>(animationState->frameValVector[index % 10]));
                animationState->sortableList[indexToModify - 1] = animationState->secondaryList[animationState->frameValAnimated];
                animationState->frameValVector[index % 10] = tuple<int,int,int,bool>(indexToModify-1,0,0,0);

                audioOutput = GetFreq(animationState->sortableList[indexToModify - 1]); //Only instance so far where more than 1 frame of audio is used - allows for simultaneous audio
                audioFrames = 1;

                animationState->frameValAnimated -=1;
            }
            else
            {
                animationState->frameValAnimated =0;
                animationState->sortState = 4;
            }
            break;
        }
        case(4):
        {
            if(animationState->frameValAnimated < LISTSIZE)
            {
                animationState->secondaryList[animationState->frameValAnimated] = animationState->sortableList[animationState->frameValAnimated];
                audioOutput = GetFreq(animationState->sortableList[animationState->frameValAnimated]); 
                audioFrames = 0;
                animationState->frameValAnimated++;
            }
            else
            {
                animationState->frameValAnimated = 0;
                animationState->sortState = 1;
                animationState->frameValInvisibleB--;
                animationState->frameValInvisibleA *=10;
            }
            break;
        }
        
    }

    return tuple<int,int>(audioOutput,audioFrames);
}