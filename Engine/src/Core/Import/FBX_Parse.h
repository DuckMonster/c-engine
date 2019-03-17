#pragma once
#include "FBX_Data.h"
#include "Core/Memory/Arena.h"

// Parses an FBX file and returns the root-node of the FBX-tree
// 		mem_arena is used for memory allocation
Fbx_Node* fbx_parse_node_tree(const char* path, Mem_Arena* mem_arena);