#include <stdio.h>
#include <vector>
#include <cmath>
#include <string.h>
#include <iostream>
#include <cstdlib>
#include <map>
#include <algorithm>

#include "readfile.h"
#include "legalization.h"

using namespace std;

#define btmLayer 0
#define topLayer 1
#define theCellDoesntPlaceToLayer 3

void cell2BestLayer( vector <instance> &instances, const Die topDie, const Die btmDie)
{
    double topDieArea = 0.0;
    double btmDieArea = 0.0;
    double topDieUtl = 0.0;
    double btmDieUtl = 0.0;
    double topDieMaxUtl = 0.0;
    double btmDieMaxUtl = 0.0;
    double maxArea = (topDie.upperRightX * topDie.upperRightY);

    int numInstances = instances.size();

    for(int i = 0; i < numInstances; i++)
    {
        if( instances[i].z < 0.5 && instances[i].layer == theCellDoesntPlaceToLayer)
        {
            instances[i].layer = btmLayer;
            
            if(instances[i].rotate == 90 || instances[i].rotate == 270)
            {
                instances[i].finalWidth = (int) instances[i].inflateHeight;
                instances[i].finalHeight = (int) instances[i].inflateWidth;
            }
            else
            {
                instances[i].finalWidth = (int) instances[i].inflateWidth;
                instances[i].finalHeight = (int) instances[i].inflateHeight;
            }

            btmDieArea += instances[i].inflateArea;
        }
        else if(instances[i].z > 0.5 && instances[i].layer == theCellDoesntPlaceToLayer)
        {
            instances[i].layer = topLayer;
        
            if(instances[i].rotate == 90 || instances[i].rotate == 270)
            {
                instances[i].finalWidth = (int) instances[i].height;
                instances[i].finalHeight = (int) instances[i].width;
            }
            else
            {
                instances[i].finalWidth = (int) instances[i].width;
                instances[i].finalHeight = (int) instances[i].height;
            }
            
            topDieArea += instances[i].area;
        }
    }

    topDieUtl = topDieArea / maxArea;
    btmDieUtl = btmDieArea / maxArea;

    topDieMaxUtl = (double) topDie.MaxUtil / 100.0;
    btmDieMaxUtl = (double) btmDie.MaxUtil / 100.0;

    /* if any die exceed the  max utilizaiton*/
    int whileLoop = 1;

    if(topDieUtl > topDieMaxUtl)
    {
        int minNumNetConnet = whileLoop;

        do{
            for(int inst = 0; inst < numInstances; inst++)
            {
                if( instances[inst].layer == topLayer && instances[inst].numNetConnection == minNumNetConnet)
                {
                    if( (btmDieArea + instances[inst].inflateArea) / maxArea > btmDieMaxUtl )
                        continue;

                    instances[inst].layer = btmLayer;

                    topDieArea -= instances[inst].area;
                    btmDieArea += instances[inst].inflateArea;

                    instances[inst].finalWidth = (int) instances[inst].inflateWidth;
                    instances[inst].finalHeight = (int) instances[inst].inflateHeight;
                
                    topDieUtl = topDieArea / (topDie.upperRightX * topDie.upperRightY);
                    btmDieUtl = btmDieArea / (btmDie.upperRightX * btmDie.upperRightY);

                    if(topDieUtl < topDieMaxUtl)
                        break;
                }
            }

            minNumNetConnet += 1;

        }while (topDieUtl > topDieMaxUtl && minNumNetConnet < 50);

        bool chan = false;
    }
    
    if ( btmDieUtl > btmDieMaxUtl)
    {
        int minNumNetConnet = whileLoop;

        do{
            for(int inst = 0; inst < numInstances; inst++)
            {
                if(instances[inst].layer == btmLayer && instances[inst].numNetConnection == minNumNetConnet)
                {
                    if( (topDieArea + instances[inst].area) / maxArea > topDieMaxUtl )
                        continue;

                    instances[inst].layer = topLayer;

                    topDieArea += instances[inst].area;
                    btmDieArea -= instances[inst].inflateArea;

                    instances[inst].finalWidth = (int) instances[inst].width;
                    instances[inst].finalHeight = (int) instances[inst].height;
                
                    topDieUtl = topDieArea / (topDie.upperRightX * topDie.upperRightY);
                    btmDieUtl = btmDieArea / (btmDie.upperRightX * btmDie.upperRightY);

                    if(btmDieUtl < btmDieMaxUtl )
                        break;
                }
            }

            minNumNetConnet += 1;

        }while (btmDieUtl > btmDieMaxUtl && minNumNetConnet < 50);

    }

    bool chan = false;
    if(topDieUtl > topDieMaxUtl || btmDieUtl > btmDieMaxUtl)
    {
        for(int inst = 0; inst < numInstances; inst++)
        {
            if( instances[inst].layer == topLayer)
            {
                for(int j = 0; j < numInstances; j++)
                {
                    if( instances[j].layer == btmLayer)
                    {
                        double tmpTopUtl = (topDieArea - instances[inst].area + instances[j].area) / maxArea;
                        double tmpBtmUtl = (btmDieArea - instances[j].inflateArea + instances[inst].inflateArea) / maxArea;

                        if( tmpTopUtl < topDieMaxUtl && tmpBtmUtl < btmDieMaxUtl)
                        {
                            topDieArea = topDieArea - instances[inst].area + instances[j].area;
                            btmDieArea = btmDieArea - instances[j].inflateArea + instances[inst].inflateArea;

                            instances[inst].layer = btmLayer; 
                            instances[j].layer = topLayer;

                            instances[inst].finalWidth = (int) instances[inst].inflateWidth;
                            instances[inst].finalHeight = (int) instances[inst].inflateHeight;

                            instances[j].finalWidth = (int) instances[j].width;
                            instances[j].finalHeight = (int) instances[j].height;

                            topDieUtl = topDieArea / maxArea;
                            btmDieUtl = btmDieArea / maxArea;
                            

                            chan = true;
                            break;
                        }
                    }
                }
            }

            if(chan)
                break;
        }
    }

}

