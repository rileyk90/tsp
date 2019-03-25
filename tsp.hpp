/********************************************************************************
 * Project Group 7: Riley Kraft, Trevor Rollins, Andrew Tjossem
 * Date: 06/08/2018
 * Description: The TSP class defines variables and functions to drive the
 *              TSP problem. Here, you can find implementations for reading
 *              in a list of cities' coordinates, calculating the distances
 *              between each city and all other cities, getting and setting
 *              the current tour as well as the best tour, keeping track of
 *              the number of algorithm iterations and running time, calculating
 *              the costs of altering the best tour, and for writing the best
 *              solution found to an output file.
 * Sources: see main.cpp header for program sources
********************************************************************************/
#ifndef TSP_HPP
#define TSP_HPP

#include <iostream>
#include <limits.h>
#include <math.h>
#include <fstream>

using std::string;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::min;
using std::max;


class TSP{
  public:
	int	maxGLS;              // Iteration limit
	double  maxTime;             // Time limit
	int	size;                // Size of problem (count of cities)
	int	iteration;           // Current iteration
	double  time;                // Current time
	int	tourCost;            // Current tour distance
	int	bestCost;            // Best tour distance

	int 	*x;                  // Cities' x values
	int 	*y;                  // Cities' y values
	int 	*tour;               // Current tour
	int	*best;               // Best tour
	int	**distance;          // Distances of all possible tour edges

	ifstream inputFile;          // Input file object
	ofstream outputFile;         // Output file object
	string fileName;             // Input file name

	clock_t start;               // Algorithm start time
	clock_t lap;                 // Algorithm iteration lap time

  public:
    	TSP();                                     // Constructor
	virtual	~TSP();                            // Deconstructor

	void setGlsIterations(int);                // Set max iterations of running GLS
    	void setMaxTime(double);                   // Set max time for running algorithm
	double getDuration();                      // Get algorithm elapsed time

	bool setProblem(string);                   // Initialize record of cities
	void initialize();                         // Initialize dynamic arrays
	void setRandomTour(int *);                 // Generate a random initial tour

	void runAlgorithm();                       // Run the algorithm
	virtual void guidedLocalSearch();          // Driving algorithm

	int getNext(int);                          // Get city's next neighbor
	int getPrev(int);                          // Get city's previous neighbor

	int getTourCost(int*);                     // Get cost of current tour
	void setBest();                            // Set best tour and cost

	bool hasReachedLimit();                    // Check if algorithm has reached iteration or time limit
	void outputResults();                      // Write best tour to output file
};
#endif


