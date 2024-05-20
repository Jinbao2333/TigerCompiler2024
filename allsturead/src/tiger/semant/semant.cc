#include "tiger/absyn/absyn.h"
#include "tiger/semant/semant.h"

namespace absyn {

void AbsynTree::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                           err::ErrorMsg *errormsg) const {
  /* DONE: lab4 code */
  root_->SemAnalyze(venv, tenv, 0, errormsg); // In the beginning, labelcount is 0
}

type::Ty *SimpleVar::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                                int labelcount, err::ErrorMsg *errormsg) const {
  /* DONE: lab4 code */
  env::EnvEntry *entry = venv->Look(sym_);
  if (entry && typeid(*entry) == typeid(env::VarEntry)) {
    return (static_cast<env::VarEntry *>(entry))->ty_->ActualTy();
  } else {
    errormsg->Error(pos_, "undefined variable %s", sym_->Name().data());
  }
  return type::NilTy::Instance();
}

type::Ty *FieldVar::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                               int labelcount, err::ErrorMsg *errormsg) const {
  /* DONE: lab4 code */
  
  // Check if var (lvalue) is record
  type::Ty *type = var_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
  if (typeid(*type) != typeid(type::RecordTy)) {
    errormsg->Error(pos_, "not a record type");
    return type::NilTy::Instance();
  }

  // Check if record has this field
  type::RecordTy *record = static_cast<type::RecordTy *>(type);
  for (type::Field *field : record->fields_->GetList()) {
    if (field->name_->Name() == sym_->Name()) {
      return field->ty_;
    }
  }
  errormsg->Error(pos_, "field %s doesn't exist", sym_->Name().c_str());
  return type::NilTy::Instance();
}

type::Ty *SubscriptVar::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                                   int labelcount,
                                   err::ErrorMsg *errormsg) const {
  /* DONE: lab4 code */

  // Ensure var (lvalue) is an array
  type::Ty *type = var_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
  
  if (typeid(*type) == typeid(type::ArrayTy)) {
    return (static_cast<type::ArrayTy *>(type)->ty_->ActualTy());
  } 
  
  // Report error if not an array type
  errormsg->Error(pos_, "array type required");
  return type::NilTy::Instance();
}

type::Ty *VarExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* DONE: lab4 code */
  return var_->SemAnalyze(venv, tenv, labelcount, errormsg);
}

type::Ty *NilExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* DONE: lab4 code */
  return type::NilTy::Instance();
}

type::Ty *IntExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* DONE: lab4 code */
  return type::IntTy::Instance();
}

type::Ty *StringExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                                int labelcount, err::ErrorMsg *errormsg) const {
  /* DONE: lab4 code */
  return type::StringTy::Instance();
}

type::Ty *CallExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                              int labelcount, err::ErrorMsg *errormsg) const {
  /* DONE: lab4 code */
  // If function is defined
  env::EnvEntry *entry = venv->Look(func_);
  if (!entry) {
    errormsg->Error(pos_, "undefined function %s", func_->Name().c_str());
    return type::VoidTy::Instance();
  }
  // If the name is a function
  if (typeid(*entry) != typeid(env::FunEntry)) {
    errormsg->Error(pos_, "function required");
    return type::NilTy::Instance();
  }
  env::FunEntry *function = static_cast<env::FunEntry *>(entry);
  const std::list<type::Ty *> &formalTys = function->formals_->GetList();
  auto formalTy = formalTys.cbegin();
  std::list<Exp *> args = args_->GetList();
  // If numbers of actual and formal parameters match
  if (args.size() > formalTys.size()) {
    errormsg->Error(pos_ - 1, "too many params in function " + func_->Name());
    return type::VoidTy::Instance();
  }
  if (args.size() < formalTys.size()) {
    errormsg->Error(pos_ - 1, "too few params in function " + func_->Name() + " para type mismatch");
    return type::VoidTy::Instance();
  }
  for (Exp *arg : args) {
    type::Ty *type = arg->SemAnalyze(venv, tenv, labelcount, errormsg);
    if (errormsg->AnyErrors()) {
      break;
    }
    // If types of actual and formal parameters match
    if (!(*formalTy)->IsSameType(type)) {
      errormsg->Error(arg->pos_, "para type mismatch");
      return type::VoidTy::Instance();
    }
    ++formalTy;
  }
  return function->result_;
}

type::Ty *OpExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                            int labelcount, err::ErrorMsg *errormsg) const {
  /* DONE: lab4 code */

  type::Ty *left_ty = left_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
  type::Ty *right_ty = right_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();

  if (oper_ < EQ_OP) {
    // Check if operands of arithmetic operators are integers
    if (!left_ty->IsSameType(type::IntTy::Instance())) {
      errormsg->Error(left_->pos_, "integer required");
    }
    if (!right_ty->IsSameType(type::IntTy::Instance())) {
      errormsg->Error(right_->pos_, "integer required");
    }
    return type::IntTy::Instance();

  } else {
    // Check if operands type is same
    if (!left_ty->IsSameType(right_ty)) {
      errormsg->Error(pos_, "same type required");
    }
    return type::IntTy::Instance();
  }
}