void place2BestRow( vector <instance> &instances, const int numInstances, Die topDie, Die btmDie, vector <instance> &macros)
{
    vector < vector<int> > topDiePlacementState( topDie.repeatCount );
    vector < vector<int> > btmDiePlacementState( btmDie.repeatCount );

    vector < vector<int> > topDieMacrosPlacement(topDie.repeatCount);
    vector < vector<int> > btmDieMacrosPlacement(btmDie.repeatCount);

    int topDieCellsWidth[topDie.repeatCount] = {0};
    int btmDieCellsWidth[btmDie.repeatCount] = {0};
    int numMacro = macros.size();

    for(int row = 0; row < topDie.repeatCount; row++)
    {
        topDiePlacementState[row].reserve(numInstances/5);
        topDieMacrosPlacement[row].reserve(numMacro);
    }
    
    for(int row = 0; row < btmDie.repeatCount; row++)
    {
        btmDiePlacementState[row].reserve(numInstances/5);
        btmDieMacrosPlacement[row].reserve(numMacro);
    }
    
    /*  place standard cell to best row */

    for(int inst = 0; inst < numInstances; inst++)
    {
        int row = 0;
        double upOrdown = instances[inst].y ;
 
        if(instances[inst].isMacro )
        {
            int uperX = instances[inst].finalX + (instances[inst].finalWidth) ;
            int uperY = instances[inst].finalY + (instances[inst].finalHeight) ;
            int lowerX = instances[inst].finalX;
            int lowerY = instances[inst].finalY;
            int uperRow, lowerRow;
            instances[inst].finalY = lowerY;  
            
            if( instances[inst].layer == topLayer)
            {
                uperRow = (uperY / topDie.rowHeight) ;
                lowerRow = (lowerY / topDie.rowHeight);
                
                if(uperRow >= topDie.repeatCount)
                    uperRow = topDie.repeatCount - 1;

                for(int row = lowerRow; row <= uperRow; row++)
                {
                    topDieCellsWidth[row] += (int) instances[inst].finalWidth;
                    topDiePlacementState[row].push_back(instances[inst].instIndex);
                    topDieMacrosPlacement[row].push_back(instances[inst].instIndex);
                } 
            }   
            else
            {
                uperRow = (uperY / btmDie.rowHeight);
                lowerRow = lowerY / btmDie.rowHeight;

                if(uperRow >= btmDie.repeatCount)
                    uperRow = btmDie.repeatCount-1;

                for(int row = lowerRow; row <= uperRow; row++)
                {
                    btmDieCellsWidth[row] += instances[inst].finalWidth;
                    btmDiePlacementState[row].push_back(instances[inst].instIndex);
                    btmDieMacrosPlacement[row].push_back(instances[inst].instIndex);
                }
            }
        }
        else
        {
            if( instances[inst].layer == topLayer)
            {
                upOrdown = fmod( upOrdown, (double) topDie.rowHeight);
                row = ( (int) instances[inst].y) / topDie.rowHeight ;

                if( upOrdown > (double) topDie.rowHeight / 2.0 )
                    row += 1;
                
                if( row >= topDie.repeatCount)
                    row = topDie.repeatCount - 1;
                    
                topDiePlacementState[row].push_back(instances[inst].instIndex);
                instances[inst].finalY = topDie.rowHeight * row;
                topDieCellsWidth[row] += instances[inst].finalWidth;
                
            }
            else
            {
                upOrdown = fmod( upOrdown, (double) btmDie.rowHeight);
                row = ((int) instances[inst].y )  / btmDie.rowHeight ;

                if( upOrdown > (double) btmDie.rowHeight / 2.0 )
                    row += 1;
                
                if( row >= btmDie.repeatCount)
                    row = btmDie.repeatCount - 1;
                
                btmDiePlacementState[row].push_back(instances[inst].instIndex);
                instances[inst].finalY = btmDie.rowHeight * row;
                btmDieCellsWidth[row] += instances[inst].finalWidth;
            }
            
            instances[inst].rotate = 0;
        }
        instances[inst].finalX = (int) instances[inst].x;
    }
    

    /* check which row is stuffed. If stuffed, place to near row*/

    place2nearRow(topDie, btmDie, topDiePlacementState, btmDiePlacementState, instances, topDieCellsWidth, btmDieCellsWidth);
    place2nearRow(btmDie, topDie, btmDiePlacementState, topDiePlacementState, instances, btmDieCellsWidth, topDieCellsWidth);

    /*  place instances to best X position*/

    placeInst2BestX(topDie, topDiePlacementState, topDieMacrosPlacement, instances, topDieCellsWidth);
    placeInst2BestX(btmDie, btmDiePlacementState, btmDieMacrosPlacement, instances, btmDieCellsWidth);
}

