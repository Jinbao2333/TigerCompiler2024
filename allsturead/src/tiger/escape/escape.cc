#include "tiger/escape/escape.h"
#include "tiger/absyn/absyn.h"

namespace esc {
void EscFinder::FindEscape() { absyn_tree_->Traverse(env_.get()); }
} // namespace esc

namespace absyn {

void AbsynTree::Traverse(esc::EscEnvPtr env) {
  /* TODO: Put your lab5 code here */
  root_->Traverse(env, 1);
}

void SimpleVar::Traverse(esc::EscEnvPtr env, int depth) {
  /* TODO: Put your lab5 code here */
  esc::EscapeEntry *entry = env->Look(sym_);
  if (entry->depth_ < depth) {
    *(entry->escape_) = true;
  }
}

void FieldVar::Traverse(esc::EscEnvPtr env, int depth) {
  /* TODO: Put your lab5 code here */
  var_->Traverse(env, depth);
}

void SubscriptVar::Traverse(esc::EscEnvPtr env, int depth) {
  /* TODO: Put your lab5 code here */
  var_->Traverse(env, depth);
  subscript_->Traverse(env, depth);
}

void VarExp::Traverse(esc::EscEnvPtr env, int depth) {
  /* TODO: Put your lab5 code here */
  var_->Traverse(env, depth);
}

void NilExp::Traverse(esc::EscEnvPtr env, int depth) {  }

void IntExp::Traverse(esc::EscEnvPtr env, int depth) {  }

void StringExp::Traverse(esc::EscEnvPtr env, int depth) {  }

void CallExp::Traverse(esc::EscEnvPtr env, int depth) {
  /* TODO: Put your lab5 code here */
  for (absyn::Exp *arg : args_->GetList()) {
    arg->Traverse(env, depth);
  }
}

void OpExp::Traverse(esc::EscEnvPtr env, int depth) {
  /* TODO: Put your lab5 code here */
  left_->Traverse(env, depth);
  right_->Traverse(env, depth);
}

void RecordExp::Traverse(esc::EscEnvPtr env, int depth) {
  /* TODO: Put your lab5 code here */
  for (absyn::EField *field : fields_->GetList()) {
    field->exp_->Traverse(env, depth);
  }
}

void SeqExp::Traverse(esc::EscEnvPtr env, int depth) {
  /* TODO: Put your lab5 code here */
  for (absyn::Exp *exp : seq_->GetList()) {
    exp->Traverse(env, depth);
  }
}

void AssignExp::Traverse(esc::EscEnvPtr env, int depth) {
  /* TODO: Put your lab5 code here */
  var_->Traverse(env, depth);
  exp_->Traverse(env, depth);
}

void IfExp::Traverse(esc::EscEnvPtr env, int depth) {
  /* TODO: Put your lab5 code here */
  test_->Traverse(env, depth);
  then_->Traverse(env, depth);
  if (elsee_) {
    elsee_->Traverse(env, depth);
  }
}

void WhileExp::Traverse(esc::EscEnvPtr env, int depth) {
  /* TODO: Put your lab5 code here */
  test_->Traverse(env, depth);
  body_->Traverse(env, depth);
}

void ForExp::Traverse(esc::EscEnvPtr env, int depth) {
  /* TODO: Put your lab5 code here */
  escape_ = false;
  env->BeginScope();
  env->Enter(var_, new esc::EscapeEntry(depth, &(escape_)));

  lo_->Traverse(env, depth);
  hi_->Traverse(env, depth);
  body_->Traverse(env, depth);

  env->EndScope();
}

void BreakExp::Traverse(esc::EscEnvPtr env, int depth) {  }

void LetExp::Traverse(esc::EscEnvPtr env, int depth) {
  /* TODO: Put your lab5 code here */
  env->BeginScope();
  for (absyn::Dec *dec : decs_->GetList()) {
    dec->Traverse(env, depth);
  }
  body_->Traverse(env, depth);
  env->EndScope();
}

void ArrayExp::Traverse(esc::EscEnvPtr env, int depth) {
  /* TODO: Put your lab5 code here */
  size_->Traverse(env, depth);
  init_->Traverse(env, depth);
}

void VoidExp::Traverse(esc::EscEnvPtr env, int depth) { }

void FunctionDec::Traverse(esc::EscEnvPtr env, int depth) {
  /* TODO: Put your lab5 code here */
  for (absyn::FunDec *fun : functions_->GetList()) {
    env->BeginScope();
    for (absyn::Field *param : fun->params_->GetList()) {
      param->escape_ = false;
      env->Enter(param->name_, new esc::EscapeEntry(depth + 1, &(param->escape_)));
    }
    fun->body_->Traverse(env, depth + 1);
    env->EndScope();
  }
}

void VarDec::Traverse(esc::EscEnvPtr env, int depth) {
  /* TODO: Put your lab5 code here */
  escape_ = false;
  env->Enter(var_, new esc::EscapeEntry(depth, &(escape_)));
  init_->Traverse(env, depth);
}

void TypeDec::Traverse(esc::EscEnvPtr env, int depth) { }

} // namespace absyn