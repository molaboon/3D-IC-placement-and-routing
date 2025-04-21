#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <map>

#include "readfile.h"

#define BUFFER_SIZE 25

void read_one_blank_line(FILE *input){
    assert(input);
    fgetc(input);
}

void readTechnologyInfo(FILE *input, int *NumTechnologies, vector <Tech_menu> *TechMenu){
    assert(input);

    //read number of technologies
    fscanf(input, "%*s %d", &(*NumTechnologies));
    TechMenu->resize( *NumTechnologies );

    for(int i = 0; i < *NumTechnologies; i++){
        Tech_menu temp;
        fscanf(input, "%*s %s %d", temp.tech, &(temp.libcell_count));

        vector <Libcell> temp_libcell(temp.libcell_count);

        //read libcell libcell count time
        for(int j = 0; j < temp.libcell_count; j++){
            char isMacro;
            fscanf(input, "%*s %s %s %f %f %d", &isMacro, temp_libcell[j].libCellName, 
                                                  &(temp_libcell[j].libCellSizeX), 
                                                  &(temp_libcell[j].libCellSizeY), 
                                                  &(temp_libcell[j].pinCount));
            
            if(isMacro == 89)
                temp_libcell[j].isMacro = true;
            else
                temp_libcell[j].isMacro = false;

            //read pinarray pinarray_count time
            vector <Pin> temp_pinarray(temp_libcell[j].pinCount);   

            for(int k = 0; k  < temp_libcell[j].pinCount; k++){
                temp_pinarray[k].pinID = k;
                fscanf(input, "%*s %*s %d %d", &(temp_pinarray[k].pinLocationX), &(temp_pinarray[k].pinLocationY));
            }
            temp_libcell[j].pinarray = temp_pinarray;
        }
        temp.libcell = temp_libcell;
        TechMenu->at(i) = temp;
    }
    read_one_blank_line(input);
} 

void printTechnologyInfo(int NumTechnologies, vector <Tech_menu> TechMenu){
    // printf("\nNumTechnologies <technologyCount>: %d\n\n", NumTechnologies);
    // for(int i = 0; i < NumTechnologies; i++){
    //     printf("Tech <techName> <libCellCount>: %s %d:\n", TechMenu[i].tech, TechMenu[i].libcell_count);
    //     for(int j = 0; j < TechMenu[i].libcell_count; j++){
    //         printf("\tLibCell <libCellName> <libCellSizeX> <libCellSizeY> <pinCount> <isMacro>: %s %f %f %d\n", TechMenu[i].libcell[j].libCellName, TechMenu[i].libcell[j].libCellSizeX, TechMenu[i].libcell[j].libCellSizeY, TechMenu[i].libcell[j].pinCount);
    //         for(int k = 0; k < TechMenu[i].libcell[j].pinCount; k++){
    //             printf("\t\tPin <pinName> <pinLocationX> <pinLocationY>: %s %d %d\n", TechMenu[i].libcell[j].pinarray[k].pinName, TechMenu[i].libcell[j].pinarray[k].pinLocationX, TechMenu[i].libcell[j].pinarray[k].pinLocationY);
    //         }
    //         printf("\n");   
    //     }
    //     printf("\n\n");
    // }
    // printf("\n");
}

void readDieInfo(FILE *input, Die *top_die, Die *bottom_die){
    assert(input);

    //read DieSize of the top die and the bottom size
    fscanf(input,"%*s %d %d %f %f", &(top_die->lowerLeftX), &(top_die->lowerLeftY), &(top_die->upperRightX), &(top_die->upperRightY));
    bottom_die->lowerLeftX = top_die->lowerLeftX;
    bottom_die->lowerLeftY = top_die->lowerLeftY;
    bottom_die->upperRightX = top_die->upperRightX;
    bottom_die->upperRightY = top_die->upperRightY;
    read_one_blank_line(input);

    //read maximum Utilization of the top die and the bottom die
    fscanf(input,"%*s %d", &(top_die->MaxUtil));
    fscanf(input,"%*s %d", &(bottom_die->MaxUtil));
    read_one_blank_line(input);

    //read DieRows Information
    fscanf(input,"%*s %d %d %d %d %d",&(top_die->startX), &(top_die->startY), &(top_die->rowLength),&(top_die->rowHeight), &(top_die->repeatCount));
    fscanf(input,"%*s %d %d %d %d %d",&(bottom_die->startX), &(bottom_die->startY), &(bottom_die->rowLength),&(bottom_die->rowHeight), &(bottom_die->repeatCount));
    read_one_blank_line(input);

    //read DieTech
    char a[10];
    char b[10];
    fscanf(input,"%*s %s", a);
    fscanf(input,"%*s %s", b); 
    top_die->index = 1;
    bottom_die->index = 0 ;
    read_one_blank_line(input);
}

