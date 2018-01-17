## Purpose

In order to port LayoutLibrary https://github.com/persistentsystems/LayoutLibrary to Linux, this repository is prepared.

Notice: at this stage, this library will crash with the simplest example on linux.


## LayoutLibrary - A C++ Library to Layout Clustred Graphs

### Features

1. Supports 4 layouting algorithms for clustred graphs
	* Hierarchical Layout
	* Force Directed Layout
	* Grid Based Layout
	* Circular Layout
2. Can be used to lay out biological networks
3. Accepts graph represented using standard GraphML format
4. Consideres the following factors while layouting the graph:
	* Crossing-number of drawing
	* Node-node overlaps
	* Area of a drawing
	* Drawing Symmetry
	* Angular resolution

### What's Different from other libraries

1. Supports clustered graphs with support for clusters hierarchy.
2. Allows the nodes in graph to have different sizes. Avoids overlap of nodes.
3. Uses standard GraphML format to store layout related information in a graph.

### Building this library

1. Differ from https://github.com/hotpeperoncino/GraphLayoutLibrary.git
2. On linux, git clone the repository
3. Setup qt dev and boost (qt above 5.9 and boost 1.55)
   You had better install qt and boost from source instead of using apt-get.
4. Run qmake
5. Run make
6. Upon successful build of client and library, set LD_LIBRARY_PATH variable
7. run a.out but it will cause SIGSEGV.
   and also valgrind reports invalid read error.
  
I checked on qt5.9 and boost 1.55.0 combination on ubuntu 14.4. 
I built both libraries from source package.


  
### Using the layout library

See example/example.cpp: it is simplest example.

1. Include 'GraphLayoutLibrary.dll' in your project 
2. Include the 'LayoutManager.h' in your code
	
    ```$
    #include <LayoutManager/LayoutManager.h>
    ```
3. Create object of class 'LayoutManager'
	
    ```$
    LayoutManager layoutManager;
    ```
5. Call methods of 'LayoutManager' class to get layout for a given graph
	
    ```$
    layoutManager.applyCircularLayout(<input_graphML_file_path>, <output_graphML_file_path>, <vertex_order>);
    ```
    
    ```$
    layoutManager.applyGridBasedLayout(<input_graphML_file_path>, <output_graphML_file_path>);
    ...
    layoutManager.applyForceDirectedLayout(<input_graphML_file_path>, <output_graphML_file_path>, <iteration_count>);
    ...
    layoutManager.applyHierarchicalLayout(<input_graphML_file_path>, <output_graphML_file_path>);
    ```

Parameters:
    
 * input_graphML_file_path : String containing path of input GraphML file
 * output_graphML_file_path : String containing path of generated GraphML file
 * vertex_order : Enum of type 'LayoutEnum::VertexOrderCriteria'. Defines the ordering criterion for nodes. Can have values 'LayoutEnum::VertexOrderCriteria::DefaultOrder' / 'LayoutEnum::VertexOrderCriteria::TopologicalOrder' / 'LayoutEnum::VertexOrderCriteria::ConnectedComponentOrder'
 * iteration_count : Integer count for which the Fore Directed Layout steps are to be applied
    
Note: These methods may throw 'LayoutException' and 'boost::exception' in case of unexpected conditions.

###  Documentation
Please check the API documentation under 'Documentation' folder to know more about the APIs.

### Author and Contributors
Persistent system, Noopur Landge, Pritish Dubey, Vrushali Inamdar, Vivek Kulkarni

###  Demo
Demo is removed.

![Alt text](/Images/all.png?raw=true "Different Layouts")