void placeInst2BestX(const Die die, vector <vector<int>> &diePlacementState, vector <vector<int>> &dieMacroPlacementState, vector <instance> &instances, int *cellWidth)
{
    for(int row = 0; row < die.repeatCount; row++)
    {
        int numInstInRow = diePlacementState[row].size();
        int numMacroInRow = dieMacroPlacementState[row].size();
        int numStdcellInRow = (numInstInRow - numMacroInRow);
        int numNoInst = 0;
        int upperRightX = (int) die.upperRightX;

        if(numInstInRow == 0 || numStdcellInRow == 0)
            continue;

        map<int, int> instMap;
        map<int, int> macroMap;

        vector <int> macroSortArray(numMacroInRow);
        vector <int> sortArray(numStdcellInRow);
        
        /* macro first */
        if( numMacroInRow > 0 )
        {   
            for(int i = 0; i < numMacroInRow; i++)
            {
                int cellId = dieMacroPlacementState[row][i];
                int cellX = instances[cellId].finalX;

                macroMap[cellX] = cellId; 
                macroSortArray[i] = cellX;
            }
            sort(macroSortArray.begin(), macroSortArray.end()); 
        }
        
        /* instance last */
        for(int inst = 0; inst < numInstInRow; inst++)
        {
            int cellId = diePlacementState[row][inst];
            int cellX = instances[cellId].finalX - instances[cellId].finalWidth/2;

            if(instances[cellId].isMacro)
            {
                numNoInst ++;
                continue;
            }
            else
            {
                while( instMap.find( cellX ) != instMap.end() )
                {
                    instances[cellId].finalX += 1.0;
                    cellX = instances[cellId].finalX - instances[cellId].finalWidth/2;
                }

                instMap[cellX] = cellId;
                sortArray[inst - numNoInst] = cellX;
                instances[cellId].finalX = cellX;
            }
            
        }
        sort(sortArray.begin(), sortArray.end());

        /*  place to best X
            1. create the sort array to sort the x of each cell
            2. use the variable "startX" to record the x position to the next cell
            3. if there is no empty space to place the cell, look down the row and find the empty space to place it
        */

        int startX = 0, macroCnt = 0, nowMacroID = 0, macroWidth = 0;
        int macroX = (int) die.upperRightX;
        int nowRowCellWidth = cellWidth[row];

        if( numMacroInRow > 0)
        {
            macroCnt = 0;
            nowMacroID = macroMap[ macroSortArray[macroCnt] ];
            macroX = instances[nowMacroID].finalX;
            macroWidth = instances[nowMacroID].finalWidth;
        }
        
        for(int inst = 0; inst < numStdcellInRow; inst++)
        {
            int cellID = instMap[ sortArray[inst] ];
            int nowCellFinalX = instances[cellID].finalX;
            int nowCellWidth = instances[cellID].finalWidth;

            if( startX + nowCellWidth >= upperRightX )
            {
                int lookDown = row + 1;
                int lookUp = row - 1;
                bool haveChanged = false;
                        
                while ( !haveChanged && lookDown < die.repeatCount )
                {   
                    if( cellWidth[lookDown] + nowCellWidth < upperRightX )
                    {
                        int cellCoor = find( diePlacementState[row].begin(), diePlacementState[row].end(), cellID) - diePlacementState[row].begin();

                        diePlacementState[lookDown].push_back(cellID);
                        diePlacementState[row].erase(diePlacementState[row].begin() + cellCoor);
                        cellWidth[row] -= nowCellWidth;
                        cellWidth[lookDown] += nowCellWidth;
                        instances[cellID].finalY = die.rowHeight * lookDown;
                        haveChanged = true;
                    }
                    lookDown++;
                }
                
                if( !haveChanged )
                {
                    int cellCoor = find( diePlacementState[row].begin(), diePlacementState[row].end(), cellID) - diePlacementState[row].begin();
                    // if (cellID = 10206)
                    // {
                    //     cout << row << endl;
                    // }

                    while ( !haveChanged && lookUp > 0 )
                    {   
                        if( cellWidth[lookUp] + nowCellWidth*2 < upperRightX )
                        {
                            int s = (int) diePlacementState[lookUp].size();
                            for(int k = 1; k < s; k++)
                            {
                                if( instances[diePlacementState[lookUp][k]].finalX - instances[diePlacementState[lookUp][k-1]].finalX - instances[diePlacementState[lookUp][k-1]].finalWidth > nowCellWidth)
                                {
                                    int lastCellID = diePlacementState[lookUp][k-1]; 
                                    diePlacementState[lookUp].insert(diePlacementState[lookUp].begin() + k, cellID);
                                    diePlacementState[row].erase(diePlacementState[row].begin() + cellCoor);
                                    cellWidth[row] -= nowCellWidth;
                                    cellWidth[lookUp] += nowCellWidth;
                                    instances[cellID].finalX = instances[lastCellID].finalX + instances[lastCellID].finalWidth;
                                    instances[cellID].finalY = die.rowHeight * lookUp;
                                    haveChanged = true;
                                }

                                if(haveChanged)
                                    break;
                            }
                        }
                        lookUp--;
                    }
                }
            }
            else
            {
                if(startX < nowCellFinalX && nowCellFinalX + nowCellWidth < macroX && upperRightX - nowRowCellWidth > startX)
                {
                    if( nowCellFinalX + nowRowCellWidth > upperRightX )
                        startX = upperRightX - nowRowCellWidth;
                    
                    else
                        startX = nowCellFinalX;
                }

                if( startX + nowCellWidth < macroX)
                {
                    instances[cellID].finalX = startX;
                    startX += nowCellWidth;
                }
                else 
                {
                    do
                    {
                        startX = macroX + macroWidth;   
                        nowRowCellWidth -= macroWidth;
                        macroCnt += 1;
    
                        if( macroCnt < numMacroInRow )
                        {
                            nowMacroID = macroMap[ macroSortArray[macroCnt] ];
                            macroX = instances[nowMacroID].finalX;
                            macroWidth = instances[nowMacroID].finalWidth;
                        }
                        else
                        {
                            macroX = upperRightX;
                        }

                    } while (startX + nowCellWidth > macroX);
                    
                    instances[cellID].finalX = startX;
                    startX += nowCellWidth;
                }
            }

            nowRowCellWidth -= nowCellWidth;            
        }

        short cnt = 0;

        for(int i = 0; i < (int) diePlacementState[row].size(); i++)
        {
            if( ! instances[ diePlacementState[row][i] ].isMacro  )
            {
                diePlacementState[row][i] = instMap[ sortArray[cnt] ];
                cnt++;
            }
        }
    }
}

