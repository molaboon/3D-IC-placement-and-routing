#include <iostream>
#include <stdlib.h>
#include <time.h>

#include "partition.h"
#include "readfile.h"

void macroPartition( vector <instance> &macros, vector <instance*> &netsOfMacros)
{
    srand( time(NULL) );

    int macrosSize = int ( macros.size() );
    int netsOfMacroSize = int( netsOfMacros.size() );
    int firstCut = 0;
    
    for(int i = 0; i < macrosSize; i++)
    {
        int x = rand() % 2;   
        macros[i].layer = x;
        cout << macros[i].layer << endl;
    }
    cout << endl;

    for(int i = 0; i < netsOfMacroSize ; i +=2 )
    {
        // cout << netsOfMacros[i]->layer << " " << netsOfMacros[i + 1]->layer << endl;
        if(netsOfMacros[i]->layer != netsOfMacros[i + 1]->layer)
            firstCut++;
    }

    for(int m = 0; m < macrosSize; m++)
    {
        int tmpCut = 0;
        int originLayer = macros[m].layer;  
        
        macros[m].layer = (macros[m].layer + 1) % 2;
        
        for(int n = 0 ; n < netsOfMacroSize; n+=2)
        {
            if(netsOfMacros[n]->layer != netsOfMacros[n + 1]->layer)
                tmpCut++;
        }
        cout << m <<": tmpCut : " << tmpCut << " "<< firstCut <<endl;

        if(tmpCut >= firstCut)
            macros[m].layer = originLayer;
        else
            firstCut = tmpCut;
        
    }

    cout << firstCut << endl;
}