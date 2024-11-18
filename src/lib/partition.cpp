#include <iostream>
#include <stdlib.h>
#include <time.h>

#include "partition.h"
#include "readfile.h"

void macroPartition( vector <instance> &macros, vector <RawNet> &netsOfMacros, Die topDie)
{
    srand( time(NULL) );

    int numMacros = int ( macros.size() );
    int cut = 0;

    double topDieArea = 0;
    double btmDieArea = 0;
     dif = 0;
    
    int finalLayer[numMacros] = {0};
    
    
    
    for(int i = 0; i < numMacros; i++)
    {
        int layer = rand() % 2;
        macros[i].layer = layer;

        if(layer)
            topDieArea += macros[i].area;
        
        else
            btmDieArea += macros[i].inflateArea;
    }  

    cut = returnCut(netsOfMacros);
    dif = abs(topDieArea - btmDieArea);

    while ( dif > topDie.upperRightX * topDie.upperRightY * 0.1 )
    {

    }
    
    

    // calculate the area and the best cut 

}

int returnCut(vector <RawNet> &netsOfMacros)
{
    int numNetsOfMacro = int( netsOfMacros.size() );
    int cut = 0;

    for(int i = 0; i < numNetsOfMacro ; i ++ )
    {
        int numPins = netsOfMacros[i].Connection.size();
        int firstlayer = netsOfMacros[i].Connection[0]->layer;

        for(int j = 1; j < numPins; j++)
        {
            if(netsOfMacros[i].Connection[j]->layer != firstlayer)
            {
                cut++;
                break;
            }
        }    
    }

    return cut;
}
