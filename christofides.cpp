/*
 * Trevor Rollins, Riley Kraft, Andrew Tjossem
 * CS 325
 * TSP with Christofides
 */

#include "christofides.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
#include <set>
#include <stack>
using std::stack;
using std::ifstream;
using std::cout;
using std::endl;
using std::set;


//Constructor to initialize values in Vertex
//Used to find MST
CHR::Vertex::Vertex(int id)
{
    this->id = id;
    parent = -1;
    key = INF;
}

//CHR constructor if input file must be read.
CHR::CHR(string fileName, clock_t start, long timeLimit){
    this->timeLimit = timeLimit;
    this->start = start;
    readInput(fileName);  //Read input file
    outputFileName = fileName + ".tour";  //set name for output file
    fillAdjMatrix();  //Init adjacency matrix
    initVertices();
}

//Constructor for the CHR class if number of cities
//and distance between each pair of cities is known.
//n is the number of cities, and distance is a matrix
//that holds the distance between any pair of citites.
CHR::CHR(int** distance, int size)
{
	n = size;
	adjMatrix = distance;
    initVertices();
}

//Reads input file and sets appopriate member varaibles
void CHR::readInput(string fileName)
{
    ifstream inFile(fileName, std::ios::in);
    if(!inFile)
    {
        cout<<"Failed file opening"<<endl;
        exit(1);
    }

    int id, x, y;
    City tempCity;  //Will store the id, x, y values in one class

    while(!inFile.eof())
    {
        //Add values to tempCity
        inFile >> id >> x >> y;
        tempCity.id = id;
        tempCity.x = x;
        tempCity.y = y;
        cities.push_back(tempCity);
    }
    cities.pop_back();  //Removes duplicate entry at eof
    n = cities.size();
    inFile.close();
}

//This calculates and returns thei distance between 2 cities
int CHR::distance(City city1, City city2)
{
    double xSqrDiff = pow((city1.x - city2.x), 2);
    double ySqrDiff = pow((city1.y - city2.y), 2);

    return (floor((sqrt(xSqrDiff + ySqrDiff)) + 0.5));
}

//Fills adjacency matrix with all distances to save
//Time with lookup later
void CHR::fillAdjMatrix()
{
    //Allocate adjMatrix
    adjMatrix = new int*[n];
    for(int i=0; i<n; i++)
    {
        adjMatrix[i] = new int[n];
    }

    for(int i=0; i<n; i++)
    {
        City* city1 = &cities[i];
        for(int j=0; j<n; j++)
        {
            City* city2 = &cities[j];
            if(city1->id == city2->id)
            {
                adjMatrix[i][j] = 0;  //If same city distance = 0
            } else{
                adjMatrix[i][j] = distance(*city1, *city2);  //Stores distance between city1 and city2
            }
        }
    }

}

//Inits the vertices array to store all cities for MST algorithm
void CHR::initVertices()
{
    vertices.resize((unsigned)n);
    for(int i=0; i<n; i++)
    {
        Vertex temp;
        temp.id = i;
        vertices[i] = temp;
    }
    //Set arbitrary starting location for MST algorithm
    vertices[0].key = 0;
}

//Looks through vertices array and returns index of
//Vertex with smallest key
int CHR::findMin()
{
    int min = 0;
    for(int i=0; i<vertices.size(); i++)
    {
        if(vertices[min].key > vertices[i].key)
        {
            min = i;
        }
    }
    return min;
}

//Uses Prim's algorithm to create an MST for the given graph
//Stores the result in MSTSet, which is collection of Vertex's
//each with storing their parent Vertex
void CHR::createMST()
{
    while(!vertices.empty())
    {
        int minIdx = findMin();  //Finds smallest key in vertices
        Vertex u = vertices[minIdx];
        vertices.erase(vertices.begin()+minIdx);  //Removes u from vertices
        MSTSet.push_back(u);  //Add u to the MST

        for(int i=0; i<vertices.size(); i++)
        {
            Vertex* v = &vertices[i];
            //The distance u->v is less than v.key
            //make u v's parent and update its key
            if(adjMatrix[u.id][v->id] < v->key)
            {
                v->parent = u.id;
                v->key = adjMatrix[u.id][v->id];
            }
        }
    }
}

//This converts the array of Vertex's
//To an adjacency list of the resulting MST
void CHR::MSTtoAdjList()
{
    MSTAdjList.resize((unsigned)n);
    for(Vertex v : MSTSet)
    {
        if (v.parent != -1)
        {
            //Adds both the child and parent to
            //each other's list
            MSTAdjList[v.id].push_back(v.parent);
            MSTAdjList[v.parent].push_back(v.id);
        }
    }
}

//Finds all the vertices with odd number of edges in
//the MST
void CHR::findOddVertices()
{
    for(int i=0; i<n; i++)
    {
        //If the list has an odd number of elements
        //That means the vertex has an odd degree
        if((MSTAdjList[i].size() % 2) != 0)
        {
            odds.push_back(i);
        }
    }
}

