#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

#define DEBUG_TYPE "CleanBogusControlFlow"

using namespace llvm;

// Stats
STATISTIC(Flattened, "Functions flattened");

/*
static cl::opt<string> FunctionName(
    "funcFLA", cl::init(""),
    cl::desc(
        "Flatten only certain functions: -mllvm -funcFLA=\"func1,func2\""));

static cl::opt<int> Percentage(
    "perFLA", cl::init(100),
    cl::desc("Flatten only a certain percentage of functions"));*/

namespace {
struct CleanBogusControlFlow : public FunctionPass {
  static char ID; // Pass identification, replacement for typeid
  bool flag;

  CleanBogusControlFlow() : FunctionPass(ID) {}
  CleanBogusControlFlow(bool flag) : FunctionPass(ID) { this->flag = flag; }

  bool runOnFunction(Function &F) override {
    Function *tmp = &F;
    errs() << "Function：" << F.getName() << '\n';
    for (Function::iterator bb = tmp->begin(); bb != tmp->end(); ++bb) {
      // errs() << "block:" << *bb << '\n';
      for (BasicBlock::iterator inst = bb->begin(); inst != bb->end(); ++inst) {
        ConstantInt *a = (ConstantInt *)ConstantInt::get(inst->getType(), -1);
        ConstantInt *b = (ConstantInt *)ConstantInt::get(inst->getType(), 0);
        if (inst->getOpcode() == 15) {
          errs() << "inst:" << *inst << '\n';
          BasicBlock::iterator instb = (--inst);
          ++inst;
          BasicBlock::iterator insta = (++inst);
          --inst;
          if (instb->getOpcode() == 11) {
            errs() << "inst:" << *instb << '\n';
            if (instb->getOperand(1) == a) {
              if (inst->getOperand(1) == instb->getOperand(0)) {
                errs() << "inst:" << *insta << '\n';
                BinaryOperator *instt = cast<BinaryOperator>(inst);
                BinaryOperator *op =
                    BinaryOperator::Create(Instruction::And, b, b, "", instt);

                instt->replaceAllUsesWith(op);
                errs() << "inst:" << *instt << '\n';
              }
            }
          }
        }
      }
    }
    return false;
  }
}; // namespace
} // namespace

char CleanBogusControlFlow::ID = 0;
static RegisterPass<CleanBogusControlFlow> X("CleanBogusControlFlow",
                                             "call CleanBogusControlFlow");