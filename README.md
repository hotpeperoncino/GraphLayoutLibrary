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

### What's Different 

1. Supports clustered graphs with support for clusters hierarchy.
2. Allows the nodes in graph to have different sizes. Avoids overlap of nodes.
3. Uses standard GraphML format to store layout related information in a graph.

### Building this library

1. Make sure Qt Creator (version 3.1.2) with Qt (version 5.3.1) is installed.
2. Download Boost version 1.55.0 and place it on D drive. (like D:\boost_1_55_0)
3. Open GraphLayout.pro as project with Qt Creator.
4. Run **Qmake** command on GraphLayout.pro 
5. Run **Build** command on GraphLayout.pro
6. You might encounter build error on an intermediate file 
   moc_MainWindow.cpp at line 178. Simply comment the line (with //) and save.
7. Run **Build** command again on GraphLayout.pro.
8. Upon successful build of client and library use **Run**  command to start execution of Graph Layout Library Client. 
  
### Using the layout library

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

### Contributors
Noopur Landge, Pritish Dubey, Vrushali Inamdar, Vivek Kulkarni

###  Demo
Please check the sample demo application 'GraphLayoutLibraryClient.exe' present under 'Demo' folder to view the layout algorithms in action. It also contains SBML to GraphML converter.

![Alt text](/Images/all.png?raw=true "Different Layouts")

