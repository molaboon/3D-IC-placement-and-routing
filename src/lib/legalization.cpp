#include <stdio.h>
#include <vector>
#include <cmath>
#include <string.h>
#include <iostream>

#include "readfile.h"


using namespace std;


void cell2BestLayer( vector <instance> instances, int numInstances)
{
    for(int i = 0; i < numInstances; i++)
    {
        if( instances[i].z < 0.5)
            instances[i].layer = 0;

        else
            instances[i].layer = 1;
    }
}


void cell2BestRow( vector <instance> instances, int numInstances)
{
    // left down 


    

}

void calculateActualHPWL(const vector <instance> instances, const vector <RawNet> rawNet)
{   
}