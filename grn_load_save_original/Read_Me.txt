
 The c++ code first creates GRN (3-3-2), which inner layer is also internally 
 connected with itself, then randomizes its weights. After this, it saves
 it as single line containing the weights of each connection with ',' as separator.
 The other characters you see in the excel file ('!' and '|') are delimiters,
 to signal end of a node's wiegits and end of a layer.
 Other information is stored as well after the weights, such as treshold values
 of the nodes, the state of each node, and the number of nodes per layer.
 This is code I wrote long ago and the big part of it comes from Jordi's Van Gestel project.
 I do not like it and is very messy, but I still haven't had the time to swap it out with
 somehting more readable. Do not focus too much on it, the point here is the format I save 
 the GRN in and how I reload it.
 
 The R code that I use to plot the network relies on the igraph R library
 where I transform the data I saved in 2 matrices. One is a one column matrix
 containing the ID of all nodes (or vertices) present in the network, the second
 is a (minimum) 2 column matrix where I store the connections (or edges) between 
 nodes, where in col 1 there is the ID of the sender and in col 2 the ID of the
 receiver. In both matrices columns can be added to store further properties of
 vertices and edges, in my case for instance I store in the edge matrix also the
 weight of each connection.