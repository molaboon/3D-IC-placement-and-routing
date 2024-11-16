#include <iostream>
#include <stdlib.h>
#include <time.h>

#include "partition.h"
#include "readfile.h"

void macroPartition( vector <instance> &macros, vector <instance*> &netsOfMacros)
{
    srand( time(NULL) );

    int numMacros = int ( macros.size() );
    int netsOfMacroSize = int( netsOfMacros.size() );
    int cut = 0;

    double topDieArea = 0;
    double btmDieArea = 0;
    
    for(int i = 0; i < numMacros; i++)
    {
        int x = rand() % 2;
        macros[i].layer = x;

        if(x)
            topDieArea += macros[i].area;
        
        else
            btmDieArea += macros[i].inflateArea;
    }  

    for(int i = 0; i < netsOfMacroSize ; i +=2 )
    {
        if(netsOfMacros[i]->layer != netsOfMacros[i + 1]->layer)
            cut++;
    }
    

    // calculate the area and the best cut 

   

}
