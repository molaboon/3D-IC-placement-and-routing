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
#include "partition.h"

using namespace std;

#define btmLayer 0
#define topLayer 1
#define theCellDoesntPlaceToLayer 3

void cell2BestLayer( vector <instance> &instances, const Die topDie, const Die btmDie, vector <RawNet> &rawNets, Hybrid_terminal hbtTech)
{
    float topDieArea = 0.0;
    float btmDieArea = 0.0;
    float topDieUtl = 0.0;
    float btmDieUtl = 0.0;
    float topDieMaxUtl = 0.0;
    float btmDieMaxUtl = 0.0;
    float maxArea = (topDie.upperRightX * topDie.upperRightY);

    int numInstances = instances.size();

    for(int i = 0; i < numInstances; i++)
    {
        if( (instances[i].z < 5000 && instances[i].layer == theCellDoesntPlaceToLayer) || instances[i].layer == 0)
        {
            instances[i].layer = btmLayer;
            
            if(instances[i].rotate == 90 || instances[i].rotate == 270)
            {
                if(!instances[i].canPass)
                {
                    instances[i].finalWidth = (int) instances[i].inflateHeight;
                    instances[i].finalHeight = (int) instances[i].inflateWidth;
                }    
            }
            else
            {
                instances[i].finalWidth = (int) instances[i].inflateWidth;
                instances[i].finalHeight = (int) instances[i].inflateHeight;
            }

            btmDieArea += instances[i].inflateArea;
        }
        else if( (instances[i].z > 5000 && instances[i].layer == theCellDoesntPlaceToLayer) || instances[i].layer == 1)
        {
            instances[i].layer = topLayer;
        
            if(instances[i].rotate == 90 || instances[i].rotate == 270)
            {
                if(!instances[i].canPass)
                {
                    instances[i].finalWidth = (int) instances[i].height;
                    instances[i].finalHeight = (int) instances[i].width;
                }
            }
            else
            {
                instances[i].finalWidth = (int) instances[i].width;
                instances[i].finalHeight = (int) instances[i].height;
            }
            
            topDieArea += instances[i].area;
        }
    }

// if #terminal > maxNumTerminal 
    
    int numTerminal = 0;
    int numMaxhbt = 0;

    numMaxhbt = ((int) topDie.upperRightX - hbtTech.spacing) / (hbtTech.sizeX + hbtTech.spacing) ;
    numMaxhbt = numMaxhbt * ((int) topDie.upperRightY - hbtTech.spacing) / (hbtTech.sizeY + hbtTech.spacing);

    for(int net = 0; net < rawNets.size(); net++)
    {
        int firstInstLater = rawNets[net].Connection[0]->layer;
        for(int inst = 1 ; inst < rawNets[net].numPins; inst++)
        {
            if( rawNets[net].Connection[inst]->layer != firstInstLater )
            {
                numTerminal++;
                break;
            }
        }
    }

    topDieUtl = topDieArea / maxArea;
    btmDieUtl = btmDieArea / maxArea;

    topDieMaxUtl = (float) topDie.MaxUtil / 100.0;
    btmDieMaxUtl = (float) btmDie.MaxUtil / 100.0;

    printf("%d, %f, %f\n", numTerminal, topDieUtl, btmDieUtl);

    // if(numTerminal > numMaxhbt)
    // {
        
    // }

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
                        float tmpTopUtl = (topDieArea - instances[inst].area + instances[j].area) / maxArea;
                        float tmpBtmUtl = (btmDieArea - instances[j].inflateArea + instances[inst].inflateArea) / maxArea;

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

    vector < vector<int> > topDieMacros(topDie.repeatCount);
    vector < vector<int> > btmDieMacros(btmDie.repeatCount);

    int topDieCellsWidth[topDie.repeatCount] = {0};
    int btmDieCellsWidth[btmDie.repeatCount] = {0};
    int numMacro = macros.size();

    for(int row = 0; row < topDie.repeatCount; row++)
    {
        topDiePlacementState[row].reserve(numInstances/5);
        topDieMacros[row].reserve(numMacro);
    }
    
    for(int row = 0; row < btmDie.repeatCount; row++)
    {
        btmDiePlacementState[row].reserve(numInstances/5);
        btmDieMacros[row].reserve(numMacro);
    }
    
    /*  place standard cell to best row */

    for(int inst = 0; inst < numInstances; inst++)
    {
        int row = 0;
        float upOrdown = instances[inst].y ;
 
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
                    topDieMacros[row].push_back(instances[inst].instIndex);
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
                    btmDieMacros[row].push_back(instances[inst].instIndex);
                }
            }
        }
        else
        {
            if( instances[inst].layer == topLayer)
            {
                upOrdown = fmod( upOrdown, (float) topDie.rowHeight);
                row = ( (int) instances[inst].y) / topDie.rowHeight ;

                if( upOrdown > (float) topDie.rowHeight / 2.0 )
                    row += 1;
                
                if( row >= topDie.repeatCount)
                    row = topDie.repeatCount - 1;
                    
                topDiePlacementState[row].push_back(instances[inst].instIndex);
                instances[inst].finalY = topDie.rowHeight * row;
                topDieCellsWidth[row] += instances[inst].finalWidth;
                
            }
            else
            {
                upOrdown = fmod( upOrdown, (float) btmDie.rowHeight);
                row = ((int) instances[inst].y )  / btmDie.rowHeight ;

                if( upOrdown > (float) btmDie.rowHeight / 2.0 )
                    row += 1;
                
                if( row >= btmDie.repeatCount)
                    row = btmDie.repeatCount - 1;
                
                btmDiePlacementState[row].push_back(instances[inst].instIndex);
                instances[inst].finalY = btmDie.rowHeight * row;
                btmDieCellsWidth[row] += instances[inst].finalWidth;
            }
        }
    }
    

    /* check which row is stuffed. If stuffed, place to near row*/

    place2nearRow(topDie, btmDie, topDiePlacementState, btmDiePlacementState, instances, topDieCellsWidth, btmDieCellsWidth);
    place2nearRow(btmDie, topDie, btmDiePlacementState, topDiePlacementState, instances, btmDieCellsWidth, topDieCellsWidth);

    /*  place instances to best X position*/

    placeInst2BestX(topDie, topDiePlacementState, topDieMacros, instances, topDieCellsWidth);
    placeInst2BestX(btmDie, btmDiePlacementState, btmDieMacros, instances, btmDieCellsWidth);
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

            needRestart:

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

                            if (startX + nowCellWidth >= upperRightX )
                                goto needRestart;
                        }

                    } while (startX + nowCellWidth > macroX);
                    
                    instances[cellID].finalX = startX;
                    startX += nowCellWidth;
                }
            }

            nowRowCellWidth -= nowCellWidth;            
        }

        // 將 sort 好的 cell 放入diePlacementState 中
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