//print the detail die information
void printDieInfo(Die top_die, Die bottom_die){
    // printf("\nTop Die Information:\n");
    // printf("DieSize <lowerLeftX> <lowerLeftY> <upperRightX> <upperRightY>: %d %d %d %d\n", top_die.lowerLeftX, top_die.lowerLeftY, top_die.upperRightX, top_die.upperRightY);
    // printf("TopDieMaxUtil: %d\n", top_die.MaxUtil);
    // printf("TopDieRows <startX> <startY> <rowLength> <rowHeight> <repeatCount>:");
    // printf("%d %d %d %d %d\n", top_die.startX, top_die.startY, top_die.rowLength, top_die.rowHeight, top_die.repeatCount);
    // printf("TopDieTech <TechName>: %s\n\n", top_die.tech);

    // printf("\nBottom Die Information:\n");
    // printf("DieSize <lowerLeftX> <lowerLeftY> <upperRightX> <upperRightY>: %d %d %d %d\n", bottom_die.lowerLeftX, bottom_die.lowerLeftY, bottom_die.upperRightX, bottom_die.upperRightY);
    // printf("BottomDieMaxUtil: %d\n", bottom_die.MaxUtil);
    // printf("BottomDieRows <startX> <startY> <rowLength> <rowHeight> <repeatCount>:");
    // printf("%d %d %d %d %d\n", bottom_die.startX, bottom_die.startY, bottom_die.rowLength, bottom_die.rowHeight, bottom_die.repeatCount);
    // printf("BottomDieTech <TechName>: %s\n\n", bottom_die.tech);
}

void readHybridTerminalInfo(FILE *input, Hybrid_terminal *terminal){
    assert(input);

    //read terminal size & spacing
    fscanf(input, "%*s %d %d",&(terminal->sizeX), &(terminal->sizeY));
    fscanf(input, "%*s %d",&(terminal->spacing));
    fscanf(input, "%*s %d",&(terminal->cost));
    read_one_blank_line(input);
}

void printHybridTerminalInfo(Hybrid_terminal terminal){
    printf("\nHybrid Terminal Information:\n");
    printf("TerminalSize <sizeX> <sizeY>: %d %d\n", terminal.sizeX, terminal.sizeY);
    printf("TerminalSpacing <spacing>: %d\n\n", terminal.spacing);
}

void readInstanceInfo(FILE *input, int *NumInstances, vector <instance> &instances, int *NumTechnologies, 
                        vector <Tech_menu> *TechMenu, vector <instance> &macros, vector <instance> &stdCells, 
                        vector< vector<instance> > &pinsInMacros)
{
    assert(input);
    int numOfTech = TechMenu->size();

    fscanf(input, "%*s %d", &(*NumInstances));

    for(int i = 0; i < *NumInstances; i++){
        instance temp;
        char libCellName[LIBCELL_NAME_SIZE];
        fscanf(input, "%*s %*s %s", libCellName); 

        char current_libCellName[LIBCELL_NAME_SIZE];
		memset(current_libCellName,'\0', LIBCELL_NAME_SIZE);
		strncpy(current_libCellName, libCellName + 2, strlen(libCellName)-2);

        temp.instIndex = i;
        temp.width = TechMenu->at(0).libcell[atoi(current_libCellName)-1].libCellSizeX;
        temp.height = TechMenu->at(0).libcell[atoi(current_libCellName)-1].libCellSizeY; 
        temp.isMacro = TechMenu->at(0).libcell[atoi(current_libCellName)-1].isMacro;
        
        if(numOfTech == 2)
        {
            temp.inflateWidth = TechMenu->at(1).libcell[atoi(current_libCellName)-1].libCellSizeX;
            temp.inflateHeight = TechMenu->at(1).libcell[atoi(current_libCellName)-1].libCellSizeY;
        }
        else
        {
            temp.inflateWidth = TechMenu->at(0).libcell[atoi(current_libCellName)-1].libCellSizeX;
            temp.inflateHeight = TechMenu->at(0).libcell[atoi(current_libCellName)-1].libCellSizeY;
        }
        
        vector <int> tmpNetConnect(TechMenu->at(0).libcell[atoi(current_libCellName)-1].pinCount);
        temp.numNetConnection = 0;
        temp.x = 0.0;
        temp.y = 0.0;
        temp.z = 0.0;
        temp.tmpX = 0.0;
        temp.tmpY = 0.0;
        temp.tmpZ = 0.0;
        temp.tmpD = 0.0;
        temp.layer = 3;
        temp.finalX = 0;
        temp.finalY = 0;
        temp.finalWidth = 0;
        temp.finalHeight = 0;
        temp.connectedNet = tmpNetConnect;
        temp.canPass = false;
        
        temp.area = temp.width * temp.height;
        temp.inflateArea = temp.inflateWidth * temp.inflateHeight;
        
        // store the pin coordinate of each macro to vector
        if(temp.isMacro)
        {
            int numPins = TechMenu->at(0).libcell[atoi(current_libCellName)-1].pinCount;
            vector <instance> pins;

            for(int p = 0; p < numPins; p++)
            {
                instance tpins;

                tpins.instIndex = i;
                tpins.isMacro = true;
                tpins.finalX = (float) TechMenu->at(0).libcell[atoi(current_libCellName)-1].pinarray[p].pinLocationX;
                tpins.finalY = (float) TechMenu->at(0).libcell[atoi(current_libCellName)-1].pinarray[p].pinLocationY;
                tpins.width = 0.0;
                tpins.height = 0.0;
                tpins.inflateWidth = 0.0;
                tpins.inflateHeight = 0.0;
                tpins.numNetConnection = 0;
                tpins.canPass = false;
                tpins.connectedNet = tmpNetConnect;
                
                pins.emplace_back(tpins);
            }

            pinsInMacros.emplace_back(pins);
        }
        
        if(temp.isMacro)
            macros.emplace_back( temp );
        else
            stdCells.emplace_back( temp );
        
        instances.emplace_back(temp);

    }
    read_one_blank_line(input);
}


