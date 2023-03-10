#ifndef __LIVE_VARIABLE_ANALYSIS_H__
#define __LIVE_VARIABLE_ANALYSIS_H__

#include <set>
#include <map>

class MachineFunction;
class MachineUnit;
class MachineOperand;
class MachineBlock;
//为活跃变量分析，用于寄存器分配过程。
class LiveVariableAnalysis
{
private:
    std::map<MachineOperand, std::set<MachineOperand *>> all_uses;
    std::map<MachineBlock *, std::set<MachineOperand *>> def, use;
    void computeUsePos(MachineFunction *);
    void computeDefUse(MachineFunction *);
    void iterate(MachineFunction *);

public:
    void pass(MachineUnit *unit);
    void pass(MachineFunction *func);
    std::map<MachineOperand, std::set<MachineOperand *>> &getAllUses() { return all_uses; };
};

#endif