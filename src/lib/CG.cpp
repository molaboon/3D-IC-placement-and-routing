#include <stdio.h>
#include <vector>
#include <cmath>

#include "CG.h"
#include "readfile.h"

#define eta 0.1
#define total_layer 2

using namespace std;

double scoreOfX( vector <RawNet> rawNet, double gamma)
{
    double score = 0.0;

    for (int net = 0; net < rawNet.size(); net++)
    {
        double numerator_1 = 0.0;
        
        double denominator_1 = 0.0;
        
        double numerator_2 = 0.0;

        double denominator_2 = 0.0;


        for (int instance = 0 ; instance < rawNet[net].Connection.size(); instance++)
        {

            double tmp = rawNet[net].Connection[instance].x;

            numerator_1 += tmp * exp(tmp / gamma);
            
            denominator_1 += exp(tmp / gamma);

            numerator_2 += tmp * exp(-tmp/ gamma);
            
            denominator_2 += exp(-tmp/ gamma);
        }

        score += numerator_1/denominator_1 - numerator_2 / denominator_2 ;

    }

    return score;

}

double scoreOfY( vector <RawNet> rawNet, double gamma)
{
    double score = 0.0;

    for (int net = 0; net < rawNet.size(); net++)
    {
        double numerator_1 = 0.0;
        
        double denominator_1 = 0.0;
        
        double numerator_2 = 0.0;

        double denominator_2 = 0.0;


        for (int instance = 0 ; instance < rawNet[net].Connection.size(); instance++)
        {

            double tmp = rawNet[net].Connection[instance].y;

            numerator_1 += tmp * exp(tmp / gamma);
            
            denominator_1 += exp(tmp / gamma);

            numerator_2 += tmp * exp(-tmp/ gamma);
            
            denominator_2 += exp(-tmp/ gamma);
        }

        score += numerator_1/denominator_1 - numerator_2 / denominator_2 ;

    }

    return score;

}

double bellShapeFunc(double z, double layer)
{     
    double distance = abs(z - layer);

    if (distance <= 0.5) 
    {
        return 1.0 - 2.0 * (distance * distance);

    } else if (distance > 0.5 && distance <= 1) {

        return 2.0 * ((distance - 1.0) * (distance - 1.0));
    
    } else 
    {
        return 0.0;
    }
}

double RSum(double z){
    
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

double scoreOfDensity(double z, double layer)
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

        score = scoreOfDensity(z, (double)i);

        psi += score;
    }

    return psi;
}

double TSVofNet( vector <RawNet> rawNet, double gamma)
{
    double score = 0.0;

    for (int net = 0; net < rawNet.size(); net++)
    {
        double numerator_1 = 0.0;
        
        double denominator_1 = 0.0;
        
        double numerator_2 = 0.0;

        double denominator_2 = 0.0;


        for (int instance = 0 ; instance < rawNet[net].Connection.size(); instance++)
        {

            double tmpPsi = returnPsi(rawNet[net].Connection[instance].z);

            numerator_1 += tmpPsi * exp(tmpPsi / gamma);
            
            denominator_1 += exp(tmpPsi / gamma);

            numerator_2 += tmpPsi * exp(-tmpPsi/ gamma);
            
            denominator_2 += exp(-tmpPsi/ gamma);
        }

        score += numerator_1/denominator_1 - numerator_2 / denominator_2 ;

    }

    return score;
}