type::Ty *RecordExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                                int labelcount, err::ErrorMsg *errormsg) const {
  /* DONE: lab4 code */

  // Check if record type is defined
  type::Ty *type = tenv->Look(typ_);
  if (!type || typeid(*(type->ActualTy())) != typeid(type::RecordTy)) {
    errormsg->Error(pos_, "undefined type %s", typ_->Name().data());
    return type::NilTy::Instance();
  }

  type::RecordTy *recordTy = static_cast<type::RecordTy *>(type->ActualTy());

  // Check for empty record
  if (fields_->GetList().empty()) {
    return type::NilTy::Instance();
  }

  // Check if number of fields in RecordExp matches the definition
  if (fields_->GetList().size() != recordTy->fields_->GetList().size()) {
    errormsg->Error(pos_, "num of fields doesn't match");
    return type->ActualTy();
  }

  // Check if each field in RecordExp matches the definition
  auto record_field = recordTy->fields_->GetList().cbegin();
  for (const absyn::EField *actual_field : fields_->GetList()) {
    // Check field name
    if (actual_field->name_->Name() != (*record_field)->name_->Name()) {
      errormsg->Error(pos_, "field %s doesn't exist", actual_field->name_->Name().data());
    }
    // Check field type
    if (!actual_field->exp_->SemAnalyze(venv, tenv, labelcount, errormsg)->IsSameType((*record_field)->ty_)) {
      errormsg->Error(pos_, "type of field %s doesn't match", actual_field->name_->Name().data());
    }
    ++record_field;
  }

  return type->ActualTy();
}

type::Ty *SeqExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* DONE: lab4 code */

  type::Ty *last_exp_ty = nullptr;
  for (absyn::Exp *exp : seq_->GetList()) {
    last_exp_ty = exp->SemAnalyze(venv, tenv, labelcount, errormsg);
  }
  
  // The type of SeqExp is the type of the last expression
  return last_exp_ty;
}

type::Ty *IfExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                            int labelcount, err::ErrorMsg *errormsg) const {
  /* DONE: lab4 code */

  // Analyze test expression
  test_->SemAnalyze(venv, tenv, labelcount, errormsg);

  // Analyze then expression
  type::Ty *thenTy = then_->SemAnalyze(venv, tenv, labelcount, errormsg);
  if (errormsg->AnyErrors()) {
    return thenTy;
  }

  if (!elsee_) {
    // No else expression
    // Then expression must produce no value
    if (!thenTy->IsSameType(type::VoidTy::Instance())) {
      errormsg->Error(then_->pos_, "if-then exp's body must produce no value");
    }
    return thenTy;
  } else {
    // Analyze else expression
    type::Ty *elseTy = elsee_->SemAnalyze(venv, tenv, labelcount, errormsg);
    // Check if then and else have same type
    if (!elseTy->IsSameType(thenTy)) {
      errormsg->Error(pos_, "then exp and else exp type mismatch");
    }
  }
  return thenTy;
}

type::Ty *WhileExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                               int labelcount, err::ErrorMsg *errormsg) const {
  /* DONE: lab4 code */

  // Analyze test expression
  test_->SemAnalyze(venv, tenv, labelcount, errormsg);

  // Analyze body of while loop
  type::Ty *bodyTy = body_->SemAnalyze(venv, tenv, labelcount + 1, errormsg);
  
  // Check if body of while loop produces no value
  if (!bodyTy->IsSameType(type::VoidTy::Instance())) {
    errormsg->Error(pos_, "while body must produce no value");
  }
  
  return type::VoidTy::Instance();
}

type::Ty *BreakExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                               int labelcount, err::ErrorMsg *errormsg) const {
  /* DONE: lab4 code */
  
  // Check if break is inside any loop
  if (labelcount == 0) {
    errormsg->Error(pos_, "break is not inside any loop");
  }
  
  return type::VoidTy::Instance();
}

type::Ty *LetExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* DONE: lab4 code */

  // Begin a new scope for variable and type declarations
  venv->BeginScope();
  tenv->BeginScope();
  
  // Analyze each declaration in the let expression
  for (Dec *dec : decs_->GetList())
    dec->SemAnalyze(venv, tenv, labelcount, errormsg);

  // Analyze the body expression of the let expression
  type::Ty *result = (body_ ? body_->SemAnalyze(venv, tenv, labelcount, errormsg) : type::VoidTy::Instance());

  tenv->EndScope();
  venv->EndScope();
  
  return result;
}

