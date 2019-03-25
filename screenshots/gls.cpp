/********************************************************************************
 * Project Group 7: Riley Kraft, Trevor Rollins, Andrew Tjossem
 * Date: 06/08/2018
 * Description: This is the implementation file for the GLS class. Here, you
 *              can find initializations of class variables and definitions
 *              of class functions.
 * Sources: see main.cpp header for program sources
********************************************************************************/
#include "gls.hpp"

/********************************************************************************
 * Constructor initializes driving variables.
********************************************************************************/
GLS::GLS(){
  alpha = 0;                 // Initialize alpha value to 0
  setFlsIterations(20);      // Initialize FLS iterations to 20
  lambda = 0;                // Initialize lambda to 0
  costChange = 0;            // Initialize costChange to 0
  penaltyChange = 0;         // Initialize penaltyChange to 0
}

/********************************************************************************
 * Deconstructor
********************************************************************************/
GLS::~GLS(){
  delete [] utility;                 // Delete utilities
  delete [] bit;                     // Delete bits
  for(int i = 0; i < size; i++){     // For each city
	  delete [] penalty[i];            // Delete edge penalty
  }
  delete [] penalty;                 // Delete penalties
}

/********************************************************************************
 * setAlpha() initializes alpha variable to programmer's input.
********************************************************************************/
void GLS::setAlpha(double tune){
  alpha = tune;
}

/********************************************************************************
 * setFlsIteration() initializes FLS iteration limit to programmer's input.
********************************************************************************/
void GLS::setFlsIterations(int limit){
  maxFLS = limit;
};

/********************************************************************************
 * setLambda() calculates lambda variable based on values of alpha, problem
 * size, and current best tour cost.
********************************************************************************/
void GLS::setLambda(){
  lambda = round((alpha*(double)(bestCost/size)));
};

/********************************************************************************
 * initialize() creates dynamic arrays for GLS's utility and penalty features.
********************************************************************************/
void GLS::initialize(){
  TSP::initialize();                     // Call TSP class initializing function

  if(size < 500){                        // If problem size is < 500
	  setAlpha(0.167);                     // Set alpha to 0.167
  }
  else if(size < 2000){                  // Else, if 500 < size < 2000
	  setAlpha(0.3);                       // Set alpha to 0.3
  }
  else if(size <= 5000){                 // Else, if 2000 < size <= 5000
	  setAlpha(0.35);                      // Set alpha to 0.35
  }
  else{                                  // Else
	  setAlpha(0.5);                       // Set alpha to maximum, 0.5
  }

  utility = new int[size];               // Create dynamic array for utilities
  bit = new bool[size];                  // Create dynamic array for penalty bits
  penalty = new int*[size];              // Create dynamic matrix for edge penalties
  for(int i = 0; i < size; i++){         // For each city in the tour
    penalty[i] = new int[size];            // Create second dimension edge penalty
  }

  setPenalties();                        // Call function to initialize edge penalty values
}

/********************************************************************************
 * setPenalties() reinitializes city penalty bits and edge penalty values
 * for new run of GLS algorithm.
********************************************************************************/
void GLS::setPenalties(){
  for(int i = 0; i < size; i++) {     // For each city
    bit[i] = true;                      // Set penalty bit to true
    for(int j = 0; j < size; j++)       // For possible edge with city
      penalty[i][j] = 0;                    // Set penalty to 0
  }
}

/********************************************************************************
 * updatePenalties() calls function to calculate lambda, calculates and
 * assigns the maximum utility to the highest cost edges, then updates
 * subsequent edge penalty values and corresponding city penalty bits.
********************************************************************************/
void GLS::updatePenalties(){
  double current;                                              // Current calculated utility
  double max = 0;                                              // Max calculated utility
  int count = 0;                                               // Count of max utility cities

  setLambda();                                                 // Call function to set lambda

  for(int i = 0; i < size; i++){                               // For each city
     current = (double)(distance[tour[i]][tour[getNext(i)]])/    // Calculate utility of edge between city and next neighbor
    	    (double)(1 + penalty[tour[i]][tour[getNext(i)]]);
     if(current > max){                                          // If the city's utility is greater than the max utility found
       utility[0] = i;                                             // Add city to list of max utilities
       max = current;                                              // Assign as max utility
       count = 1;                                                  // Reset count of max utility cities
     }
     else if(current == max) {                                   // If city's utility matches the max utility found
       utility[count] = i;                                         // Add city to list of max utilities
       count++;                                                    // Increment count of max utility cities
     }
   }

  int c1, c2;
  for(int i = 0; i < count; i++){                              // For each max utility city
     c1 = tour[utility[i]];                                      // Get city's place in the current tour
     c2 = tour[getNext(utility[i])];                             // Get city's edge neighbor
     penalty[c1][c2]++;                                          // Increment penalty of edge
     penalty[c2][c1]++;                                          // Increment penalty of corresponding edge
     bit[c1] = bit[c2] = true;                                   // Set city's and neighbor's penalty bit to true
   }
}

