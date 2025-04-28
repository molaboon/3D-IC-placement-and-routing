#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <string.h>

#include "initial_placement.h"
#include "CG.h"
#include "readfile.h"



void returnGridInfo(Die *die, gridInfo *binInfo, int Numinstance, vector <instance> &instances)
{   
    binInfo->dieWidth = die->upperRightX ; 
    binInfo->dieHeight = die->upperRightY ;
    binInfo->Numinstance = Numinstance;
    binInfo->binHeight = instances[0].height;


    int size = instances.size();
    int totalX = 999;
    int totalY = 999;

    for(int i = 0; i < size; i++)
    {   
        if ( (int) instances[i].width < totalX )
            totalX = (int) instances[i].width;

        if ( (int) instances[i].height < totalY )
            totalY = (int) instances[i].height;
        
    }        
        
    binInfo->binWidth = (float) totalX;
    binInfo->binHeight = (float) totalY;
    binInfo->binXnum = floor( binInfo->dieWidth / binInfo->binWidth );
    binInfo->binYnum = floor( binInfo->dieHeight / binInfo->binHeight );

    while (binInfo->binXnum * binInfo->binWidth < binInfo->dieWidth)
        binInfo->binXnum += 1;
    
    while (binInfo->binYnum * binInfo->binHeight < binInfo->dieHeight)
        binInfo->binYnum += 1;
    

}

void firstPlacement(vector <instance> &instances, gridInfo binInfo, Die topDie)
{
    // give each cell initial solution

    // srand( time(NULL) );

    int cnt = 0;

    for(int i = 0; i < instances.size(); i++)
    {
        float minX = instances[i].inflateWidth;
        float maxX = binInfo.dieWidth - instances[i].inflateWidth;
        float minY = instances[i].inflateHeight;
        float maxY = binInfo.dieHeight - instances[i].inflateHeight;
        // float minX = 400;
        // float maxX = 600;
        // float minY = 400;
        // float maxY = 600;

        float X = fmod( (float) rand(), ( maxX - minX + 1) ) + minX ;
        float Y = fmod( (float) rand(), ( maxY - minY + 1) ) + minY ;
        float Z = 0.5 ;

        instances[i].rotate = 0;

        instances[i].x = X;
        instances[i].y = Y;
        instances[i].z = Z ;
        instances[i].tmpX = instances[i].x;
        instances[i].tmpY = instances[i].y;
        instances[i].tmpZ = instances[i].z;
    }
}

void stdCellFirstPlacement( vector <instance> &instances, vector <instance> &macros, gridInfo binInfo, Die topDie) 
{
    int cellSize = instances.size();
    float minX, minY, maxX, maxY, minZ, maxZ;


    minX = topDie.upperRightX / 2;
    maxX = topDie.upperRightX / 2 + 100;
    minY = topDie.upperRightY / 2;
    maxY = topDie.upperRightY / 2 + 100;

    for(int i = 0; i < cellSize; i++)
    {
        if(!instances[i].isMacro)
        {      
            float X = fmod( (float) rand(), ( maxX - minX + 1) ) + minX ;
            float Y = fmod( (float) rand(), ( maxY - minY + 1) ) + minY ;
            float Z = fmod( (float) rand(), ( maxZ - minZ + 1) ) + minZ;

            instances[i].rotate = 0;

            instances[i].x = X;
            instances[i].y = Y;
            
            if(instances[i].layer == 1)
                instances[i].z = 9999;
            else
                instances[i].z = 1;

            instances[i].tmpX = instances[i].x;
            instances[i].tmpY = instances[i].y;
            instances[i].tmpZ = instances[i].z;
        }
    }
}

float returnPenaltyWeight(vector <RawNet> &rawNet, const float gamma, vector <instance> &instances, gridInfo binInfo, float *densityMap)
{
    float h = 1;
    float penaltyWeight;
    float xScore = 0.0, yScore = 0.0 , tsvScore = 0.0, penaltyScore = 0.0;
    float grax = 0.0, gray = 0.0, graz = 0.0, grad = 0.0;
    float *originfl = createBins(binInfo);
    float *originsl = createBins(binInfo);
    float *fl = createBins(binInfo);
    float *sl = createBins(binInfo);

    int size = instances.size();

    xScore = scoreOfX(rawNet, gamma, false, instances[0], 0);
    yScore = scoreOfY(rawNet, gamma, false, instances[0], 0);

    for(int i = 0; i < size; i++)
    {
        instances[i].density = returnDensity(instances[i].z, densityMap);
        instances[i].tmpD = instances[i].density;
        penaltyInfoOfinstance(instances[i], binInfo, originfl, originsl, false, false, NULL, 0);
    }
    tsvScore = TSVofNet(rawNet, false, instances[0], 0, densityMap);

    penaltyScore = scoreOfPenalty(originfl, originsl, binInfo);

    // printf("penalty Time: %fs\n", (endTime - startTime) / (float) CLOCKS_PER_SEC );

    return penaltyWeight;
}
