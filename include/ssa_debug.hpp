#ifndef _SSA_DEBUG_HPP
#define _SSA_DEBUG_HPP

#include "cfg_data.hpp"

void print_dom_tree(CFG* cfg);
void print_dom_frontier(CFG* cfg);
void print_vars(CFG* cfg);
void print_dom_debug(vector<CFG*> cfg);

#endif