/********************************************************************************
 * getSwapCost() calculates and returns the total distance and resulting penalty
 * costs of a potential edge swap.
********************************************************************************/
int GLS::getSwapCost(int c1,int c2){
  int c1n2 = getNext(c1);               // Get city1's next neighbor
  int c2n2 = getNext(c2);               // Get city2's next neighbor

  // Calculate distance change of swapping edges
  costChange = distance[tour[c1]][tour[c2]] + distance[tour[c1n2]][tour[c2n2]] -
	           distance[tour[c1]][tour[c1n2]] - distance[tour[c2]][tour[c2n2]];

  // Calculate penalty of swapping edges
  penaltyChange = (penalty[tour[c1]][tour[c2]] + penalty[tour[c1n2]][tour[c2n2]] -
				   penalty[tour[c1]][tour[c1n2]] - penalty[tour[c2]][tour[c2n2]])*lambda;

  // Return sum of cost and penalty changes
  return costChange + penaltyChange;
}

/********************************************************************************
 * twoOptSwap() finds the  nearest neighbor edges to swap for a city and its
 * neighbor passed into the function.
********************************************************************************/
void GLS::twoOptSwap(int city, int neighbor, int *tour){
  int c1 = min(city, neighbor);             // Get earliest city
  int c2 = max(city, neighbor);             // Get latest city
  int rightNeighbor = getNext(c1);          // Get right neighbor of earliest city
  int leftNeighbor = c2;                    // Set left neighbor as max city
  int temp1, temp2;                         // Hold neighbors for swapping

  while(rightNeighbor < leftNeighbor){      // While the right neighbor is earlier than the left neighbor
    if(rightNeighbor == size){                // If city1's right neighbor is equal to the count of cities
      rightNeighbor = 0;                        // Set to first city in tour
    }
    if(leftNeighbor == -1){                   // If city1's left neighbor is -1
      leftNeighbor = size-1;                    // Set to last city in tour
    }

    temp1 = tour[rightNeighbor];	          // Hold right neighbor
    temp2 = tour[leftNeighbor];               // Hold left neighbor
    tour[leftNeighbor--] = temp1;             // Swap left neighbor with right neighbor, go to next left neighbor
    tour[rightNeighbor++] = temp2;            // Swap right neighbor with left neighbor, got to next right neighbor
  }
}