void insertTerminal(const vector <instance> &instances, const vector <RawNet> &rawNet, vector <terminal> &terminals, const Hybrid_terminal terminalTech, Die topDie)
{ 
    const int numNet = rawNet.size();
    int numTerminal = 0;
    const int dieWidth = (int) topDie.upperRightX;
    const int dieHight = (int) topDie.upperRightY;

    const unsigned int sizeX = terminalTech.sizeX;
    const unsigned int sizeY = terminalTech.sizeY;
    const unsigned int space = terminalTech.spacing;
    const unsigned int spaceX = space + sizeX;
    const unsigned int spaceY = space + sizeY;;
    const unsigned int numTerOfX = (dieWidth - space) / spaceX - 1;
    const unsigned int numTerOfY = (dieHight - space) / spaceY - 1;
    char terminalArray[numTerOfY][numTerOfX] = {0};

    terminals.resize(rawNet.size());

    // calculate the number of the terminal in rawnet
    for(int net = 0; net < numNet; net++)
    {
        int firstInstLater = rawNet[net].Connection[0]->layer;
        for(int inst = 1 ; inst < rawNet[net].numPins; inst++)
        {
            if( rawNet[net].Connection[inst]->layer != firstInstLater )
            {
                terminal newTerminal;
                newTerminal.netID = net;
                newTerminal.x = 0;
                newTerminal.y = 0;
                terminals[numTerminal] = newTerminal;
                numTerminal++;
                break;
            }
        }
    }

    terminals.resize(numTerminal);

    // place the terminal to the position
    for (int t = 0; t < numTerminal; t++)
    {
        int minX = 99999, minY= 99999, maxX = 0, maxY = 0;
        int numPins = rawNet[ terminals[t].netID ].numPins;

        for(int i = 0; i < numPins; i++ )
        {
            if( rawNet[terminals[t].netID].Connection[i]->finalX < minX )
                minX = rawNet[terminals[t].netID].Connection[i]->finalX;

            if( rawNet[terminals[t].netID].Connection[i]->finalX > maxX )
                maxX = rawNet[terminals[t].netID].Connection[i]->finalX;

            if( rawNet[terminals[t].netID].Connection[i]->finalY < minY )
                minY = rawNet[terminals[t].netID].Connection[i]->finalY;

            if( rawNet[terminals[t].netID].Connection[i]->finalY > maxY )
                maxY = rawNet[terminals[t].netID].Connection[i]->finalY;
        }
        
        terminals[t].x = ((maxX - minX)/2 + minX ) - ((maxX - minX)/2 + minX ) % spaceX + space + sizeX/2 ;
        terminals[t].y = ((maxY - minY)/2 + minY ) - ((maxY - minY)/2 + minY ) % spaceY + space + sizeY/2;
        
        if (terminals[t].x > dieWidth - spaceX - sizeX/2)
            terminals[t].x = dieWidth - spaceX - sizeX/2;
        
        if (terminals[t].y > dieHight - spaceY - sizeY/2)
            terminals[t].y = dieHight - spaceY - sizeY/2;

        if(terminalArray[(terminals[t].y - space - sizeY/2) / spaceY][(terminals[t].x - space - sizeX/2) / spaceX] == 0)
        {
            terminalArray[(terminals[t].y - space - sizeY/2) / spaceY][(terminals[t].x - space - sizeX/2) / spaceX] = 1;
        }
        else
        {
            bool found = false;
            minY = minY / spaceY ;
            maxY = maxY / spaceY ;
            minX = minX / spaceX ;
            maxX = maxX / spaceX ;
            
            if(minX < 1)
                minX = 0;
            if(minY < 1)
                minY = 0;
            
            if(maxX <= minX)
                maxX = minX + 1;
            if(maxY <= minY) 
                maxY = minY + 1;

            do
            {
                for (int yy = minY; yy < maxY; yy++)
                {
                    for(int xx = minX; xx < maxX; xx++)
                    {
                        if(terminalArray[yy][xx] != 1)
                        {
                            terminals[t].x = xx * (spaceX) + space + sizeX/2;
                            terminals[t].y = yy * (spaceY) + space + sizeY/2;
                            terminalArray[yy][xx] = 1;
                            goto nextRound;
                        }
                    }
                }

                minY = minY >> 1;
                minX = minX >> 1;
                maxX = maxX << 1;
                maxY = maxY << 1;
                
                if(maxX >= numTerOfX)
                    maxX = numTerOfX;
                
                if(maxY >= numTerOfY)
                    maxY = numTerOfY;
                
                if(minX < 1)
                    minX = 0;
                if(minY < 1)
                    minY = 0;

            } while (!found);
            
            nextRound:
            continue;

        }
    }
}

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
        if( instances[inst].z >= 5000 && instances[inst].layer == 3 )
            fprintf(output, "Inst %d %d %d %d %d\n", 
            instances[inst].instIndex + 1, 
            (int) (instances[inst].x - instances[inst].width/2), 
            (int) (instances[inst].y - instances[inst].height/2),
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
        if( instances[inst].z < 5000 && instances[inst].layer == 3 )
            fprintf(output, "Inst %d %d %d %d %d\n", 
            instances[inst].instIndex + 1, 
            (int) (instances[inst].x - instances[inst].inflateWidth/2), 
            (int) (instances[inst].y - instances[inst].inflateHeight/2),
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

void writeFile(const vector <instance> &instances, const vector <RawNet> &rawNet, const int numInstances, const vector <terminal> &terminals)
{
    FILE *output;
    
    char filename[30];
    snprintf(filename, sizeof(filename), "output.txt");

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

void macroPlace(vector <instance> &macros, Die topDie, Die btmDie)
{
    // int topDieMacros [ (int)topDie.upperRightX ][ (int)topDie.upperRightY ] = {0};
    // int btmDieMacros [ (int)topDie.upperRightX ][ (int)topDie.upperRightY ] = {0};
    int numMacro = macros.size();

    int topDieMacro[3] = {0, 1, 4};
    int btmDieMacro[3] = {2, 3, 5};
    
    /*  place standard cell to best row */

    macros[ topDieMacro[2] ].layer = topLayer;
    macros[ btmDieMacro[2] ].layer = btmLayer;

    macros[ topDieMacro[1] ].layer = topLayer;
    macros[ btmDieMacro[1] ].layer = btmLayer;

    macros[ topDieMacro[0]].layer = topLayer;
    macros[ btmDieMacro[0]].layer = btmLayer;
    
    macros[ topDieMacro[0] ].rotate = 180;
    macros[ btmDieMacro[0] ].rotate = 90;
    
    macros[ topDieMacro[1] ].rotate = 180;
    macros[ btmDieMacro[1] ].rotate = 180;

    macros[topDieMacro[2]].rotate = 270;
    macros[btmDieMacro[2]].rotate = 0;

    for(int i = 0; i < macros.size(); i++)
    {
        if(macros[i].rotate == 90 || macros[i].rotate == 270)
        {
            float tmp = macros[i].width;
            macros[i].width = macros[i].height;
            macros[i].height = tmp;
            macros[i].finalWidth = (int) macros[i].width;
            macros[i].finalHeight = (int) macros[i].height;
            
        }
        else
        {
            macros[i].finalWidth = (int) macros[i].width;
            macros[i].finalHeight = (int) macros[i].height;
        }   
    }

    macros[ topDieMacro[0] ].finalX = 100;
    macros[ btmDieMacro[0] ].finalX = 100;

    macros[ topDieMacro[0] ].finalY = 100;
    macros[ btmDieMacro[0] ].finalY = 1000;

    
    macros[ topDieMacro[1] ].finalX = 100;
    macros[ btmDieMacro[1] ].finalX = btmDie.upperRightX - macros[btmDieMacro[1]].finalWidth - 100;

    macros[ topDieMacro[1] ].finalY = topDie.upperRightY - macros[ topDieMacro[1] ].finalHeight - 100;
    macros[ btmDieMacro[1] ].finalY = btmDie.upperRightY - macros[ btmDieMacro[1] ].finalHeight - 100;
    
    macros[ topDieMacro[2] ].finalX = topDie.upperRightX - macros[ topDieMacro[2] ].finalWidth - 100;
    macros[ btmDieMacro[2] ].finalX = topDie.upperRightX - macros[ btmDieMacro[2] ].finalWidth - 100;

    macros[ topDieMacro[2] ].finalY = 100;
    macros[ btmDieMacro[2] ].finalY = 100;

}

void macroRotate(vector <instance> &macros, vector < vector<instance> > &pinsInMacros, vector <RawNet> &rawnets, vector <instance> stdCells)
{

    // calculate the grade of the corners in each macro
    // according to the grade, we rotate the heighest grade of corner of macro to the center 

    int numOfmacros = macros.size();
    int numCells = stdCells.size();
    map <int, int> indexMap; 

    // int gradeOfCorners[numOfmacros][numOfmacros][4] = {0};

    // for(int i = 0; i < numCells; i++)
    // {
    //     for(int j = 0; j < stdCells[i].numNetConnection; j ++)
    //     {
    //         vector <int> macro;
    //         int netID = stdCells[i].connectedNet[j];
            
    //         for(int k = 0; k < rawnets[netID].numPins; k++)
    //         {                    
    //         }
    //     }

    // }

    for(int i = 0; i < numOfmacros; i++)
    {
        int gradeOfCorners[4] = {0};
        int numOfPinsInMacros = pinsInMacros[i].size();
        int nowMacroIndex = macros[i].instIndex;

        for(int j = 0; j < numOfPinsInMacros; j++)
        {
            int nowPinx = pinsInMacros[i][j].x;
            int nowPiny = pinsInMacros[i][j].y;
            int lineX = (macros[i].finalX + macros[i].finalWidth) / 2;
            int lineY = (macros[i].finalY + macros[i].finalHeight) / 2;
            int gradeIndex = 0;

            if(nowPinx < lineX && nowPiny < lineY)
                gradeIndex = 0;
            else if(nowPinx > lineX && nowPiny < lineY)
                gradeIndex = 1;
            else if(nowPinx < lineX && nowPiny > lineY)
                gradeIndex = 2;
            else
                gradeIndex = 3;

            for(int k = 0; k < pinsInMacros[i][j].numNetConnection; k++)
            {
                int netIndex = pinsInMacros[i][j].connectedNet[k];
                
                for(int l = 0; l < rawnets[netIndex].numPins; l++)
                {
                    int instIndex = rawnets[netIndex].Connection[l]->instIndex;
                    int numNets = stdCells[instIndex].numNetConnection;
                    
                    for(int m = 0; m < numNets; m++)
                    {
                        for(int n = 0; n < rawnets[ stdCells[instIndex].connectedNet[m] ].numPins; n++)
                        {
                            int instIndex2 = rawnets[ stdCells[instIndex].connectedNet[m] ].Connection[n]->instIndex;

                            if(stdCells[instIndex2].isMacro && stdCells[instIndex2].instIndex != nowMacroIndex)
                            {
                                // int otherPinx = rawnets[ stdCells[instIndex].connectedNet[m] ].Connection[n]->finalX;
                                // int otherPiny = rawnets[ stdCells[instIndex].connectedNet[m] ].Connection[n]->finalY;

                                gradeOfCorners[gradeIndex] += 1 ; 
                            }
                        }
                    }
                }
            }
        }
        printf("a");
    }
}

void writeData(const float hpwl, const float hbt, const float penalty)
{
    FILE *hpwlOutput;
    // FILE *hbtOutput;
    // FILE *penaltyOutput;
    
    char filename[30];
    // char filename1[30];
    // char filename2[30];

    snprintf(filename, sizeof(filename), "hpwl.txt");
    // snprintf(filename1, sizeof(filename), "HBT.txt");
    // snprintf(filename2, sizeof(filename), "Penalty.txt");

    hpwlOutput = fopen(filename, "a");
    // hbtOutput = fopen(filename1, "a");
    // penaltyOutput = fopen(filename2, "a");

    fprintf(hpwlOutput, "%f\n", hpwl);
    // fprintf(hbtOutput, "%f\n", hbt);
    // fprintf(penaltyOutput, "%f\n", penalty);

    fclose(hpwlOutput);
    // fclose(hbtOutput);
    // fclose(penaltyOutput);
}

void wirteNodes(vector <instance> &instances, vector <instance> &macros)
{
    FILE *output;
    FILE *output2;

    
    char filename[30];
    char filename2[30];

    int numInstances = 0;
    int numInst2 = 0;
    int numMacros = 3;
    int qqq = 0;
    int jjj = 0;

    for(int i = 0; i < instances.size(); i++)   
    {
        if(instances[i].layer == 1)
            numInstances ++;
        else
            numInst2 ++;
    }
        
    snprintf(filename, sizeof(filename), "./test/test.nodes");
    snprintf(filename2, sizeof(filename2), "./test/test2.nodes");

    output = fopen(filename, "w");
    output2 = fopen(filename2, "w");

    fprintf(output, "UCLA nodes 1.0\n# Created : \n# User    :   Modified MMS Benchmark\n\n");
    fprintf(output, "NumNodes :   %d\n", numInstances);
    fprintf(output, "NumTerminals :   %d\n\n", numMacros);

    fprintf(output2, "UCLA nodes 1.0\n# Created : \n# User    :   Modified MMS Benchmark\n\n");
    fprintf(output2, "NumNodes :   %d\n", numInst2);
    fprintf(output2, "NumTerminals :   %d\n\n", numMacros);

    for(int i = 0; i < instances.size(); i++)
    {
        if(instances[i].layer == 1 && !instances[i].isMacro)
        {
            fprintf(output, "o%d %d %d\n", qqq, instances[i].finalWidth, instances[i].finalHeight);
            qqq++;
        }
        else if(instances[i].layer == 0 && !instances[i].isMacro)
        {
            fprintf(output2, "o%d %d %d\n", jjj, instances[i].finalWidth, instances[i].finalHeight);
            jjj++;   
        }
    }

    fclose(output);
    fclose(output2);
}

void wirtePl(vector <instance> &instances, vector <instance> &macros)
{
    FILE *output;
    FILE *output2;
    
    char filename[30];
    char filename2[30];

    snprintf(filename, sizeof(filename), "./test/test.ntup.pl");
    output = fopen(filename, "w");

    snprintf(filename2, sizeof(filename2), "./test/test2.ntup.pl");
    output2 = fopen(filename2, "w");

    int numInstances = 0;
    int numInst2 = 0;
    
    fprintf(output, "UCLA pl 1.0\n\n");
    fprintf(output2, "UCLA pl 1.0\n\n");

    for(int i = 0; i < instances.size(); i++)
    {
        if(instances[i].layer == 1 && !instances[i].isMacro)
        {
            fprintf(output, "o%d %d %d : N\n", numInstances, (int) instances[i].x, (int) instances[i].y);
            numInstances++;
        }
        else if(instances[i].layer == 0 && !instances[i].isMacro)
        {
            fprintf(output2, "o%d %d %d : N\n", numInst2, (int) instances[i].x, (int) instances[i].y);
            numInst2++;
        }
    }

    fclose(output);
    fclose(output2);
}

void writeRow(vector <instance> &macros, Die topDie, Die btmDie)
{
    vector < vector<int> > topDieMacros(topDie.repeatCount);
    vector < vector<int> > btmDieMacros(btmDie.repeatCount);
    const int numRow = topDie.repeatCount;
    const int height = topDie.rowHeight;
    const int diewidth = (int) topDie.upperRightX;
    int totalRow = 0;

    int numMacro = macros.size();

    for(int row = 0; row < numRow; row++)
        topDieMacros[row].reserve(numMacro);
    for(int row = 0; row< btmDie.repeatCount; row++)
        btmDieMacros[row].reserve(numMacro);
    
    for(int inst = 0; inst < numMacro; inst++)
    {
        int row = 0;
        int uperY = macros[inst].finalY + (macros[inst].finalHeight) ;
        int lowerY = macros[inst].finalY;
        int uperRow, lowerRow;
        macros[inst].finalY = lowerY;  
        
        if( macros[inst].layer == topLayer)
        {
            uperRow = (uperY / topDie.rowHeight) ;
            lowerRow = (lowerY / topDie.rowHeight);
            
            if(uperRow >= topDie.repeatCount)
                uperRow = topDie.repeatCount - 1;

            for(int row = lowerRow; row <= uperRow; row++)
                topDieMacros[row].push_back(inst);
        }   
        else
        {
            uperRow = (uperY / btmDie.rowHeight);
            lowerRow = lowerY / btmDie.rowHeight;
            
            if(uperRow >= btmDie.repeatCount)
                uperRow = btmDie.repeatCount-1;

            for(int row = lowerRow; row <= uperRow; row++)
                btmDieMacros[row].push_back(inst);
        }
    }
    
    FILE *output;
    char filename[30];
    snprintf(filename, sizeof(filename), "./test/test.scl");
    output = fopen(filename, "w");

    fprintf(output, "UCLA scl 1.0 \n# Created	:	2005 \n# User   	:	Gi-Joon\n\n");

    for(int i = 0; i < numRow; i++)
    {
        vector <int> subrow;

        for(int j = 0; j < topDieMacros[i].size(); j++)
        {
            int x =  macros[topDieMacros[i].at(j)].finalX;
            int w =  macros[topDieMacros[i].at(j)].finalWidth;
            subrow.push_back( x );
            subrow.push_back( x+w ) ;
        }

        sort(subrow.begin(), subrow.end());
        totalRow += topDieMacros[i].size() + 1;
        
        if(subrow.size() > 0)
        {
            if(subrow[0] == 0)
                totalRow--;
            if(subrow.back() == (int) topDie.upperRightX)
                totalRow--;
        }
        
    }
    
    fprintf(output, "NumRows : %d\n", totalRow);

    for(int i = 0; i < numRow; i++)
    {
        vector <int> subrow;
        
        subrow.push_back(0);

        for(int j = 0; j < topDieMacros[i].size(); j++)
        {
            int x =  macros[topDieMacros[i].at(j)].finalX;
            int w =  macros[topDieMacros[i].at(j)].finalWidth;
            subrow.push_back( x );
            subrow.push_back( x+w ) ;
        }
        subrow.push_back(diewidth);

        sort(subrow.begin(), subrow.end());
        
        for(int k = 0; k < topDieMacros[i].size() + 1; k++)
        {
            fprintf(output, "CoreRow Horizontal\n");
            fprintf(output, " Coordinate    :   %d\n", i * height);
            fprintf(output, " Height        :   %d\n", height);
            fprintf(output, " Sitewidth     :    1\n");
            fprintf(output, " Sitespacing   :    1\n");
            fprintf(output, " Siteorient    :    1\n");
            fprintf(output, " Sitesymmetry  :    1\n");
            fprintf(output, " SubrowOrigin  :    %d	NumSites  :  %d\n", subrow[2*k], subrow[2*k+1] - subrow[2*k]);
            fprintf(output, "End\n");
        }
    }

    fclose(output);

    // btm die
    FILE *output2;
    char filename2[30];
    snprintf(filename2, sizeof(filename2), "./test/test2.scl");
    output2 = fopen(filename2, "w");

    fprintf(output2, "UCLA scl 1.0 \n# Created	:	2005 \n# User   	:	Gi-Joon\n\n");

    for(int i = 0; i < numRow; i++)
    {
        vector <int> subrow;

        for(int j = 0; j < btmDieMacros[i].size(); j++)
        {
            int x =  macros[btmDieMacros[i].at(j)].finalX;
            int w =  macros[btmDieMacros[i].at(j)].finalWidth;
            subrow.push_back( x );
            subrow.push_back( x+w ) ;
        }

        sort(subrow.begin(), subrow.end());
        totalRow += btmDieMacros[i].size() + 1;
        
        if(subrow.size() < 1)
            continue;
        if(subrow[0] == 0)
            totalRow--;
        if(subrow.back() == (int) topDie.upperRightX)
            totalRow--;
    }
    
    fprintf(output2, "NumRows : %d\n", totalRow);

    for(int i = 0; i < numRow; i++)
    {
        vector <int> subrow;
        
        subrow.push_back(0);

        for(int j = 0; j < btmDieMacros[i].size(); j++)
        {
            int x =  macros[btmDieMacros[i].at(j)].finalX;
            int w =  macros[btmDieMacros[i].at(j)].finalWidth;
            subrow.push_back( x );
            subrow.push_back( x+w ) ;
        }
        subrow.push_back(diewidth);

        sort(subrow.begin(), subrow.end());
        
        for(int k = 0; k < btmDieMacros[i].size() + 1; k++)
        {
            fprintf(output2, "CoreRow Horizontal\n");
            fprintf(output2, " Coordinate    :   %d\n", i * btmDie.rowHeight);
            fprintf(output2, " Height        :   %d\n", btmDie.rowHeight);
            fprintf(output2, " Sitewidth     :    1\n");
            fprintf(output2, " Sitespacing   :    1\n");
            fprintf(output2, " Siteorient    :    1\n");
            fprintf(output2, " Sitesymmetry  :    1\n");
            fprintf(output2, " SubrowOrigin  :    %d	NumSites  :  %d\n", subrow[2*k], subrow[2*k+1] - subrow[2*k]);
            fprintf(output2, "End\n");
        }
    }

    fclose(output2);
}

void writeMacroNest(vector <instance> &macros, vector < vector<instance> > &pinsInMacros, vector <RawNet> &rawnets, vector <instance> stdCells)
{
    int numOfmacros = macros.size();
    int numCells = stdCells.size();

    FILE *output;
    
    char filename[30];
    
    snprintf(filename, sizeof(filename), "./case3.nets");
    output = fopen(filename, "w");

    int numInstances = 0;
    int numInst2 = 0;
    
    fprintf(output, "Numnet\n\n");

    for(int i = 0; i < numOfmacros; i++)
    {
        int numOfPinsInMacros = pinsInMacros[i].size();
        int nowMacroIndex = macros[i].instIndex;

        for(int j = 0; j < numOfPinsInMacros; j++)
        {
            vector<int> mm;
            
            for(int k = 0; k < pinsInMacros[i][j].numNetConnection; k++)
            {
                int netIndex = pinsInMacros[i][j].connectedNet[k];
                mm.push_back(nowMacroIndex);
                
                for(int l = 0; l < rawnets[netIndex].numPins; l++)
                {
                    int instIndex = rawnets[netIndex].Connection[l]->instIndex;
                    int numNets = stdCells[instIndex].numNetConnection;
                    
                    for(int m = 0; m < numNets; m++)
                    {
                        for(int n = 0; n < rawnets[ stdCells[instIndex].connectedNet[m] ].numPins; n++)
                        {
                            int instIndex2 = rawnets[ stdCells[instIndex].connectedNet[m] ].Connection[n]->instIndex;

                            if(stdCells[instIndex2].isMacro && stdCells[instIndex2].instIndex != nowMacroIndex)
                            {
                                
                            }
                        }
                    }
                }
            }
        }
    }
}

void macroPlacement(vector <instance> &macros, vector<RawNet> &rawnets, Die topDie)
{
    bool a =  cooradinate(macros, topDie, rawnets);
}

bool cooradinate(vector <instance> &macros, Die topDie, vector <RawNet> &rawnets)
{
    int topY = topDie.upperRightY;
    int btmY = 0;
    int leftX = 0;
    int rightX = topDie.upperRightX;
    int direction = 0;  // 0 = left, 1 = up, 2 = right, 3 = down
    
    int list[9] = {8, 7, 4, 5, 6, 3, 0 ,1 ,2};
    int tmp[4] = {btmY, rightX, topY, leftX};
    
    bool changeTwice = false;

    macros[list[0]].finalX = 0;
    macros[list[0]].finalY = 0;
    macros[list[0]].layer = 1;
    updateRotate(macros[list[0]]);
        
    for(int i = 1; i < macros.size(); i++)
    {
        macros[list[i]].layer = 1;
        updateRotate(macros[list[i]]);
        int _w = macros[list[i]].finalWidth;
        int _h = macros[list[i]].finalHeight;
        bool chang = false;
        
        direction = direction % 4;

        switch (direction) {
            case 0: // 向右
                macros[list[i]].finalX = macros[list[i-1]].finalX + macros[list[i-1]].finalWidth;
                macros[list[i]].finalY = btmY;

                if(btmY + _h >= tmp[direction])
                    tmp[direction] = btmY + _h;

                if(macros[list[i]].finalX + _w > rightX) 
                    chang = true;

                break;
            case 1: // 向上
                macros[list[i]].finalX = rightX - _w;
                macros[list[i]].finalY = macros[list[i-1]].finalY + macros[list[i-1]].finalHeight;

                if( rightX - _w <= tmp[direction])
                    tmp[direction] = rightX - _w;

                if( macros[list[i]].finalY + macros[list[i]].finalHeight > topY)
                    chang = true;

                break;
            case 2: // 向左
                macros[list[i]].finalX = macros[list[i-1]].finalX - _w;
                macros[list[i]].finalY = topY - _h;

                if( topY - _h <= tmp[direction])
                    tmp[direction] = topY - _h;
                
                if( macros[list[i]].finalX < leftX)
                    chang = true;
                break;
            case 3: // 向下
                macros[list[i]].finalX = leftX ;
                macros[list[i]].finalY = macros[list[i-1]].finalY - _h;

                if(leftX + macros[list[i]].finalWidth > tmp[direction] )
                    tmp[direction] = leftX + macros[list[i]].finalWidth;
                
                if(macros[list[i]].finalY < btmY)
                    chang = true;
                break;

        }

        if(changeTwice && chang)
            return false;

        if(chang)
        {
            changeTwice = true;
            i--;
            direction ++;
            btmY = tmp[0];
            rightX = tmp[1];
            topY = tmp[2];
            leftX = tmp[3];
        }
        else
            changeTwice = false;
    }

    return true;
}

int actualHPWL(vector <RawNet> &rawnets)
{
    int numnet = rawnets.size();
    float sum = 0;

    for(int i = 0; i < numnet; i++)
    {
        float maxx = 0;
        float maxy = 0;
        float minx = 999999;
        float miny = 999999;

        for(int j = 0; j < rawnets[i].numPins; j++)
        {
            if(rawnets[i].Connection[j]->x < minx)
                minx = rawnets[i].Connection[j]->x;
            
            if(rawnets[i].Connection[j]->x > maxx)
                maxx = rawnets[i].Connection[j]->x;

            
            if(rawnets[i].Connection[j]->y < miny)
                miny = rawnets[i].Connection[j]->y;
            
            if(rawnets[i].Connection[j]->y > maxy)
                maxy = rawnets[i].Connection[j]->y;
        }

        sum += (maxx - minx) + (maxy - miny);

    }

    return (int) sum;
}