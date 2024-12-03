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
            fscanf(input, "%*s %s %s %lf %lf %d", &isMacro, temp_libcell[j].libCellName, &(temp_libcell[j].libCellSizeX), &(temp_libcell[j].libCellSizeY), &(temp_libcell[j].pinCount));
            
            if(isMacro == 89)
                temp_libcell[j].isMacro = true;
            else
                temp_libcell[j].isMacro = false;

            //read pinarray pinarray_count time
            vector <Pin> temp_pinarray(temp_libcell[j].pinCount);   
            for(int k = 0; k  < temp_libcell[j].pinCount; k++){
                fscanf(input, "%*s %s %d %d", temp_pinarray[k].pinName, &(temp_pinarray[k].pinLocationX), &(temp_pinarray[k].pinLocationY));
            }
            temp_libcell[j].pinarray = temp_pinarray;
        }
        temp.libcell = temp_libcell;
        TechMenu->at(i) = temp;
    }
    read_one_blank_line(input);
} 

void printTechnologyInfo(int NumTechnologies, vector <Tech_menu> TechMenu){
    printf("\nNumTechnologies <technologyCount>: %d\n\n", NumTechnologies);
    for(int i = 0; i < NumTechnologies; i++){
        printf("Tech <techName> <libCellCount>: %s %d:\n", TechMenu[i].tech, TechMenu[i].libcell_count);
        for(int j = 0; j < TechMenu[i].libcell_count; j++){
            printf("\tLibCell <libCellName> <libCellSizeX> <libCellSizeY> <pinCount> <isMacro>: %s %lf %lf %d\n", TechMenu[i].libcell[j].libCellName, TechMenu[i].libcell[j].libCellSizeX, TechMenu[i].libcell[j].libCellSizeY, TechMenu[i].libcell[j].pinCount);
            for(int k = 0; k < TechMenu[i].libcell[j].pinCount; k++){
                printf("\t\tPin <pinName> <pinLocationX> <pinLocationY>: %s %d %d\n", TechMenu[i].libcell[j].pinarray[k].pinName, TechMenu[i].libcell[j].pinarray[k].pinLocationX, TechMenu[i].libcell[j].pinarray[k].pinLocationY);
            }
            printf("\n");   
        }
        printf("\n\n");
    }
    printf("\n");
}

void readDieInfo(FILE *input, Die *top_die, Die *bottom_die){
    assert(input);

    //read DieSize of the top die and the bottom size
    fscanf(input,"%*s %d %d %lf %lf", &(top_die->lowerLeftX), &(top_die->lowerLeftY), &(top_die->upperRightX), &(top_die->upperRightY));
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
    fscanf(input, "%*s %d %d",&(terminal->sizeX),&(terminal->sizeY));
    fscanf(input, "%*s %d",&(terminal->spacing));
    fscanf(input, "%*s %d",&(terminal->cost));
    read_one_blank_line(input);
}

void printHybridTerminalInfo(Hybrid_terminal terminal){
    printf("\nHybrid Terminal Information:\n");
    printf("TerminalSize <sizeX> <sizeY>: %d %d\n", terminal.sizeX, terminal.sizeY);
    printf("TerminalSpacing <spacing>: %d\n\n", terminal.spacing);
}

void readInstanceInfo(FILE *input, int *NumInstances, vector <instance> &instances, int *NumTechnologies, vector <Tech_menu> *TechMenu, vector <instance> &macros, vector <instance> &stdCells)
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
        temp.finalX = 0;
        temp.finalY = 0;
        temp.finalWidth = 0;
        temp.finalHeight = 0;
        temp.connectedNet = tmpNetConnect;
        
        temp.area = temp.width * temp.height;
        temp.inflateArea = temp.inflateWidth * temp.inflateHeight;
        
        if(temp.isMacro)
            macros.emplace_back( temp );
        else
            stdCells.emplace_back( temp );
        
        instances.emplace_back(temp);

    }
    read_one_blank_line(input);
}

void printInstanceInfo(int NumInstances, vector <instance> instances){
    // printf("NumInstances <instanceCount>: %d\n", NumInstances);
    // for(int i = 0; i < NumInstances; i++){
    //     printf("\tInst <Name> <lib> <w> <h> <x> <y>: %s %s %lf %lf %lf %lf %lf\n", 
    //     instances[i].instName, 
    //     instances[i].libCellName, 
    //     instances[i].width, 
    //     instances[i].height,
    //     instances[i].x,
    //     instances[i].y,
    //     instances[i].z
    //     );
    // }
    // printf("\n");
}

void readNetInfo(FILE *input, int *NumNets, vector <RawNet> &rawnet, vector <instance> &instances, vector <instance> &macros, vector <RawNet> &netsOfMacros, vector <int> &numStdCellConnectMacro)
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
    		
            memset(current_libCellName,'\0', LIBCELL_NAME_SIZE);

            strncpy(current_libCellName, token + 1, strlen(token)-1);

            temp_connection[pin] = ( &instances[atoi(current_libCellName)-1] );
            instances[atoi(current_libCellName)-1].connectedNet[instances[atoi(current_libCellName)-1].numNetConnection] = i;
            instances[atoi(current_libCellName)-1].numNetConnection += 1;
            
            // temp_connection[pin]->netsConnect[i] = 1;

            if(instances[atoi(current_libCellName)-1].isMacro)
            {
                numMacro++;

                for(int j = 0; j < (int )macros.size(); j++)
                {
                    if(macros[j].instIndex == (atoi(current_libCellName)-1))
                        macros_connection.push_back( &macros[j] );
                }

                numStdCellConnectMacro[ macrosMap[atoi(current_libCellName)-1] ] += temp.numPins;

                if(numMacro > 1)
                    haveMacro = true;
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

    fclose(input);
}

void printNetInfo(int NumNets, vector <RawNet> rawnet){
    printf("\nNumNets <netCount>: %d\n", NumNets);
    for(int i = 0; i < NumNets; i++){
        // printf("\tNet <netName> <numPins>: %s %d\n", rawnet[i].netName, rawnet[i].numPins);
        for(int j = 0; j < rawnet[i].numPins; j++){
            printf("\t\t<instance>: %lf %lf %lf \n",  rawnet[i].Connection[j]->x, rawnet[i].Connection[j]->y, rawnet[i].Connection[j]->z);
        }
        printf("\n");
    }
    printf("\n");
}
