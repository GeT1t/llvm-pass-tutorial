#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

#define DEBUG_TYPE "FixCallAddr"

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
struct FixCallAddr : public FunctionPass {
  static char ID; // Pass identification, replacement for typeid
  bool flag;

  FixCallAddr() : FunctionPass(ID) {}
  FixCallAddr(bool flag) : FunctionPass(ID) { this->flag = flag; }

  bool runOnFunction(Function &F) override {
    Function *tmp = &F;
    for (Function::iterator bb = tmp->begin(); bb != tmp->end(); ++bb) {
      for (BasicBlock::iterator inst = bb->begin(); inst != bb->end(); ++inst) {
        if (isa<CallInst>(inst)) {
          if (inst->getOperand(0)->getName() == "__asm_it") {
            CallInst *instt = cast<CallInst>(inst);
            errs() << "getcall:" << *instt << '\n';
            MDNode *N = inst->getMetadata("insn.addr");
            Metadata *n1;
            n1 = &*(N->getOperand(0));
            Value *v = cast<ValueAsMetadata>(n1)->getValue();
            ConstantInt *a = cast<ConstantInt>(v);
            TruncInst *t = new TruncInst(a, inst->getType(), "", instt);
            errs() << "t:" << *t << '\n';
            instt->replaceAllUsesWith(t);
            // instt->eraseFromParent();//这里不知道为啥CallInst没有被清除掉
          }
        }
      }
    }
    Function *tmp2 = &*tmp;
    for (auto &B : *tmp2) {
      auto It = B.begin();
      // we modify B, so we must reevaluate end()
      while (It != B.end()) {
        auto &I = *It;
        if (isa<CallInst>(&I)) {
          if (I.getOperand(0)->getName() == "__asm_it")
            errs() << "getcall:" << I << '\n';
          // we continue with the next element
          It = I.eraseFromParent();
        } else {
          ++It;
        }
      }
    }
    return false;
  }
}; // namespace
} // namespace

char FixCallAddr::ID = 0;
static RegisterPass<FixCallAddr> X("FixCallAddr", "Call FixCallAddr");