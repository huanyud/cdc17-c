#include <cstdio>
#include <fstream>
#include <lemon/smart_graph.h>
#include <lemon/network_simplex.h>
#include <lemon/cost_scaling.h>
#include <lemon/capacity_scaling.h>
#include <lemon/dimacs.h>
#include "wintimer.c"

// #include "smart_graph.h"
// #include "network_simplex.h"
// #include "dimacs.h"

using namespace std;
using namespace lemon;

// This reads a dimacs format and calls network simplex to solve it
int main() {
    double t;

    // Create a directed graph
    DIGRAPH_TYPEDEFS(SmartDigraph);
    SmartDigraph g;

    // Create data structures (i.e. maps) associating values to nodes and arcs of the graph
    IntArcMap lower(g), capacity(g), cost(g);
    IntNodeMap supply(g);

    // Read DIMACS input file
    ifstream input("input.dimacs");
    readDimacsMin(input, g, lower, capacity, cost, supply);
    input.close();

    // Initialize NetworkSimplex algorithm object
    NetworkSimplex<SmartDigraph> ns(g);
    ns.lowerMap(lower).upperMap(capacity).costMap(cost).supplyMap(supply);

    // Run NetworkSimplex
    t = timer();
    ns.run();
    t = timer() - t;

    // Print total flow cost
    printf("Network Simplex total flow cost: %d\n", ns.totalCost());
    double Pd;
    Pd = (double) (0.00000001 * (-ns.totalCost()));
    printf("Final prob. detection: %lf \n", Pd);
    printf("Run time: %lf \n\n", t);

    // Now repeat with cost scaling
    CostScaling<SmartDigraph> cs(g);
    cs.lowerMap(lower).upperMap(capacity).costMap(cost).supplyMap(supply);
    // Run NetworkSimplex
    t = timer();
    cs.run();
    t = timer() - t;

    // Print total flow cost
    printf("Cost Scaling total flow cost: %d\n", cs.totalCost());
    Pd = (double) (0.00000001 * (-cs.totalCost()));
    printf("Final prob. detection: %lf \n", Pd);
    printf("Run time: %lf \n", t);

    return 0;
}
