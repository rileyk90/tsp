/********************************************************************************
 * Project Group 7: Riley Kraft, Trevor Rollins, Andrew Tjossem
 * Date: 06/08/2018
 * Description: This is the implementation file for the TSP class. Here, you
 *              can find the initializations of class variables and definitions
 *              of class functions.
 * Sources: see main.cpp header for program sources
********************************************************************************/
#include <algorithm>
#include "tsp.hpp"

/********************************************************************************
 * Constructor initializes variables.
********************************************************************************/
TSP::TSP(){
  setMaxTime(179);              // Maximum time - specify 180 seconds or INT_MAX
  maxGLS = 0;                   // Initialize max GLS iterations to 0
  size = 0;                     // Initialize problem size to 0
  iteration = 0;                // Initialize current iteration to 0
  time = 0;                     // Initialize current elapsed time to 0
  tourCost = 0;                 // Initialize current tour cost to 0
  bestCost = 0;                 // Initialize best tour cost to 0
}

/********************************************************************************
 * Deconstructor deletes all dynamically allocated memory
********************************************************************************/
TSP::~TSP(){
  delete [] x;                     // Delete x values
  delete [] y;                     // Delete y values
  delete [] tour;                  // Delete current tour
  delete [] best;                  // Delete best tour
  for(int i = 0; i < size; i++){   // For each edge
 	delete [] distance[i];           // Delete distance
  }
  delete [] distance;              // Delete distances
}

/********************************************************************************
 * setGlsIterations() initializes the algorithm's max count of iterations.
********************************************************************************/
void TSP::setGlsIterations(int limit){
  maxGLS = limit;
}

/********************************************************************************
 * setMaxTime() initializes the algorithm time limit.
********************************************************************************/
void TSP::setMaxTime(double limit){
  maxTime = limit;
}

/********************************************************************************
 * getDuration() calculates and returns the algorithm's elapsed time.
********************************************************************************/
double TSP::getDuration(){
  lap = clock();
  double elapsed = (double)(lap - start) / (double) CLOCKS_PER_SEC;
  return elapsed;
}

/********************************************************************************
 * setProblem() reads in cities from user defined input file, creates dynamic
 * arrays for city coordinates, edge distances, current tour, and initial
 * tour, and calculates and records all possible edge distances.
********************************************************************************/
bool TSP::setProblem(string input){
  start = clock();                                      // Set program timer

  fileName = input;                                     // Initialize filename to user input

  inputFile.open(fileName);                             // Open file by name
  if(inputFile){                                        // If file exists
    string line;                                          // Holds file line
    while(!inputFile.eof()){                              // While haven't reached EOF
      getline(inputFile, line);                             // Get next line from file
      if(line != ""){                                       // If the line is not null
    	  size++;                                             // Increment the problem size
      }
    }
    inputFile.close();                                    // Close input file
  }
  else{                                                 // Else
    cout << "ERROR: FILE NOT FOUND" << endl;              // Print error message
	return false;                                         // End function by returning false
  }

  inputFile.open(fileName);                             // Open file again
  if(inputFile){                                        // If file opens
    if(size < 200){                                       // If problem size < 200
    	setGlsIterations(5000);                             // Set GLS iterations to 5,000
    }
    else if(size < 250){                                  // Else, if 200 < size < 300
    	setGlsIterations(10000);                            // Set GLS iterations to 10,000
    }
    else if(size < 400){                                  // Else, if 300 < size < 400
    	setGlsIterations(85000);                           // Set GLS iterations to 100,000
    }
    else if(size < 500){                                  // Else, if 400 < size < 500
    	setGlsIterations(200000);                           // Set GLS  iterations to 200,000
    }
    else if(size < 1000){                                 // Else, if 500 < size < 1,000
    	setGlsIterations(300000);                           // Set GLS iterations to 300,000
    }
    else{                                                 // Else
    	setGlsIterations(INT_MAX);                          // Set GLS iterations to maximum possible within time limit
    }

    int null;                                             // Holds city ID (not needed since we assume cities are in order)
    x = new int[size];                                    // Create array for x coordinate values
    y = new int[size];                                    // Create array for y coordinate values
    tour = new int[size];                                 // Create tour array to hold current order of cities
    best = new int[size];                                 // Create best array to hold best order of cities
    distance = new int*[size];                            // Create matrix to hold distances between each city

    for(int i = 0; i < size; i++){                        // For each city
      distance[i] = new int[size];                          // Create second dimension for distance to each city
    }
  
    int xValue, yValue;                                   // Hold  city's coordinates
    for(int i = 0; i < size; i++) {                       // For each city
      inputFile >> null >> xValue >> yValue;                // Read ID, x value and y value into variables
      x[i] = xValue;                                        // Assign x variable to corresponding city ID in x array
      y[i] = yValue;                                        // Assign y variable to corresponding city ID in y array
    }
  
    for(int i = 0; i < size; i++){                        // For each city
      for(int j = 0; j < size; j++) {                       // For each edge to every city
        double dist = round(sqrt(pow((x[i] - x[j]), 2) +      // Calculate Euclidean distance
        		      pow((y[i] - y[j]), 2)));
        distance[j][i] = dist;                                // Record distance from city i to j
        distance[i][j] = dist;                                // Record same distance from city j to i
      }
    }
    inputFile.close();                                    // Close input file
    return true;                                          // End function by returning true
  }
  else{                                                 // Else
    cout << "ERROR: FILE NOT FOUND" << endl;              // Print error message
    return false;                                         // End function by returning false
  }
}