void place2nearRow(const Die die, const Die theOtherDie, vector <vector<int>> &diePlacementState, vector <vector<int>> &theOtherDiePlacementState, vector <instance> &instances, int *dieCellWidth, int *theOtherDieCellWidth)
{
    for(int row = 0; row < die.repeatCount; row++)
    {
        int upperRightX = die.upperRightX;

        while(dieCellWidth[row] > upperRightX)
        {
            int size = diePlacementState[row].size();
            int lastInstIndex = 0, lastInstWidth = 0;
            int lookUp = row - 1, lookDown = row + 1;
            int chooseCellCoor = 0;
            bool haveChanged = false;

            for(int i = 0; i < size; i++)
            {
                lastInstIndex = diePlacementState[row][i];

                if( !instances[lastInstIndex].isMacro)
                {
                    if(die.index == topLayer)
                        lastInstWidth = (int) instances[lastInstIndex].width;

                    else
                        lastInstWidth = (int) instances[lastInstIndex].inflateWidth;
                
                    chooseCellCoor = i;
                }
                else
                    continue;
                        
                while ( !haveChanged && (lookUp >= 0 || lookDown < die.repeatCount) )
                {   
                    if(dieCellWidth[lookUp] + lastInstWidth < upperRightX && lookUp >= 0)
                    {
                        diePlacementState[lookUp].push_back(lastInstIndex);
                        diePlacementState[row].erase(diePlacementState[row].begin() + chooseCellCoor);
                        dieCellWidth[row] -= lastInstWidth;
                        dieCellWidth[lookUp] += lastInstWidth;
                        instances[lastInstIndex].finalY = die.rowHeight * lookUp;
                        haveChanged = true;
                    }
                    else if (dieCellWidth[lookDown] + lastInstWidth < upperRightX && lookDown < die.repeatCount && !haveChanged)
                    {
                        diePlacementState[lookDown].push_back(lastInstIndex);
                        diePlacementState[row].erase(diePlacementState[row].begin() + chooseCellCoor);
                        dieCellWidth[row] -= lastInstWidth;
                        dieCellWidth[lookDown] += lastInstWidth;
                        instances[lastInstIndex].finalY = die.rowHeight * lookDown ;
                        haveChanged = true;
                    }
                    lookDown++;
                    lookUp--;
                }

                if(!haveChanged)
                {   
                    lookUp = row - 1;
                    lookDown = row + 1;
                    upperRightX = (int) theOtherDie.upperRightX;

                    if(theOtherDie.index == topLayer)
                    {
                        lastInstWidth = (int) instances[lastInstIndex].width;
                        instances[lastInstIndex].finalWidth = lastInstWidth;
                        instances[lastInstIndex].finalHeight = (int) instances[lastInstIndex].height;
                        instances[lastInstIndex].layer = topLayer;
                    }
                    else
                    {
                        lastInstWidth = (int) instances[lastInstIndex].inflateWidth;
                        instances[lastInstIndex].finalWidth = lastInstWidth;
                        instances[lastInstIndex].finalHeight = (int) instances[lastInstIndex].inflateHeight;
                        instances[lastInstIndex].layer = btmLayer;
                    }
                    
                    while ( !haveChanged && (lookUp >= 0 || lookDown < die.repeatCount) )
                    {   
                        if(theOtherDieCellWidth[lookUp] + lastInstWidth < upperRightX && lookUp >= 0)
                        {
                            theOtherDiePlacementState[lookUp].push_back(lastInstIndex);
                            diePlacementState[row].erase(diePlacementState[row].begin() + chooseCellCoor);
                            dieCellWidth[row] -= lastInstWidth;
                            theOtherDieCellWidth[lookUp] += lastInstWidth;
                            instances[lastInstIndex].finalY = theOtherDie.rowHeight * lookUp;
                            haveChanged = true;
                        }
                        else if (theOtherDieCellWidth[lookDown] + lastInstWidth < upperRightX && lookDown < theOtherDie.repeatCount && !haveChanged)
                        {
                            theOtherDiePlacementState[lookDown].push_back(lastInstIndex);
                            diePlacementState[row].erase(diePlacementState[row].begin() + chooseCellCoor);
                            dieCellWidth[row] -= lastInstWidth;
                            theOtherDieCellWidth[lookDown] += lastInstWidth;
                            instances[lastInstIndex].finalY = theOtherDie.rowHeight * lookDown ;
                            haveChanged = true;
                        }
                        lookDown++;
                        lookUp--;
                    }
                    if(haveChanged)
                        break;
                }
                if(haveChanged)
                    break;
            }
        }
    }
}