type::Ty *ArrayExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                               int labelcount, err::ErrorMsg *errormsg) const {
  /* DONE: lab4 code */

  // Check if array type is defined
  type::Ty *type = tenv->Look(typ_);
  if (!type || typeid(*(type->ActualTy())) != typeid(type::ArrayTy)) {
    errormsg->Error(pos_, "undefined array %s", typ_);
    return type::VoidTy::Instance();
  }

  // Check if size is int
  type::Ty *size_ty = size_->SemAnalyze(venv, tenv, labelcount, errormsg);
  if (typeid(*size_ty) != typeid(type::IntTy)) {
    errormsg->Error(pos_, "size of array should be int");
    return type::VoidTy::Instance();
  }

  // Check if type of init is same as array element type
  type::Ty *arrayTy = static_cast<type::ArrayTy *>(type->ActualTy())->ty_;
  type::Ty *initTy = init_->SemAnalyze(venv, tenv, labelcount, errormsg);

  if (!initTy->IsSameType(arrayTy)) {
    errormsg->Error(pos_, "type mismatch");
    return type->ActualTy();
  }

  return type->ActualTy();
}

type::Ty *VoidExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                              int labelcount, err::ErrorMsg *errormsg) const {
  /* DONE: lab4 code */
  // Void expression always returns void type
  return type::VoidTy::Instance();
}

type::Ty *AssignExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                                int labelcount, err::ErrorMsg *errormsg) const {

  // Analyze the type of the variable being assigned
  type::Ty *varTy = var_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
  
  // ... expression ...
  type::Ty *expTy = exp_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
  
  // Check if the types of the variable and expression match
  if (varTy->IsSameType(expTy)) {
    // Check if loop variable is assigned
    if (typeid(*var_) == typeid(absyn::SimpleVar)) {
      absyn::SimpleVar *var = static_cast<absyn::SimpleVar *>(var_);
      env::VarEntry *entry = static_cast<env::VarEntry *>(venv->Look(var->sym_));
      if (entry->readonly_) {
        errormsg->Error(pos_, "loop variable can't be assigned");
      }
    }
  } else {
    errormsg->Error(pos_, "unmatched assign exp");
  }
  
  return type::VoidTy::Instance();
}

type::Ty *ForExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  // Add loop variable to variable environment as readonly
  venv->BeginScope();
  venv->Enter(var_, new env::VarEntry(type::IntTy::Instance(), true));

  // Check if lo and hi are both of type integer
  if (!lo_->SemAnalyze(venv, tenv, labelcount, errormsg)->IsSameType(type::IntTy::Instance())) {
    errormsg->Error(lo_->pos_, "for exp's range type is not integer");
  }
  if (!hi_->SemAnalyze(venv, tenv, labelcount, errormsg)->IsSameType(type::IntTy::Instance())) {
    errormsg->Error(hi_->pos_, "for exp's range type is not integer");
  }

  // Check if body produces no value
  type::Ty *bodyTy = body_->SemAnalyze(venv, tenv, labelcount + 1, errormsg);
  if (!bodyTy->IsSameType(type::VoidTy::Instance())) {
    errormsg->Error(pos_, "for body should produce no value");
  }

  // End the scope for the loop variable
  venv->EndScope();
  
  return type::VoidTy::Instance();
}

void FunctionDec::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* DONE: lab4 code */

  // ## 1: Add function prototypes to env
  for (absyn::FunDec *function : functions_->GetList()) {
    // Look up result type from type environment, default to Void if not found
    type::Ty *result_ty = function->result_ ? tenv->Look(function->result_) : type::VoidTy::Instance();
    // Construct formal parameter types list
    type::TyList *formals_ty = function->params_->MakeFormalTyList(tenv, errormsg);

    // Check for duplicate function names in the same batch of declarations
    for (absyn::FunDec *anotherFunction : functions_->GetList()) {
      if (function != anotherFunction && function->name_ == anotherFunction->name_) {
        errormsg->Error(pos_, "two functions have the same name");
        return;
      }
    }

    // Enter function prototype into variable environment
    venv->Enter(function->name_, new env::FunEntry(formals_ty, result_ty));
  }

  // ## 2: Analyze function bodies
  for (absyn::FunDec *function : functions_->GetList()) {
    // Begin a new scope for the function's parameters
    venv->BeginScope();

    // Look up result type from type environment (default to Void if not found)
    type::Ty *result_ty = function->result_ ? tenv->Look(function->result_) : type::VoidTy::Instance();
    // Construct formal parameter types list
    type::TyList *formals_ty = function->params_->MakeFormalTyList(tenv, errormsg);

    // Enter formal parameters into variable environment
    auto formal_ty = formals_ty->GetList().cbegin();
    for (absyn::Field *param : function->params_->GetList()) {
      venv->Enter(param->name_, new env::VarEntry(*formal_ty));
      ++formal_ty;
    }

    // Analyze function body using the new environment
    type::Ty *body_ty = function->body_->SemAnalyze(venv, tenv, labelcount, errormsg);
    // Check if body type matches result type
    if (!body_ty->IsSameType(result_ty)) {
      errormsg->Error(pos_, "procedure returns value");
    }

    // End the scope for the function's parameters
    venv->EndScope();
  }
}

