# cdc17-c
C code for our CDC 2017 paper.

## Algorithms
#### MR_Search
MR_Search is the algorithm for multiagent sparse search from “**Multi-Agent Discrete Search with Limited Visibility**” by **H. Ding** and **D. Castanon**, Proc. IEEE Conference on Decision and Control (CDC), 2017.

MR_Search takes input from the prompt (number of sources, number of sinks, graph density), and generate an input file ('input.dimacs') in DIMACS format to be used in CS2 and LEMON.

#### CS2 (IG Systems CS2 Software)
> CS2 is the second version of a scaling algorithm for minimum-cost flow problems. For more detailed description, see "An Efficient Implementation of a Scaling Minimum-Cost Flow Algorithm" by A.V. Goldberg, J. Algorithms, Vol. 22 (1997), pp. 1--29.

> CS2 was developed by Andrew Goldberg (goldberg@intertrust.com) and Boris Cherkassky (cher@cher.msk.su).

> The program takes input in DIMACS format. 

Please refer to http://www.igsystems.com/cs2/index.html or https://github.com/iveney/cs2 for more information about CS2.

NOTE: CS2 requires integer-valued costs. The costs from mr_search are truncated to five digits and scaled to be integers.  This requires multiplying by 10^8, so the output of CS2 is scaled down by 10^-8.

#### LEMON
To use LEMON, one needs to install the LEMON graph library, preferrably under the HOME directory. Please refer to http://lemon.cs.elte.hu/trac/lemon for more information about LEMON.

Here we compare our MR_Search algorithm with two general min-cost flow algorithms implemented in LEMON: the Network Simplex algorithm and the Cost Scaling algorithm.

## How to run
A Makefile is included to make the programs. To make MR_Search, type `make mr_search`. To make CS2, type `make cs2`. To make LEMON, type `make lemon`. To make all three, type `make all`.

One needs to run mr_search first. You will be prompted with entering the input: number of sources, number of sinks, graph density. mr_search will randomly generate a problem and solves it. It will also generate an input file in the DIMACS format for the same problem for cs2 and lemon.

The order to run is then:
1. `cd cdc17_c`
2. `make all`
3. `./mr_search`
4. `./cs2` or `./lemon`

