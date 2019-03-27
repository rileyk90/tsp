/*
 * Trevor Rollins, Riley Kraft, Andrew Tjossem
 * CS 325
 * TSP with Christofides
 */
//#include <cstdlib>
//#include <vector>
//#include <string>
#include <functional>
#include <queue>
//#include <ctime>

using std::priority_queue;
using std::vector;
using std::string;

#ifndef CHRISTOFIDES_TSP_H
#define CHRISTOFIDES_TSP_H

#define INF 100000000000000;

class CHR{

    class City {
    public:
        int id, x, y;
    };

    class Vertex {
    public:
        int id, parent;
        long key;
        Vertex(int id = -1);
    };

private:
    clock_t start;
    long timeLimit;
    int n;  //Number of Cities
    string outputFileName;
    vector<City> cities;
    vector<Vertex> vertices;
    vector<int> odds;
    int** adjMatrix;
    vector<Vertex> MSTSet;
    vector<vector<int>> MSTAdjList;
    vector<int> EulerCycle;
    vector<int> TSPPath;
    long pathLength;

    void readInput(string fileName);
    bool checkTime();
    int distance(City city1, City city2);
    void fillAdjMatrix();
    void initVertices();
    int findMin();
    void createMST();
    void MSTtoAdjList();
    void findOddVertices();
    void matchOdds();
    void EulerTour();
    void EulertoTSP();
    void twoOpt();
    void checkNewPath(int A, int B, int C, int D);
    void swapBC(int B, int C);

    int* christofides;



public:
    int* getChristofides();  //returns array with path
    long getDistance();
    void outputFile();
    void createTSPPath();  //creates output file and runs 2-opt.  Can't be run without input file
    CHR(string fileName, clock_t start, long timeLimit);  //Input file constructor
    CHR(int**, int);  //distance matrix constructor
    virtual ~CHR();

};


#endif //CHRISTOFIDES_TSP_H
