#pragma once

#include "types.h"
#include <iostream>
#include <math.h>

//Validation Functions
void CheckListIsSorted(AnimationState* animationState);

int ShuffleList(AnimationState* animationState);

//Sorts
int InsertionSort(AnimationState* animationState);

int BubbleSort(AnimationState* animationState);

int CocktailShakerSort(AnimationState* animationState);

int QuickSort(AnimationState* animationState);

tuple<int,int> QuickSortPartition(int low, int high, AnimationState* animationState); //returns the new pivot index

int MergeSort(int n, AnimationState* animationState);

int IterateMerge(int begin, int middle, int end, bool isSwapped, AnimationState* animationState);

void MergeSplit(int begin, int end, bool isSwapped, AnimationState* animationState); //is Swapped tells the code if the arrays should be swapped in the next function call. null is just so it can be used in the queue

tuple<int,int> RadixSort(AnimationState* animationState);

//Accessory Functions
double GetFreq(int inVal);

void GenerateVector(AnimationState* animationState);

void CopyArray(int begin, int end, AnimationState* animationState);
