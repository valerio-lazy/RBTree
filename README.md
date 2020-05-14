# RBTree
define the struct Red-Black Tree

Red-Black Tree(RBTree) is the key for implementing the container std::map and std::set;

In file named M_tools.hpp, I have implemented some template structs relating to RBTree like iterator, predicate
and some being used in classes map and set.

For convenience, I make the root node becoming iterator::end() by making the last node`s right pointer pointe to the root node. Before adding or deleting node from RBTree, make it pointe to nullptr and recover after action.
When travering at the end( ++ or --), operator '++' return itself, '--'return the last node in RBTree ( implemented in class iterator).