//This is edge matching greedy algorithm to
//Add edges to the odd nodes until there are none left.
//It tries to find a minimal edge cost
void CHR::matchOdds()
{
    findOddVertices();
    int neighbor;

    while(!odds.empty())
    {
        int idx;
        int odd1 = odds.back();
        odds.pop_back();
        long edge = INF;

        for(int i=0; i<odds.size(); i++)
        {
            int odd2 = odds[i];
            if(adjMatrix[odd1][odd2] < edge)
            {
                neighbor = odd2;
                edge = adjMatrix[odd1][odd2];
                idx = i;
            }
        }
        //Add matching edges to MST
        MSTAdjList[odd1].push_back(neighbor);
        MSTAdjList[neighbor].push_back(odd1);

        //Neighbor is no longer odd
        odds.erase(odds.begin()+idx);
    }
}


//Performs a Euler Tour and stores the results in EulerCycle
void CHR::EulerTour()
{
    int location = 0;
    stack<int> stk;

    //While location has remaining neighbors and the stack
    //Is not empty
    while((!MSTAdjList[location].empty()) || (!stk.empty()))
    {
        //location has neighbors
        if(!MSTAdjList[location].empty())
        {
            stk.push(location);
            int tempLoc = MSTAdjList[location].back();
            MSTAdjList[location].pop_back();  //Removes edge between location and neighbor
            location = tempLoc;
        }
        //location has no neighbors
        else
        {
            EulerCycle.push_back(location);
            location = stk.top();
            stk.pop();
        }
    }
}

//This converts EulerCycle to the final CHR path
//by skipping an city that was already visited
void CHR::EulertoTSP()
{
    set<int> used;

    for(int city : EulerCycle)
    {
        //Only add to TSPPath if city has not been visited
        if(used.count(city) == 0)
        {
            TSPPath.push_back(city);
            used.insert(city);  //add to visited cities
        }
    }
}

bool CHR::checkTime()
{
    clock_t timePoint = clock();
    double elapsed = (double)(timePoint - start) / (double)CLOCKS_PER_SEC;
    return (elapsed <= timeLimit);
}

//Reverses the positions of B and C
//and all cities between them
void CHR::swapBC(int B, int C)
{
    int size = TSPPath.size();

    while(C - B > 0)
    {
        int temp = TSPPath[B % size];
        TSPPath[B % size] = TSPPath[C % size];
        TSPPath[C % size] = temp;
        B++;
        C--;
    }
}

//Checks to see if path would be shorter if B and C are swapped
//Swaps if true
void CHR::checkNewPath(int A, int B, int C, int D)
{
    //if A->C + B->D < A->B + C->D
    if(adjMatrix[TSPPath[A]][TSPPath[C]]+adjMatrix[TSPPath[B]][TSPPath[D]] < adjMatrix[TSPPath[A]][TSPPath[B]]+adjMatrix[TSPPath[C]][TSPPath[D]])
    {
        swapBC(B, C);
    }
}

//Performs a 2-opt iterative improvement
//Continues until no more improvements are made
void CHR::twoOpt()
{
    long oldDistance = getDistance();
    long newDistance = oldDistance;
    int size = TSPPath.size();
    do {
        oldDistance = newDistance;
        for (int i = 0; (i < size) && (checkTime()); i++) {
            int A = i;
            int B = (i + 1) % size;

            for (int j = i + 2; (j + 1) % size != i; j++) {
                int C = j % size;
                int D = (j + 1) % size;
                checkNewPath(A, B, C, D);
            }
        }
        newDistance = getDistance();
    } while((newDistance < oldDistance) && (checkTime()));  //Continue while distance is still being shortened and time is not up
}

//Returns an allocated array of ints.  Each int refers to the
//a city idand together is the TSP path created by Christofide's algorithm
int* CHR::getChristofides()
{
    createMST();
    MSTtoAdjList();
    matchOdds();
    EulerTour();
    EulertoTSP();

    christofides = new int[TSPPath.size()];

    //Converting TSPPath vector into an
    //array of ints
    for(int i=0; i<TSPPath.size(); i++)
    {
        christofides[i] = TSPPath[i];
    }

    return christofides; //Must be deleted later
}

//This combines all of the functions need to create a
//TSP path using Christofide's algorithm.
//It then writes the results to the output file
void CHR::createTSPPath()
{
    createMST();
    MSTtoAdjList();
    matchOdds();
    EulerTour();
    EulertoTSP();
    twoOpt();
    pathLength = getDistance();
    outputFile();
}

//Takes the TSPPath and calculates the resulting
//total distance traveled.
long CHR::getDistance()
{
    int firstCity = TSPPath.front();
    int city1, city2;
    long distance = 0;
    for(int i=0; i<TSPPath.size(); i++)
    {
        city1 = TSPPath[i];
        if(i+1 < TSPPath.size())
        {
            city2 = TSPPath[i + 1];
            distance += adjMatrix[city1][city2];
        }
        else
        {
            city2 = firstCity;
            distance += adjMatrix[city1][city2];
        }
    }
    return distance;
}

//Writes the results to the output file
void CHR::outputFile()
{
    std::ofstream outFile(outputFileName);
    outFile << pathLength << endl;
    for(auto city : TSPPath)
    {
        outFile << city << endl;
    }
    outFile.close();
}

//Cleans up adjMatrix
CHR::~CHR()
{   /*
    for(int i=0; i<n; i++)
    {
        delete [] adjMatrix[i];
    }
    delete [] adjMatrix;
    */

    delete [] christofides;

}
