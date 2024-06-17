#include "tiger/codegen/codegen.h"

#include <cassert>
#include <sstream>

extern frame::RegManager *reg_manager;

namespace {

constexpr int maxlen = 1024;

} // namespace

namespace cg {

void CodeGen::PushRegOnStack(assem::InstrList &instr_list, temp::Temp *reg) {
  int word_size = reg_manager->WordSize();
  frame_->offset_ -= word_size;
  temp::TempList *stack_pointer = new temp::TempList(reg_manager->StackPointer());

  instr_list.Append(new assem::OperInstr(
      "subq $" + std::to_string(word_size) + ", `d0",
      stack_pointer, nullptr, nullptr));
  instr_list.Append(new assem::OperInstr(
      "movq `s0, (`d0)", stack_pointer,
      new temp::TempList(reg), nullptr));
}

void CodeGen::PopRegFromStack(assem::InstrList &instr_list, temp::Temp *reg) {
  int word_size = reg_manager->WordSize();
  temp::TempList *stack_pointer = new temp::TempList(reg_manager->StackPointer());

  instr_list.Append(new assem::OperInstr(
      "movq (`s0), `d0", new temp::TempList(reg),
      stack_pointer, nullptr));
  instr_list.Append(new assem::OperInstr(
      "addq $" + std::to_string(word_size) + ", `d0",
      stack_pointer, nullptr, nullptr));
}

void CodeGen::PushRegToPos(assem::InstrList &instr_list, temp::Temp *pos, temp::Temp *to_be_push) {
  int word_size = reg_manager->WordSize();
  frame_->offset_ -= word_size;
  temp::TempList *pos_list = new temp::TempList(pos);

  instr_list.Append(new assem::OperInstr(
      "subq $" + std::to_string(word_size) + ", `d0",
      pos_list, nullptr, nullptr));
  instr_list.Append(new assem::OperInstr(
      "movq `s0, (`d0)", pos_list,
      new temp::TempList(to_be_push), nullptr));
}

void CodeGen::PopRegFromPos(assem::InstrList &instr_list, temp::Temp *pos, temp::Temp *to_be_pop) {
  int word_size = reg_manager->WordSize();
  temp::TempList *pos_list = new temp::TempList(pos);

  instr_list.Append(new assem::OperInstr(
      "subq $" + std::to_string(word_size) + ", `d0",
      pos_list, nullptr, nullptr));
  instr_list.Append(new assem::OperInstr(
      "movq (`s0), `d0", new temp::TempList(to_be_pop),
      pos_list, nullptr));
}

void CodeGen::Codegen() {
  fs_ = frame_->GetLabel() + "_framesize";
  assem::InstrList *instr_list = new assem::InstrList();

  auto callee_save_reg = reg_manager->GetRegister(frame::X64RegManager::X64Reg::RAX);
  instr_list->Append(new assem::OperInstr(
      "leaq " + fs_ + "(%rsp), `d0",
      new temp::TempList(callee_save_reg), nullptr, nullptr));
  instr_list->Append(new assem::OperInstr(
      "addq $" + std::to_string(frame_->offset_) + ", `d0",
      new temp::TempList(callee_save_reg), nullptr, nullptr));

  for (auto reg : reg_manager->CalleeSaves()->GetList()) {
    PushRegToPos(*instr_list, callee_save_reg, reg);
  }

  instr_list->Append(new assem::OperInstr(
      "leaq " + fs_ + "(`s0), `d0",
      new temp::TempList(reg_manager->FramePointer()),
      new temp::TempList(reg_manager->StackPointer()), nullptr));

  for (auto stm : traces_->GetStmList()->GetList()) {
    stm->Munch(*instr_list, fs_);
  }

  auto pos_rbx = reg_manager->GetRegister(frame::X64RegManager::X64Reg::RBX);
  auto callee_saves = reg_manager->CalleeSaves()->GetList();
  int offset_adjustment = frame_->offset_ + callee_saves.size() * reg_manager->WordSize();

  instr_list->Append(new assem::OperInstr(
      "leaq " + fs_ + "(%rsp), `d0",
      new temp::TempList(pos_rbx), nullptr, nullptr));
  instr_list->Append(new assem::OperInstr(
      "addq $" + std::to_string(offset_adjustment) + ", `d0",
      new temp::TempList(pos_rbx), nullptr, nullptr));

  for (auto reg : callee_saves) {
    PopRegFromPos(*instr_list, pos_rbx, reg);
  }

  assem_instr_ = std::make_unique<AssemInstr>(frame::ProcEntryExit2(instr_list));
}

void AssemInstr::Print(FILE *out, temp::Map *map) const {
  for (auto instr : instr_list_->GetList()) {
    instr->Print(out, map);
  }
  fprintf(out, "\n");
}

} // namespace cg