void VarDec::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv, int labelcount,
                        err::ErrorMsg *errormsg) const {
  /* DONE: lab4 code */

  // Analyze the type of the initialization expression
  type::Ty *init_ty = init_->SemAnalyze(venv, tenv, labelcount, errormsg);

  if (typ_) {
    // Case 1: var x : type_id := exp
    // Look up ...
    type::Ty *type_id = tenv->Look(typ_);

    // Check if the specified type exists
    if (!type_id) {
      errormsg->Error(pos_, "undefined type %s", typ_->Name().data());
      return;
    }

    // Check if the type of the initialization expression matches the specified type
    if (!init_ty->ActualTy()->IsSameType(type_id)) {
      errormsg->Error(init_->pos_, "type mismatch");
      return;
    }

    // Enter the variable into the variable environment with the specified type
    venv->Enter(var_, new env::VarEntry(type_id));

  } else {
    // Case 2: var x := exp
    // If the type of the initialization expression is NilTy, a type must be specified
    if (init_ty->ActualTy()->IsSameType(type::NilTy::Instance()) &&
        typeid(*(init_ty->ActualTy())) != typeid(type::RecordTy)) {
      errormsg->Error(pos_, "init should not be nil without type specified");
      return;
    }

    // Enter the variable into the variable environment with the type of the initialization expression
    venv->Enter(var_, new env::VarEntry(init_ty->ActualTy()));
  }
}

void TypeDec::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv, int labelcount,
                         err::ErrorMsg *errormsg) const {
  /* DONE: lab4 code */

  // ## 1: Add type names to type environment
  for (NameAndTy *type : types_->GetList()) {
    // Check for duplicate type names in the same batch of declarations
    for (NameAndTy *anotherType : types_->GetList()) {
      if (type != anotherType && type->name_ == anotherType->name_) {
        errormsg->Error(pos_, "two types have the same name");
        return;
      }
    }
    // Enter type name into the type environment with a placeholder type
    tenv->Enter(type->name_, new type::NameTy(type->name_, nullptr));
  }

  // ## 2: Resolve type definitions
  for (NameAndTy *type : types_->GetList()) {
    // Look up ...
    type::NameTy *name_ty = static_cast<type::NameTy *>(tenv->Look(type->name_));

    // Resolve the type definition and update the type environment
    name_ty->ty_ = type->ty_->SemAnalyze(tenv, errormsg);

    // Check if the resolved type exists
    if (!name_ty->ty_) {
      errormsg->Error(pos_, "undefined type %s", type->name_);
      break;
    }

    // Check for illegal type cycles
    type::Ty *tmp = tenv->Look(type->name_), *next, *start = tmp;
    while (tmp) {
      // Break if the type is not a name type
      if (typeid(*tmp) != typeid(type::NameTy)) break;

      next = (static_cast<type::NameTy *>(tmp))->ty_;
      if (next == start) {
        errormsg->Error(pos_, "illegal type cycle");
        return;
      }

      tmp = next;
    }
  }
}

type::Ty *NameTy::SemAnalyze(env::TEnvPtr tenv, err::ErrorMsg *errormsg) const {
  /* DONE: lab4 code */

  // Look up ...
  type::Ty *type = tenv->Look(name_);

  // Check if the type exists
  if (!type) {
    errormsg->Error(pos_, "undefined type %s", name_->Name().data());
    return type::NilTy::Instance();
  }

  return type;
}

type::Ty *RecordTy::SemAnalyze(env::TEnvPtr tenv, err::ErrorMsg *errormsg) const {
  /* DONE: lab4 code */

  // Create a new record type with fields obtained from the record declaration
  return new type::RecordTy(record_->MakeFieldList(tenv, errormsg));
}

type::Ty *ArrayTy::SemAnalyze(env::TEnvPtr tenv, err::ErrorMsg *errormsg) const {
  /* DONE: lab4 code */

  // Look up ...
  type::Ty *type = tenv->Look(array_);

  // Check ...
  if (!type) {
    errormsg->Error(pos_, "undefined type %s", array_->Name().data());
    return type::NilTy::Instance();
  }

  // Create a new array type with the element type
  return new type::ArrayTy(type);
}

} // namespace absyn

namespace sem {

void ProgSem::SemAnalyze() {
  FillBaseVEnv();
  FillBaseTEnv();
  absyn_tree_->SemAnalyze(venv_.get(), tenv_.get(), errormsg_.get());
}

} // namespace sem
