# Knapsack for Benefit Aggregation Top-k
This project aims to implement the algorithms described in the paper, ["IO-Top-k: Index-access Optimized Top-k Query Processing"] .

> The current paper takes an integrated view of the scheduling
issues and develops novel strategies that outperform prior
proposals by a large margin. Our main contributions are
new, principled, scheduling methods based on a Knapsack related
optimization for sequential accesses and a cost model
for random accesses. The methods can be further boosted by
harnessing probabilistic estimators for scores, selectivities,
and index list correlations.

### Data Collections
This program uses a subset of the data extracted from [IMDB] containing the actors, genres, keywords, and movies lists. These subsets  contain data for 1,250,000 movies and 2,000,000 actors. This program parses each file and adds the relevant information to the data structures. Any issues with lines unable to be parsed or unable to be inserted into the data structures can be found in the log file, located at logs/imdb_error.log.

Information on obtaining the data is located [here][imdb-data].

*TV shows were removed from the genres, keywords, and movies lists for this program*

### Getting Started
To run this program, simply load up the Visual Studio solution file, build, and run! The Release configuration is best unless you need the features of the debugger; it runs 519% faster and uses 63% less memory.

### Notes on System Requirements
* Windows (64 bit) (tested on 8.1)
    * This program uses Windows exclusive libraries for memory mapping files and for parallel execution of functions.
    * The program uses over 2GB of RAM, thus 64-bit is required for it to work correctly.
* Visual Studio (created using Visual Studio Premium 2013)
* Multi-core processor
* 4GB RAM

### Version
0.5.0

### License
GPL2

["IO-Top-k: Index-access Optimized Top-k Query Processing"]: <http://www.vldb.org/conf/2006/p475-bast.pdf>
[IMDB]: <http://www.imdb.com/>
[imdb-data]: <http://www.imdb.com/interfaces>