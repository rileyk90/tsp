tsp: main.cpp tsp.cpp gls.cpp christofides.cpp
	g++ -g -std=c++11 main.cpp tsp.cpp gls.cpp christofides.cpp -o tsp

clean:
	rm tsp