#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "initial_placement.h"
#include "readfile.h"



void returnGridInfo(Die die, gridInfo &binInfo)
{   
    binInfo.dieWidth = die.upperRightX ; 
    
    binInfo.dieHeight = die.upperRightY ;

    binInfo.binWidth = 100;

    binInfo.binHeight = 100;

    binInfo.binXnum = binInfo.dieWidth / binInfo.binWidth;

    binInfo.binYnum = binInfo.dieHeight / binInfo.binHeight;

}

void calculatePenaltyWeight()
{

}

void firstPlacement(vector <Instance> &InstanceArray, gridInfo binInfo)
{
    // give each cell initial solution

    srand( time(NULL) );

    for(int i = 0; i < InstanceArray.size(); i++)
    {

        double minX = InstanceArray[i].width ;

        double maxX = binInfo.dieWidth - InstanceArray[i].width;

        double minY = InstanceArray[i].height ;

        double maxY = binInfo.dieHeight - InstanceArray[i].height;

        double x = (maxX - minX) * rand() / (RAND_MAX +1.0) + minX;

        double y = (maxY - minY) * rand() / (RAND_MAX +1.0) + minY;

        InstanceArray[i].x = x;

        InstanceArray[i].y = y;

        InstanceArray[i].z = 0.5;

    }
}

