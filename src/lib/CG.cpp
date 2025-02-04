#include <stdio.h>
#include <vector>
#include <cmath>
#include <string.h>
#include <iostream>

#include "initial_placement.h"
#include "CG.h"
#include "readfile.h"

#define eta 0.1
#define total_layer 2
#define alpha 10
#define h 0.00001
#define zChanged 1
#define znotChanged 0
#define Dimensions 3
#define spaceZweight 0.01
#define cellMoveSize 200
#define onlyPenalty 1

using namespace std;


double scoreOfX( const vector <RawNet> &rawNet, const double gamma, const bool isGra, instance graInstance, double originScore)
{
    double score = 0.0;

    if(isGra)
    {
        for(int net = 0; net < graInstance.numNetConnection; net++)
        {
            double n_1 = 0.0, d_1 = 0.0, n_2 = 0.0, d_2 = 0.0;
            double mn_1 = 0.0, md_1 = 0.0, mn_2 = 0.0, md_2 = 0.0;
            int nowNet = 0;

            for (int ins = 0 ; ins < rawNet[ graInstance.connectedNet[net] ].numPins; ins++)
            {
                double tmpGra = 0.0, tmpOri = 0.0;
                
                tmpGra = rawNet[ graInstance.connectedNet[net] ].Connection[ins]->tmpX;
                tmpOri = rawNet[ graInstance.connectedNet[net] ].Connection[ins]->x;

                n_1 += tmpOri * exp(tmpOri / gamma);
                d_1 += exp(tmpOri / gamma);
                n_2 += tmpOri * exp(-tmpOri/ gamma);
                d_2 += exp(-tmpOri/ gamma);

                mn_1 += tmpGra * exp(tmpGra / gamma);
                md_1 += exp(tmpGra / gamma);    
                mn_2 += tmpGra * exp(-tmpGra/ gamma);
                md_2 += exp(-tmpGra/ gamma);
            }
            score += ( mn_1/md_1 - mn_2/md_2 ) - ( n_1/d_1 - n_2/d_2 ); 
        }

        score = originScore + score; 

    }
    else{

        for (int net = 0; net < rawNet.size(); net++)
        {
            double numerator_1 = 0.0;
            double denominator_1 = 0.0;
            double numerator_2 = 0.0;
            double denominator_2 = 0.0;

            for (int instance = 0 ; instance < rawNet[net].numPins; instance++)
            {
                double tmp = 0.0;

                tmp = rawNet[net].Connection[instance]->x;

                numerator_1 += tmp * exp(tmp / gamma);
                denominator_1 += exp(tmp / gamma);
                numerator_2 += tmp * exp(-tmp/ gamma);
                denominator_2 += exp(-tmp/ gamma);
            }
            score += numerator_1/denominator_1 - numerator_2 / denominator_2 ;
        }
    }

    return score;
}

double scoreOfY( const vector <RawNet> &rawNet, const double gamma, const bool isGra, instance graInstance, double originScore)
{
    double score = 0.0;

    if(isGra)
    {
        for(int net = 0; net < graInstance.numNetConnection; net++)
        {
            double n_1 = 0.0, d_1 = 0.0, n_2 = 0.0, d_2 = 0.0;
            double mn_1 = 0.0, md_1 = 0.0, mn_2 = 0.0, md_2 = 0.0;

            for (int ins = 0 ; ins < rawNet[ graInstance.connectedNet[net] ].numPins; ins++)
            {
                double tmpGra = 0.0, tmpOri = 0.0;

                tmpGra = rawNet[ graInstance.connectedNet[net] ].Connection[ins]->tmpY;
                tmpOri = rawNet[ graInstance.connectedNet[net] ].Connection[ins]->y;

                n_1 += tmpOri * exp(tmpOri / gamma);
                d_1 += exp(tmpOri / gamma);
                n_2 += tmpOri * exp(-tmpOri/ gamma);
                d_2 += exp(-tmpOri/ gamma);

                mn_1 += tmpGra * exp(tmpGra / gamma);
                md_1 += exp(tmpGra / gamma);
                mn_2 += tmpGra * exp(-tmpGra/ gamma);
                md_2 += exp(-tmpGra/ gamma);
            }
            score += ( mn_1/md_1 - mn_2/md_2 ) - ( n_1/d_1 - n_2/d_2 ); 
        }

        score = originScore + score; 
    }
    else
    {
        for (int net = 0; net < (int) rawNet.size(); net++)
        {
            double numerator_1 = 0.0;
            double denominator_1 = 0.0;
            double numerator_2 = 0.0;
            double denominator_2 = 0.0;

            for (int instance = 0 ; instance < rawNet[net].numPins; instance++)
            {
                double tmp = 0.0;

                tmp = rawNet[net].Connection[instance]->y;

                numerator_1 += tmp * exp(tmp / gamma);
                denominator_1 += exp(tmp / gamma);
                numerator_2 += tmp * exp(-tmp/ gamma);
                denominator_2 += exp(-tmp/ gamma);
            }
            score += numerator_1/denominator_1 - numerator_2 / denominator_2 ;
        }
    }
    return score;
}

