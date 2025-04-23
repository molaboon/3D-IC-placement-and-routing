#include <stdio.h>
#include <assert.h>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <iostream>
#include <map>
#include <unistd.h>

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
#define stdCellPart 0
#define useEplace 0

int main(int argc, char *argv[]){
	
	// start_time = omp_get_wtime();
	// srand(time(NULL));

	time_t startTime, endTime;
	
	char *inputName = *(argv + 1);
	int totalIter = atoi(*(argv + 2));

	FILE *input = fopen(inputName, "r");
	assert(input);

	Die topDie, btmDie;												//store the die information
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
	vector <instance> fillers; 

	float *densityMap = new float[ 100001 ]{0};

	/*	read data	*/
	readTechnologyInfo(input, &NumTechnologies, techMenuPtr);	
	readDieInfo(input, &topDie, &btmDie);
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
	returnGridInfo(&topDie, &binInfo, numStdCells, instances);
	returnDensityMap(densityMap);
	fillerPreprocess(fillers, binInfo, topDie, btmDie);
	readkj(instances, macros);
	topDie.index = 1;
	btmDie.index = 0;
	
	/*	macro gradient and placement	*/
	
	if(macroPart)
	{
		bool needupdate = false;
		int roatation = 0b0;
		int topList[] = {33,32,30,29,27,26,24,22,21,8,15,16,4,12,19,6,18,10,0, 2};
		int topPinsGrade [][4] = {{45, 29, 62, 40}, {44, 38, 50, 40 },  {50, 32, 58, 29 },  {5, 36, 72, 57 },  {47, 43, 48, 37 },  {38, 135, 0, 0 },  {4, 38, 76, 55 },  {49, 43, 48, 44 },  {39, 133, 0, 0 },  {44, 29, 57, 35 },  {55, 34, 56, 29 },  {36, 136, 0, 0 },  {37, 140, 0, 0 },  {46, 30, 55, 40 },  {4, 41, 75, 59 },  {45, 38, 48, 50 },  {44, 30, 59, 45 },  {45, 34, 63, 38 },  {4, 46, 73, 57 },  {56, 32, 62, 32 }};
		int btmList[] = {31, 28, 25, 23, 20, 17, 7, 9, 5, 11, 3, 13, 1, 14 };
		int btmPinsGrade[][4] = {{40, 144, 0, 0}, {44, 31, 55, 40}, {52, 30, 70, 28}, {46, 31, 56, 42}, {50, 34, 60, 24}, {47, 39, 44, 42}, {48, 40, 40, 35}, {46, 29, 58, 37}, {39, 133, 0, 0}, {44, 33, 53, 41}, {50, 31, 58, 34}, {47, 30, 60, 39}, {4, 45, 77, 52}, {4, 43, 91, 57}};
		
		do{
			needupdate = cooradinate(macros, topDie, rawnet, roatation, topList, topPinsGrade);
			roatation++;
		}while (needupdate && roatation < 16384);

		roatation = 0;

		do{
			needupdate = cooradinate(macros, btmDie, rawnet, roatation, btmList, btmPinsGrade);
			roatation++;
		}while (needupdate && roatation < 16384);
		
		updatePinsInMacroInfo(macros, pinsInMacros, instances);
		
		writeVisualFile(macros, 999, topDie);
		wirteNodes(instances, macros);
		wirtePl(instances, macros, topDie);
		writeNet(macros, pinsInMacros, rawnet, instances);
		writeRow(macros, topDie, btmDie);
		
	}

	/*	first placement and CG preprocessing	*/
	
	if (stdCellPart){
		system("rm hpwl.txt");
		float gamma, penaltyWeight, totalScore = 0.0, newScore = 0.0;
		float wireLength, newWireLength;

		float *lastCG = new float[ numStdCells * 3 ]{0.0};
		float *nowCG = new float[ numStdCells * 3 ]{0.0};
		float *lastGra = new float[ numStdCells * 3 ]{0.0};
		float *nowGra = new float[ numStdCells * 3 ]{0.0};

		int qqq = -1;

		stdCellFirstPlacement(instances, macros, binInfo, topDie);
		updatePinsInMacroInfo( macros, pinsInMacros, instances);

		gamma = 0.05 * binInfo.dieWidth;
		penaltyWeight = returnPenaltyWeight(rawnet, gamma, instances, binInfo, densityMap);
		penaltyWeight = 1e-7;
		
		/*	Refinement(CG)	*/

		startTime = time(NULL);
		
		for(int i = 0; i < totalIter; i++)
		{
			totalScore = returnTotalScore( rawnet, gamma, binInfo, penaltyWeight, instances, densityMap, fillers);
			updateGra(rawnet, gamma, instances, binInfo, lastGra, nowGra, lastCG, nowCG, penaltyWeight, densityMap, fillers);
			CGandGraPreprocessing(instances, nowGra, nowCG, lastGra, lastCG);

			if( !useEplace )
			{
				for(int j = 0; j < 300; j++)
				{
					qqq++;
					// mvFiller(fillers, binInfo);
					newSolution(instances, nowCG, binInfo);
					updatePinsInMacroInfo( macros, pinsInMacros, instances);
					writeVisualFile(instances, qqq, topDie);

					newScore = returnTotalScore( rawnet, gamma, binInfo, penaltyWeight, instances, densityMap, fillers);
					
					if( newScore > totalScore)
						break;
					else if(OvRatio(instances, binInfo))
						break;
					else
						totalScore = newScore;

					updateGra(rawnet, gamma, instances, binInfo, lastGra, nowGra, lastCG, nowCG, penaltyWeight, densityMap, fillers);
					conjugateGradient(nowGra, nowCG, lastCG, lastGra, numStdCells, 1);
				}
				
				// refPosition(instances);
				if(penaltyWeight < 4)
					penaltyWeight *= 2;
			}
			else
			{

				float optParam = 1.0;
				float *curRefSoltion = new float [numStdCells *3] {0.0};
    			float *newRefSolution = new float [numStdCells *3] {0.0};
    
				for(int j = 0; j < totalIter; j++)
				{	
					clacBktrk(instances, lastGra, nowGra, j, &optParam, rawnet, gamma, binInfo, lastCG, nowCG, 
							  penaltyWeight, densityMap, curRefSoltion, newRefSolution);
					writeVisualFile(instances, j, topDie);
				}
			}

			if( OvRatio(instances, binInfo) )
				break;
			
		}

		endTime = time(NULL);
		finalUpdatePinsInMacro( macros, pinsInMacros, instances);
		newScore = returnTotalScore( rawnet, gamma, binInfo, 1, instances, densityMap, fillers);
		printf("Time: %lds, iter: %d, score: %f\n", (endTime - startTime), qqq , newScore);
		updateGra(rawnet, gamma, instances, binInfo, lastGra, nowGra, lastCG, nowCG, penaltyWeight, densityMap, fillers);

	}
	
	if(false)
	{	
		
		system("./hw4 ./test/test.aux");
		system("./hw4 ./test/test2.aux");
		system("rm visulization/graph/*");
		sleep(3);

		readAbcusResult(instances, 1);
		readAbcusResult(instances, 0);

		// place2BestRow(instances, numStdCells, topDie, btmDie, macros);
		insertTerminal(instances, rawnet, terminals, terminalTech, topDie);
		writeVisualFile(instances, 0, topDie);
		writeFile(instances, rawnet, numStdCells, terminals);
		

	}

	return 0;
}
