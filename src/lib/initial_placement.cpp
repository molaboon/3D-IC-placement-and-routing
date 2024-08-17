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

void firstPlacement(vector <Instance> &instances, gridInfo binInfo)
{
    // give each cell initial solution

    srand( time(NULL) );

    // double x[] = {10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0};

    // double y[] = {10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0};

    // double z[] = { 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8};

    for(int i = 0; i < instances.size(); i++)
    {

        double minX = instances[i].width ;

        double maxX = binInfo.dieWidth - instances[i].width;

        double minY = instances[i].height ;

        double maxY = binInfo.dieHeight - instances[i].height;

        double x = (maxX - minX) * rand() / (RAND_MAX +1.0) + minX;

        double y = (maxY - minY) * rand() / (RAND_MAX +1.0) + minY;

        instances[i].x = x;

        instances[i].y = y;

        instances[i].z = 0.5;

        // instances[i].x = x[i];

        // instances[i].y = y[i];

        // instances[i].z = z[i];

    }
}

double returnPenaltyWeight(vector <RawNet> rawNet, const double gamma, vector <Instance> &instances, gridInfo binInfo)
{
    double h = 0.00001;
    double penaltyWeight;
    double xScore = 0.0, yScore = 0.0 , tsvScore = 0.0, penaltyScore = 0.0;
    double grax = 0.0, gray = 0.0, graz = 0.0, grad = 0.0;

    int size = instances.size();

    xScore = scoreOfX(rawNet, gamma);

    yScore = scoreOfY(rawNet, gamma);

    // printf("Xscore : %lf\n", xScore);
    
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

        // printf("%lf \n", ((tmpXscore - xScore) / h) + 0.000404* (tmpDen - tsvScore - penaltyScore) / h);

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


    gradientX(rawNet, gamma, instances, binInfo, penaltyWeight, xScore, penaltyScore);
    
    gradientY(rawNet, gamma, instances, binInfo, penaltyWeight, yScore, penaltyScore);

    gradientZ(rawNet, gamma, instances, binInfo, penaltyWeight, tsvScore, penaltyScore);

    return penaltyWeight;
}