double bellShapeFunc( const double z, const double layer)
{     
    double distance = abs(z - layer);

    if (distance <= 0.5) 
        return 1.0 - 2.0 * (distance * distance);

    else if (distance > 0.5 && distance <= 1) 
        return 2.0 * ((distance - 1.0) * (distance - 1.0));
    
    else 
        return 0.0;
}

double RSum( const double z){
    
    double rsum = 0.0 ;
    
    // double eta = 0.1 ;

    for (int i = 0 ; i < total_layer; i++)
    {
        double l = (double) i;
        double tmp = bellShapeFunc(z, l);

        rsum += exp( tmp / eta);
    }
    return rsum;
}

double returnDensity(const double z, const double *densityMap)
{
    int index = 0;

    index = int(z/1e-5);

    return densityMap[index] ;
}

double returnDensityInDensityMap(const double z, const double layer)
{
    double Rz, Bz, BzRSum;

    Rz = bellShapeFunc(z, layer);
    Bz = exp(Rz/ eta);
    BzRSum = RSum(z);

    Bz = Bz / BzRSum;
    return Bz;
}

double returnPsi(double z)
{
    double psi = 0;
    
    for(int i = 0; i < total_layer; i++)
    {
        double score ;

        score = (double)i * returnDensityInDensityMap(z, (double)i);
        psi += score;
    }

    return psi;
}

double TSVofNet( vector <RawNet> &rawNet, bool isGra, instance graInstance, double originScore)
{
    double score = 0.0;
    int size = rawNet.size();

    if(isGra)
    {
        for(int net = 0; net < graInstance.numNetConnection; net++)
        {
            double n_1 = 0.0, d_1 = 0.0, n_2 = 0.0, d_2 = 0.0;
            double mn_1 = 0.0, md_1 = 0.0, mn_2 = 0.0, md_2 = 0.0;

            for (int ins = 0 ; ins < rawNet[ graInstance.connectedNet[net] ].numPins; ins++)
            {
                double tmpGra = 0.0, tmpOri = 0.0, tmpGraPsi = 0.0, tmpPsi = 0.0;
                
                tmpGra = rawNet[ graInstance.connectedNet[net] ].Connection[ins]->tmpZ;
                tmpOri = rawNet[ graInstance.connectedNet[net] ].Connection[ins]->z;

                tmpGraPsi = returnPsi(tmpGra);
                tmpPsi = returnPsi(tmpOri);

                n_1 += tmpPsi * exp(tmpPsi / 0.05);
                d_1 += exp(tmpPsi / 0.05);
                n_2 += tmpPsi * exp(-tmpPsi/ 0.05);
                d_2 += exp(-tmpPsi/ 0.05);

                mn_1 += tmpGraPsi * exp(tmpGraPsi / 0.05);
                md_1 += exp(tmpGraPsi / 0.05);
                mn_2 += tmpGraPsi * exp(-tmpGraPsi/ 0.05);
                md_2 += exp(-tmpGraPsi/ 0.05);
            }
            score += ( mn_1/md_1 - mn_2/md_2 ) - ( n_1/d_1 - n_2/d_2 ); 
        }

        score = originScore + score; 
    }
    else
    {
        for (int net = 0; net < size; net++)
        {
            double numerator_1 = 0.0;
            double denominator_1 = 0.0;
            double numerator_2 = 0.0;
            double denominator_2 = 0.0;

            for (int instance = 0 ; instance < rawNet[net].numPins; instance++)
            {
                double tmpPsi = 0.0;

                tmpPsi = returnPsi(rawNet[net].Connection[instance]->z);
               
                numerator_1 += tmpPsi * exp(tmpPsi / 0.05);
                denominator_1 += exp(tmpPsi / 0.05);
                numerator_2 += tmpPsi * exp(-tmpPsi/ 0.05);
                denominator_2 += exp(-tmpPsi/ 0.05);
            }
            score += numerator_1/denominator_1 - numerator_2 / denominator_2 ;
        }
    }
    
    return score;
}

double scoreOfz( vector <RawNet> &rawNets, vector <instance> &instances, gridInfo binInfo, bool zisChanged, double *densityMap)
{
    double score = 0.0;
    int size = instances.size();
    double *firstLayer = createBins(binInfo);
	double *secondLayer = createBins(binInfo);

    //  claculate score of penalty

    for(int i = 0; i < size; i++)
    {
        if(zisChanged)
        {
            double tmpD = returnDensity(instances[i].z, densityMap);

            instances[i].density = tmpD;
        }
        penaltyInfoOfinstance(instances[i], binInfo, firstLayer, secondLayer, false, false, 0);
    }
    score = scoreOfPenalty(firstLayer, secondLayer, binInfo);

    free(firstLayer);
    free(secondLayer);
        
    return score;
}

