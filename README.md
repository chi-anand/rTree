# rTree
Implementation of R tree data structure with insertion operation in C.Implemented the "quadratic split" for node splits. Built the R-tree over the dataset given in data.txt and also with dataset of over 1 lakh entries. R-tree parameters as m=2 and M=4 which also can be altered in the code.
Then implemented a pre order traveral of the tree built which prints the MBR values (top right point and bottom left point of the MBR being printed) for each internal node traversed, and prints the 2-D objects stored in them while traversing the leaf nodes. Clear distinction is to be made while printing, whether the node you are printing is internal node or external node.


