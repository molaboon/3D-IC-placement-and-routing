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

void firstPlacement(vector <instance> &instances, gridInfo binInfo)
{
    // give each cell initial solution

    srand( time(NULL) );

    double x[] = {24.0, 1.0, 20.0, 25.0, 23.0,0.0, 37, 17, };
    double y[] = {15.0, 1.0, 0.0, 0.0, 20.0, 0.0, 20.0, 15.0};
    double z[] = { 0.2, 0.8, 0.8, 0.8, 0.8, 0.2, 0.8, 0.8, 0.2};
    int cnt = 0;

    for(int i = 0; i < instances.size(); i++)
    {
        double minX = instances[i].width;
        double maxX = binInfo.dieWidth - instances[i].width;
        double minY = instances[i].height ;
        double maxY = binInfo.dieHeight - instances[i].height;

        double X = (maxX - minX) * rand() / (RAND_MAX +1.0) + minX;
        double Y = (maxY - minY) * rand() / (RAND_MAX +1.0) + minY;
        double Z = rand() / (RAND_MAX + 1.0);

        instances[i].x = x[cnt];
        instances[i].y = y[cnt];
        instances[i].z = z[cnt];
        cnt++;
        // instances[i].x = X;
        // instances[i].y = Y;
        // instances[i].z = Z;
    
        
    }
}

double returnPenaltyWeight(vector <RawNet> rawNet, const double gamma, vector <instance> &instances, gridInfo binInfo)
{
    double h = 0.00001;
    double penaltyWeight;
    double xScore = 0.0, yScore = 0.0 , tsvScore = 0.0, penaltyScore = 0.0;
    double grax = 0.0, gray = 0.0, graz = 0.0, grad = 0.0;

    int size = instances.size();

    xScore = scoreOfX(rawNet, gamma);
    yScore = scoreOfY(rawNet, gamma);
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

        tmpXscore = scoreOfX(rawNet, gamma);
        tmpDen = scoreOfz(rawNet, instances, binInfo, 0);

        grax += fabs( (tmpXscore - xScore) / h) ;
        grad += fabs( (tmpDen - tsvScore - penaltyScore) / h );

        instances[i].x = tmpx;

        free(fl);
        free(sl);

        // part of y

        instances[i].y += h;

        tmpYscore = scoreOfY(rawNet, gamma);

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
        penaltyInfoOfinstance(instances[j], instances[j].density, binInfo, originFirstLayer, originSecondLayer, false);

    gradientX(rawNet, gamma, instances, binInfo, penaltyWeight, xScore, penaltyScore, originFirstLayer, originSecondLayer);
    
    gradientY(rawNet, gamma, instances, binInfo, penaltyWeight, yScore, penaltyScore, originFirstLayer, originSecondLayer);

    gradientZ(rawNet, gamma, instances, binInfo, penaltyWeight, tsvScore, penaltyScore, originFirstLayer, originSecondLayer);

    return penaltyWeight;
}