double *createBins(gridInfo binInfo)
{
    double *bins;

    bins = (double *)calloc( binInfo.binXnum * binInfo.binYnum, sizeof( double ) );

    return bins;
}

void penaltyInfoOfinstance( const instance instance, const gridInfo binInfo, double *firstLayer, double *secondLayer, bool isGra, bool needMinus, double *graGrade)
{
    int leftXnum, rightXnum, topYnum, btmYnum;
    int row = (int) binInfo.binXnum;

    double coordinate[4] = { 0.0 };
    int length[4] = {0};
    
    double leftX = 0, rightX = 0, topY = 0, btmY = 0;

    if(isGra)
    {
        leftX = instance.tmpX - (instance.width * 0.5);
        rightX = instance.tmpX + (instance.width * 0.5);
        topY = instance.tmpY - (instance.height * 0.5);
        btmY = instance.tmpY + (instance.height * 0.5);
    }
    else
    {
        leftX = instance.x - (instance.width * 0.5);
        rightX = instance.x + (instance.width * 0.5);
        topY = instance.y - (instance.height * 0.5);
        btmY = instance.y + (instance.height * 0.5);
    }
    
    leftXnum = (int) (leftX / binInfo.binWidth);
    rightXnum = (int) (rightX / binInfo.binWidth);
    topYnum = (int) (topY / binInfo.binHeight);
    btmYnum =  (int) (btmY / binInfo.binHeight);

    coordinate[0] = leftX;
    coordinate[1] = rightX;
    coordinate[2] = topY;
    coordinate[3] = btmY;

    length[0] = leftXnum;
    length[1] = rightXnum;
    length[2] = topYnum;
    length[3] = btmYnum;

    double grade[2] = {0.0};

    if(needMinus)
        calculatePenaltyArea( coordinate, length, firstLayer, secondLayer, row, instance, binInfo, needMinus, grade);
    
    else
        calculatePenaltyArea( coordinate, length, firstLayer, secondLayer, row, instance, binInfo, false, grade);

    if( graGrade != NULL)
        *graGrade = grade[0] + grade[1];

    // if_left_x_num = int( if_left_x // grid_info[2] )
    // if_right_x_num = int( if_right_x // grid_info[2])
    // if_top_y_num = int( if_top_y // grid_info[3] )
    // if_btm_y_num = int( if_btm_y // grid_info[3])     
    // if_coordinate = [if_left_x, if_right_x , if_top_y, if_btm_y]
    // if_length = [if_left_x_num, if_right_x_num, if_top_y_num, if_btm_y_num ]
}

void calculatePenaltyArea( double coordinate[], int *length, double *firstLayer, double *secondLayer, int row, instance instance, gridInfo binInfo, bool needMinus, double *grade)
{   
    // double routing_overflow;

    double density = instance.density;
    double firstLayerGrade = 0.0;
    double secondLayerGrade = 0.0;

    for (int y = 0; y <= length[3] - length[2]; ++y) 
    {
        double y_length, x_length;
        if (length[3] == length[2]) 
            y_length = coordinate[3] - coordinate[2];

        else if (y == 0)  // top bin
            y_length = binInfo.binHeight - fmod(coordinate[2], binInfo.binHeight);
    
        else if (y == length[3] - length[2])  // btm bin
            y_length = fmod(coordinate[3], binInfo.binHeight);
    
        else
            y_length = binInfo.binHeight;

        for (int x = 0; x <= length[1] - length[0]; ++x) 
        {
            if (length[1] == length[0]) {
                x_length = coordinate[1] - coordinate[0];
    
            } else if (x == 0) { // left bin
                x_length = binInfo.binWidth - fmod(coordinate[0], binInfo.binWidth);
    
            } else if (x == length[1] - length[0]) { // right bin
                x_length = fmod(coordinate[1], binInfo.binWidth);
    
            } else { // other bins
                x_length = binInfo.binWidth;
            }

            int bin_index = (length[0] + x) + (row * (length[2] + y));

            if(bin_index >= binInfo.binXnum * binInfo.binYnum)
                cout << instance.instIndex << endl;

            if(needMinus)
            {
                firstLayerGrade += (binInfo.binHeight * binInfo.binWidth - firstLayer[bin_index] ) * (binInfo.binHeight * binInfo.binWidth - firstLayer[bin_index] );
                secondLayerGrade += (binInfo.binHeight * binInfo.binWidth - secondLayer[bin_index] ) * (binInfo.binHeight * binInfo.binWidth - secondLayer[bin_index] );
                
                firstLayer[bin_index] -= y_length * x_length * density;
                secondLayer[bin_index] -= y_length * x_length * (1.0 - density);
            }
            else
            {
                firstLayer[bin_index] += y_length * x_length * density ;
                secondLayer[bin_index] += y_length * x_length * (1.0 - density) ;
                
                firstLayerGrade += (binInfo.binHeight * binInfo.binWidth - firstLayer[bin_index] ) * (binInfo.binHeight * binInfo.binWidth - firstLayer[bin_index] );
                secondLayerGrade += (binInfo.binHeight * binInfo.binWidth - secondLayer[bin_index] ) * (binInfo.binHeight * binInfo.binWidth - secondLayer[bin_index] );
                
            }
            // routing_overflow = ((x_length + y_length) / 2.0) * y_length * x_length;
        }
    }

    grade[0] = firstLayerGrade;
    grade[1] = secondLayerGrade;

    // instance.inflate_ratio = inflation_ratio(instance, routing_overflow / (instance.width * instance.height));
    // instance.inflate_ratio = 1;
}

