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

void cell2BestLayer( vector <instance> instances, int numInstances, Die topDie, Die btmDie)
{
    double topDieArea = 0.0;
    double btmDieArea = 0.0;
    double topDieUtl = 0.0;
    double btmDieUtl = 0.0;
    double topDieMaxUtl = 0.0;
    double btmDieMaxUtl = 0.0;

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

    if(topDieUtl > topDieMaxUtl)
        cout << "aaa" << endl;

}

void place2BestRow( vector <instance> instances, int numInstances, Die topDie, Die btmDie)
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






    /* check which row is stuffed. If stuffed, place to near row*/

    for(int row = 0; row < topDie.repeatCount; row++)
    {
        if( topDieCellsWidth[row] < (int) topDie.upperRightX)
            cout << "top die is stuffed!" << endl;
    }

    for(int row = 0; row < btmDie.repeatCount; row++)
    {
        if( btmDieCellsWidth[row] < (int) btmDie.upperRightX)
            cout << "bottom die is stuffed!" << endl;
    }





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
            
        }
    }

}

void calculateActualHPWL(const vector <instance> instances, const vector <RawNet> rawNet)
{   }