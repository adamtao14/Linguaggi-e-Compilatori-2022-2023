#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/ADT/APInt.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Constants.h"
#include <string.h>
#include <cmath>

using namespace llvm;

bool runOnBasicBlockAlgebric(BasicBlock &B)
{
    //per ogni basic block otteniamo la funzione padre e il relativo contesto
    Function *F = B.getParent();
    LLVMContext &context = F->getContext();

    for (auto Iter = B.begin(); Iter != B.end(); ++Iter)
    {
        // Per ogni istruzione del BasicBlock

        Instruction &Inst = *Iter;

        if (!strcmp(Inst.getOpcodeName(), "add"))
        {
            // Se l'istruzione è una addizione

            outs() << Inst << "\n";
            Value *operand_1 = Inst.getOperand(0);
            Value *operand_2 = Inst.getOperand(1);
            outs() << "\t" << *operand_1 << "\n\t" << *operand_2 << "\n";

            ConstantInt *C2 = dyn_cast<ConstantInt>(operand_2);
            //controlliamo se il secondo operando dell'istruzione è una costante intera
            if (C2 && C2->getValue().isZero())
            {
                //se è una costante intera e il vlaore è 0
                outs() << "\tIstruzione rimpiazzata con " << *operand_1 << "\n";
                //rimpiazzamo tutti gli usi con solo l'operando 1
                Inst.replaceAllUsesWith(operand_1);
            }
        }
        else if (!strcmp(Inst.getOpcodeName(), "mul"))
        {
            // Se l'istruzione è una moltiplicazione

            outs() << Inst << "\n";
            Value *operand_1 = Inst.getOperand(0);
            Value *operand_2 = Inst.getOperand(1);
            outs() << "\t" << *operand_1 << "\n\t" << *operand_2 << "\n";

            ConstantInt *C1 = dyn_cast<ConstantInt>(operand_1);
            ConstantInt *C2 = dyn_cast<ConstantInt>(operand_2);
            // la costante può essere nel primo o nel secondo
            if (C2 && C2->getValue().getLimitedValue() == 1)
            {
                outs() << "\tIstruzione rimpiazzata con " << *operand_1 << "\n";
                //rimpiazzamo tutti gli usi con solo l'operando 1
                Inst.replaceAllUsesWith(operand_1);
            }
            else if (C1 && C1->getValue().getLimitedValue() == 1)
            {
                outs() << "\tIstruzione rimpiazzata con " << *operand_2 << "\n";
                //rimpiazzamo tutti gli usi con solo l'operando 1
                Inst.replaceAllUsesWith(operand_2);
            }
        }
        outs() << "\n";
    }

    return true;
}

bool runOnFunctionAlgebric(Function &F)
{
    bool Transformed = false;

    for (auto Iter = F.begin(); Iter != F.end(); ++Iter)
    {
        //per ogni basic block applichiamo l'ottimizzazione 
        if (runOnBasicBlockAlgebric(*Iter))
        {
            Transformed = true;
        }
    }
    // se non c'è stato neanche un miglioramento la variabile Transoformed sarà false
    return Transformed;
}

PreservedAnalyses AlgebraicIdentityPass::run([[maybe_unused]] Module &M,
                                             ModuleAnalysisManager &)
{

    // Un semplice passo di esempio di manipolazione della IR
    for (auto Iter = M.begin(); Iter != M.end(); ++Iter)
    {
        //per ogni funzione applichiamo l'ottimizzazione 
        if (runOnFunctionAlgebric(*Iter))
        {
            return PreservedAnalyses::none();
        }
    }

    return PreservedAnalyses::none();
}
