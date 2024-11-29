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



void returnGridInfo(Die *die, gridInfo *binInfo, int Numinstance)
{   
    binInfo->dieWidth = die->upperRightX ; 
    binInfo->dieHeight = die->upperRightY ;

    // binXnum * binYnum ~= Numinstance
    binInfo->binXnum = floor( sqrt( (double) Numinstance ));
    binInfo->binYnum = floor( sqrt( (double) Numinstance ));

    binInfo->binWidth = floor(die->upperRightX / binInfo->binXnum);
    binInfo->binHeight = floor(die->upperRightX / binInfo->binYnum);

    
    binInfo->Numinstance = Numinstance;

}

void firstPlacement(vector <instance> &instances, gridInfo binInfo, Die topDie)
{
    // give each cell initial solution

    // srand( time(NULL) );

    // double x[] = {0.0, 0.0,    0.0, 6000.0,     0.0,  8613.0,     0,  5478,   0, 8000, 16000, 24000,     0,  8000, 12804, 12000, 17226, 12000, 16000, 19206, 18000, 25839, 18000, 16000, 25608, 24000, 20000, 24000,  8000, 32010, 30000, 30000, 30000,      0};
    // double y[] = {0.0,    0.0, 5800.0, 5800.0, 15000.0, 15000.0, 20000, 20000,   0,    0,     0,     0, 12000, 12000,     0,  5800, 15000, 25000, 12000,     0,  5800, 15000, 25000, 24000,     0,  5800, 20000, 25000, 24000,     0,  5800, 20000, 25000,  31000};
    // double z[] = {0.2,    0.8,    0.2,    0.8,     0.2,     0.8,   0.2,   0.8, 0.2,  0.8,   0.2,   0.8,   0.2,   0.8,   0.8,   0.2,   0.2,   0.8,   0.2,   0.8,   0.2,   0.8,   0.2,   0.2,   0.2,   0.2,   0.8,   0.2,   0.2,   0.8,   0.2,   0.2,   0.8,   0.2};
    // double x[] = {15.0, 16.0, 17.0, 18.0, 15.0, 16.0, 17.0, 18.0};
    // double y[] = {15.0, 16.0, 17.0, 18.0, 15.0, 16.0, 17.0, 18.0};
    
    int cnt = 0;

    for(int i = 0; i < instances.size(); i++)
    {
        double minX = instances[i].inflateWidth;
        double maxX = binInfo.dieWidth - instances[i].inflateWidth;
        double minY = instances[i].inflateHeight;
        double maxY = binInfo.dieHeight - instances[i].inflateHeight;

        double X = fmod( (double) rand(), ( maxX - minX + 1) ) + minX ;
        double Y = fmod( (double) rand(), ( maxY - minY + 1) ) + minY ;
        double Z = 0.5;

        // if(instances[i].isMacro)
        // // {
        // instances[i].x = x[cnt];
        // instances[i].y = y[cnt];
        // instances[i].z = z[cnt];
        instances[i].rotate = 0;

        // cnt++;
        
        instances[i].x = X;
        instances[i].y = Y;
        instances[i].z = Z;
        instances[i].tmpX = instances[i].x;
        instances[i].tmpY = instances[i].y;
        instances[i].tmpZ = instances[i].z;
    }
}

void macroFirstPlacement( vector <instance> &macros, vector <instance*> &netOfmacros ) 
{

}

