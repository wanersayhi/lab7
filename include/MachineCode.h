#ifndef __MACHINECODE_H__
#define __MACHINECODE_H__
#include <algorithm>
#include <fstream>
#include <set>
#include <string>
#include <vector>
#include "SymbolTable.h"

/* Hint:
 * MachineUnit: Compiler unit
 * MachineFunction: Function in assembly code
 * MachineInstruction: Single assembly instruction
 * MachineOperand: Operand in assembly instruction, such as immediate number,
 * register, address label */

/* Todo:
 * We only give the example code of "class BinaryMInstruction" and "class
 * AccessMInstruction" (because we believe in you !!!), You need to complete
 * other the member function, especially "output()" , After that, you can use
 * "output()" to print assembly code . */

class MachineUnit;
class MachineFunction;
class MachineBlock;
class MachineInstruction;

class MachineOperand {
   private:
    MachineInstruction* parent;
    int type;
    int val;            // value of immediate number立即数的值
    int reg_no;         // register no寄存器编号
    std::string label;  // address label地址编号，主要为 BranchMInstruction 及 LoadMInstruction 的操作数
   public:
    enum { IMM, VREG, REG, LABEL };
    MachineOperand(int tp, int val);
    MachineOperand(std::string label);
    bool operator==(const MachineOperand&) const;
    bool operator<(const MachineOperand&) const;
    bool isImm() { return this->type == IMM; };
    bool isReg() { return this->type == REG; };
    bool isVReg() { return this->type == VREG; };
    bool isLabel() { return this->type == LABEL; };
    int getVal() { return this->val; };
    int getReg() { return this->reg_no; };
    void setReg(int regno) {
        this->type = REG;
        this->reg_no = regno;
    };
    std::string getLabel() { return this->label; };
    void setParent(MachineInstruction* p) { this->parent = p; };
    MachineInstruction* getParent() { return this->parent; };
    void PrintReg();
    void output();
    //新增
    void setVal(int val) { this->val = val; };
    std::string getRegString();//获取寄存器编号
    std::string getOperandString();
    enum RegType { FP=11,SP=13,LR=14,PC=15 };
    static MachineOperand* newReg(RegType);
    static MachineOperand* newVReg();
    static MachineOperand* newImm(int val){
        return new MachineOperand(MachineOperand::IMM, val);
    }
};

class MachineInstruction {
protected:
    MachineBlock* parent;
    int no;
    int type;                             // Instruction type指令类型，为instType中指定
    int cond = MachineInstruction::NONE;  // Instruction execution condition,//条件，condType中
                                          // optional !!
    int op;                               // Instruction opcode指令码
    // Instruction operand list, sorted by appearance order in assembly
    // instruction
    std::vector<MachineOperand*> def_list;//定义
    std::vector<MachineOperand*> use_list;//使用
    void addDef(MachineOperand* ope) { def_list.push_back(ope); };
    void addUse(MachineOperand* ope) { use_list.push_back(ope); };
    // Print execution code after printing opcode
    void PrintCond();
    enum instType { BINARY, LOAD, STORE, MOV, BRANCH, CMP, STACK };

public:
    enum condType { EQ, NE, LT, LE, GT, GE, NONE };
    virtual void output() = 0;
    void setNo(int no) { this->no = no; };
    int getNo() { return no; };
    std::vector<MachineOperand*>& getDef() { return def_list; };
    std::vector<MachineOperand*>& getUse() { return use_list; };
    //新增
    void insertBefore(MachineInstruction*);
    void insertAfter(MachineInstruction*);
    MachineBlock* getParent() const { return parent; };
    // 简单起见这样写了
    bool isBX() const { return type == BRANCH && op == 2; };
    bool isStore() const { return type == STORE; };
    bool isAdd() const { return type == BINARY && op == 0; };
    std::string getCondString();
    void addUseList(std::vector<MachineOperand*>);
};

class BinaryMInstruction : public MachineInstruction 
{
   public:
    enum opType { ADD, SUB, MUL, DIV, AND, OR };
    BinaryMInstruction(MachineBlock* p,
                       int op,
                       MachineOperand* dst,
                       MachineOperand* src1,
                       MachineOperand* src2,
                       int cond = MachineInstruction::NONE);
    void output();
    std::string getCodeString();
};

class LoadMInstruction : public MachineInstruction 
{
   public:
    LoadMInstruction(MachineBlock* p,
                     MachineOperand* dst,
                     MachineOperand* src1,
                     MachineOperand* src2 = nullptr,
                     int cond = MachineInstruction::NONE);
    void output();
    std::string getLoadCodeString();
};

class StoreMInstruction : public MachineInstruction 
{
   public:
    StoreMInstruction(MachineBlock* p,
                      MachineOperand* src1,
                      MachineOperand* src2,
                      MachineOperand* src3 = nullptr,
                      int cond = MachineInstruction::NONE);
    void output();
    std::string getStoreCodeString();
};

class MovMInstruction : public MachineInstruction {
   public:
    enum opType { MOV, MVN };
    MovMInstruction(MachineBlock* p,
                    int op,
                    MachineOperand* dst,
                    MachineOperand* src,
                    int cond = MachineInstruction::NONE);
    void output();
    std::string getMovCodeString();
};

