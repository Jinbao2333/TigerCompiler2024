//
// Created by wzl on 2021/10/12.
//

#ifndef TIGER_COMPILER_X64FRAME_H
#define TIGER_COMPILER_X64FRAME_H

#include "tiger/frame/frame.h"

namespace frame {

class X64RegManager : public RegManager {
public:
  enum X64Reg {
    RAX = 0, RBX, RCX, RDX, RSI, RDI, RSP, RBP,
    R8, R9, R10, R11, R12, R13, R14, R15
  };

  const std::string X64RegNames[16] = {
    "rax", "rbx", "rcx", "rdx", "rsi", "rdi",
    "rsp", "rbp", "r8",  "r9",  "r10", "r11",
    "r12", "r13", "r14", "r15"
  };

  const int WORD_SIZE = 8;

  X64RegManager();

  /**
   * Get general-purpose registers excluding RSI
   * NOTE: returned temp list should be in the order of calling convention
   * @return general-purpose registers
   */
  temp::TempList *Registers();

  /**
   * Get registers for arguments
   * NOTE: returned temp list must be in the order of calling convention
   * @return argument registers
   */
  temp::TempList *ArgRegs();

  /**
   * Get caller-saved registers
   * NOTE: returned registers must be in the order of calling convention
   * @return caller-saved registers
   */
  temp::TempList *CallerSaves();

  /**
   * Get callee-saved registers
   * NOTE: returned registers must be in the order of calling convention
   * @return callee-saved registers
   */
  temp::TempList *CalleeSaves();

  /**
   * Get return-sink registers
   * @return return-sink registers
   */
  temp::TempList *ReturnSink();

  /**
   * Get word size
   * @return size of a word in bytes
   */
  int WordSize();

  temp::Temp *FramePointer();
  temp::Temp *StackPointer();
  temp::Temp *ReturnValue();
};

/* InFrameAccess: visiting variable in frame */
class InFrameAccess : public Access {
public:
  int offset;

  explicit InFrameAccess(int offset) : offset(offset) {}

  // return offset(frame pointer) (for accessing variable on stack)
  tree::Exp *ToExp(tree::Exp *framePtr) const override;
};

/* InRegAccess: visiting variable in register */
class InRegAccess : public Access {
public:
  temp::Temp *reg; // Temp represents virtual registers

  explicit InRegAccess(temp::Temp *reg) : reg(reg) {}

  tree::Exp *ToExp(tree::Exp *framePtr) const override;
};

/* X64Frame: frame structure for x64 architecture */
class X64Frame : public Frame {
public:
  X64Frame(temp::Label *name, std::list<bool> formals);

  /* Allocate local variable */
  int AllocLocal();

  /* Get formals */
  std::list<frame::Access *> *Formals();
};

} // namespace frame

#endif // TIGER_COMPILER_X64FRAME_H