double scoreOfPenalty(double *firstLayer, double *secondLayer, gridInfo binInfo)
{
    double score = 0.0;
    double area = binInfo.binWidth * binInfo.binHeight;
    
    int binXnum = (int) binInfo.binXnum;
    int binYnum = (int) binInfo.binYnum;

    // if bin area > die area
    double overAreaOfX = 0.0;
    double overAreaOfY = 0.0;
    double overAreaOfXY = 0.0;

    // if( binInfo.binXnum * binInfo.binWidth > binInfo.dieWidth )
    //     overAreaOfX = (binInfo.dieWidth - (binInfo.binXnum * binInfo.binWidth) ) * binInfo.binHeight;
    // else
    //     overAreaOfX = area;
    
    // if( binInfo.binYnum * binInfo.binHeight > binInfo.dieHeight )
    //     overAreaOfY = (binInfo.dieHeight - (binInfo.binYnum * binInfo.binHeight) ) * binInfo.binWidth;
    // else
    //     overAreaOfY = area;
    
    // if(binInfo.binYnum * binInfo.binHeight > binInfo.dieHeight && binInfo.binXnum * binInfo.binWidth > binInfo.dieWidth)
    //     overAreaOfXY = (binInfo.dieWidth - (binInfo.binXnum * binInfo.binWidth) ) * (binInfo.dieHeight - (binInfo.binYnum * binInfo.binHeight) );
    // else 
    //     overAreaOfXY = area;

    // for(int i = 0; i < binYnum; i++)
    // {
    //     for(int j = 0; j < binXnum; j++)
    //     {
    //         int b = j + i*binXnum;
            
    //         if(j == binXnum - 1 && i == binYnum - 1)
    //             score += (firstLayer[b] - overAreaOfXY) * (firstLayer[b] - overAreaOfXY) + (secondLayer[b] - overAreaOfXY) * (secondLayer[b] - overAreaOfXY);

    //         else if(j == binXnum - 1)
    //             score += (firstLayer[b] - overAreaOfX) * (firstLayer[b] - overAreaOfX) + (secondLayer[b] - overAreaOfX) * (secondLayer[b] - overAreaOfX);
            
    //         else if (i == binYnum - 1)
    //             score += (firstLayer[b] - overAreaOfY) * (firstLayer[b] - overAreaOfY) + (secondLayer[b] - overAreaOfY) * (secondLayer[b] - overAreaOfY);

    //         else
    //             score +=  (firstLayer[b] - area) * (firstLayer[b] - area) + (secondLayer[b] - area) * (secondLayer[b] - area);                
    //     }
    // }

    for(int i = 0; i < binYnum; i++)
    {
        for(int j = 0; j < binXnum; j++)
        {
            int b = j + i*binXnum;
            score +=  (firstLayer[b] - area) * (firstLayer[b] - area) + (secondLayer[b] - area) * (secondLayer[b] - area);                
        }
    }
    
    
    return score;    
}

