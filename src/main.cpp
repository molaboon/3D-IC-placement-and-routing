#include <stdio.h>
#include <assert.h>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <iostream>
#include <map>

#include "lib/readfile.h"
#include "lib/initial_placement.h"
#include "lib/CG.h"
#include "lib/coarsening.h"
#include "lib/partition.h"
#include "lib/legalization.h"

using namespace std;
using std::vector;

#define dimention 3
#define macroPart 1
#define stdCellPart 1
#define useEplace 0

int main(int argc, char *argv[]){
	
	// start_time = omp_get_wtime();
	// srand(time(NULL));

	double startTime, endTime;
	
	char *inputName = *(argv + 1);
	int totalIter = atoi(*(argv + 2));

	FILE *input = fopen(inputName, "r");
	assert(input);

	Die top_die, bottom_die;												//store the die information
	Hybrid_terminal terminalTech;											//store the size of the hybrid bond terminal connect between two dies
	
	int NumTechnologies;													//TA and TB
	vector <Tech_menu> *techMenuPtr = new vector<Tech_menu>;				//The detail of the library of the standardcell by different technology

	int numStdCells;														//How many instances need to be placeed in the two dies
	vector <instance> instances;							    			//The information of standard cell and Macro 
	
	int NumNets;															//How many nets connect betweem Instances
	vector <RawNet> rawnet;													//All of the netlist
 	gridInfo binInfo;														// bin infomation(bin w/h, num of bin, die w/h)

	vector <instance> macros;												//devide the "instances" into "macro" and "std cell",
	vector <instance> stdCells;												// but they are store in diff memory, so be carefull
	vector <RawNet> netsOfMacros;
	vector <int> numStdCellConncetMacro;
	vector <terminal> terminals;											// sotre the position of terminal (for output)
	vector < vector<instance> > pinsInMacros;

	double *densityMap = new double[ 100001 ]{0};

	/*	read data	*/
	readTechnologyInfo(input, &NumTechnologies, techMenuPtr);	
	readDieInfo(input, &top_die, &bottom_die);
	readHybridTerminalInfo(input, &terminalTech);
	readInstanceInfo(input, &numStdCells, instances, &NumTechnologies, techMenuPtr, macros, stdCells, pinsInMacros);
	delete techMenuPtr;
	
	/*  
		create a vector called "pinsInMacro". 
		Because the macro are fixed after placement of macro, the "pinsInMacros" is stored the pin info of each macro 
		netOfMacros = [*macros, ...]
		rawnet = [*instances, *pinsInMacro, ...]
	*/
	readNetInfo(input, &NumNets, rawnet, instances, macros, netsOfMacros, numStdCellConncetMacro, pinsInMacros);
	returnGridInfo(&top_die, &binInfo, numStdCells, instances);
	returnDensityMap(densityMap);
	/*	macro gradient and placement	*/
	
	if(macroPart)
	{
		macroGradient( macros, netsOfMacros, top_die, 20, densityMap);
		macroLegalization(macros, top_die, bottom_die);
		macroRotate(macros, pinsInMacros, rawnet, instances);
		updatePinsInMacroInfo( macros, pinsInMacros, instances);

		writeVisualFile(macros, 2, top_die);
		// macroPartition( macros, netsOfMacros, top_die);
	}

	/*	coarsening */

	// coarsen(rawnet, instances);

	/*	first placement and CG preprocessing	*/
	
	if (stdCellPart){

		double gamma, penaltyWeight, totalScore = 0.0, newScore = 0.0;
		double wireLength, newWireLength;

		double *lastCG = new double[ numStdCells * 3 ]{0.0};
		double *nowCG = new double[ numStdCells * 3 ]{0.0};
		double *lastGra = new double[ numStdCells * 3 ]{0.0};
		double *nowGra = new double[ numStdCells * 3 ]{0.0};

		int qqq = -1;

		stdCellFirstPlacement(instances, macros, binInfo, top_die);
		updatePinsInMacroInfo( macros, pinsInMacros, instances);

		gamma = 0.05 * binInfo.dieWidth;
		penaltyWeight = returnPenaltyWeight(rawnet, gamma, instances, binInfo, densityMap);
		
		/*	Refinement(CG)	*/

		startTime = clock();
		
		for(int i = 0; i < 100; i++)
		{
			totalScore = returnTotalScore( rawnet, gamma, binInfo, penaltyWeight, instances, densityMap);
			updateGra(rawnet, gamma, instances, binInfo, lastGra, nowGra, lastCG, nowCG, penaltyWeight, densityMap);
			CGandGraPreprocessing(instances, nowGra, nowCG, lastGra, lastCG);

			if( !useEplace )
			{
				for(int j = 0; j < totalIter; j++)
				{
					qqq++;

					newSolution(instances, nowCG, binInfo);
					updatePinsInMacroInfo( macros, pinsInMacros, instances);
					writeVisualFile(instances, qqq, top_die);

					newScore = returnTotalScore( rawnet, gamma, binInfo, penaltyWeight, instances, densityMap);
					
					if( newScore > totalScore)
						break;

					updateGra(rawnet, gamma, instances, binInfo, lastGra, nowGra, lastCG, nowCG, penaltyWeight, densityMap);
					conjugateGradient(nowGra, nowCG, lastCG, lastGra, numStdCells, 1);
					
				}
				
				refPosition(instances);
				if(penaltyWeight < 500.0)
					penaltyWeight *= 2;
			}
			else
			{

				double optParam = 1.0;
				double *curRefSoltion = new double [numStdCells *3] {0.0};
    			double *newRefSolution = new double [numStdCells *3] {0.0};
    
				for(int j = 0; j < totalIter; j++)
				{	
					clacBktrk(instances, lastGra, nowGra, j, &optParam, rawnet, gamma, binInfo, lastCG, nowCG, 
							  penaltyWeight, densityMap, curRefSoltion, newRefSolution);
					writeVisualFile(instances, j, top_die);
				}
			}
		}

		endTime = clock();
		finalUpdatePinsInMacro( macros, pinsInMacros, instances);
		newScore = returnTotalScore( rawnet, gamma, binInfo, 1, instances, densityMap);
		printf("Time: %fs, iter: %d, score: %f\n", (endTime - startTime) / (double) CLOCKS_PER_SEC, qqq , newScore);

	}

	if(true)
	{	
		cell2BestLayer(instances, top_die, bottom_die);
		place2BestRow(instances, numStdCells, top_die, bottom_die, macros);
		insertTerminal(instances, rawnet, terminals, terminalTech, top_die);
		writeVisualFile(instances, 0, top_die);
		writeFile(instances, rawnet, numStdCells, terminals);
	}

	return 0;
}
