#include <stdio.h>
#include <vector>
#include <cmath>
#include <string.h>
#include <iostream>
#include <cstdlib>
#include <map>

#include "readfile.h"

using namespace std;

#define btmLayer 0
#define topLayer 1

void cell2BestLayer( vector <instance> instances, int numInstances)
{
    for(int i = 0; i < numInstances; i++)
    {
        if( instances[i].z < 0.5)
            instances[i].layer = btmLayer;

        else
            instances[i].layer = topLayer;
    }
}

void place2BestRow( vector <instance> instances, int numInstances, Die topDie, Die btmDie)
{
    // left down

    /*  place to best row*/

    vector < vector<int> >  btmDiePlacementState( btmDie.repeatCount );
    vector < vector<int> >  topDiePlacementState( topDie.repeatCount );

    for(int inst = 0; inst < numInstances; inst++)
    {
        int row = 0;
        double upOrdown = instances[inst].y ;
 
        if( instances[inst].layer == btmLayer)
        {
            upOrdown = fmod( upOrdown, (double) btmDie.rowHeight);
            row = (int) ( instances[inst].y / (double) btmDie.rowHeight );

            if( upOrdown > (double) btmDie.rowHeight / 2.0 )
                btmDiePlacementState[row + 1].push_back(instances[inst].instIndex);
                
            else
                btmDiePlacementState[row].push_back(instances[inst].instIndex);
            
            instances[inst].y = btmDie.rowHeight * row;
        }
        else
        {
            upOrdown = fmod( upOrdown, (double) topDie.rowHeight);
            row = (int) ( instances[inst].y / (double) topDie.rowHeight );

            if( upOrdown > (double) topDie.rowHeight / 2.0 )
                topDiePlacementState[row + 1].push_back(instances[inst].instIndex);
            
            else
                topDiePlacementState[row].push_back(instances[inst].instIndex);
            
            instances[inst].y = topDie.rowHeight * row;
        }
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

void writeFile()
{

}

void calculateActualHPWL(const vector <instance> instances, const vector <RawNet> rawNet)
{   
}