void gradientX(vector <RawNet> &rawNet, const double gamma, vector <instance> &instances, gridInfo binInfo, const double penaltyWeight, const double xScore, const double penaltyScore, const double *originFirstLayer, const double *originSecondLayer)
{
    int size = instances.size();
    double *firstLayer = createBins(binInfo);
    double *secondLayer = createBins(binInfo);

    memcpy(firstLayer, originFirstLayer,  binInfo.binXnum * binInfo.binYnum * sizeof(double));
    memcpy(secondLayer, originSecondLayer,  binInfo.binXnum * binInfo.binYnum * sizeof(double));

    for(int i = 0; i < size; i++)
    {
        if(instances[i].canPass)
            continue;
        
        bool isGra, needMinus;
        double score = 0.0, score2 = 0.0, graGrade = 0.0;
        
        instances[i].tmpX = instances[i].x;
        instances[i].tmpX += h;
        
        score = scoreOfX(rawNet, gamma, true, instances[i], xScore);

        // Dedecut the original block(needMinus = ture) first and the add the gra one(isGra = true).

        score2 = penaltyScore;
        penaltyInfoOfinstance(instances[i], binInfo, firstLayer, secondLayer, isGra = false, needMinus = true, &graGrade);
        score2 -= graGrade;
        penaltyInfoOfinstance(instances[i], binInfo, firstLayer, secondLayer, isGra = true, needMinus = false, &graGrade);
        score2 += graGrade;
        penaltyInfoOfinstance(instances[i], binInfo, firstLayer, secondLayer, isGra = true, needMinus = true, &graGrade);
        penaltyInfoOfinstance(instances[i], binInfo, firstLayer, secondLayer, isGra = false, needMinus = false, &graGrade);

        instances[i].tmpX = instances[i].x;
        instances[i].gra_x =  onlyPenalty*(score - xScore)/h + ( penaltyWeight * ( score2 - penaltyScore ) ) / h;
    }
    free(firstLayer);
    free(secondLayer);
}

void gradientY(vector <RawNet> &rawNet, const double gamma, vector <instance> &instances, gridInfo binInfo, const double penaltyWeight, const double yScore, const double penaltyScore, const double *originFirstLayer, const double *originSecondLayer)
{
    int size = instances.size();
    
    double *firstLayer = createBins(binInfo);      
    double *secondLayer = createBins(binInfo);

    memcpy(firstLayer, originFirstLayer,  binInfo.binXnum * binInfo.binYnum * sizeof(double));
    memcpy(secondLayer, originSecondLayer,  binInfo.binXnum * binInfo.binYnum * sizeof(double));

    for(int i = 0; i < size; i++)
    {   
        if(instances[i].canPass)
            continue;

        bool needMinus = false, isGra = false;
        double score = 0.0, score2 = 0.0, graGrade = 0.0;

        instances[i].tmpY = instances[i].y;
        instances[i].tmpY += h;

        score = scoreOfY(rawNet, gamma, true, instances[i], yScore);

        score2 = penaltyScore;
        penaltyInfoOfinstance(instances[i], binInfo, firstLayer, secondLayer, isGra = false, needMinus = true, &graGrade);
        score2 -= graGrade;
        penaltyInfoOfinstance(instances[i], binInfo, firstLayer, secondLayer, isGra = true, needMinus = false, &graGrade);
        score2 += graGrade;            
        penaltyInfoOfinstance(instances[i], binInfo, firstLayer, secondLayer, isGra = true, needMinus = true, &graGrade);
        penaltyInfoOfinstance(instances[i], binInfo, firstLayer, secondLayer, isGra = false, needMinus = false, &graGrade);
        
        instances[i].tmpY = instances[i].y;
        instances[i].gra_y = ( onlyPenalty*(score - yScore) + penaltyWeight * (score2 - penaltyScore) ) / h;
    }
    
    free(firstLayer);
    free(secondLayer);
}

void gradientZ(vector <RawNet> &rawNet, const double gamma, vector <instance> &instances, gridInfo binInfo, const double penaltyWeight, const double zScore, const double penaltyScore, const double *originFirstLayer, const double *originSecondLayer, double *densityMap)
{   
    double *fl = createBins(binInfo);
    double *sl = createBins(binInfo);

    memcpy(fl, originFirstLayer,  binInfo.binXnum * binInfo.binYnum * sizeof(double));
    memcpy(sl, originSecondLayer,  binInfo.binXnum * binInfo.binYnum * sizeof(double));

    for(int i = 0; i < instances.size(); i++)
    {
        if(instances[i].canPass)
            continue;

        double tmpd = instances[i].density;
        double score , score2 = penaltyScore, graGrade = 0.0;
        bool needMinus = false;
        
        penaltyInfoOfinstance(instances[i], binInfo, fl, sl, false, true, &graGrade);
        score2 -= graGrade;

        instances[i].tmpZ = instances[i].z;
        instances[i].tmpZ += h;
        instances[i].density = returnDensity(instances[i].tmpZ, densityMap);

        score = TSVofNet(rawNet, true, instances[i], zScore);
     
        penaltyInfoOfinstance(instances[i], binInfo, fl, sl, false, false, &graGrade);
        score2 += graGrade;

        penaltyInfoOfinstance(instances[i], binInfo, fl, sl, false, needMinus = true, &graGrade);
        instances[i].tmpZ = instances[i].z;
        instances[i].density = tmpd;

        penaltyInfoOfinstance(instances[i], binInfo, fl, sl, false, needMinus = false, &graGrade);
        
        instances[i].gra_z = (score - zScore) / h;
        instances[i].gra_d = penaltyWeight * ( score2 - penaltyScore) / h;
    }

    free(fl);
    free(sl);
}