namespace tree {

void SeqStm::Munch(assem::InstrList &instr_list, std::string_view fs) {
  left_->Munch(instr_list, fs);
  right_->Munch(instr_list, fs);
}

void LabelStm::Munch(assem::InstrList &instr_list, std::string_view fs) {
  instr_list.Append(new assem::LabelInstr(temp::LabelFactory::LabelString(label_), label_));
}

void JumpStm::Munch(assem::InstrList &instr_list, std::string_view fs) {
  auto dst_label = exp_->name_->Name();
  instr_list.Append(new assem::OperInstr(
      "jmp " + dst_label, nullptr, nullptr, new assem::Targets(jumps_)));
}

void CjumpStm::Munch(assem::InstrList &instr_list, std::string_view fs) {
  temp::Temp *left = left_->Munch(instr_list, fs);
  temp::Temp *right = right_->Munch(instr_list, fs);

  instr_list.Append(new assem::OperInstr(
      "cmpq `s1, `s0", nullptr, new temp::TempList({left, right}), nullptr));

  std::string cjump_instr;
  switch (op_) {
    case EQ_OP: cjump_instr = "je"; break;
    case NE_OP: cjump_instr = "jne"; break;
    case LT_OP: cjump_instr = "jl"; break;
    case GT_OP: cjump_instr = "jg"; break;
    case LE_OP: cjump_instr = "jle"; break;
    case GE_OP: cjump_instr = "jge"; break;
    case ULT_OP: cjump_instr = "jnb"; break;
    case ULE_OP: cjump_instr = "jnbe"; break;
    case UGT_OP: cjump_instr = "jna"; break;
    case UGE_OP: cjump_instr = "jnae"; break;
    default: assert(0);
  }

  instr_list.Append(new assem::OperInstr(
      cjump_instr + " `j0", nullptr, nullptr,
      new assem::Targets(new std::vector<temp::Label *>{true_label_, false_label_})));
}

void MoveStm::Munch(assem::InstrList &instr_list, std::string_view fs) {
  temp::Temp *src = src_->Munch(instr_list, fs);

  if (typeid(*dst_) == typeid(tree::MemExp)) {
    temp::Temp *dst = ((MemExp *)dst_)->exp_->Munch(instr_list, fs);
    instr_list.Append(new assem::MoveInstr(
        "movq `s0, (`s1)", nullptr, new temp::TempList({src, dst})));
  } else {
    temp::Temp *dst = dst_->Munch(instr_list, fs);
    instr_list.Append(new assem::MoveInstr(
        "movq `s0, `d0", new temp::TempList({dst}), new temp::TempList({src})));
  }
}

void ExpStm::Munch(assem::InstrList &instr_list, std::string_view fs) {
  exp_->Munch(instr_list, fs);
}

temp::Temp *BinopExp::Munch(assem::InstrList &instr_list, std::string_view fs) {
  std::string op_instr;
  switch (op_) {
    case PLUS_OP: op_instr = "addq"; break;
    case MINUS_OP: op_instr = "subq"; break;
    case MUL_OP: op_instr = "imulq"; break;
    case DIV_OP: op_instr = "idivq"; break;
    case AND_OP: op_instr = "andq"; break;
    case OR_OP: op_instr = "orq"; break;
    case LSHIFT_OP: op_instr = "salq"; break;
    case RSHIFT_OP: op_instr = "shrq"; break;
    case ARSHIFT_OP: op_instr = "sarq"; break;
    case XOR_OP: op_instr = "xorq"; break;
    default: assert(0);
  }

  temp::Temp *left = left_->Munch(instr_list, fs);
  temp::Temp *right = right_->Munch(instr_list, fs);
  temp::Temp *reg = temp::TempFactory::NewTemp();
  temp::TempList *result = new temp::TempList(reg);
  temp::Temp *rax = reg_manager->GetRegister(frame::X64RegManager::X64Reg::RAX);
  temp::Temp *rdx = reg_manager->GetRegister(frame::X64RegManager::X64Reg::RDX);

  switch (op_) {
    case BinOp::MUL_OP:
      instr_list.Append(new assem::MoveInstr("movq `s0, `d0", new temp::TempList(rax), new temp::TempList(left)));
      instr_list.Append(new assem::OperInstr("imulq `s0", new temp::TempList({rax, rdx}), new temp::TempList(right), nullptr));
      instr_list.Append(new assem::MoveInstr("movq `s0, `d0", new temp::TempList(reg), new temp::TempList(rax)));
      return reg;
    case BinOp::DIV_OP:
      instr_list.Append(new assem::MoveInstr("movq `s0, `d0", new temp::TempList(rax), new temp::TempList(left)));
      instr_list.Append(new assem::OperInstr("cqto", new temp::TempList(rdx), new temp::TempList(rax), nullptr));
      instr_list.Append(new assem::OperInstr("idivq `s0", new temp::TempList({rax, rdx}), new temp::TempList(right), nullptr));
      instr_list.Append(new assem::MoveInstr("movq `s0, `d0", new temp::TempList(reg), new temp::TempList(rax)));
      return reg;
    default:
      instr_list.Append(new assem::MoveInstr("movq `s0, `d0", result, new temp::TempList(left)));
      instr_list.Append(new assem::OperInstr(op_instr + " `s0, `d0", result, new temp::TempList(right), nullptr));
      return reg;
  }
}

temp::Temp *MemExp::Munch(assem::InstrList &instr_list, std::string_view fs) {
  temp::Temp *reg = temp::TempFactory::NewTemp();
  temp::Temp *exp = exp_->Munch(instr_list, fs);
  instr_list.Append(new assem::OperInstr("movq (`s0), `d0", new temp::TempList(reg), new temp::TempList(exp), nullptr));
  return reg;
}

temp::Temp *TempExp::Munch(assem::InstrList &instr_list, std::string_view fs) {
  if (temp_ == reg_manager->FramePointer()) {
    temp::Temp *fp = temp::TempFactory::NewTemp();
    std::ostringstream assem;
    assem << "leaq " << fs << "(`s0), `d0";
    instr_list.Append(new assem::OperInstr(assem.str(), new temp::TempList(fp), new temp::TempList(reg_manager->StackPointer()), nullptr));
    return fp;
  }
  return temp_;
}

temp::Temp *EseqExp::Munch(assem::InstrList &instr_list, std::string_view fs) {
  stm_->Munch(instr_list, fs);
  return exp_->Munch(instr_list, fs);
}

temp::Temp *NameExp::Munch(assem::InstrList &instr_list, std::string_view fs) {
  temp::Temp *dst = temp::TempFactory::NewTemp();
  instr_list.Append(new assem::OperInstr("leaq " + name_->Name() + "(%rip), `d0", new temp::TempList(dst), nullptr, nullptr));
  return dst;
}

temp::Temp *ConstExp::Munch(assem::InstrList &instr_list, std::string_view fs) {
  temp::Temp *dst = temp::TempFactory::NewTemp();
  instr_list.Append(new assem::OperInstr("movq $" + std::to_string(consti_) + ", `d0", new temp::TempList(dst), nullptr, nullptr));
  return dst;
}


temp::Temp *CallExp::Munch(assem::InstrList &instr_list, std::string_view fs) {
  temp::TempList *arg_regs = args_->MunchArgs(instr_list, fs);
  instr_list.Append(new assem::OperInstr("callq " + static_cast<NameExp*>(fun_)->name_->Name(), reg_manager->CallerSaves(), arg_regs, nullptr));
  temp::Temp *return_temp = temp::TempFactory::NewTemp();
  instr_list.Append(new assem::MoveInstr("movq `s0, `d0", new temp::TempList(return_temp), new temp::TempList(reg_manager->ReturnValue())));

  int arg_reg_num = reg_manager->ArgRegs()->GetList().size();
  int arg_stack_num = std::max(static_cast<int>(args_->GetList().size()) - arg_reg_num, 0);
  if (arg_stack_num > 0) {
    instr_list.Append(new assem::OperInstr("addq $" + std::to_string(arg_stack_num * reg_manager->WordSize()) + ", `d0", new temp::TempList(reg_manager->StackPointer()), nullptr, nullptr));
  }
  return return_temp;
}

temp::TempList *ExpList::MunchArgs(assem::InstrList &instr_list, std::string_view fs) {
  temp::TempList *arg_regs = new temp::TempList();
  int arg_idx = 0;
  int arg_reg_num = reg_manager->ArgRegs()->GetList().size();
  
  for (Exp *exp : exp_list_) {
    temp::Temp *src = exp->Munch(instr_list, fs);
    if (arg_idx < arg_reg_num) {
      temp::Temp *reg = reg_manager->ArgRegs()->NthTemp(arg_idx);
      instr_list.Append(new assem::MoveInstr("movq `s0, `d0", new temp::TempList(reg), new temp::TempList(src)));
      arg_regs->Append(reg);
    } else {
      instr_list.Append(new assem::OperInstr("subq $" + std::to_string(reg_manager->WordSize()) + ", `d0", new temp::TempList(reg_manager->StackPointer()), nullptr, nullptr));
      instr_list.Append(new assem::OperInstr("movq `s0, (`d0)", new temp::TempList(reg_manager->StackPointer()), new temp::TempList(src), nullptr));
    }
    ++arg_idx;
  }
  return arg_regs;
}

} // namespace tree
