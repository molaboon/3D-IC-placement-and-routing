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
    int macroSize = macros.size();
    
    for(int i = 0; i < cellSize; i++)
    {
        float minX, minY, maxX, maxY;

        if(instances[i].isMacro)
        {
            minX = instances[i].inflateWidth;
            maxX = binInfo.dieWidth - instances[i].inflateWidth;
            minY = instances[i].inflateHeight;
            maxY = binInfo.dieHeight - instances[i].inflateHeight;
        }
        else
        {
            minX = 13500;
            maxX = 13600;
            minY = 9800;
            maxY = 10000;
        }
        
        float X = fmod( (float) rand(), ( maxX - minX + 1) ) + minX ;
        float Y = fmod( (float) rand(), ( maxY - minY + 1) ) + minY ;
        float Z = 5000 ;

        instances[i].rotate = 0;

        instances[i].x = X;
        instances[i].y = Y;
        instances[i].z = Z;
        instances[i].tmpX = instances[i].x;
        instances[i].tmpY = instances[i].y;
        instances[i].tmpZ = instances[i].z;
        instances[i].refX = X;
        instances[i].refY = Y;
        instances[i].refZ = Z;
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
        penaltyInfoOfinstance(instances[i], binInfo, originfl, originsl, false, false, NULL);
    }
    tsvScore = TSVofNet(rawNet, false, instances[0], 0, densityMap);

    penaltyScore = scoreOfPenalty(originfl, originsl, binInfo);

    memcpy(fl, originfl, binInfo.binXnum * binInfo.binYnum * sizeof(float));
    memcpy(sl, originsl, binInfo.binXnum * binInfo.binYnum * sizeof(float));

    if(true)
    {
        for(int i = 0; i < size; i++)
        {        
            float tmpXscore = 0.0, tmpYscore = 0.0, tmpDen = 0.0;
            float graGrade = 0.0;

            // part of x
            instances[i].tmpX = instances[i].x;
            instances[i].tmpX += h;
            
            tmpXscore = scoreOfX(rawNet, gamma, true, instances[i], xScore);

            tmpDen = penaltyScore;
            penaltyInfoOfinstance(instances[i], binInfo, fl, sl, false, true, &graGrade);
            tmpDen -= graGrade;
            penaltyInfoOfinstance(instances[i], binInfo, fl, sl, true, false, &graGrade);
            tmpDen += graGrade;
            penaltyInfoOfinstance(instances[i], binInfo, fl, sl, true, true, &graGrade);
            penaltyInfoOfinstance(instances[i], binInfo, fl, sl, false, false, &graGrade);

            instances[i].tmpX = instances[i].x;
            
            grax += fabs( (tmpXscore) / h);
            grad += fabs( (tmpDen - penaltyScore) / h );

            // part of y /////////////////////////////////////////////////////////////

            instances[i].tmpY = instances[i].y;
            instances[i].tmpY += h;

            tmpYscore = scoreOfY(rawNet, gamma, true, instances[i], yScore);

            tmpDen = penaltyScore;
            penaltyInfoOfinstance(instances[i], binInfo, fl, sl, false, true, &graGrade);
            tmpDen -= graGrade;
            penaltyInfoOfinstance(instances[i], binInfo, fl, sl, true, false, &graGrade);
            tmpDen += graGrade;
            penaltyInfoOfinstance(instances[i], binInfo, fl, sl, true, true, &graGrade);
            penaltyInfoOfinstance(instances[i], binInfo, fl, sl, false, false, &graGrade);

            gray += fabs( (tmpYscore) / h);
            grad += fabs( (tmpDen - penaltyScore) / h );
            instances[i].tmpY = instances[i].y;

            // part of z /////////////////////////////////////////////////////////////
                        
            float tmpTSV = 0.0;

            penaltyInfoOfinstance(instances[i], binInfo, fl, sl, false, true, &graGrade);

            instances[i].tmpZ = instances[i].z;
            instances[i].tmpZ += h;
            instances[i].density = returnDensity(instances[i].tmpZ, densityMap);

            tmpTSV = TSVofNet(rawNet, true, instances[i], tsvScore, densityMap);

            tmpDen = penaltyScore;
            tmpDen -= graGrade;
            penaltyInfoOfinstance(instances[i], binInfo, fl, sl, false, false, &graGrade);
            tmpDen += graGrade;
            penaltyInfoOfinstance(instances[i], binInfo, fl, sl, false, true, &graGrade);
            instances[i].tmpZ = instances[i].z;
            instances[i].density = returnDensity(instances[i].z, densityMap);
            penaltyInfoOfinstance(instances[i], binInfo, fl, sl, false, false, &graGrade);
            
            graz += fabs( (tmpTSV) / h);
            grad += fabs( (tmpDen - penaltyScore) / h);
        } 

        free(fl);
        free(sl);
        penaltyWeight = ((grax + gray + graz) / grad);
    }

    free(originfl);
    free(originsl);

    // printf("penalty Time: %fs\n", (endTime - startTime) / (float) CLOCKS_PER_SEC );

    return penaltyWeight;
}