double infaltionRatio(instance instance, double routingOverflow)
{   
    return 0.0;
}

double returnTotalScore(vector<RawNet> &rawNet, const double gamma, const gridInfo binInfo, const double penaltyWeight, vector <instance> &instances, double *densityMap)
{
    double score_of_x, score_of_y = 0.0, score_of_z = 0.0, densityScore = 0.0, totalScore, wireLength;

    score_of_x = scoreOfX(rawNet, gamma, false, instances[0], 0);
    score_of_y = scoreOfY(rawNet, gamma, false, instances[0], 0);
    densityScore = scoreOfz(rawNet, instances, binInfo, 1, densityMap);
    score_of_z = TSVofNet(rawNet, false, instances[0], 0);

    totalScore = score_of_x + score_of_y + score_of_z * alpha + (densityScore) * penaltyWeight;
    wireLength = score_of_x + score_of_y;

    if(penaltyWeight == 1)
        printf("HPWL: %f, HBT: %f\n", wireLength, score_of_z);

    return totalScore;
}

void CGandGraPreprocessing( vector <instance> &instances, double *nowGra, double *nowCG, double *lastGra, double *lastCG)
{
    int size = instances.size();

    for(int i = 0; i < size; i++)
    {
        nowGra[i*3] = instances[i].gra_x;
        nowGra[i*3 + 1] = instances[i].gra_y;
        nowGra[i*3 + 2] = instances[i].gra_z + instances[i].gra_d;

        lastGra[i*3] = instances[i].gra_x;
        lastGra[i*3 + 1] = instances[i].gra_y;
        lastGra[i*3 + 2] = instances[i].gra_z + instances[i].gra_d;

        nowCG[i*3] = -instances[i].gra_x;
        nowCG[i*3 + 1] = -instances[i].gra_y;
        nowCG[i*3 + 2] = -(instances[i].gra_z + instances[i].gra_d);

        lastCG[i*3] = -instances[i].gra_x;
        lastCG[i*3 + 1] = -instances[i].gra_y;
        lastCG[i*3 + 2] = -(instances[i].gra_z + instances[i].gra_d);

        // printf("%lf, %lf, %lf, %lf, %lf, %lf\n", nowGra[i*3], nowGra[i*3 + 1], nowGra[i*3+2], nowCG[i*3], nowCG[i*3+1], nowCG[i*3+2]);
    }
}

void conjugateGradient(double *nowGra, double *nowCG, double *lastCG, double *lastGra, int Numinstance, int iteration)
{
    
    // for(int index = 0; index < Numinstance; index++)
    // {
    //     double beta = 0.0, norm = 0.0;
    //     for(int i = 0; i < Dimensions; i++)
    //     {
    //         int a = index * Dimensions + i;
    //         norm += fabs(lastGra[a]);
    //         beta += nowGra[a] * ( nowGra[a] - lastGra[a]);
    //     } 
            
    //     norm = norm * norm;
    //     beta = beta/norm;

    //     for(int i = 0; i < Dimensions; i++)
    //     {
    //         int a = index * Dimensions + i;
    //         nowCG[a] = (-nowGra[a]) + (beta * lastCG[a]);
    //     }
    // }

        
    double beta = 0.0, norm = 0.0;
    for(int index = 0; index < Numinstance * Dimensions; index++)
    {
        norm += fabs(lastGra[index]);
        beta += nowGra[index] * ( nowGra[index] - lastGra[index]);
    }

    norm = norm * norm;
    beta = beta/norm;
    
    if(beta < 0)
        beta = 0;

    for(int index = 0; index < Numinstance * Dimensions; index ++)
        nowCG[index] = (-nowGra[index]) + (beta * lastCG[index]);
    
}

double returnAlpha(double nowCG[])
{   
    double Alpha = 0.0;

    for(int i = 0; i < 3; i++)
        Alpha += nowCG[i] * nowCG[i];

    Alpha = sqrt(Alpha);
    Alpha =  1.0 / Alpha;

    return Alpha;
}

void glodenSearch(instance &inst, const gridInfo binInfo)
{   
    if( inst.x + inst.width/2 > binInfo.dieWidth)
        inst.x = binInfo.dieWidth - inst.width ;

    else if (inst.x - inst.width <= 0.0)
        inst.x = inst.width + 1.0;

    if(inst.y + inst.height/2> binInfo.dieHeight)
        inst.y = binInfo.dieHeight - inst.height;

    else if (inst.y - inst.height/2 <= 0.0)
        inst.y = 0.0 + inst.height + 1.0;

    if(inst.z > 1.0)
        inst.z = 1.0;

    else if(inst.z < 0.0)
        inst.z = 0.0;    
}