void readNetInfo(FILE *input, int *NumNets, vector <RawNet> &rawnet, vector <instance> &instances, vector <instance> &macros, vector <RawNet> &netsOfMacros, vector <int> &numStdCellConnectMacro, vector < vector<instance> > &pinsInMacros)
{
    assert(input);
    int size = instances.size();
    int numMacro = macros.size();
    
    map<int, int> macrosMap;

    fscanf(input, "%*s %d", &(*NumNets));
    
    for(int i = 0; i < numMacro; i++)
    {
        macrosMap[ macros[i].instIndex ] = i;
        numStdCellConnectMacro.push_back(0);
    }
        
    for(int i = 0; i < *NumNets; i++)
    {
        RawNet temp;
        RawNet macroTmp;

        int firstMacro = 0;
        int numMacro = 0;
        bool aa = 0;
        bool havenet = false;
        bool haveMacro = false;
        fscanf(input, "%*s %*s %d", &temp.numPins);
        temp.netIndex = i;

        //allocate memory for the detail connection in the net
        vector <instance*> temp_connection(temp.numPins);
        vector <instance*> macros_connection(numMacro);

        for(int pin = 0; pin < temp.numPins; pin++)
        {
            char buffer[BUFFER_SIZE];
            memset(buffer, '\0', BUFFER_SIZE);
            fscanf(input, "%*s %s", buffer);
            //divide the string by using delimiter "/"
            char *token = strtok(buffer, "/");
            char current_libCellName[LIBCELL_NAME_SIZE];
            char current_pin[LIBCELL_NAME_SIZE];
    		
            memset(current_libCellName,'\0', LIBCELL_NAME_SIZE);
            memset(current_pin,'\0', LIBCELL_NAME_SIZE);
            
            strncpy(current_libCellName, token + 1, strlen(token)-1);
            token = strtok(NULL, "/");
            strncpy(current_pin, token + 1, strlen(token)-1);
            
            instances[atoi(current_libCellName)-1].connectedNet[instances[atoi(current_libCellName)-1].numNetConnection] = i;
            instances[atoi(current_libCellName)-1].numNetConnection += 1;
            
            if(instances[atoi(current_libCellName)-1].isMacro)
            {
                numMacro++;

                int macroIndex = macrosMap[atoi(current_libCellName)-1];

                for(int j = 0; j < (int)macros.size(); j++)
                {
                    if(macros[j].instIndex == (atoi(current_libCellName)-1))
                    {
                        macros_connection.push_back( &macros[j] );
                        break;
                    }
                }

                numStdCellConnectMacro[ macroIndex ] += temp.numPins;
                // temp_connection[pin] = ( &instances[atoi(current_libCellName)-1] );
                temp_connection[pin] = (&pinsInMacros[macroIndex][atoi(current_pin)-1]);
                int nn = pinsInMacros[macroIndex][atoi(current_pin)-1].numNetConnection;
                pinsInMacros[macroIndex][atoi(current_pin)-1].connectedNet[nn] = i;
                pinsInMacros[macroIndex][atoi(current_pin)-1].numNetConnection ++;

                if(numMacro > 1) 
                    haveMacro = true;
            }
            else
            {
                temp_connection[pin] = ( &instances[atoi(current_libCellName)-1] );
            }
        }

        temp.hasTerminal = false;
        temp.terminalX = 0;
        temp.terminalY = 0;
        temp.Connection = temp_connection;
        rawnet.emplace_back(temp);

        if(haveMacro && macros_connection[0]->instIndex != macros_connection[1]->instIndex)
        {
            macroTmp.hasTerminal = false;
            macroTmp.terminalX = 0;
            macroTmp.terminalY = 0;
            macroTmp.Connection = macros_connection;
            macroTmp.numPins = (int) macros_connection.size();
            netsOfMacros.emplace_back(macroTmp);
        }
    }

    // copy the connectedNet array from "instances" to "macros"

    // for(int i = 0; i < macros.size(); i++)
    // {
    //     macros[i].numNetConnection = 0;
        
    //     for(int net = 0; net < netsOfMacros.size(); net++)
    //     {
    //         for(int pin = 0; pin < netsOfMacros[net].numPins; pin++)
    //         {
    //             if(netsOfMacros[net].Connection[pin]->instIndex == macros[i].instIndex)
    //             {
    //                 macros[i].connectedNet[ macros[i].numNetConnection ] = net;
    //                 macros[i].numNetConnection++;
    //             }
    //         }
    //     }
    // }

    fclose(input);
}

