#include <vector>

using namespace std;
using std::vector;

#ifndef _READFILE_H_
#define _READFILE_H_

#define TECH_NAME_SIZE 20
#define PIN_NAME_SIZE 20
#define LIBCELL_NAME_SIZE 20
#define INSTANCE_NAME_SIZE 20
#define NET_NAME_SIZE 20

typedef struct _pin{
    char pinName[PIN_NAME_SIZE]; //P1, P2, ...
    int pinLocationX;
    int pinLocationY;
}Pin;

typedef struct _Libcell{
    char libCellName[LIBCELL_NAME_SIZE]; //MC1,MC2...
    double libCellSizeX;
    double libCellSizeY;
    int pinCount;
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
    
    double upperRightX;
    double upperRightY;
    int MaxUtil;

    int startX;
    int startY;

    int repeatCount;
    
    int rowLength;
    int rowHeight;

    char tech[TECH_NAME_SIZE];
    vector <vector <int>> PlacementState;
}Die;

typedef struct _terminal{
    int sizeX;
    int sizeY;
    int spacing;   //between terminals and between terminal and boundary
    vector <vector<int>> HBPlacementState;
}Hybrid_terminal;

typedef struct _Instance{
    char tech[TECH_NAME_SIZE];              //TA, TB, ...
    char instName[INSTANCE_NAME_SIZE];      //C1, C2, ...
    char libCellName[LIBCELL_NAME_SIZE];    //MC1, MC2, ...
    char libPinName[PIN_NAME_SIZE];

    double x;
	double y;
	double z;

    double width;
    double height;

    double density;
    
    double inflateWidth;
    double inflateHeight;

    double inflationRatio;

    double gra_x;
    double gra_y;
    double gra_z;

}Instance;

typedef struct _NetConnection{
    char instName[INSTANCE_NAME_SIZE];      //C1, C2, ...
    char libPinName[PIN_NAME_SIZE];         //P1, P2, ...
    
}NetConnection;

typedef struct _RawNet{
    char netName[NET_NAME_SIZE];
    int numPins;
    
    vector < Instance* > Connection;

}RawNet;

typedef struct  _Net
{
    /* data */
}Net;


void readTechnologyInfo(FILE *input, int *NumTechnologies, vector <Tech_menu> &TechMenu);
void printTechnologyInfo(int NumTechnologies, vector <Tech_menu> TechMenu);

void readDieInfo(FILE *input, Die *top_die, Die *bottom_die);
void printDieInfo(Die top_die, Die bottom_die);

void readHybridTerminalInfo(FILE *input, Hybrid_terminal *terminal);
void printHybridTerminalInfo(Hybrid_terminal terminal);

void readInstanceInfo(FILE *input, int *NumInstances, vector <Instance> &InstanceArray, int *NumTechnologies, vector <Tech_menu> TechMenu );
void printInstanceInfo(int NumInstances, vector <Instance> InstanceArray);

void readNetInfo(FILE *input, int *NumNets, vector <RawNet> &rawnet, vector <Instance> &InstanceArray);
void printNetInfo(int NumNets, vector <RawNet> rawnet);

#endif
