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
            instances[i].finalWidth = (int) instances[i].inflateWidth;
            instances[i].finalHeight = (int) instances[i].inflateHeight;
            btmDieArea += instances[i].inflateArea;
        }
        else
        {
            instances[i].layer = topLayer;
            instances[i].finalWidth = (int) instances[i].width;
            instances[i].finalHeight = (int) instances[i].height;
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
                    if ( overLayer == btmLayer)
                    {
                        overDieArea -= instances[inst].inflateArea;
                        instances[inst].finalWidth = (int) instances[inst].width;
                        instances[inst].finalHeight = (int) instances[inst].height;
                    }
                    else
                    {
                        overDieArea -= instances[inst].area;
                        instances[inst].finalWidth = (int) instances[inst].inflateWidth;
                        instances[inst].finalHeight = (int) instances[inst].inflateHeight;
                    }
                    
                    overLayerUtl = overDieArea / overDieTotalArea;

                    if(overLayerUtl < overLayerMaxUtl)
                        break;
                }
            }

            minNumNetConnet += 1;

        }while (overLayerUtl > overLayerMaxUtl);
    }

}

void place2BestRow( vector <instance> &instances, const int numInstances, Die topDie, Die btmDie, vector <instance> &macros)
{
    vector < vector<int> > topDiePlacementState( topDie.repeatCount );
    vector < vector<int> > btmDiePlacementState( btmDie.repeatCount );

    vector < vector<int> > topDieMacrosPlacement(topDie.repeatCount);
    vector < vector<int> > BtmDieMacrosPlacement(btmDie.repeatCount);

    int topDieCellsWidth[topDie.repeatCount] = {0};
    int btmDieCellsWidth[btmDie.repeatCount] = {0};
    int numMacro = macros.size();

    
    /*  place standard cell to best row */

    for(int inst = 0; inst < numInstances; inst++)
    {
        int row = 0;
        double upOrdown = instances[inst].y ;
 
        if(instances[inst].isMacro )
        {
            int uperX = (int) instances[inst].x + (instances[inst].finalWidth) ;
            int uperY = (int) instances[inst].y + (instances[inst].finalHeight) ;
            int lowerX = (int) instances[inst].x;
            int lowerY = (int) instances[inst].y;
            int uperRow, lowerRow;
            
            if( instances[inst].layer == topLayer)
            {
                uperRow = (uperY / topDie.rowHeight) ;
                lowerRow = (lowerY / topDie.rowHeight);
                
                if(uperRow >= topDie.repeatCount)
                    uperRow = topDie.repeatCount - 1;

                for(int row = lowerRow; row <= uperRow; row++)
                {
                    topDieCellsWidth[row] += (int) instances[inst].finalWidth;
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
                    BtmDieMacrosPlacement[row].push_back(instances[inst].instIndex);
                }
            }
        }
        else
        {
            if( instances[inst].layer == topLayer)
            {
                upOrdown = fmod( upOrdown, (double) topDie.rowHeight);
                row = ( (int) instances[inst].y - (instances[inst].finalHeight/2) ) / topDie.rowHeight ;

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
                row = ((int) instances[inst].y - (instances[inst].finalHeight/2) ) / btmDie.rowHeight ;

                if( upOrdown > (double) btmDie.rowHeight / 2.0 )
                    row += 1;

                btmDiePlacementState[row].push_back(instances[inst].instIndex);
                instances[inst].finalY = btmDie.rowHeight * row;
                btmDieCellsWidth[row] += instances[inst].finalWidth;
            }
        }
        instances[inst].finalX = (int) instances[inst].x;
        instances[inst].rotate = 0;
    }
    
    
    /* check which row is stuffed. If stuffed, place to near row*/

    place2nearRow(topDie, topDiePlacementState, instances, topDieCellsWidth);
    place2nearRow(btmDie, btmDiePlacementState, instances, btmDieCellsWidth);

    /*  place instances to best X position*/

    placeInst2BestX(topDie, topDiePlacementState, topDieMacrosPlacement, instances, topDieCellsWidth);
    placeInst2BestX(btmDie, btmDiePlacementState, BtmDieMacrosPlacement, instances, btmDieCellsWidth);
}