void readAbcusResult(vector <instance> &instances, int layer)
{
    vector <int> instMap;
    int numIns = 0;

    instMap.reserve(instances.size());

    for(int i = 0; i < instances.size(); i++)
    {
        if(instances[i].layer == layer && !instances[i].isMacro)
        {
            instMap.push_back(i);
            numIns++;
        }
    }
    
    FILE *input;
    char filename[30];
    if(layer == 1)
        snprintf(filename, sizeof(filename), "./output/test.result");
    else
        snprintf(filename, sizeof(filename), "./output/test2.result");
    
    input = fopen(filename, "r");

    for(int i = 0; i < numIns; i++)
    {
        int x = 0;
        int y = 0;
        int instIndex = 0;
        char libCellName[LIBCELL_NAME_SIZE];
        fscanf(input, "%s %d %d %*s %*s", libCellName, &x, &y); 

        char current_libCellName[LIBCELL_NAME_SIZE];
		memset(current_libCellName,'\0', LIBCELL_NAME_SIZE);
		strncpy(current_libCellName, libCellName + 1, strlen(libCellName)-1);
        
        instIndex = instMap[ atoi(current_libCellName) ];

        instances[instIndex].finalX = x;
        instances[instIndex].finalY = y;
    }
    
    fclose(input);

}

void readkj(vector <instance> &instances, vector <instance> &macros)
{
    int numCell = instances.size();
    FILE *input;
    char filename[30];
    snprintf(filename, sizeof(filename), "./kjoutput.txt");
    
    input = fopen(filename, "r");


    for(int i = 0; i < numCell; i++)
    {
        int cellIndex = 0;
        int layer = 0;
        
        fscanf(input, "%d %d %*s %*s", &cellIndex, &layer);
        if(layer == 0)
        {
            instances[cellIndex-1].layer = 1;
            instances[cellIndex-1].rotate = 0;
            instances[cellIndex-1].finalWidth = instances[cellIndex-1].width;
            instances[cellIndex-1].finalHeight = instances[cellIndex-1].height;
        }
        else
        {
            instances[cellIndex-1].layer = 0;
            instances[cellIndex-1].rotate = 0;
            instances[cellIndex-1].finalWidth = instances[cellIndex-1].inflateWidth;
            instances[cellIndex-1].finalHeight = instances[cellIndex-1].inflateHeight;
        }
        
        if(instances[cellIndex].isMacro)
        {
            for(int j = 0; j < macros.size(); j++)
            {
                if(macros[j].instIndex == cellIndex-1)
                {
                    macros[j].layer = instances[cellIndex-1].layer;
                    macros[j].rotate = instances[cellIndex-1].rotate;
                    macros[j].finalWidth = instances[cellIndex-1].finalWidth;
                    macros[j].finalHeight = instances[cellIndex-1].finalHeight;
                    
                    break;
                }
            }
        }
    }
}