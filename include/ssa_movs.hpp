#ifndef _SSA_MOVS_HPP

#define _SSA_MOVS_HPP

void ssa_remove_movs(CFG* cfg);

bool ssa_remove_movs_attr(CFG_Command* cmd, map<string, CFG_Exp*> &replace_map);

#endif