double returnPenaltyWeight(vector <RawNet> rawNet, const double gamma, vector <instance> &instances, gridInfo binInfo)
{
    double h = 0.00001;
    double penaltyWeight;
    double xScore = 0.0, yScore = 0.0 , tsvScore = 0.0, penaltyScore = 0.0;
    double grax = 0.0, gray = 0.0, graz = 0.0, grad = 0.0;
    double *originfl = createBins(binInfo);
    double *originsl = createBins(binInfo);
    double *fl = createBins(binInfo);
    double *sl = createBins(binInfo);

    int size = instances.size();

    xScore = scoreOfX(rawNet, gamma, false);
    yScore = scoreOfY(rawNet, gamma, false);
    tsvScore = TSVofNet(rawNet);

    for(int i = 0; i < size; i++)
    {
        instances[i].density = returnDensity(instances[i].z, 0.0);
        penaltyInfoOfinstance(instances[i], binInfo, originfl, originsl, false, false);
    }

    penaltyScore = scoreOfPenalty(originfl, originsl, binInfo);

    penaltyScore -= tsvScore;

    double startTime = clock();

    for(int i = 0; i < size; i++)
    {        
        double tmpx = instances[i].x;
        double tmpy = instances[i].y;
        double tmpDen = 0.0;
        double tmpXscore = 0.0, tmpYscore = 0.0;

        // part of x
        instances[i].tmpX = instances[i].x;
        instances[i].tmpX += h;
        
        memcpy(fl, originfl,  binInfo.binXnum * binInfo.binYnum * sizeof(double));
        memcpy(sl, originsl,  binInfo.binXnum * binInfo.binYnum * sizeof(double));

        tmpXscore = scoreOfX(rawNet, gamma, true);

        penaltyInfoOfinstance(instances[i], binInfo, fl, sl, false, true);
        penaltyInfoOfinstance(instances[i], binInfo, fl, sl, true, false);

        tmpDen = scoreOfPenalty(fl, sl, binInfo);

        grax += fabs( (tmpXscore - xScore) / h) ;
        grad += fabs( (tmpDen - penaltyScore) / h );

        // part of y /////////////////////////////////////////////////////////////

        memcpy(fl, originfl,  binInfo.binXnum * binInfo.binYnum * sizeof(double));
        memcpy(sl, originsl,  binInfo.binXnum * binInfo.binYnum * sizeof(double));

        instances[i].tmpY = instances[i].y;
        instances[i].tmpY += h;

        tmpYscore = scoreOfY(rawNet, gamma, true);

        penaltyInfoOfinstance(instances[i], binInfo, fl, sl, false, true);
        penaltyInfoOfinstance(instances[i], binInfo, fl, sl, true, false);

        tmpDen = scoreOfPenalty(fl, sl, binInfo);

        gray += fabs( (tmpYscore - yScore) / h);
        grad += fabs( (tmpDen - penaltyScore) / h );

        // part of z /////////////////////////////////////////////////////////////
                       
        double tmpz = instances[i].z;
        double tmpTSV = 0.0;

        instances[i].z += h;
        instances[i].density = returnDensity(instances[i].z, 0.0);

        tmpTSV = TSVofNet(rawNet);

        memcpy(fl, originfl,  binInfo.binXnum * binInfo.binYnum * sizeof(double));
        memcpy(sl, originsl,  binInfo.binXnum * binInfo.binYnum * sizeof(double));

        penaltyInfoOfinstance(instances[i], binInfo, fl, sl, false, true);
        penaltyInfoOfinstance(instances[i], binInfo, fl, sl, true, false);

        tmpDen = scoreOfPenalty(fl, sl, binInfo);

        graz += fabs( (tmpTSV - tsvScore) / h);
        grad += fabs( (tmpDen - penaltyScore) / h);

        instances[i].z = tmpz;
        instances[i].density = returnDensity(instances[i].z, 0.0);        
    } 

    free(fl);
    free(sl);

    penaltyWeight = (grax + gray + graz) / grad;

    gradientX(rawNet, gamma, instances, binInfo, penaltyWeight, xScore, penaltyScore, originfl, originsl);
    gradientY(rawNet, gamma, instances, binInfo, penaltyWeight, yScore, penaltyScore, originfl, originsl);
    gradientZ(rawNet, gamma, instances, binInfo, penaltyWeight, tsvScore, penaltyScore, originfl, originsl);

    double endTime = clock();

    printf("penalty Time: %fs\n", (endTime - startTime) / (double) CLOCKS_PER_SEC );

    return penaltyWeight;
}
