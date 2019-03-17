#pragma once
#include "FBX_Data.h"

// Parses an FBX file and returns the root-node of the FBX-tree
Fbx_Node* fbx_parse_node_tree(const char* path);