void placeInst2BestX(const Die die, vector <vector<int>> &diePlacementState, vector <vector<int>> &dieMacroPlacementState, vector <instance> &instances, int *cellWidth)
{
    for(int row = 0; row < die.repeatCount; row++)
    {
        int numInstInRow = diePlacementState[row].size();
        int numMacroInRow = dieMacroPlacementState[row].size();
        
        map<int, int> instMap;
        map<int, int> macroMap;

        vector <int> sortArray(numInstInRow);
        vector <int> macroSortArray(numMacroInRow);
        vector <int> gapOfmacros;
        
        if( numInstInRow == 0 )
            continue;

        if( numMacroInRow > 0 )
        {
            for(int i = 0; i < numMacroInRow; i++)
            {
                int cellId = dieMacroPlacementState[row][i];
                int cellX = ( instances[cellId].finalX - instances[cellId].finalWidth/2 );

                while ( instMap.find( cellX ) != instMap.end() )
                {
                    instances[cellId].finalX += 1.0;
                    cellX = instances[cellId].finalX - instances[cellId].finalWidth/2;
                }

                macroMap[cellX] = cellId; 
                macroSortArray[i] = cellX;
            }
            sort(macroSortArray.begin(), macroSortArray.end()); 
        }

        for(int inst = 0; inst < numInstInRow; inst++)
        {
            int cellId = diePlacementState[row][inst];
            int cellX = instances[cellId].finalX - instances[cellId].finalWidth/2;

            while ( instMap.find( cellX ) != instMap.end() )
            {
                instances[cellId].finalX += 1.0;
                cellX = instances[cellId].finalX - instances[cellId].finalWidth/2;
            }
            
            instMap[cellX] = cellId;
            sortArray[inst] = cellX;
        }

        sort(sortArray.begin(), sortArray.end());

        /*  place to best X*/

        int firstCell = instMap[ sortArray[0] ];
        int startX = instances[ firstCell ].finalX;
        int macroCnt, nowMacroID, macroX = 999999, macroWidth;

        if(startX + cellWidth[row] > (int) die.upperRightX)
        {
            startX = (int) die.upperRightX - cellWidth[row];
            instances[ firstCell ].finalX = startX;
        }

        if( numMacroInRow > 0)
        {
            macroCnt = 0;
            nowMacroID = macroMap[ macroSortArray[macroCnt] ];
            macroX = instances[nowMacroID].finalX;
            macroWidth = instances[nowMacroID].finalWidth;
        }
        
        for(int inst = 1; inst < numInstInRow; inst++)
        {
            int cellID = instMap[ sortArray[inst] ];
            int lastCellWidth = instances[ instMap[ sortArray[inst-1] ] ].finalWidth;
            int nowCellFinalX = instances[cellID].finalX;
            int nowCellWidth = instances[cellID].finalWidth;
            

            if( startX + nowCellWidth < macroX)
            {
                startX += lastCellWidth;
                instances[cellID].finalX = startX; 
            }
            else 
            {
                startX = macroX + macroWidth;
                instances[cellID].finalX = startX;

                macroCnt += 1;
                nowMacroID = macroMap[ macroSortArray[macroCnt] ];
                macroX = instances[nowMacroID].finalX;
                macroWidth = instances[nowMacroID].finalWidth;
            }

            if (macroCnt == (int) macroSortArray.size() - 1)
                macroWidth = 999999;
        }
    }
}