void insertTerminal(const vector <instance> instances, const vector <RawNet> rawNet, vector <terminal> &terminals, Hybrid_terminal terminalTech, Die topDie)
{ 
    int numNet = rawNet.size();
    int numTerminal = 0;
    int dieWidth = (int) topDie.upperRightX;
    int dieHight = (int) topDie.upperRightY;

    int terminalX = terminalTech.spacing + terminalTech.sizeX / 2;
    int terminalY = terminalTech.spacing + terminalTech.sizeY / 2;

    for(int net = 0; net < numNet; net++)
    {
        int firstInstLater = rawNet[net].Connection[0]->layer;

        for(int inst = 1 ; inst < rawNet[net].numPins; inst++)
        {
            if( rawNet[net].Connection[inst]->layer != firstInstLater )
            {
                terminal newTerminal;

                numTerminal++;
                newTerminal.netID = net;
                newTerminal.x = 0;
                newTerminal.y = 0;

                terminals.push_back(newTerminal);

                break;
            }
        }
    }

    for (int t = 0; t < numTerminal; t++)
    {
        terminals[t].x = terminalX;
        terminals[t].y = terminalY;

        terminalX += terminalTech.sizeX + terminalTech.spacing;

        if (terminalX + terminalTech.sizeX + terminalTech.spacing > dieWidth)
        {
            terminalX = terminalTech.spacing + terminalTech.sizeX / 2;
            terminalY += terminalTech.sizeY + terminalTech.spacing;
        }
    }
}

