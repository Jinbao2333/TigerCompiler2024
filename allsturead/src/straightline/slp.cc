#include "straightline/slp.h"

#include <iostream>

namespace A {
int A::CompoundStm::MaxArgs() const {
  // TODO: put your code here (lab1).
  int args1 = stm1->MaxArgs();
  int args2 = stm2->MaxArgs();
  if(args1 > args2)
  {
    return args1;
  }
  else
  return args2;
}

Table *A::CompoundStm::Interp(Table *t) const {
  // TODO: put your code here (lab1).
  return stm2->Interp(stm1->Interp(t));
}

int A::AssignStm::MaxArgs() const {
  // TODO: put your code here (lab1).
  return exp->MaxArgs();
}

Table *A::AssignStm::Interp(Table *t) const {
  // TODO: put your code here (lab1).
  IntAndTable* result = exp->InterpExp(t);
  return result->t->Update(id,result->i);
}

int A::PrintStm::MaxArgs() const {
  // TODO: put your code here (lab1).
  int args1 = exps->NumExps();
  int args2 = exps->MaxArgs();
  if(args1 > args2)
  {
    return args1;
  }
  else
  return args2;
}

Table *A::PrintStm::Interp(Table *t) const {
  // TODO: put your code here (lab1).
  return exps->Interp(t)->t;
}

int IdExp::MaxArgs() const {
  return 0; // should be 0 for none print stm testcase
}

IntAndTable *IdExp::InterpExp(Table *t) const {
  return new IntAndTable(t->Lookup(id), t);
}

int NumExp::MaxArgs() const {
  return 0; // should be 0 for none print stm testcase
}

IntAndTable *NumExp::InterpExp(Table *t) const {
  return new IntAndTable(num, t);
}

int OpExp::MaxArgs() const {
  int args1 = left->MaxArgs();
  int args2 = right->MaxArgs();
  // std::cout << args1 << " " << args2 << std::endl;
  return args1 > args2 ? args1 : args2;
}

IntAndTable *OpExp::InterpExp(Table *t) const {
  IntAndTable *expValue1 = left->InterpExp(t);
  IntAndTable *expValue2 = right->InterpExp(t);
  switch (oper) {
  case PLUS:
    return new IntAndTable(expValue1->i + expValue2->i, expValue2->t);
  case MINUS:
    return new IntAndTable(expValue1->i - expValue2->i, expValue2->t);
  case TIMES:
    return new IntAndTable(expValue1->i * expValue2->i, expValue2->t);
  case DIV:
    return new IntAndTable(expValue1->i / expValue2->i, expValue2->t);
  default: // add default for robustness
    break;
  }
  return nullptr;
}

int EseqExp::MaxArgs() const {
  int args1 = stm->MaxArgs();
  int args2 = exp->MaxArgs();
  return args1 > args2 ? args1 : args2;
}

IntAndTable *EseqExp::InterpExp(Table *t) const {
  return exp->InterpExp(stm->Interp(t));
}

int PairExpList::MaxArgs() const {
  int num1 = exp->MaxArgs();
  int num2 = tail->MaxArgs();
  return num1 > num2 ? num1 : num2;
}

int PairExpList::NumExps() const { return 1 + tail->NumExps(); }

IntAndTable *PairExpList::Interp(Table *t) const {
  IntAndTable *temp_table = exp->InterpExp(t);
  printf("%d ", temp_table->i);
  return tail->Interp(temp_table->t);
}

int LastExpList::MaxArgs() const { return exp->MaxArgs(); }

int LastExpList::NumExps() const { return 1; }

IntAndTable *LastExpList::Interp(Table *t) const {
  // return exp->InterpExp(t);
  IntAndTable *int_and_table = exp->InterpExp(t);
  printf("%d\n", int_and_table->i);
  return int_and_table;
}

int Table::Lookup(const std::string &key) const {
  if (id == key) {
    return value;
  } else if (tail != nullptr) {
    return tail->Lookup(key);
  } else {
    assert(false);
  }
}

Table *Table::Update(const std::string &key, int val) const {
  return new Table(key, val, this);
}
} // namespace A