void place2nearRow(const Die die, vector <vector<int>> &diePlacementState, vector <instance> &instances, int *dieCellWidth)
{
    for(int row = 0; row < die.repeatCount; row++)
    {
        if(false)
        while( dieCellWidth[row] > (int) die.upperRightX)
        {
            cout << row << endl;
            int size = diePlacementState[row].size();
            int lastInstIndex = 0;
            int lastInstWidth = 0;
            int lookUp = row - 1;
            int lookDown = row + 1;
            bool haveChanged = false;

            for(int i = 0; i < size; i++)
            {
                lastInstIndex = diePlacementState[row][i];
                if( !instances[lastInstIndex].isMacro )
                {
                    lastInstWidth = instances[lastInstIndex].finalWidth;
                    break;
                }
            }
            
            while (lookUp >= 0 && !haveChanged)
            {   
                if(dieCellWidth[lookUp] + lastInstWidth < (int) die.upperRightX)
                {
                    diePlacementState[lookUp].push_back(lastInstIndex);
                    diePlacementState[row].pop_back();
                    dieCellWidth[row] -= lastInstWidth;
                    dieCellWidth[lookUp] += lastInstWidth;
                    instances[lastInstIndex].finalY += die.rowHeight * (row - lookUp);
                    haveChanged = true;
                }
                lookUp -= 1;
            }

            while (lookDown < die.repeatCount && !haveChanged)
            {   
                if (dieCellWidth[lookDown] + lastInstWidth < (int) die.upperRightX)
                {
                    diePlacementState[lookDown].push_back(lastInstIndex);
                    diePlacementState[row].pop_back();
                    dieCellWidth[row] -= lastInstWidth;
                    dieCellWidth[lookDown] += lastInstWidth;
                    instances[lastInstIndex].finalY -= die.rowHeight * ( lookDown - row );
                    haveChanged = true;
                }
                lookDown += 1;
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

    int terminalX = terminalTech.spacing > terminalTech.sizeX ? terminalTech.spacing : terminalTech.sizeX;
    int terminalY = terminalTech.spacing > terminalTech.sizeY ? terminalTech.spacing : terminalTech.sizeY;

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
            terminalX = terminalTech.spacing;
            terminalY += terminalTech.sizeY + terminalTech.spacing;
        }
    }

    // for (int t = 0; t < numTerminal; t++)
    // {
    //     printf("terminal: %d, %d\n", terminals[t].x, terminals[t].y);
    // }
    
}

void calculateActualHPWL(const vector <instance> instances, const vector <RawNet> rawNet, vector <terminal> &terminals)
{}

void writeVisualFile(const vector <instance> instances, char *outputFile, const int numInstances, Die &topDie)
{
    FILE *output;

    output = fopen(outputFile, "w");

    int numInstOnTopDie = 0;
    int numInstOnBtmDie = 0;

    fprintf(output, "DieInfo %d %d\n", (int)topDie.upperRightX, (int)topDie.upperRightY);

    fprintf(output, "TopDiePlacement %d\n", numInstOnTopDie);

    for(int inst = 0; inst < numInstances; inst++)
    {
        if( instances[inst].layer == topLayer )
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
        if( instances[inst].layer == btmLayer ) 
            fprintf(output, "Inst %d %d %d %d %d\n", 
            instances[inst].instIndex + 1, 
            (int) instances[inst].finalX, 
            (int) instances[inst].finalY,
            (int) instances[inst].finalWidth,
            (int) instances[inst].finalHeight);
    }

    fclose(output);

}

void writeFile(const vector <instance> instances, char *outputFile, const vector <RawNet> rawNet, const int numInstances, const vector <terminal> terminals)
{
    FILE *output;

    output = fopen(outputFile, "w");

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
            (int) instances[inst].rotate);
    }

    fprintf(output, "BottomDiePlacement %d\n", numInstOnBtmDie);

    for(int inst = 0; inst < numInstances; inst++)
    {
        if( instances[inst].layer == btmLayer )
            fprintf(output, "Inst C%d %d %d R%d\n", 
            instances[inst].instIndex + 1, 
            instances[inst].finalX, 
            instances[inst].finalY,
            (int) instances[inst].rotate
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