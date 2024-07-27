#include <stdio.h>
#include <vector>
#include <cmath>

#include "CG.h"
#include "readfile.h"



using namespace std;

double socreOfX( vector <RawNet> rawNet, double gamma)
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

double socreOfY( vector <RawNet> rawNet, double gamma)
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
    double distance = std::abs(z - layer);

    if (distance <= 0.5) {
        return 1 - 2 * (distance * distance);
    } else if (distance > 0.5 && distance <= 1) {
        return 2 * ((distance - 1) * (distance - 1));
    } else {
        return 0;
    }
}

