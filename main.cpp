/********************************************************************************
 * Author: Riley Kraft
 * Date: 03/25/2019
 * Description: This is the main function to set up the TSP problem and call
 *              the GLS + FLS-2opt algorithm to find the optimal, or approximate,
 *              tour.
 * SOURCES
 *   https://developers.google.com/optimization/routing/tsp
 *   https://www.geeksforgeeks.org/traveling-salesman-problem-tsp-implementation/
 *   http://www.cleveralgorithms.com/nature-inspired/stochastic/guided_local_search.html
 *   http://www.lia.disi.unibo.it/Staff/MicheleLombardi/or-tools-doc/user_manual/manual/metaheuristics/GLS.html
 *   http://www.technical-recipes.com/2012/applying-c-implementations-of-2-opt-to-travelling-salesman-problems/
 *   http://cswww.essex.ac.uk/CSP/glsdemo.html/tsp_x11.tar.Z
 *   http://www.cplusplus.com/reference/algorithm/random_shuffle/
********************************************************************************/
#include "tsp.hpp"
#include "gls.hpp"

int main(int argc, char *argv[]){
  srand(time(NULL));                                  // Seed random functions
  string fileName = argv[1];                          // Get file name from user input

  GLS obj;                                            // Create an algorithm object
  if(obj.TSP::setProblem(fileName.c_str())){            // If algorithm successfully initializes the tour
	obj.initialize();                                   // Initialize dynamic arrays
    obj.TSP::runAlgorithm();                            // Run the algorithm
  }

  return 0;                                           // End program
}
