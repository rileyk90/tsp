tsp: main.cpp tsp.cpp gls.cpp
	g++ -g -std=c++11 main.cpp tsp.cpp gls.cpp -o tsp

clean:
	rm tsp