void newSolution(vector<instance> &instances, double *nowCG, grid_info binInfo)
{
    double score = 0.0, wireLength = 0.0, weight = 0.3;
    
    for(int index = 0; index < binInfo.Numinstance; index++)
    {
        if(instances[index].canPass)
            continue;
        
        double tmp[Dimensions] = {0.0};
        double Alpha = 1, spaceX, spaceY, spaceZ;

        tmp[0] = nowCG[index * Dimensions];
        tmp[1] = nowCG[index * Dimensions + 1] ;
        tmp[2] = nowCG[index * Dimensions + 2] ;

        // Alpha = returnAlpha(tmp);

        spaceX = tmp[0] * Alpha * weight * binInfo.binWidth / fabs(tmp[0]);
        spaceY = tmp[1] * Alpha * weight * binInfo.binHeight / fabs(tmp[1]);
        spaceZ = tmp[2] * Alpha * weight * 0.001 / fabs(tmp[2]);

        instances[index].refX = instances[index].x;
        instances[index].refY = instances[index].y;
        instances[index].refZ = instances[index].z;

        instances[index].x += spaceX;
        instances[index].y += spaceY;
        instances[index].z += spaceZ;

        glodenSearch(instances[index], binInfo);
    }

    for(int i = 0; i < (int) instances.size(); i++)
    {
        instances[i].tmpX = instances[i].x;
        instances[i].tmpY = instances[i].y;
        instances[i].tmpZ = instances[i].z;
    }
}

void updateGra(vector <RawNet> &rawNets, double gamma, vector<instance> &instances, grid_info &binInfo, double *lastGra, double *nowGra, double *lastCG, double *nowCG, double &penaltyWeight, double *densityMap)
{
    double xScore, yScore, zScore, penaltyScore, tmpScore = 0.0;
    double *originFirstLayer = createBins(binInfo);
    double *originSecondLayer = createBins(binInfo);
    double numInstances = instances.size();

    xScore = scoreOfX(rawNets, gamma, false, instances[0], 0);
    yScore = scoreOfY(rawNets, gamma, false, instances[0], 0);
    
    for(int j = 0; j < numInstances; j++)
    {
        instances[j].density = returnDensity(instances[j].z, densityMap);
    }
        
        
    zScore = TSVofNet(rawNets, false, instances[0], 0);

    for(int j = 0; j < numInstances; j++)
        penaltyInfoOfinstance(instances[j], binInfo, originFirstLayer, originSecondLayer, false, false, NULL);
    
    penaltyScore = scoreOfPenalty(originFirstLayer, originSecondLayer, binInfo);  
    
    gradientX(rawNets, gamma, instances, binInfo, penaltyWeight, xScore, penaltyScore, originFirstLayer, originSecondLayer);
    gradientY(rawNets, gamma, instances, binInfo, penaltyWeight, yScore, penaltyScore, originFirstLayer, originSecondLayer);
    gradientZ(rawNets, gamma, instances, binInfo, penaltyWeight, zScore, penaltyScore, originFirstLayer, originSecondLayer, densityMap);
    

    memcpy( lastGra, nowGra, Dimensions * numInstances * sizeof(double) );
    memcpy( lastCG, nowCG, Dimensions * numInstances * sizeof(double) );

    for(int i = 0; i < numInstances; i++)
    {
        nowGra[i*3] = instances[i].gra_x;
        nowGra[i*3 + 1] = instances[i].gra_y;
        nowGra[i*3 + 2] = instances[i].gra_z + instances[i].gra_d;
    }

    free(originFirstLayer);
    free(originSecondLayer);
}

void returnDensityMap(double *densityMap)
{
    double z = 0.0;

    for(int i = 0 ; i < 100001; i++)
    {
        densityMap[i] = returnDensityInDensityMap(z, 0.0);
        z += 1e-5;
    }
}

