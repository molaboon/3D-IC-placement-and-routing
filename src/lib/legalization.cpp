#include <stdio.h>
#include <vector>
#include <cmath>
#include <string.h>
#include <iostream>
#include <cstdlib>
#include <map>

#include "readfile.h"
#include "legalization.h"

using namespace std;

#define btmLayer 0
#define topLayer 1

void cell2BestLayer( vector <instance> &instances, const int numInstances, const Die topDie, const Die btmDie)
{
    double topDieArea = 0.0;
    double btmDieArea = 0.0;
    double topDieUtl = 0.0;
    double btmDieUtl = 0.0;
    double topDieMaxUtl = 0.0;
    double btmDieMaxUtl = 0.0;
    double overLayerUtl = 0.0;
    double overLayerMaxUtl = 0.0;
    double overDieArea = 0.0;
    double overDieTotalArea = 0.0;

    int overLayer = 0;
    bool over = false;

    for(int i = 0; i < numInstances; i++)
    {
        if( instances[i].z < 0.5)
        {
            instances[i].layer = btmLayer;
            btmDieArea += instances[i].area;
        }
        else
        {
            instances[i].layer = topLayer;
            topDieArea += instances[i].area;
        }
    }

    topDieUtl = topDieArea / (topDie.upperRightX * topDie.upperRightY);
    btmDieUtl = btmDieArea / (btmDie.upperRightX * btmDie.upperRightY);

    topDieMaxUtl = (double) topDie.MaxUtil / 100.0;
    btmDieMaxUtl = (double) btmDie.MaxUtil / 100.0;

    /* if any die exceed the  max utilizaiton*/

    if(topDieUtl > topDieMaxUtl)
    {
        overLayer = topLayer;
        overLayerUtl = topDieUtl;
        overLayerMaxUtl = topDieMaxUtl;
        overDieArea = topDieArea;
        overDieTotalArea = topDie.upperRightX * topDie.upperRightY;
        over = true;
    }
    else if ( btmDieUtl > btmDieMaxUtl)
    {
        overLayer = btmLayer;
        overLayerUtl = btmDieUtl;
        overLayerMaxUtl = btmDieMaxUtl;
        overDieArea = btmDieArea;
        overDieTotalArea = btmDie.upperRightX * btmDie.upperRightY;
        over = true;
    }

    if(over)
    {
        int minNumNetConnet = 1;

        do{
            for(int inst = 0; inst < numInstances; inst++)
            {
                if(instances[inst].numNetConnection == minNumNetConnet && instances[inst].layer == overLayer)
                {
                    instances[inst].layer = 1 - overLayer;
                    overDieArea -= instances[inst].area;
                    overLayerUtl = overDieArea / overDieTotalArea;

                    if(overLayerUtl < overLayerMaxUtl)
                        break;
                }
            }

            minNumNetConnet += 1;

        }while (overLayerUtl > overLayerMaxUtl);
    }
    
}

void place2BestRow( vector <instance> &instances, const int numInstances, const Die topDie, const Die btmDie)
{

    /*  place to best row*/

    vector < vector<int> >  topDiePlacementState( topDie.repeatCount );
    vector < vector<int> >  btmDiePlacementState( btmDie.repeatCount );

    int topDieCellsWidth[topDie.repeatCount] = {0};
    int btmDieCellsWidth[btmDie.repeatCount] = {0};

    for(int inst = 0; inst < numInstances; inst++)
    {
        int row = 0;
        double upOrdown = instances[inst].y ;
 
        if( instances[inst].layer == topLayer)
        {
            upOrdown = fmod( upOrdown, (double) topDie.rowHeight);
            row = (int) ( instances[inst].y / (double) topDie.rowHeight );

            if( upOrdown > (double) topDie.rowHeight / 2.0 )
                row += 1;
                
            topDiePlacementState[row].push_back(instances[inst].instIndex);
            instances[inst].y = topDie.rowHeight * row;
            topDieCellsWidth[row] += instances[inst].width;
            
        }
        else
        {
            upOrdown = fmod( upOrdown, (double) btmDie.rowHeight);
            row = (int) ( instances[inst].y / (double) btmDie.rowHeight );

            if( upOrdown > (double) btmDie.rowHeight / 2.0 )
                row += 1;

            btmDiePlacementState[row].push_back(instances[inst].instIndex);
            instances[inst].y = btmDie.rowHeight * row;
            btmDieCellsWidth[row] += instances[inst].width;
        }
    }
    // for(int row = 0; row < topDie.repeatCount; row++)
    //     cout << topDieCellsWidth[row] << endl;
    
    /* check which row is stuffed. If stuffed, place to near row*/

    for(int row = 0; row < topDie.repeatCount; row++)
    {
        while( topDieCellsWidth[row] > (int) topDie.upperRightX)
        {
            int size = topDiePlacementState[row].size();
            int lastInstIndex = topDiePlacementState[row][size - 1];
            int lastInstWidth = instances[lastInstIndex].width;
            int tmpRow = row - 1;

            while (tmpRow >= 0)
            {   
                if (topDieCellsWidth[tmpRow] + lastInstWidth < (int) topDie.upperRightX)
                {
                    topDiePlacementState[tmpRow].push_back(lastInstIndex);
                    topDiePlacementState[row].pop_back();

                    topDieCellsWidth[tmpRow] += lastInstWidth;
                    topDieCellsWidth[row] -= lastInstWidth;
                    
                } 
                tmpRow -= 1;
            }
        }
    }

    for(int row = 0; row < btmDie.repeatCount; row++)
    {
        while( btmDieCellsWidth[row] > (int) btmDie.upperRightX)
        {
            int size = btmDiePlacementState[row].size();
            int lastInstIndex = btmDiePlacementState[row][size - 1];
            int lastInstWidth = instances[lastInstIndex].width;
            int tmpRow = row - 1;

            while (tmpRow >= 0)
            {   
                if (btmDieCellsWidth[tmpRow] + lastInstWidth < (int) btmDie.upperRightX)
                {
                    btmDiePlacementState[tmpRow].push_back(lastInstIndex);
                    btmDiePlacementState[row].pop_back();
                    btmDieCellsWidth[row] -= lastInstWidth;
                    btmDieCellsWidth[tmpRow] += lastInstWidth;
                }
                tmpRow -= 1;
            }
        }
    }
    for(int row = 0; row < topDie.repeatCount; row++)
        cout << topDieCellsWidth[row] << endl;

    for(int row = 0; row < btmDie.repeatCount; row++)
        cout << btmDieCellsWidth[row] << endl;

    /*  place to best X position*/

    for(int count = 0; count < btmDie.repeatCount; count++)
    {
        int numInstInRow = btmDiePlacementState[count].size();

        map<int, int> instMap;
        vector <int> sortArray(numInstInRow);

        for(int inst = 0; inst < numInstInRow; inst++)
        {
            int cellId = btmDiePlacementState[count][inst];
            instMap[cellId] = (int) instances[cellId].x;  
            // cout << cellId << " "<<instMap[cellId] << endl;
        }
    }
}

void calculateActualHPWL(const vector <instance> instances, const vector <RawNet> rawNet)
{   }