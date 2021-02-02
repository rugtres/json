library(tidyverse)
library(ggplot2)
library(stringi)
library(igraph)


dir = dirname(rstudioapi::getActiveDocumentContext()$path)
setwd(dir)

network_raw = read.csv("test.csv", header = F, sep = ",")

###plot network####
#assuming we know network architecture:
# 3-3-2, fully connected, with hidden layer fully connected to itself
n_nodes_l1 = 3
n_nodes_l2 = 3
n_nodes_l3 = 2

#generate coordinates for positioning nodes correctly in plot

l =   cbind(
  c(
    rep(1,n_nodes_l1)
    ,rep(2,n_nodes_l2)
    ,rep(3,n_nodes_l3)
  ),
  c(seq(1:n_nodes_l1),
    seq(1:n_nodes_l2),
    seq(1:n_nodes_l3) + 0.5 ))
l[n_nodes_l1 + n_nodes_l2 / 2 + 1] = l[n_nodes_l1 + n_nodes_l2 / 2 + 1] + 0.5
colnames(l) = c("x","y")

#get clean dataframe(no characters)
network_clean = network_raw  %>% select_if(is.numeric)

connections = data.frame()

#I2H connections
for( i in 1:(n_nodes_l1 * n_nodes_l2))
{
  ID_sender = 1 + ((i - 1) %/% n_nodes_l2)
  ID_receiver = n_nodes_l1 + if(i %% n_nodes_l2 == 0)  n_nodes_l2 else i %% n_nodes_l2
  weight = as.numeric(network_clean[i])
  node = data.frame("ID1" = ID_sender, "ID2" = ID_receiver, "weight" = weight)
  connections = rbind(connections, node)
}

#H2H connections
#the offset from which we will start iterating in the weights vector
offset1 =  n_nodes_l1 * n_nodes_l2

for( i in 1:(n_nodes_l2 * n_nodes_l2))
{
  #add the number of nodes in previous layer to find ID
  ID_sender = n_nodes_l1 + 1 + ((i - 1) %/% n_nodes_l2)
  #same as before
  ID_receiver = n_nodes_l1 + if(i %% n_nodes_l2 == 0)  n_nodes_l2 else i %% n_nodes_l2
  #apply the offset to the iterator
  weight = as.numeric(network_clean[offset1 + i ])
  node = data.frame("ID1" = ID_sender, "ID2" = ID_receiver, "weight" = weight)
  connections = rbind(connections, node)
}

#H2O connections
#calculate the offset from which we will start iterating in the weights vector
offset2 =  n_nodes_l1 * n_nodes_l2 + n_nodes_l2 * n_nodes_l2

for( i in 1:(n_nodes_l2 * n_nodes_l3))
{
  #add the number of nodes in previous layer to find ID
  ID_sender = n_nodes_l1 + 1 + ((i - 1) %/% n_nodes_l3)
  #same as before
  ID_receiver = n_nodes_l1 + n_nodes_l2 + if(i %% n_nodes_l3 == 0)  n_nodes_l3 else i %% n_nodes_l3
  #apply the offset to the iterator
  weight = as.numeric(network_clean[offset2 + i ])
  node = data.frame("ID1" = ID_sender, "ID2" = ID_receiver, "weight" = weight)
  connections = rbind(connections, node)
}

#create column to show if weight is above or below 0

connections = connections %>% 
  mutate(w_sign = if_else(weight < 0, 1, 2))

#Node ID list 

nodes = as.data.frame(unique(c(connections[,1],connections[,2])))
colnames(nodes) = "nodes"

##create igraph or ggraph object

#undirected to calculate curvature
network <- igraph::graph_from_data_frame(d = connections,
                                         vertices = nodes,
                                         directed = F)
d = curve_multiple(network)

#adjusting curvature to show connections
d[12] = 0.5
d[15] = 0.5
d[13] = -0.5

#directed to plot arrows
network_d <- igraph::graph_from_data_frame(d = connections,
                                           vertices = nodes,
                                           directed = T)

E(network_d)$color = as.factor(connections$w_sign)

plot(network_d, layout = l,
     edge.curved = d,
     edge.arrow.size = 0.5,                           # Arrow size, defaults to 1
     edge.arrow.width = 0.7,                          # Arrow width, defaults to 1
     edge.arrow.height = 0.9,                          # Arrow width, defaults to 1
     edge.lty = c("solid"),
     edge.width = abs(E(network_d)$weight/max(E(network_d)$weight) * 10),             # Edge width, defaults to 1
     )

