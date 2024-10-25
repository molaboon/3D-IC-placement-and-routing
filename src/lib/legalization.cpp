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

void place2BestRow( vector <instance> &instances, const int numInstances, Die topDie, Die btmDie, vector <instance> &macros)
{
    vector < vector<int> > topDiePlacementState( topDie.repeatCount );
    vector < vector<int> > btmDiePlacementState( btmDie.repeatCount );

    vector < vector<int> > topDieMacrosPlacement(topDie.repeatCount);
    vector < vector<int> > BtmDieMacrosPlacement(btmDie.repeatCount);

    int topDieCellsWidth[topDie.repeatCount] = {0};
    int btmDieCellsWidth[btmDie.repeatCount] = {0};
    int numMacro = macros.size();

    int topDieArray [topDie.repeatCount][numMacro * 2] = {0};
    int btmDieArray [btmDie.repeatCount][numMacro * 2] = {0};


    /* place macro first */
    
    for(int m = 0; m < numMacro; m++)
    {   
        int uperX = (int) (macros[m].x + (macros[m].width/2.0) );
        int uperY = (int) (macros[m].y + (macros[m].height/2.0) );
        int lowerX = (int) (macros[m].x - (macros[m].width/2.0) );
        int lowerY = (int) (macros[m].y - (macros[m].height/2.0) );

        int uperRow, lowerRow;

        if( macros[m].layer == topLayer)
        {
            uperRow = uperY % topDie.repeatCount + 1;
            lowerRow = lowerY % topDie.repeatCount;

            if(uperRow >= topDie.repeatCount)
                uperRow = topDie.repeatCount-1;

            for(int row = lowerRow; row < uperRow; row++)
            {
                int macroWidth = (int) macros[m].width;
                int macroX = (int) macros[m].x;

                topDieCellsWidth[row] += macroWidth;
                topDieMacrosPlacement[row].push_back(macros[m].instIndex);
            } 
        }   
        else
        {
            uperRow = uperY % btmDie.repeatCount + 1;
            lowerRow = lowerY % btmDie.repeatCount;

            if(uperRow >= topDie.repeatCount)
                uperRow = topDie.repeatCount-1;

            for(int row = lowerRow; row < uperRow; row++)
            {
                int macroWidth = (int) macros[m].width;
                int macroX = (int) macros[m].x;

                btmDieCellsWidth[row] += macroWidth;
                topDieMacrosPlacement[row].push_back(macros[m].instIndex);
            }
        }
    }

    
    /*  place standard cell to best row */

    
    for(int inst = 0; inst < numInstances; inst++)
    {
        int row = 0;
        double upOrdown = instances[inst].y ;
 
        if( !instances[inst].isMacro )
        {
            if( instances[inst].layer == topLayer)
            {
                upOrdown = fmod( upOrdown, (double) topDie.rowHeight);
                row = (int) ( (instances[inst].y - (instances[inst].height/2.0) ) / (double) topDie.rowHeight );

                if( upOrdown > (double) topDie.rowHeight / 2.0 )
                    row += 1;
                    
                topDiePlacementState[row].push_back(instances[inst].instIndex);
                instances[inst].finalY = topDie.rowHeight * row;
                topDieCellsWidth[row] += instances[inst].width;
                
            }
            else
            {
                upOrdown = fmod( upOrdown, (double) btmDie.rowHeight);
                row = (int) ( (instances[inst].y - (instances[inst].height/2.0) ) / (double) btmDie.rowHeight );

                if( upOrdown > (double) btmDie.rowHeight / 2.0 )
                    row += 1;

                btmDiePlacementState[row].push_back(instances[inst].instIndex);
                instances[inst].finalY = btmDie.rowHeight * row;
                btmDieCellsWidth[row] += instances[inst].width;
            }
        }

        instances[inst].finalX = (int) instances[inst].x;
        instances[inst].finalWidth = (int) instances[inst].width;
        instances[inst].finalHeight = (int) instances[inst].height;
    }
    
    /* check which row is stuffed. If stuffed, place to near row*/

    place2nearRow(topDie, topDiePlacementState, instances, topDieCellsWidth);
    place2nearRow(btmDie, btmDiePlacementState, instances, btmDieCellsWidth);

    /*  place instances to best X position*/

    placeInst2BestX(topDie, topDiePlacementState, instances);
    placeInst2BestX(btmDie, btmDiePlacementState, instances);
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

void insertTerminal(const vector <instance> instances, const vector <RawNet> rawNet, vector <terminal> &terminals, Hybrid_terminal terminalTech, Die topDie)
{
    int numNet = rawNet.size();
    int numTerminal = 0;
    int dieWidth = (int) topDie.upperRightX;
    int dieHight = (int) topDie.upperRightY;

    int terminalX = terminalTech.spacing;
    int terminalY = terminalTech.spacing;

    for(int net = 0; net < numNet; net++)
    {
        int firstInstLater = rawNet[net].Connection[0]->layer;

        for(int inst = 1 ; inst < rawNet[net].numPins; inst++)
        {
            if( rawNet[net].Connection[inst]->layer != firstInstLater )
            {
                numTerminal++;

                terminal newTerminal;

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

void placeInst2BestX(const Die die, vector <vector<int>> &diePlacementState, vector <instance> &instances)
{
    for(int count = 0; count < die.repeatCount; count++)
    {
        int numInstInRow = diePlacementState[count].size();
        int numMacroInRow = diePlacementState[count].size();
        
        map<int, int> instMap;

        vector <int> sortArray(numInstInRow);
        vector <int> macroSortArray(numMacroInRow);
        vector <int> gapOfmacros;
        
        if( numInstInRow == 0)
            continue;

        for(int i = 0; i < numMacroInRow; i++)
        {
            int cellId = diePlacementState[count][i];
            int cellX = ( instances[cellId].finalX - instances[cellId].finalWidth/2 );

            instMap[cellX] = cellId; 
            macroSortArray[i] = cellX;
        }

        for(int inst = 0; inst < numInstInRow; inst++)
        {
            int cellId = diePlacementState[count][inst];
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
        sort(macroSortArray.begin(), macroSortArray.end()); 

        for(int i = 0; i < numMacroInRow; i++)
        {
            int cell = instMap[ macroSortArray[i] ];
            int start =  ( instances[cell].finalX - instances[cell].finalWidth/2 );
            int end = ( instances[cell].finalX + instances[cell].finalWidth/2 );
            
            gapOfmacros.push_back(start);
        }
        
        /*  place to best X*/

        int firstCell = instMap[ sortArray[0] ];
        int startX = instances[ firstCell ].finalX;
        int macroCnt = 0;
        int nowMacroID = instMap[ macroSortArray[macroCnt] ];
        int macroX = instances[nowMacroID].finalX;
        
        
        for(int inst = 1; inst < numInstInRow; inst++)
        {
            int cellID = instMap[ sortArray[inst] ];
            int lastCellWidth = instances[ instMap[ sortArray[inst-1] ] ].finalWidth;
            int nowCellFinalX = instances[cellID].finalX;
            int nowCellWidth = instances[cellID].width;

            if( nowCellFinalX + nowCellWidth > macroX)
            {
                startX += instances[cellID].width;
            }    
            else
            {
                if(nowCellFinalX < startX + lastCellWidth)
                    instances[cellID].finalX = startX + lastCellWidth;
                
                startX = instances[cellID].finalX;
            }
        }
    }
}

void place2nearRow(const Die die, vector <vector<int>> &diePlacementState, vector <instance> &instances, int *dieCellWidth)
{
    for(int row = 0; row < die.repeatCount; row++)
    {
        while( dieCellWidth[row] > (int) die.upperRightX)
        {
            int size = diePlacementState[row].size();
            int lastInstIndex = diePlacementState[row][size - 1];
            int lastInstWidth = instances[lastInstIndex].width;
            int tmpRow = row - 1;

            while (tmpRow >= 0)
            {   
                if (dieCellWidth[tmpRow] + lastInstWidth < (int) die.upperRightX)
                {
                    diePlacementState[tmpRow].push_back(lastInstIndex);
                    diePlacementState[row].pop_back();
                    dieCellWidth[row] -= lastInstWidth;
                    dieCellWidth[tmpRow] += lastInstWidth;
                }
                tmpRow -= 1;
            }
        }
    }

}

void calculateActualHPWL(const vector <instance> instances, const vector <RawNet> rawNet, vector <terminal> &terminals)
{}