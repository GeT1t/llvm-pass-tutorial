#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Scalar.h"

#define DEBUG_TYPE "CleanGlobalVar"

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
struct CleanGlobalVar : public ModulePass {
  static char ID; // Pass identification, replacement for typeid
  bool flag;

  CleanGlobalVar() : ModulePass(ID) {}
  CleanGlobalVar(bool flag) : ModulePass(ID) { this->flag = flag; }

  bool runOnModule(Module &M) override {
    int i = 0;
    for (Module::global_iterator gi = M.global_begin(), ge = M.global_end();
         gi != ge; ++gi) {
      GlobalVariable *gv = &(*gi);

      if (!gv->hasInitializer()) continue;
      if (!gv->getInitializer()->getType()->isIntegerTy()) continue;

      std::vector<Value *> obj;
      std::vector<Constant *> obj2;

      for (User *U : gv->users()) {
        errs() << "usr:" << *U << '\n';
        if (U->getType()->isIntegerTy()) { //这里只替换作为整数使用的global
          //errs() << "usr:" << *U << '\n';
          for (Use &U1 : U->operands()) {
            Value *v = U1.get();

            if (isa<GlobalVariable>(v)){
              obj.push_back(v);
              errs() << "valueV:" << *v << '\n';
              Constant *initializer = gv->getInitializer();
              errs() << "valueC:" << *initializer << '\n';
              obj2.push_back(initializer);
            }
            
          }
        }
      }
      for (int i = 0; i < obj2.size(); i++) {
        errs() << "obj:" << *obj[i] << '\n';
        errs() << "obj2:" << *obj2[i] << '\n';
        if (dyn_cast<ConstantInt>(obj2[i])) {
          obj[i]->replaceAllUsesWith(obj2[i]);
        }
      }
      if (obj2.size() > 0) {
        errs() << "count:" << i++ << '\n';
      }
    }
    return false;
  }
  
};
} // namespace

char CleanGlobalVar::ID = 0;
static RegisterPass<CleanGlobalVar> X("CleanGlobalVar", "Call CleanGlobalVar");