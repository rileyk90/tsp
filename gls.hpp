/********************************************************************************
 * Project Group 7: Riley Kraft, Trevor Rollins, Andrew Tjossem
 * Date: 06/08/2018
 * Description: The GLS class defines variables and functions to drive the
 *              GLS + FLS-2opt TSP algorithm. Here, you can find implementations
 *              for setting the driving variables of the algorithms, setting
 *              arrays and calculating values for the city and edge utilities
 *              and penalties, and functions driving the 2-opt algorithm, the
 *              FLS algorithm, and the GLS algorithm.
 * Sources: see main.cpp header for program sources
********************************************************************************/
#ifndef GLS_HPP
#define GLS_HPP

#include "tsp.hpp"

class GLS: public TSP{
  public:
    double alpha;          // Parameter to tune lambda
    int lambda;            // Parameter to tune algorithm
	int maxFLS;            // Maximum fast local search iterations

    int *utility;          // Edge utilities
    bool *bit;             // City penalty bits
    int **penalty;         // Edge penalties

    int costChange;        // Cost change by swapping edges
    int penaltyChange;     // Penalty change by swapping edges

  public:
    GLS();                                  // Constructor
    ~GLS();                                 // Deconstructor

    void setAlpha(double);                  // Set alpha variable
    void setLambda();                       // Calculate lambda
    void setFlsIterations(int);             // Set count of FLS iterations

    void initialize();                      // Initialize dynamic arrays
    void setPenalties();                    // Reset edge penalties
    void updatePenalties();                 // Update edge penalties

    int	getSwapCost(int, int);              // Calculate cost of swapping edges
    void twoOptSwap(int, int, int*);        // Swap edges
    bool twoOPT(int, int);                  // 2-opt algorithm
    void fastLocalSearch();                 // Fast Local Search algorithm
    void guidedLocalSearch();               // Guided Local Search algorithm
};
#endif