//分支跳转指令
class BranchMInstruction : public MachineInstruction {
   public:
    enum opType { B, BL, BX };
    BranchMInstruction(MachineBlock* p,
                       int op,
                       MachineOperand* dst,
                       int cond = MachineInstruction::NONE);
    void output();
    std::string getBranchCodeString();
};

class CmpMInstruction : public MachineInstruction {
   public:
    enum opType { CMP };
    CmpMInstruction(MachineBlock* p,
                    MachineOperand* src1,
                    MachineOperand* src2,
                    int cond = MachineInstruction::NONE);
    void output();
    std::string getCmpCodeString();
};

class StackMInstrcuton : public MachineInstruction {
   public:
    enum opType { PUSH, POP };
    StackMInstrcuton(MachineBlock* p,
                     int op,
                     std::vector<MachineOperand*> srcs,
                     MachineOperand* src,
                     MachineOperand* src1 = nullptr,
                     int cond = MachineInstruction::NONE);
    void output();
    std::string getStackCodeString();
    void addSrc(MachineOperand* src);
};

class MachineBlock {
   private:
    MachineFunction* parent;
    int no;//块号
    std::vector<MachineBlock*> pred, succ;
    std::vector<MachineInstruction*> inst_list;
    std::set<MachineOperand*> live_in;
    std::set<MachineOperand*> live_out;
    int cmpCond;
    static int label;
    //新增
    typedef std::vector<MachineInstruction*>::iterator InsIterType;

   public:
    std::vector<MachineInstruction*>& getInsts() { return inst_list; };
    std::vector<MachineInstruction*>::iterator begin() {
        return inst_list.begin();
    };
    std::vector<MachineInstruction*>::iterator end() {
        return inst_list.end();
    };
    MachineBlock(MachineFunction* p, int no) {
        this->parent = p;
        this->no = no;
    };
    void InsertInst(MachineInstruction* inst) {
        this->inst_list.push_back(inst);
    };
    void InsertLoadInst(MachineOperand* dst, MachineOperand*& vreg) {
        vreg = MachineOperand::newVReg();
        auto cur_inst = new LoadMInstruction(this, vreg, dst);
        this->InsertInst(cur_inst);
    };
    void addPred(MachineBlock* p) { this->pred.push_back(p); };
    void addSucc(MachineBlock* s) { this->succ.push_back(s); };
    std::set<MachineOperand*>& getLiveIn() { return live_in; };
    std::set<MachineOperand*>& getLiveOut() { return live_out; };
    std::vector<MachineBlock*>& getPreds() { return pred; };
    std::vector<MachineBlock*>& getSuccs() { return succ; };
    void output();
    //以下为新增
    int getCmpCond() const { return cmpCond; };
    void setCmpCond(int cond) { cmpCond = cond; };
    int getSize() const { return inst_list.size(); };
    MachineFunction* getParent() const { return parent; };
    //输出
    void outputBlockBx();
    void outputBlockStore(InsIterType it, bool& first, int& offst);
    void outputBlockAdd(InsIterType it);
    void outputInst(InsIterType it, int& offset, int& count, bool&, int);
};


class MachineFunction {
   private:
    MachineUnit* parent;
    std::vector<MachineBlock*> block_list;
    int stack_size;
    std::set<int> saved_regs;
    SymbolEntry* sym_ptr;
    int paramsNum;

   public:
    std::vector<MachineBlock*>& getBlocks() { return block_list; };
    std::vector<MachineBlock*>::iterator begin() { return block_list.begin(); };
    std::vector<MachineBlock*>::iterator end() { return block_list.end(); };
    MachineFunction(MachineUnit* p, SymbolEntry* sym_ptr);
    /* HINT:
     * Alloc stack space for local variable;
     * return current frame offset ;
     * we store offset in symbol entry of this variable in function
     * AllocInstruction::genMachineCode() you can use this function in
     * LinearScan::genSpillCode() 
     * */
    int AllocSpace(int size) //为局部变量分配栈空间，返回的是当前frame偏移量
    {
        this->stack_size += size;
        return this->stack_size;
    };
    void InsertBlock(MachineBlock* block) {
        this->block_list.push_back(block);
    };
    void addSavedRegs(int regno) { saved_regs.insert(regno); };
    void output();
    //新增
    std::vector<MachineOperand*> getSavedRegs();//获取保存的寄存器
    int getParamsNum() const { return paramsNum; };
    MachineUnit* getParent() const { return parent; };
};


class MachineUnit 
{
private:
    std::vector<SymbolEntry*> global_list;//全局常变量
    std::vector<MachineFunction*> func_list;
    void PrintGlobalDecl();
    int n;//新增

public:
    std::vector<MachineFunction*>& getFuncs() { return func_list; };
    std::vector<MachineFunction*>::iterator begin() {
        return func_list.begin();
    };
    std::vector<MachineFunction*>::iterator end() { return func_list.end(); };
    void InsertFunc(MachineFunction* func) { func_list.push_back(func); };
    void output();
    //新增
    void insertGlobal(SymbolEntry*);
    void printGlobal();
    int getN() const { return n; };
    //数组输出实现
    void printConstIndices(std::vector<int> constIdx);
    void printZeroIndices(std::vector<int> zeroIdx);
    void printIDSymbleEntry(IdentifierSymbolEntry* se);
};

#endif