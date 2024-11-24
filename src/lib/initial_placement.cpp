#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <iostream>

#include "initial_placement.h"
#include "CG.h"
#include "readfile.h"



void returnGridInfo(Die &die, gridInfo &binInfo, int Numinstance)
{   
    binInfo.dieWidth = die.upperRightX ; 
    binInfo.dieHeight = die.upperRightY ;

    binInfo.binWidth = 10.0;
    binInfo.binHeight = 10.0;

    binInfo.binXnum = (binInfo.dieWidth / binInfo.binWidth);
    binInfo.binYnum = binInfo.dieHeight / binInfo.binHeight;

    binInfo.Numinstance = Numinstance;

}

void firstPlacement(vector <instance> &instances, gridInfo binInfo, Die topDie)
{
    // give each cell initial solution

    srand( time(NULL) );

    // double x[] = {0.0, 0.0,    0.0, 6000.0,     0.0,  8613.0,     0,  5478,   0, 8000, 16000, 24000,     0,  8000, 12804, 12000, 17226, 12000, 16000, 19206, 18000, 25839, 18000, 16000, 25608, 24000, 20000, 24000,  8000, 32010, 30000, 30000, 30000,      0};
    // double y[] = {0.0,    0.0, 5800.0, 5800.0, 15000.0, 15000.0, 20000, 20000,   0,    0,     0,     0, 12000, 12000,     0,  5800, 15000, 25000, 12000,     0,  5800, 15000, 25000, 24000,     0,  5800, 20000, 25000, 24000,     0,  5800, 20000, 25000,  31000};
    // double z[] = {0.2,    0.8,    0.2,    0.8,     0.2,     0.8,   0.2,   0.8, 0.2,  0.8,   0.2,   0.8,   0.2,   0.8,   0.8,   0.2,   0.2,   0.8,   0.2,   0.8,   0.2,   0.8,   0.2,   0.2,   0.2,   0.2,   0.8,   0.2,   0.2,   0.8,   0.2,   0.2,   0.8,   0.2};
    int cnt = 0;

    for(int i = 0; i < instances.size(); i++)
    {
        double minX = instances[i].inflateWidth;
        double maxX = binInfo.dieWidth - instances[i].inflateWidth;
        double minY = instances[i].inflateHeight;
        double maxY = binInfo.dieHeight - instances[i].inflateHeight;

        double X = (maxX - minX) * rand() / (RAND_MAX + 1.0) + minX;
        double Y = (maxY - minY) * rand() / (RAND_MAX +1.0) + minY;
        double Z = rand() / (RAND_MAX + 1.0);

        // if(instances[i].isMacro)
        // {
        //     instances[i].x = x[cnt];
        //     instances[i].y = y[cnt];
        //     instances[i].z = z[cnt];
        //     instances[i].rotate = 0;

        //     cnt++;
        // }
        // else
        
        instances[i].x = X;
        instances[i].y = Y;
        instances[i].z = Z;
        instances[i].tmpX = X;
        instances[i].tmpY = Y;
        instances[i].tmpZ = Z;
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

    int size = instances.size();

    xScore = scoreOfX(rawNet, gamma, false);
    yScore = scoreOfY(rawNet, gamma, false);
    tsvScore = TSVofNet(rawNet);
    penaltyScore = scoreOfz(rawNet, instances, binInfo, 1);

    penaltyScore -= tsvScore;

    for(int i = 0; i < size; i++)
    {        
        double tmpx = instances[i].x;
        double tmpy = instances[i].y;
        double tmpDen = 0.0;
        double tmpXscore = 0.0, tmpYscore = 0.0;
        
        double *fl = createBins(binInfo);
        double *sl = createBins(binInfo);

        // part of x
        instances[i].x += h;

        tmpXscore = scoreOfX(rawNet, gamma, false);
        tmpDen = scoreOfz(rawNet, instances, binInfo, 0);

        grax += fabs( (tmpXscore - xScore) / h) ;
        grad += fabs( (tmpDen - tsvScore - penaltyScore) / h );

        instances[i].x = tmpx;

        free(fl);
        free(sl);

        // part of y

        instances[i].y += h;

        tmpYscore = scoreOfY(rawNet, gamma, false);

        tmpDen = scoreOfz(rawNet, instances, binInfo, 0);

        gray += fabs( (tmpYscore - yScore) / h);

        grad += fabs( (tmpDen - tsvScore - penaltyScore) / h );
                    
        instances[i].y = tmpy;
    }

    for(int i = 0; i < size ; i++)
    {
        double tmpz = instances[i].z;
        double tmpTSV = 0.0, tmpDen = 0.0;

        instances[i].z += h;

        tmpTSV = TSVofNet(rawNet);

        tmpDen = scoreOfz(rawNet, instances, binInfo, 1);

        tmpDen -= tmpTSV;

        graz += fabs( (tmpTSV - tsvScore) / h);

        grad += fabs( (tmpDen - penaltyScore) / h);

        instances[i].z = tmpz;
    } 

    penaltyWeight = (grax + gray + graz) / grad;

    double *originFirstLayer = createBins(binInfo);
    double *originSecondLayer = createBins(binInfo);

    for(int j = 0; j < size; j++)
        penaltyInfoOfinstance(instances[j], instances[j].density, binInfo, originFirstLayer, originSecondLayer, false, false);

    gradientX(rawNet, gamma, instances, binInfo, penaltyWeight, xScore, penaltyScore, originFirstLayer, originSecondLayer);
    
    gradientY(rawNet, gamma, instances, binInfo, penaltyWeight, yScore, penaltyScore, originFirstLayer, originSecondLayer);

    gradientZ(rawNet, gamma, instances, binInfo, penaltyWeight, tsvScore, penaltyScore, originFirstLayer, originSecondLayer);

    return penaltyWeight;
}