void clacBktrk( vector <instance> &instances, double *lastGra, double *nowGra, int iteration, double *optParam, 
                vector <RawNet> &rawNets, double gamma, grid_info &binInfo, double *lastCG, double *nowCG, 
                double &penaltyWeight, double *densityMap, double *lastRefSoltion, double *curRefSoltion)
{
    double stepSize = 0.0;
    double newParam =  (1+ sqrt( 4.0 * *optParam * *optParam + 1.0) ) / 2;
    int numInstances = instances.size();

    double *curRefGra = new double [numInstances *3] {0.0};
    double *curUk = new double [numInstances *3] {0.0};
    double *newRefSolution = new double [numInstances *3] {0.0};
    double param = ((*optParam-1) / newParam);


    memcpy(curRefGra, nowGra, numInstances * 3 * sizeof(double));

    for(int i = 0; i < numInstances; i++)
    {
        curUk[i*3] = instances[i].x;
        curUk[i*3 + 1] = instances[i].y;
        curUk[i*3 + 2] = instances[i].z;

        curRefSoltion[i*3] = instances[i].refX;
        curRefSoltion[i*3+1] = instances[i].refY;
        curRefSoltion[i*3+2] = instances[i].refZ;
    }

    if(iteration == 0)
    {
        stepSize = 1;
        memcpy(lastRefSoltion, curRefSoltion, numInstances * 3 * sizeof(double));
    }
    else
        stepSize = calcLipschitz(lastRefSoltion, curRefSoltion, lastGra, nowGra, numInstances);
    

    while (true)
    {
        for(int i = 0; i < numInstances; i++)
        {
            double refX, refY, refZ;
            refX = curRefSoltion[i * 3] - curRefGra[i * 3] * stepSize;
            refY = curRefSoltion[i * 3 + 1] - curRefGra[i * 3 + 1] * stepSize;
            refZ = curRefSoltion[i * 3 + 2] - curRefGra[i * 3 + 2] * stepSize;

            instances[i].x = refX + (refX - curUk[i*3]) * param;
            instances[i].y = refY + (refY - curUk[i*3]) * param;
            instances[i].z = refZ + (refZ - curUk[i*3]) * param;

            glodenSearch(instances[i], binInfo);

            newRefSolution[i*3] = instances[i].x;
            newRefSolution[i*3+1] = instances[i].y;
            newRefSolution[i*3+2] = instances[i].z;
            
        }

        updateGra(rawNets, gamma, instances, binInfo, lastGra, nowGra, lastCG, nowCG, penaltyWeight, densityMap);

        double newStepsize = calcLipschitz(curRefSoltion, newRefSolution, curRefGra, nowGra, numInstances);
        
        if( 0.95 * stepSize <= newStepsize)
        {
            stepSize = newStepsize;
            break;
        }

        stepSize = newStepsize;
    }

    //Net's Method

    for(int i = 0; i < numInstances; i++)
    {   
        instances[i].x = curRefSoltion[i*3] - curRefGra[i*3] * stepSize;
        instances[i].y = curRefSoltion[i*3+1] - curRefGra[i*3+1] * stepSize;
        instances[i].z = curRefSoltion[i*3+2] - curRefGra[i*3+2] * stepSize;

        glodenSearch(instances[i], binInfo);

        instances[i].refX = instances[i].x + ( instances[i].x - curUk[i*3]) * param;
        instances[i].refY = instances[i].y + ( instances[i].y - curUk[i*3]) * param;
        instances[i].refZ = instances[i].z + ( instances[i].z - curUk[i*3]) * param;

        instances[i].x = instances[i].refX;
        instances[i].y = instances[i].refY;
        instances[i].z = instances[i].refZ;

        glodenSearch(instances[i], binInfo);

        instances[i].refX = instances[i].x;
        instances[i].refY = instances[i].y;
        instances[i].refZ = instances[i].z;
    }

    updateGra(rawNets, gamma, instances, binInfo, lastGra, nowGra, lastCG, nowCG, penaltyWeight, densityMap);

    for(int i = 0; i < numInstances; i++)
    {
        instances[i].x = curRefSoltion[i*3] - curRefGra[i*3] * stepSize;
        instances[i].y = curRefSoltion[i*3+1] - curRefGra[i*3+1] * stepSize;
        instances[i].z = curRefSoltion[i*3+2] - curRefGra[i*3+2] * stepSize;

        glodenSearch(instances[i], binInfo);

    }
    
    memcpy(lastRefSoltion, curRefSoltion, numInstances * 3 * sizeof(double));
    memcpy(lastGra, curRefGra, numInstances * 3 * sizeof(double));

    *optParam = newParam;

    free(curUk);
    free(curRefGra);
    free(newRefSolution);

}


double calcLipschitz(double *lastRefSolution, double *nowRefSolution, double *lastGra, double *nowGra, int numOfStdCell)
{
    double denomenator = 0.0;
    double numerator = 0.0;
    
    for (int i = 0; i < numOfStdCell*3; i++)
    {
        numerator += (nowRefSolution[i] - lastRefSolution[i]) * (nowRefSolution[i] - lastRefSolution[i]);
        denomenator += (nowGra[i] - lastGra[i]) * (nowGra[i] - lastGra[i]);
    }

    return sqrt(numerator)/sqrt(denomenator);
}

void refPosition(vector <instance> &instances)
{
    int size = instances.size();

    for(int i = 0; i < size; i++)
    {
        instances[i].x = instances[i].refX;
        instances[i].y = instances[i].refY;
        instances[i].z = instances[i].refZ;
    }
}