/********************************************************************************
 * twoOPT() takes two cities found to be penalized in the FLS and calculates
 * the benefit of swapping their nearest neighbor edges to reduce the total
 * tour distance. If the swap is beneficial the function returns true, if not
 * the function returns false.
********************************************************************************/
bool GLS::twoOPT(int c1,int c2){
  int c1n1 = getPrev(c1);          // Get city1's previous neighbor
  int c1n2 = getNext(c1);          // Get city1's next neighbor
  int c2n1 = getPrev(c2);          // Get city2's previous neighbor
  int c2n2 = getNext(c2);          // Get city2's next neighbor

  if(getSwapCost(c1n1, c2) < 0){   // If the cost of swapping city1's previous neighbor with city2 is beneficial
  	bit[tour[c1n1]] = 1;             // Set city1's previous neighbor's penalty bit to true
  	bit[tour[c1]] = 1;               // Set city1's penalty bit to true
  	bit[tour[c2]] = 1;               // Set city2's penalty bit to true
  	bit[tour[c2n2]]= 1;              // Set city2's next neighbor's bit to true
  	twoOptSwap(c1n1, c2, tour);      // Swap city2 with city1's previous neighbor
	tourCost += costChange;          // Add costChange to the total tourCost
	if(bestCost > tourCost){         // If the tour cost is less than the current best tour cost
	  setBest();                       // Set the best tour cost to the current tour cost
	}
  	return true;                     // Return that beneficial swap was found
  }

  if(getSwapCost(c1, c2) < 0){     // If the cost of swapping city1 with city2 is beneficial
  	bit[tour[c1n2]] = 1;             // Set city1's next neighbor's penalty bit to true
  	bit[tour[c1]] = 1;               // Set city1's penalty bit to true
  	bit[tour[c2]] = 1;               // Set city2's penalty bit to true
  	bit[tour[c2n2]]=1;               // Set city2's next neighbor's bit to true
  	twoOptSwap(c1,c2, tour);         // Swap city1 with city2
	tourCost += costChange;          // Add cost gain to total tour cost
	if(bestCost > tourCost){         // If the tour cost is less than the current best tour
	  setBest();                       // Set the best tour cost to the current tour cost
	}
  	return true;                     // Return that beneficial swap was found
  }

  if(getSwapCost(c2n1, c1) < 0){   // If the cost of swapping city2's previous neighbor with city1 is beneficial
    bit[tour[c2n1]] = 1;             // Set city2's previous neighbor's penalty bit to true
    bit[tour[c1]] = 1;               // Set city1's penalty bit to true
    bit[tour[c2]] = 1;               // Set city2's penalty bit to true
    bit[tour[c1n2]]= 1;              // Set city1's next neighbor's bit to true
    twoOptSwap(c2n1, c1, tour);      // Swap city1 with city2's previous neighbor
  	tourCost += costChange;          // Add costChange to the total tourCost
  	if(bestCost > tourCost){         // If the tour cost is less than the current best tour cost
  	  setBest();                       // Set the best tour cost to the current tour cost
  	}
    return true;                     // Return that beneficial swap was found
    }

  return false;                    // Else, return that beneficial swap was not found
}

/********************************************************************************
 * fastLocalSearch() runs through a programmer defined number of iterations
 * for a current tour, finding cities that have been penalized by the GLS
 * and sending sending them to the 2-opt algorithm to determine if an
 * improvement to the tour can be made by swapping edges with their neighbors'.
********************************************************************************/
void GLS::fastLocalSearch(){
  int count = 0;                                        // Count of neighbors visited
  int city = 0;                                         // Tour index

  while(count < size && !hasReachedLimit()){            // While we haven't visited all the city's neighbors, and max time hasn't been met
    bool improved = false;                                // Track if swap was made
    if(bit[tour[city]]){                                  // If the penalty bit for the current node is set to true
      int neighbor = 0;                                     // Initialize neighbor city to first city in tour

      while((neighbor < size) && (!improved)){              // While there is a neighbor left to check and no beneficial swap has been found
	    if(neighbor != city &&                                // If neighbor is not the city
	       neighbor != getNext(city) &&                       // Or the neighbor just before the city
		   neighbor != getPrev(city)){                        // Or the neighbor just after the city
	      improved = twoOPT(city, neighbor);                    // Call function to make edge swap if beneficial
	    }
	    neighbor++;                                           // Go to next neighbor in the tour
	  }
    }
	
    if(improved){                                         // If the swap was beneficial
	  city = getPrev(city);                                 // Go to previous city in the tour
	  count = -1;                                           // Reset count to 0 for new city
    }
    else{                                                 // Else, swap was not beneficial
	  bit[tour[city]] = 0;                                  // Set city's penalty bit to false
	  city = getNext(city);                                 // Go to next city in the tour
    }
    count++;                                              // Increment count of visited neighbors
  }
}	

/********************************************************************************
 * guidedLocalSearch() runs the main TSP algorithm, keeping track of the
 * time and iterations limits, ass well as calling the functions to run
 * the FLS, calculate and set the tour's penalties, and output the final
 * solution.
********************************************************************************/
void GLS::guidedLocalSearch(){
  if(!hasReachedLimit()){                // If algorithm limits have not been met
	  iteration++;                         // Increment iteration of algorithm
      fastLocalSearch();                   // Call FLS to run
    if(iteration == maxFLS){               // If FLS has reached its limit of iterations
      setPenalties();                        // Reset the tour penalties for next iteration of GLS
    }
    updatePenalties();                     // Update the penalties for the current best tour
  }
  else{                                 // Else
    outputResults();                      // Call function to write final solution to file
  }
}