/********************************************************************************
 * initialize() creates the first tour, gets the cost, and copies them to
 * the best tour.
********************************************************************************/
void TSP::initialize(){
  setRandomTour(tour);               // Call function to generate random first tour
  tourCost = getTourCost(tour);      // Get the cost of the tour
  setBest();                         // Set the best tour as current tour
}

/********************************************************************************
 * setRandomTour() generates a random permutation of the tour of cities.
********************************************************************************/
void TSP::setRandomTour(int *tour){
  for(int i = 0; i < size; i++){                  // For each city in tour
    tour[i] = i;                                    // Initialize ID to i
  }

  std::random_shuffle(&tour[0], &tour[size-1]);   // Call function to randomize tour order
}

/********************************************************************************
 * runAlgorithm() starts the algorithm timer, initiates the algorithm, keeps
 * track of the algorithm running limits, and calls functions to record the
 * finish time and output the algorithm results.
********************************************************************************/
void TSP::runAlgorithm(){
  while(!hasReachedLimit()){         // While algorithm hasn't reached iteration or time limit
      iteration++;                     // Increment count of GLS iterations
      guidedLocalSearch();             // Run guided local search
  }

  time = getDuration();              // Get running duration of algorithm
  outputResults();                   // Call function to print algorithm results
}

/********************************************************************************
 * guidedLocalSearch() is a pure virtual function defined in the GLS class.
********************************************************************************/
void TSP::guidedLocalSearch(){
	// pure virtual
}

/********************************************************************************
 * getNext() returns the argument city's next neighbor in the tour.
********************************************************************************/
int	TSP::getNext(int city){
  if(city != size-1){                  // If the city is not the last in the tour
    return city+1;                       // Return next city in the tour
  }
  else{                                // Else, city is last in the tour
    return 0;                            // Next city is first in the tour
  }
}

/********************************************************************************
 * getPrev() returns the argument city's previous neighbor in the tour.
********************************************************************************/
int TSP::getPrev(int city){
  if(city != 0){                      // If the city is not first in the tour
    return city-1;                      // Return previous city in the tour
  }
  else{                               // Else, city is first in the tour
    return size-1;                      // Previous city is last in the tour
  }
}

/********************************************************************************
 * getTourCost() calculates and returns the cost of the current tour.
********************************************************************************/
int TSP::getTourCost(int *tour){
  int cost = 0;                                     // Initialize cost to 0
  for(int i = 0; i < size; i++){                    // For each city
    cost += distance[tour[i]][tour[getNext(i)]];      // Calculate and add distance to next city
  }
  return cost;                                      // Return total cost
}

/********************************************************************************
 * setBest() copies current tour to best tour array, and copies current tour
 * cost to best tour cost.
********************************************************************************/
void TSP::setBest(){
  for(int i = 0; i < size; i++){     // For each city
    best[i] = tour[i];                 // Copy current tour city to best tour
  }
  bestCost = tourCost;               // Copy current tour cost to best tour cost
}

/********************************************************************************
 * hasReachedLimit() returns whether or not the algorithm has reached the
 * iteration or time limit set by the programmer.
********************************************************************************/
bool TSP::hasReachedLimit(){
  time = getDuration();
  return ((iteration >= maxGLS) || (time >= maxTime));
}

/********************************************************************************
 * outputResults() writes the best cost and tour of the TSP GLS algorithm to
 * an output file, and prints the final limits and best tour to the terminal.
********************************************************************************/
void TSP::outputResults(){
  fileName.append(".tour");                        // Append .tour to file name
  outputFile.open(fileName.c_str());               // Create output file

  outputFile << bestCost << "\n";                  // Write best cost to file
  for(int i = 0; i < size; i++){                   // For each city in best tour
    outputFile  << best[i] << "\n";                  // Write city identifier file
  }
  outputFile.close();                              // Close output file

  cout << "Iterations: " << iteration << "    "    // Print computing time
	"Best Tour: " << bestCost << "   "
	"Total Time(sec): " << time << endl;
}
