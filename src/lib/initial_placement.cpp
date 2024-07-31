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

    double x[] = {10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0};

    double y[] = {10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0};

    double z[] = { 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8};

    for(int i = 0; i < InstanceArray.size(); i++)
    {

        // double minX = InstanceArray[i].width ;

        // double maxX = binInfo.dieWidth - InstanceArray[i].width;

        // double minY = InstanceArray[i].height ;

        // double maxY = binInfo.dieHeight - InstanceArray[i].height;

        // double x = (maxX - minX) * rand() / (RAND_MAX +1.0) + minX;

        // double y = (maxY - minY) * rand() / (RAND_MAX +1.0) + minY;

        // InstanceArray[i].x = x;

        // InstanceArray[i].y = y;

        InstanceArray[i].x = x[i];

        InstanceArray[i].y = y[i];

        InstanceArray[i].z = z[i];

    }
}

