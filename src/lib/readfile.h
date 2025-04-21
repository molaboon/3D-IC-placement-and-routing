#include <vector>
#include <cstdio>

using namespace std;
using std::vector;

#ifndef _READFILE_H_
#define _READFILE_H_

#define TECH_NAME_SIZE 10
#define PIN_NAME_SIZE 20
#define LIBCELL_NAME_SIZE 10
#define INSTANCE_NAME_SIZE 10
#define NET_NAME_SIZE 20

typedef struct _pin{
    int pinID;
    int pinLocationX;
    int pinLocationY;
}Pin;

typedef struct _Libcell{
    char libCellName[LIBCELL_NAME_SIZE]; //MC1,MC2...
    float libCellSizeX;
    float libCellSizeY;
    int pinCount;
    bool isMacro;
    vector <Pin> pinarray;
}Libcell;

typedef struct _tech_menu{
    char tech[TECH_NAME_SIZE]; //TA,TB
    int libcell_count;
    vector <Libcell> libcell;
}Tech_menu;

typedef struct _die{

    int lowerLeftX;
    int lowerLeftY;
    
    float upperRightX;
    float upperRightY;
    int MaxUtil;

    int startX;
    int startY;

    int repeatCount;
    
    int rowLength;
    int rowHeight;

    int numInstance;
    int index;
    vector <vector <int>> PlacementState;

}Die;

typedef struct _termina{
    int sizeX;
    int sizeY;
    int spacing;   //between terminals and between terminal and boundary
    int cost;
}Hybrid_terminal;

typedef struct _Instance{
    // char libPinName[PIN_NAME_SIZE];

    bool isMacro;
    bool canPass;

    int rotate;
    int instIndex = 0;

    float x;
	float y;
	float z;
    
    int layer;
    int finalX;
    int finalY;
    int finalWidth;
    int finalHeight;

    float tmpX;
    float tmpY;
    float tmpZ;
    float tmpD;

    float width;
    float height;
    float inflateWidth;
    float inflateHeight;
    float area;
    float inflateArea;

    float density = 0.0;

    float inflationRatio = 1.0;

    float gra_x = 0.0;
    float gra_y = 0.0;
    float gra_z = 0.0;
    float gra_d = 0.0;

    float refX = 0.0;
    float refY = 0.0;
    float refZ = 0.0;

    int numNetConnection;
    vector<int> connectedNet;

}instance;

typedef struct _NetConnection{
    char instName[INSTANCE_NAME_SIZE];      //C1, C2, ...
    char libPinName[PIN_NAME_SIZE];         //P1, P2, ...
    
}NetConnection;

typedef struct _RawNet{
    // char netName[NET_NAME_SIZE];
    int netIndex;
    int numPins;
    
    bool hasTerminal;
    int terminalX;
    int terminalY;
    
    float hpwlX;
    float hpwlY;
    
    vector < instance* > Connection;

}RawNet;

typedef struct _eachLevel
{
    int levelIndex;

    _eachLevel *nextLevel = NULL;
    _eachLevel *previousLevel = NULL;

}eachLevel;

typedef struct  _levelNets
{
    int numLevel;

    eachLevel *levels;

}levelNets;

void readTechnologyInfo(FILE *input, int *NumTechnologies, vector <Tech_menu> *TechMenu);
void printTechnologyInfo(int NumTechnologies, vector <Tech_menu> TechMenu);

void readDieInfo(FILE *input, Die *top_die, Die *bottom_die);
void printDieInfo(Die top_die, Die bottom_die);

void readHybridTerminalInfo(FILE *input, Hybrid_terminal *terminal);
void printHybridTerminalInfo(Hybrid_terminal terminal);

void readInstanceInfo(FILE *input, int *NumInstances, vector <instance> &instances, int *NumTechnologies, 
                        vector <Tech_menu> *TechMenu, vector <instance> &macros, vector <instance> &stdCells, 
                        vector< vector<instance> > &pinsInMacros);

void readNetInfo(FILE *input, int *NumNets, vector <RawNet> &rawnet, vector <instance> &InstanceArray, 
                vector <instance> &macros, vector <RawNet> &netsOfMacros, vector <int> &numStdCellConnectMacro,
                vector <vector<instance>> &pinsInMacro);

void readAbcusResult(vector <instance> &instances, int layer);

void readkj(vector <instance> &instances, vector <instance> &macros);

#endif