void calculateActualHPWL(const vector <instance> instances, const vector <RawNet> rawNet, vector <terminal> &terminals)
{}

void writeVisualFile(const vector <instance> instances, int iteration, Die &topDie)
{
    char filename[30];
    snprintf(filename, sizeof(filename), "visulization/data/%d.txt", iteration);

    FILE *output = fopen(filename, "w");

    if (output == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
 
    int numInstOnTopDie = 0;
    int numInstOnBtmDie = 0;
    int numInstances = instances.size();

    fprintf(output, "DieInfo %d %d\n", (int)topDie.upperRightX, (int)topDie.upperRightY);

    fprintf(output, "TopDiePlacement %d\n", numInstOnTopDie);

    for(int inst = 0; inst < numInstances; inst++)
    {
        if( instances[inst].z > 0.5 && instances[inst].layer == 3 )
            fprintf(output, "Inst %d %d %d %d %d\n", 
            instances[inst].instIndex + 1, 
            (int) (instances[inst].x - instances[inst].width), 
            (int) (instances[inst].y - instances[inst].height),
            (int) instances[inst].width,
            (int) instances[inst].height);
        
        else if( instances[inst].layer == topLayer )
            fprintf(output, "Inst %d %d %d %d %d\n", 
            instances[inst].instIndex + 1, 
            (int) instances[inst].finalX, 
            (int) instances[inst].finalY,
            (int) instances[inst].finalWidth,
            (int) instances[inst].finalHeight);
    }

    fprintf(output, "BottomDiePlacement %d\n", numInstOnBtmDie);

    for(int inst = 0; inst < numInstances; inst++)
    {
        if( instances[inst].z < 0.5 && instances[inst].layer == 3 )
            fprintf(output, "Inst %d %d %d %d %d\n", 
            instances[inst].instIndex + 1, 
            (int) (instances[inst].x - instances[inst].inflateWidth), 
            (int) (instances[inst].y - instances[inst].inflateHeight),
            (int) instances[inst].inflateWidth,
            (int) instances[inst].inflateHeight);

        else if( instances[inst].layer == btmLayer) 
            fprintf(output, "Inst %d %d %d %d %d\n", 
            instances[inst].instIndex + 1, 
            (int) instances[inst].finalX, 
            (int) instances[inst].finalY,
            (int) instances[inst].finalWidth,
            (int) instances[inst].finalHeight);
    }

    fclose(output);

}

void writeFile(const vector <instance> instances, const vector <RawNet> rawNet, const int numInstances, const vector <terminal> terminals)
{
    FILE *output;
    
    char filename[30];
    snprintf(filename, sizeof(filename), "out.txt");

    output = fopen(filename, "w");

    int numInstOnTopDie = 0;
    int numInstOnBtmDie = 0;
    int numTerminal = (int) terminals.size();

    for(int inst = 0; inst < numInstances; inst++)
    {
        if( instances[inst].layer == topLayer )
            numInstOnTopDie +=1;
        else
            numInstOnBtmDie +=1;
    }

    fprintf(output, "TopDiePlacement %d\n", numInstOnTopDie);

    for(int inst = 0; inst < numInstances; inst++)
    {
        if( instances[inst].layer == topLayer )
            fprintf(output, "Inst C%d %d %d R%d\n", 
            instances[inst].instIndex + 1, 
            instances[inst].finalX, 
            instances[inst].finalY,
            instances[inst].rotate);
    }

    fprintf(output, "BottomDiePlacement %d\n", numInstOnBtmDie);

    for(int inst = 0; inst < numInstances; inst++)
    {
        if( instances[inst].layer == btmLayer )
            fprintf(output, "Inst C%d %d %d R%d\n", 
            instances[inst].instIndex + 1, 
            instances[inst].finalX, 
            instances[inst].finalY,
            instances[inst].rotate
            );
    }

    fprintf(output, "NumTerminals %d\n", numTerminal);

    for(int t = 0; t < numTerminal; t++)
    {   
        fprintf(output, "Terminal N%d %d %d\n", 
            terminals[t].netID + 1,
            terminals[t].x,
            terminals[t].y
            );
    }
    fclose(output);
}

void macroPlaceAndRotate(vector <instance> &macros, Die topDie, Die btmDie)
{
    // int topDieMacrosPlacement [ (int)topDie.upperRightX ][ (int)topDie.upperRightY ] = {0};
    // int btmDieMacrosPlacement [ (int)topDie.upperRightX ][ (int)topDie.upperRightY ] = {0};
    int numMacro = macros.size();

    vector <int> topDieMacro;
    vector <int> btmDieMacro;
    
    /*  place standard cell to best row */

    for(int inst = 0; inst < numMacro; inst++)
    {
        macros[inst].finalX = (int) macros[inst].x - macros[inst].width/2;
        macros[inst].finalY = (int) macros[inst].y - macros[inst].height/2;
        macros[inst].finalWidth = (int) macros[inst].width;
        macros[inst].finalHeight = (int) macros[inst].height;
        macros[inst].rotate = 0;

        if(macros[inst].layer)
            topDieMacro.push_back(inst);
        else
            btmDieMacro.push_back(inst);
    }

    int topX = 0;
    int topY = 0;
    int maxTopX = 0;
    int btmX = 0;
    int btmY = 0;
    int maxBtmX = 0;

    for(int inst = 0; inst < 3; inst++)
    {
        if(topY + macros[ topDieMacro[inst] ].finalHeight > topDie.upperRightY)
        {
            topY = 0;
            topX = maxTopX;
        }

        if(btmY + macros[ btmDieMacro[inst] ].finalHeight > topDie.upperRightY)
        {
            btmY = 0;
            btmX = maxBtmX;
        }

        if(topX + macros[ topDieMacro[inst] ].finalWidth > topDie.upperRightX)
        {
            macros[inst].rotate = 90;
            macros[inst].finalWidth = (int) macros[inst].width;
            macros[inst].finalHeight = (int) macros[inst].height;
        }

        if(btmX + macros[ btmDieMacro[inst] ].finalWidth > topDie.upperRightX)
        {
            macros[inst].rotate = 90;
            macros[inst].finalWidth = (int) macros[inst].width;
            macros[inst].finalHeight = (int) macros[inst].height;
        }
        
        macros[ topDieMacro[inst] ].finalY = topY;
        macros[ btmDieMacro[inst] ].finalY = btmY;

        macros[ topDieMacro[inst] ].finalX = topX;
        macros[ btmDieMacro[inst] ].finalX = btmX;
        
        topY += macros[ topDieMacro[inst] ].finalHeight;
        btmY += macros[ btmDieMacro[inst] ].finalHeight;

        
        if(macros[ topDieMacro[inst] ].finalWidth > maxTopX)
            maxTopX = macros[ topDieMacro[inst] ].finalWidth;
        
        if(macros[ btmDieMacro[inst] ].finalWidth > maxBtmX)
            maxBtmX = macros[ btmDieMacro[inst] ].finalWidth;

    }
}