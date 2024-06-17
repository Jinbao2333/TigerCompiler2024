#include "tiger/translate/translate.h"

#include <tiger/absyn/absyn.h>

#include "tiger/env/env.h"
#include "tiger/errormsg/errormsg.h"
#include "tiger/frame/frame.h"
#include "tiger/frame/temp.h"
#include "tiger/frame/x64frame.h"

extern frame::Frags *frags;
extern frame::RegManager *reg_manager;

namespace {
frame::ProcFrag *ProcEntryExit(tr::Level *level, tr::Exp *body);
}

namespace tr {

Access *Access::AllocLocal(Level *level, bool escape) {
    auto localAccess = frame::Access::AllocLocal(level->frame_, escape);
    return new Access(level, localAccess);
}

tree::Exp *Access::ToExp(Level *currentLevel) {
    auto framePtr = currentLevel->StaticLink(level_);
    return access_->ToExp(framePtr);
}

class Cx {
public:
    PatchList trues_;
    PatchList falses_;
    tree::Stm *stm_;

    Cx(PatchList trues, PatchList falses, tree::Stm *stm)
        : trues_(std::move(trues)), falses_(std::move(falses)), stm_(stm) {}
};

class Exp {
public:
    virtual ~Exp() = default;
    [[nodiscard]] virtual tree::Exp *UnEx() = 0;
    [[nodiscard]] virtual tree::Stm *UnNx() = 0;
    [[nodiscard]] virtual Cx UnCx(err::ErrorMsg *errormsg) = 0;
};

class ExpAndTy {
public:
    Exp *exp_;
    type::Ty *ty_;

    ExpAndTy(Exp *exp, type::Ty *ty) : exp_(exp), ty_(ty) {}
};

class ExExp : public Exp {
public:
    explicit ExExp(tree::Exp *exp) : exp_(exp) {}

    [[nodiscard]] tree::Exp *UnEx() override {
        return exp_;
    }

    [[nodiscard]] tree::Stm *UnNx() override {
        return new tree::ExpStm(exp_);
    }

    [[nodiscard]] Cx UnCx(err::ErrorMsg *errormsg) override {
        auto *stm = new tree::CjumpStm(tree::NE_OP, exp_, new tree::ConstExp(0), nullptr, nullptr);
        std::list<temp::Label **> truePatchList{&(stm->true_label_)};
        std::list<temp::Label **> falsePatchList{&(stm->false_label_)};
        return {PatchList(truePatchList), PatchList(falsePatchList), stm};
    }

private:
    tree::Exp *exp_;
};

class NxExp : public Exp {
public:
    explicit NxExp(tree::Stm *stm) : stm_(stm) {}

    [[nodiscard]] tree::Exp *UnEx() override {
        return new tree::EseqExp(stm_, new tree::ConstExp(0));
    }

    [[nodiscard]] tree::Stm *UnNx() override {
        return stm_;
    }

    [[nodiscard]] Cx UnCx(err::ErrorMsg *errormsg) override {
        assert(false);
    }

private:
    tree::Stm *stm_;
};

class CxExp : public Exp {
public:
    Cx cx_;

    CxExp(PatchList trues, PatchList falses, tree::Stm *stm)
        : cx_(std::move(trues), std::move(falses), stm) {}

    [[nodiscard]] tree::Exp *UnEx() override {
        auto r = temp::TempFactory::NewTemp();
        auto t = temp::LabelFactory::NewLabel();
        auto f = temp::LabelFactory::NewLabel();

        cx_.trues_.DoPatch(t);
        cx_.falses_.DoPatch(f);

        return new tree::EseqExp(
            new tree::MoveStm(new tree::TempExp(r), new tree::ConstExp(1)),
            new tree::EseqExp(
                cx_.stm_,
                new tree::EseqExp(
                    new tree::LabelStm(f),
                    new tree::EseqExp(
                        new tree::MoveStm(new tree::TempExp(r), new tree::ConstExp(0)),
                        new tree::EseqExp(new tree::LabelStm(t), new tree::TempExp(r))))));
    }

    [[nodiscard]] tree::Stm *UnNx() override {
        return cx_.stm_;
    }

    [[nodiscard]] Cx UnCx(err::ErrorMsg *errormsg) override {
        return cx_;
    }
};

void ProgTr::Translate() {
    FillBaseTEnv();
    FillBaseVEnv();

    auto mainLabel = temp::LabelFactory::NamedLabel("tigermain");
    main_level_ = std::make_unique<Level>(nullptr, mainLabel, std::list<bool>{});

    auto main = absyn_tree_->Translate(venv_.get(), tenv_.get(), main_level_.get(),
                                       mainLabel, errormsg_.get());

    frags->PushBack(new frame::ProcFrag(main->exp_->UnNx(), main_level_->frame_));
}

static tr::ExExp *getVoidExp() { return new tr::ExExp(new tree::ConstExp(0)); }

static tr::ExpAndTy *getVoidExpAndNilTy() {
    return new tr::ExpAndTy(new tr::ExExp(new tree::ConstExp(0)), type::NilTy::Instance());
}

static tr::ExpAndTy *getVoidExpAndVoidTy() {
    return new tr::ExpAndTy(new tr::ExExp(new tree::ConstExp(0)), type::VoidTy::Instance());
}

static tree::ExpStm *getVoidStm() {
    return new tree::ExpStm(new tree::ConstExp(0));
}

tree::Exp *Level::StaticLink(Level *targetLevel) {
    Level *currentLevel = this;
    tree::Exp *framePtr = new tree::TempExp(reg_manager->FramePointer());
    while (currentLevel && currentLevel != targetLevel) {
        framePtr = currentLevel->frame_->formals_->front()->ToExp(framePtr);
        currentLevel = currentLevel->parent_;
    }
    return framePtr;
}

std::list<Access *> *Level::Formals() {
    std::list<frame::Access *> *formalList = frame_->formals_;
    std::list<tr::Access *> *formalListWithLevel = new std::list<tr::Access *>();

    bool first = true;
    for (frame::Access *formal : *formalList) {
        if (first) {
            first = false;
            continue;
        }
        formalListWithLevel->push_back(new tr::Access(this, formal));
    }

    return formalListWithLevel;
}

Level::Level(Level *parent, temp::Label *name, std::list<bool> formals)
    : parent_(parent) {
    formals.push_front(true);
    frame_ = new frame::X64Frame(name, formals);
}

} // namespace tr

namespace absyn {

tr::ExpAndTy *AbsynTree::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                   tr::Level *level, temp::Label *label,
                                   err::ErrorMsg *errormsg) const {
  return root_->Translate(venv, tenv, level, label, errormsg);
}

tr::ExpAndTy *SimpleVar::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                   tr::Level *level, temp::Label *label,
                                   err::ErrorMsg *errormsg) const {
  env::EnvEntry *entry = venv->Look(sym_);
  if (entry && typeid(*entry) == typeid(env::VarEntry)) {
    env::VarEntry *var_entry = static_cast<env::VarEntry *>(entry);
    tree::Exp *exp = var_entry->access_->ToExp(level);
    return new tr::ExpAndTy(new tr::ExExp(exp), var_entry->ty_->ActualTy());
  } else {
    errormsg->Error(pos_, "undefined variable %s", sym_->Name().data());
  }
  return tr::getVoidExpAndNilTy();
}

tr::ExpAndTy *FieldVar::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                  tr::Level *level, temp::Label *label,
                                  err::ErrorMsg *errormsg) const {
  tr::ExpAndTy *exp_and_ty = var_->Translate(venv, tenv, level, label, errormsg);
  type::Ty *type = exp_and_ty->ty_->ActualTy();
  if (typeid(*type) != typeid(type::RecordTy)) {
    errormsg->Error(pos_, "not a record type");
    return tr::getVoidExpAndNilTy();
  }

  type::RecordTy *record = static_cast<type::RecordTy *>(type);
  int idx = 0;
  for (type::Field *field : record->fields_->GetList()) {
    if (field->name_->Name() == sym_->Name()) {
      tree::MemExp *exp = new tree::MemExp(new tree::BinopExp(
          tree::BinOp::PLUS_OP, exp_and_ty->exp_->UnEx(),
          new tree::ConstExp(idx * (reg_manager->WordSize()))));
      return new tr::ExpAndTy(new tr::ExExp(exp), field->ty_);
    }
    ++idx;
  }
  errormsg->Error(pos_, "field %s doesn't exist", sym_->Name().c_str());
  return tr::getVoidExpAndNilTy();
}

tr::ExpAndTy *SubscriptVar::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                      tr::Level *level, temp::Label *label,
                                      err::ErrorMsg *errormsg) const {
  tr::ExpAndTy *var = var_->Translate(venv, tenv, level, label, errormsg);
  tr::ExpAndTy *subscript = subscript_->Translate(venv, tenv, level, label, errormsg);
  type::Ty *var_type = var->ty_;
  if (typeid(*var_type) == typeid(type::ArrayTy)) {
    tree::MemExp *exp = new tree::MemExp(new tree::BinopExp(
        tree::BinOp::PLUS_OP, var->exp_->UnEx(),
        new tree::BinopExp(tree::BinOp::MUL_OP, subscript->exp_->UnEx(),
                           new tree::ConstExp(reg_manager->WordSize()))));
    return new tr::ExpAndTy(
        new tr::ExExp(exp),
        (static_cast<type::ArrayTy *>(var_type)->ty_->ActualTy()));
  } else {
    errormsg->Error(pos_, "array type required");
    return tr::getVoidExpAndNilTy();
  }
}

tr::ExpAndTy *VarExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                tr::Level *level, temp::Label *label,
                                err::ErrorMsg *errormsg) const {
  return var_->Translate(venv, tenv, level, label, errormsg);
}

tr::ExpAndTy *NilExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                tr::Level *level, temp::Label *label,
                                err::ErrorMsg *errormsg) const {
  return new tr::ExpAndTy(new tr::ExExp(new tree::ConstExp(0)),
                          type::NilTy::Instance());
}

tr::ExpAndTy *IntExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                tr::Level *level, temp::Label *label,
                                err::ErrorMsg *errormsg) const {
  return new tr::ExpAndTy(new tr::ExExp(new tree::ConstExp(val_)),
                          type::IntTy::Instance());
}

tr::ExpAndTy *StringExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                   tr::Level *level, temp::Label *label,
                                   err::ErrorMsg *errormsg) const {
  temp::Label *lab = temp::LabelFactory::NewLabel();
  frags->PushBack(new frame::StringFrag(lab, str_));
  return new tr::ExpAndTy(new tr::ExExp(new tree::NameExp(lab)),
                          type::StringTy::Instance());
}

tr::ExpAndTy *CallExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                 tr::Level *level, temp::Label *label,
                                 err::ErrorMsg *errormsg) const {
  env::EnvEntry *entry = venv->Look(func_);
  if (entry && typeid(*entry) == typeid(env::FunEntry)) {
    env::FunEntry *func = static_cast<env::FunEntry *>(entry);
    if (func->formals_->GetList().size() != args_->GetList().size()) {
      if (args_->GetList().size() > func->formals_->GetList().size()) {
        errormsg->Error(pos_ - 1,
                        "too many params in function " + func_->Name());
        return tr::getVoidExpAndVoidTy();
      } else {
        errormsg->Error(pos_ - 1, "para type mismatch");
        return tr::getVoidExpAndVoidTy();
      }
    }

    tree::ExpList *args_list = new tree::ExpList();
    if (func->level_) {
      args_list->Append(level->StaticLink(func->level_));
    }

    auto formal_type = func->formals_->GetList().cbegin();
    for (Exp *arg : args_->GetList()) {
      tr::ExpAndTy *actual_exp_and_type = arg->Translate(venv, tenv, level, label, errormsg);
      type::Ty *actual_type = actual_exp_and_type->ty_;
      if (!actual_type->IsSameType(*formal_type)) {
        errormsg->Error(arg->pos_, "para type mismatch");
        return tr::getVoidExpAndVoidTy();
      }
      args_list->Append(actual_exp_and_type->exp_->UnEx());
      ++formal_type;
    }

    tree::CallExp *exp = new tree::CallExp(new tree::NameExp(func_), args_list);
    return new tr::ExpAndTy(new tr::ExExp(exp), func->result_);
  } else {
    errormsg->Error(pos_, "undefined function %s", func_->Name().data());
    return tr::getVoidExpAndVoidTy();
  }
}

tr::ExpAndTy *OpExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                               tr::Level *level, temp::Label *label,
                               err::ErrorMsg *errormsg) const {

  /* TODO: Put your lab5 code here */
  tr::ExpAndTy *left = left_->Translate(venv, tenv, level, label, errormsg);
  tr::ExpAndTy *right = right_->Translate(venv, tenv, level, label, errormsg);
  type::Ty *left_ty = left->ty_;
  type::Ty *right_ty = right->ty_;
  tr::ExpAndTy *result = nullptr;

  if (oper_ == absyn::PLUS_OP || oper_ == absyn::MINUS_OP ||
      oper_ == absyn::TIMES_OP || oper_ == absyn::DIVIDE_OP) {
    if (typeid(*left_ty) != typeid(type::IntTy)) {
      errormsg->Error(left_->pos_, "integer required");
      result = new tr::ExpAndTy(tr::getVoidExp(), type::IntTy::Instance());
    }
    if (typeid(*right_ty) != typeid(type::IntTy)) {
      errormsg->Error(right_->pos_, "integer required");
      result = new tr::ExpAndTy(tr::getVoidExp(), type::IntTy::Instance());
    }

    tree::BinOp bin_op;
    switch (oper_) {
    case absyn::PLUS_OP:
      bin_op = tree::BinOp::PLUS_OP;
      break;
    case absyn::MINUS_OP:
      bin_op = tree::BinOp::MINUS_OP;
      break;
    case absyn::TIMES_OP:
      bin_op = tree::BinOp::MUL_OP;
      break;
    case absyn::DIVIDE_OP:
      bin_op = tree::BinOp::DIV_OP;
      break;
    }

    if (!result) {
      result = new tr::ExpAndTy(
          new tr::ExExp(new tree::BinopExp(bin_op, left->exp_->UnEx(),
                                           right->exp_->UnEx())),
          type::IntTy::Instance());
    }
  } else {
    if (!left_ty->IsSameType(right_ty)) {
      errormsg->Error(pos_, "same type required");
      result = left;
    }

    if (oper_ == absyn::AND_OP || oper_ == absyn::OR_OP) {
      temp::Label *second_condition_label = temp::LabelFactory::NewLabel();
      tr::Cx left_cx = left->exp_->UnCx(errormsg);
      tr::Cx right_cx = right->exp_->UnCx(errormsg);

      switch (oper_) {
      case absyn::AND_OP: {
        left_cx.trues_.DoPatch(second_condition_label);
        tr::PatchList true_list = tr::PatchList(right_cx.trues_);
        tr::PatchList false_list =
            tr::PatchList::JoinPatch(left_cx.falses_, right_cx.falses_);
        tree::SeqStm *stm = new tree::SeqStm(
            left_cx.stm_,
            new tree::SeqStm(new tree::LabelStm(second_condition_label),
                             right_cx.stm_));
        result = new tr::ExpAndTy(new tr::CxExp(true_list, false_list, stm),
                                  type::IntTy::Instance());
        break;
      }

      case absyn::OR_OP: {
        left_cx.falses_.DoPatch(second_condition_label);
        tr::PatchList true_list =
            tr::PatchList::JoinPatch(left_cx.trues_, right_cx.trues_);
        tr::PatchList false_list = tr::PatchList(right_cx.falses_);
        tree::SeqStm *stm = new tree::SeqStm(
            left_cx.stm_,
            new tree::SeqStm(new tree::LabelStm(second_condition_label),
                             right_cx.stm_));
        result = new tr::ExpAndTy(new tr::CxExp(true_list, false_list, stm),
                                  type::IntTy::Instance());
        break;
      }
      }

    } else {

      tree::CjumpStm *cjump_stm = nullptr;
      tree::RelOp rel_op;
      switch (oper_) {
      case absyn::EQ_OP:
      case absyn::NEQ_OP: {
        rel_op =
            (oper_ == absyn::EQ_OP) ? tree::RelOp::EQ_OP : tree::RelOp::NE_OP;
        if (left_ty->IsSameType(type::StringTy::Instance())) {
          tree::ExpList *args = new tree::ExpList();
          args->Append(left->exp_->UnEx());
          args->Append(right->exp_->UnEx());
          tree::ConstExp *expected = (oper_ == absyn::EQ_OP)
                                         ? new tree::ConstExp(1)
                                         : new tree::ConstExp(0);
          // string_equal return 1 for equal, 0 for not
          cjump_stm = new tree::CjumpStm(
              tree::RelOp::EQ_OP, frame::ExternalCall("string_equal", args), expected,
              nullptr, nullptr);
        } else {
          cjump_stm = new tree::CjumpStm(rel_op, left->exp_->UnEx(),
                                         right->exp_->UnEx(), nullptr, nullptr);
        }
      }
      case absyn::LT_OP:
        rel_op = tree::RelOp::LT_OP;
        break;
      case absyn::LE_OP:
        rel_op = tree::RelOp::LE_OP;
        break;
      case absyn::GT_OP:
        rel_op = tree::RelOp::GT_OP;
        break;
      case absyn::GE_OP:
        rel_op = tree::RelOp::GE_OP;
        break;
      case absyn::ABSYN_OPER_COUNT:
        rel_op = tree::RelOp::REL_OPER_COUNT;
        break;
      }

      if (!cjump_stm) {
        cjump_stm = new tree::CjumpStm(rel_op, left->exp_->UnEx(),
                                       right->exp_->UnEx(), nullptr, nullptr);
      }
      tr::PatchList true_list = tr::PatchList({&(cjump_stm->true_label_)});
      tr::PatchList false_list = tr::PatchList({&(cjump_stm->false_label_)});
      result = new tr::ExpAndTy(new tr::CxExp(true_list, false_list, cjump_stm),
                                type::IntTy::Instance());
    }
  }

  return result;
}

tr::ExpAndTy *RecordExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                   tr::Level *level, temp::Label *label,
                                   err::ErrorMsg *errormsg) const {

  // check if record type is defined
  type::Ty *type = tenv->Look(typ_);
  if (type && typeid(*(type->ActualTy())) == typeid(type::RecordTy)) {
    type::RecordTy *recordTy = static_cast<type::RecordTy *>(type->ActualTy());

    // Record creation and initialization
    // { f1 = e1 ; f2 = e2; … ; fn = en }

    temp::Temp *r = temp::TempFactory::NewTemp();
    tree::ExpList *args = new tree::ExpList();
    size_t fieldSize = recordTy->fields_->GetList().size();
    args->Append(new tree::ConstExp(fieldSize * reg_manager->WordSize()));

    tree::MoveStm *left_move_stm = new tree::MoveStm(
        new tree::TempExp(r), frame::ExternalCall("alloc_record", args));

    tree::SeqStm *initialization_list_head = nullptr;
    tree::SeqStm *left_init_seq_stm =
        new tree::SeqStm(left_move_stm, initialization_list_head);
    tree::SeqStm *last_seq_stm = left_init_seq_stm;

    size_t actualFieldSize = fields_->GetList().size();
    if (actualFieldSize != fieldSize) {
      errormsg->Error(pos_, "num of field doesn't match");
    }

    auto record_field = recordTy->fields_->GetList().cbegin();
    type::FieldList *fieldList = new type::FieldList();
    int idx = 0;
    for (absyn::EField *actual_field : fields_->GetList()) {

      auto actual_field_exp_and_ty =
          actual_field->exp_->Translate(venv, tenv, level, label, errormsg);
      fieldList->Append(
          new type::Field(actual_field->name_, actual_field_exp_and_ty->ty_));

      std::string actualFieldName = actual_field->name_->Name();
      if (actualFieldName != (*record_field)->name_->Name()) {
        errormsg->Error(pos_, "field %s doesn't exist",
                        actualFieldName.data());
      }
      if (!actual_field_exp_and_ty->ty_->IsSameType((*record_field)->ty_)) {
        errormsg->Error(pos_, "type of field %s doesn't match",
                        actualFieldName.data());
      }

      tree::BinopExp *address_exp =
          new tree::BinopExp(tree::BinOp::PLUS_OP, new tree::TempExp(r),
                             new tree::ConstExp(idx * reg_manager->WordSize()));
      tree::MoveStm *move_stm = new tree::MoveStm(
          new tree::MemExp(address_exp), actual_field_exp_and_ty->exp_->UnEx());
      tree::SeqStm *seq_stm = new tree::SeqStm(move_stm, nullptr);
      last_seq_stm->right_ = seq_stm;
      last_seq_stm = seq_stm;

      ++record_field;
      ++idx;
    }

    last_seq_stm->right_ = tr::getVoidStm();
    tree::EseqExp *eseq_exp =
        new tree::EseqExp(left_init_seq_stm, new tree::TempExp(r));
    if (actualFieldSize == 0) {
      return new tr::ExpAndTy(new tr::ExExp(eseq_exp), type::NilTy::Instance());
    }
    return new tr::ExpAndTy(new tr::ExExp(eseq_exp), type->ActualTy());
  } else {
    errormsg->Error(pos_, "undefined type %s", typ_->Name().data());
    return tr::getVoidExpAndNilTy();
  }
}

tr::ExpAndTy *SeqExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                tr::Level *level, temp::Label *label,
                                err::ErrorMsg *errormsg) const {

  std::list<absyn::Exp *> exp_list = seq_->GetList();
  absyn::Exp *last_exp = exp_list.back();
  exp_list.pop_back();

  if (exp_list.empty()) {
    tr::ExpAndTy *last_exp_and_ty =
        last_exp->Translate(venv, tenv, level, label, errormsg);
    return new tr::ExpAndTy(last_exp_and_ty->exp_, last_exp_and_ty->ty_);
  }

  tree::SeqStm *stm = nullptr;
  tree::SeqStm *current_stm = stm;
  bool first = true;

  for (absyn::Exp *exp : exp_list) {
    tr::ExpAndTy *exp_and_ty =
        exp->Translate(venv, tenv, level, label, errormsg);

    if (first) {
      first = false;
      current_stm = stm = new tree::SeqStm(exp_and_ty->exp_->UnNx(), nullptr);
    } else {
      current_stm->right_ = new tree::SeqStm(exp_and_ty->exp_->UnNx(), nullptr);
      current_stm = static_cast<tree::SeqStm *>(current_stm->right_);
    }
  }

  tr::ExpAndTy *last_exp_and_ty =
      last_exp->Translate(venv, tenv, level, label, errormsg);

  tree::Exp *exp = nullptr;
  current_stm->right_ = tr::getVoidStm();
  exp = new tree::EseqExp(stm, last_exp_and_ty->exp_->UnEx());

  return new tr::ExpAndTy(new tr::ExExp(exp), last_exp_and_ty->ty_);
}

tr::ExpAndTy *AssignExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                   tr::Level *level, temp::Label *label,
                                   err::ErrorMsg *errormsg) const {

  tr::ExpAndTy *var = var_->Translate(venv, tenv, level, label, errormsg);
  tr::ExpAndTy *exp = exp_->Translate(venv, tenv, level, label, errormsg);
  type::Ty *varTy = var->ty_->ActualTy();
  type::Ty *expTy = exp->ty_->ActualTy();

  bool isSameType = varTy->IsSameType(expTy);
  if (isSameType) {
    if (typeid(*var_) == typeid(absyn::SimpleVar)) {
      absyn::SimpleVar *var = static_cast<absyn::SimpleVar *>(var_);
      env::VarEntry *entry =
          static_cast<env::VarEntry *>(venv->Look(var->sym_));
      if (entry->readonly_) {
        errormsg->Error(pos_, "loop variable can't be assigned");
      }
    }
  } else {
    errormsg->Error(pos_, "unmatched assign exp");
  }

  tree::MoveStm *stm = new tree::MoveStm(var->exp_->UnEx(), exp->exp_->UnEx());
  return new tr::ExpAndTy(new tr::NxExp(stm), type::VoidTy::Instance());
}

tr::ExpAndTy *IfExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                               tr::Level *level, temp::Label *label,
                               err::ErrorMsg *errormsg) const {

  tr::ExpAndTy *test_exp_and_ty =
      test_->Translate(venv, tenv, level, label, errormsg);
  tr::ExpAndTy *then_exp_and_ty =
      then_->Translate(venv, tenv, level, label, errormsg);
  type::Ty *thenTy = then_exp_and_ty->ty_;

  bool hasErrors = errormsg->AnyErrors();

  if (hasErrors) {
    return new tr::ExpAndTy(tr::getVoidExp(), thenTy);
  }

  temp::Label *t = temp::LabelFactory::NewLabel();
  temp::Label *f = temp::LabelFactory::NewLabel();
  temp::Label *joint = temp::LabelFactory::NewLabel();
  temp::Temp *r = temp::TempFactory::NewTemp();
  tr::Cx test_cx = test_exp_and_ty->exp_->UnCx(errormsg);
  test_cx.trues_.DoPatch(t);
  test_cx.falses_.DoPatch(f);

  if (!elsee_) {
    if (!thenTy->IsSameType(type::VoidTy::Instance())) {
      errormsg->Error(then_->pos_, "if-then exp's body must produce no value");
      return new tr::ExpAndTy(tr::getVoidExp(), thenTy);
    }
    tree::SeqStm *stm = new tree::SeqStm(
        test_cx.stm_,
        new tree::SeqStm(new tree::LabelStm(t),
                         new tree::SeqStm(then_exp_and_ty->exp_->UnNx(),
                                          new tree::LabelStm(f))));
    return new tr::ExpAndTy(new tr::NxExp(stm), thenTy);
  } else {
    tr::ExpAndTy *else_exp_and_ty =
        elsee_->Translate(venv, tenv, level, label, errormsg);
    type::Ty *elseTy = else_exp_and_ty->ty_;
    // check if then and else have same type
    if (!elseTy->IsSameType(thenTy)) {
      errormsg->Error(pos_, "then exp and else exp type mismatch");
      return new tr::ExpAndTy(tr::getVoidExp(), thenTy);
    }

    tree::SeqStm *true_stm = new tree::SeqStm(
        new tree::LabelStm(t),
        new tree::SeqStm(
            new tree::MoveStm(new tree::TempExp(r),
                              then_exp_and_ty->exp_->UnEx()),
            new tree::JumpStm(new tree::NameExp(joint),
                              new std::vector<temp::Label *>{joint})));
    tree::SeqStm *false_stm = new tree::SeqStm(
        new tree::LabelStm(f),
        new tree::SeqStm(
            new tree::MoveStm(new tree::TempExp(r),
                              else_exp_and_ty->exp_->UnEx()),
            new tree::JumpStm(new tree::NameExp(joint),
                              new std::vector<temp::Label *>{joint})));
    tree::EseqExp *exp = new tree::EseqExp(
        new tree::SeqStm(
            test_cx.stm_,
            new tree::SeqStm(
                true_stm,
                new tree::SeqStm(false_stm, new tree::LabelStm(joint)))),
        new tree::TempExp(r));
    return new tr::ExpAndTy(new tr::ExExp(exp), thenTy);
  }
}

tr::ExpAndTy *WhileExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                  tr::Level *level, temp::Label *label,
                  err::ErrorMsg *errormsg) const {

  temp::Label *doneLabel = temp::LabelFactory::NewLabel();
  temp::Label *extraLabel = temp::LabelFactory::NewLabel();

  tr::ExpAndTy *test_exp_and_ty =
    test_->Translate(venv, tenv, level, label, errormsg);
  tr::ExpAndTy *extra_exp_and_ty =
    test_->Translate(venv, tenv, level, extraLabel, errormsg);

  tr::ExpAndTy *body_exp_and_ty =
    body_->Translate(venv, tenv, level, doneLabel, errormsg);
  type::Ty *bodyTy = body_exp_and_ty->ty_;
  if (!bodyTy->IsSameType(type::VoidTy::Instance())) {
  errormsg->Error(pos_, "while body must produce no value");
  }

  temp::Label *testLabel = temp::LabelFactory::NewLabel();
  temp::Label *bodyLabel = temp::LabelFactory::NewLabel();
  tr::Cx test_cx = test_exp_and_ty->exp_->UnCx(errormsg);
  tr::Cx extra_cx = extra_exp_and_ty->exp_->UnCx(errormsg);

  test_cx.trues_.DoPatch(bodyLabel);
  test_cx.falses_.DoPatch(doneLabel);
  extra_cx.trues_.DoPatch(extraLabel);
  extra_cx.falses_.DoPatch(doneLabel);

  tree::SeqStm *seq_stm = new tree::SeqStm(
    new tree::LabelStm(testLabel),
    new tree::SeqStm(
      test_cx.stm_,
      new tree::SeqStm(
        new tree::LabelStm(bodyLabel),
        new tree::SeqStm(
          body_exp_and_ty->exp_->UnNx(),
          new tree::SeqStm(
            new tree::JumpStm(
              new tree::NameExp(testLabel),
              new std::vector<temp::Label *>{testLabel}),
            new tree::SeqStm(
              new tree::LabelStm(extraLabel),
              new tree::SeqStm(
                extra_exp_and_ty->exp_->UnNx(),
                new tree::SeqStm(
                  new tree::JumpStm(
                    new tree::NameExp(extraLabel),
                    new std::vector<temp::Label *>{extraLabel}),
                  new tree::LabelStm(doneLabel)))))))));

  return new tr::ExpAndTy(new tr::NxExp(seq_stm), type::VoidTy::Instance());
}

tr::ExpAndTy *ForExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                tr::Level *level, temp::Label *label,
                err::ErrorMsg *errormsg) const {

  venv->BeginScope();
  venv->Enter(var_, new env::VarEntry(tr::Access::AllocLocal(level, false),
                    type::IntTy::Instance(), true));
  tr::ExpAndTy *lo_exp_and_ty =
    lo_->Translate(venv, tenv, level, label, errormsg);
  tr::ExpAndTy *hi_exp_and_ty =
    hi_->Translate(venv, tenv, level, label, errormsg);

  if (!(lo_exp_and_ty->ty_)->IsSameType(type::IntTy::Instance())) {
  errormsg->Error(lo_->pos_, "for exp's range type is not integer");
  }
  if (!(hi_exp_and_ty->ty_)->IsSameType(type::IntTy::Instance())) {
  errormsg->Error(hi_->pos_, "for exp's range type is not integer");
  }

  temp::Label *body_label = temp::LabelFactory::NewLabel();
  temp::Label *done_label = temp::LabelFactory::NewLabel();
  temp::Label *inc_label = temp::LabelFactory::NewLabel();
  temp::Label *extra_label = temp::LabelFactory::NewLabel();

  tr::ExpAndTy *body_exp_and_ty =
    body_->Translate(venv, tenv, level, done_label, errormsg);
  type::Ty *bodyTy = body_exp_and_ty->ty_;
  if (!bodyTy->IsSameType(type::VoidTy::Instance())) {
  errormsg->Error(pos_, "for body should produce no value");
  }

  temp::Temp *limit = temp::TempFactory::NewTemp();
  temp::Temp *extra = temp::TempFactory::NewTemp();
  env::VarEntry *loop_i_entry = static_cast<env::VarEntry *>(venv->Look(var_));
  temp::Temp *loop_i =
    (static_cast<frame::InRegAccess *>(loop_i_entry->access_->access_))->reg;

  auto loop_i_init_stmt =
    new tree::MoveStm(new tree::TempExp(loop_i), lo_exp_and_ty->exp_->UnEx());
  auto limit_init_stmt =
    new tree::MoveStm(new tree::TempExp(limit), hi_exp_and_ty->exp_->UnEx());
  auto extra_init_stmt =
    new tree::MoveStm(new tree::TempExp(extra), hi_exp_and_ty->exp_->UnEx());

  auto i_gt_limit_cjump_stmt =
    new tree::CjumpStm(tree::RelOp::GT_OP, new tree::TempExp(loop_i),
             new tree::TempExp(limit), done_label, body_label);

  auto i_eq_limit_cjump_stmt =
    new tree::CjumpStm(tree::RelOp::EQ_OP, new tree::TempExp(loop_i),
             new tree::TempExp(limit), done_label, inc_label);

  auto loop_i_increase_stmt = new tree::MoveStm(
    new tree::TempExp(loop_i),
    new tree::BinopExp(tree::BinOp::PLUS_OP, new tree::TempExp(loop_i),
             new tree::ConstExp(1)));

  auto extra_increase_stmt = new tree::MoveStm(
    new tree::TempExp(extra),
    new tree::BinopExp(tree::BinOp::PLUS_OP, new tree::TempExp(extra),
             new tree::ConstExp(1)));

  tree::Stm *stm = new tree::SeqStm(
    loop_i_init_stmt,
    new tree::SeqStm(
      limit_init_stmt,
      new tree::SeqStm(
        extra_init_stmt,
        new tree::SeqStm(
          i_gt_limit_cjump_stmt,
          new tree::SeqStm(
            new tree::LabelStm(body_label),
            new tree::SeqStm(
              body_exp_and_ty->exp_->UnNx(),
              new tree::SeqStm(
                i_eq_limit_cjump_stmt,
                new tree::SeqStm(
                  new tree::LabelStm(inc_label),
                  new tree::SeqStm(
                    loop_i_increase_stmt,
                    new tree::SeqStm(
                      extra_increase_stmt,
                      new tree::SeqStm(
                        new tree::JumpStm(
                          new tree::NameExp(body_label),
                          new std::vector<temp::Label *>(
                            {body_label})),
                        new tree::LabelStm(done_label))))))))))));

  venv->EndScope();
  return new tr::ExpAndTy(new tr::NxExp(stm), type::VoidTy::Instance());
}

tr::ExpAndTy *BreakExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                  tr::Level *level, temp::Label *label,
                                  err::ErrorMsg *errormsg) const {
  auto jump_stm = new tree::JumpStm(
      new tree::NameExp(label), new std::vector<temp::Label *>{label});
  auto result = new tr::ExpAndTy(new tr::NxExp(jump_stm), type::VoidTy::Instance());
  return result;
}

tr::ExpAndTy *LetExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                tr::Level *level, temp::Label *label,
                                err::ErrorMsg *errormsg) const {
  venv->BeginScope();
  tenv->BeginScope();
  tree::Stm *dec_list_stm = nullptr;
  for (Dec *dec : decs_->GetList()) {
    auto dec_stm = dec->Translate(venv, tenv, level, label, errormsg)->UnNx();
    dec_list_stm = dec_list_stm ? new tree::SeqStm(dec_list_stm, dec_stm) : dec_stm;
  }

  auto body_exp_and_ty = new tr::ExpAndTy(nullptr, nullptr);
  if (!body_) {
    body_exp_and_ty->ty_ = type::VoidTy::Instance();
    body_exp_and_ty->exp_ = tr::getVoidExp();
  } else {
    body_exp_and_ty = body_->Translate(venv, tenv, level, label, errormsg);
  }

  tenv->EndScope();
  venv->EndScope();

  auto result_exp = dec_list_stm ? new tree::EseqExp(dec_list_stm, body_exp_and_ty->exp_->UnEx()) : body_exp_and_ty->exp_->UnEx();

  return new tr::ExpAndTy(new tr::ExExp(result_exp), body_exp_and_ty->ty_->ActualTy());
}

tr::ExpAndTy *ArrayExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                  tr::Level *level, temp::Label *label,
                                  err::ErrorMsg *errormsg) const {
  auto type = tenv->Look(typ_);

  if (type && typeid(*(type->ActualTy())) == typeid(type::ArrayTy)) {
    auto size_exp_and_ty = size_->Translate(venv, tenv, level, label, errormsg);
    auto size_ty = size_exp_and_ty->ty_;
    if (typeid(*size_ty) != typeid(type::IntTy)) {
      errormsg->Error(pos_, "size of array should be int");
      return new tr::ExpAndTy(tr::getVoidExp(), type::VoidTy::Instance());
    }

    auto arrayTy = static_cast<type::ArrayTy *>(type->ActualTy())->ty_;
    auto init_exp_and_ty = init_->Translate(venv, tenv, level, label, errormsg);
    auto initTy = init_exp_and_ty->ty_;
    if (!initTy->IsSameType(arrayTy)) {
      errormsg->Error(pos_, "type mismatch");
      return init_exp_and_ty;
    }

    auto args = new tree::ExpList();
    args->Append(size_exp_and_ty->exp_->UnEx());
    args->Append(init_exp_and_ty->exp_->UnEx());
    auto r = temp::TempFactory::NewTemp();
    auto exp = new tree::EseqExp(
        new tree::MoveStm(new tree::TempExp(r),
                          frame::ExternalCall("init_array", args)),
        new tree::TempExp(r));

    return new tr::ExpAndTy(new tr::ExExp(exp), type->ActualTy());
  } else {
    errormsg->Error(pos_, "undefined array %s", typ_);
    return new tr::ExpAndTy(tr::getVoidExp(), type::VoidTy::Instance());
  }
}

tr::ExpAndTy *VoidExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                 tr::Level *level, temp::Label *label,
                                 err::ErrorMsg *errormsg) const {
  return tr::getVoidExpAndVoidTy();
}

tr::Exp *FunctionDec::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                tr::Level *level, temp::Label *label,
                                err::ErrorMsg *errormsg) const {

  for (absyn::FunDec *function : functions_->GetList()) {

    temp::Label *function_label =
        temp::LabelFactory::NamedLabel(function->name_->Name());

    type::Ty *result_ty = function->result_ ? tenv->Look(function->result_)
                                            : type::VoidTy::Instance();
    type::TyList *formals_ty =
        function->params_->MakeFormalTyList(tenv, errormsg);

    for (absyn::FunDec *anotherFunction : functions_->GetList()) {
      if (function != anotherFunction &&
          function->name_ == anotherFunction->name_) {
        errormsg->Error(pos_, "two functions have the same name");
        return new tr::ExExp(new tree::ConstExp(0));
      }
    }

    venv->Enter(function->name_, new env::FunEntry(level, function_label,
                                                   formals_ty, result_ty));
  }

  for (absyn::FunDec *function : functions_->GetList()) {

    env::FunEntry *entry =
        static_cast<env::FunEntry *>(venv->Look(function->name_));

    std::list<bool> formal_escapes;
    for (Field *param : function->params_->GetList()) {
      formal_escapes.push_back(param->escape_);
    }

    tr::Level *function_level =
        new tr::Level(level, entry->label_, formal_escapes);

    venv->BeginScope();

    type::Ty *result_ty = function->result_ ? tenv->Look(function->result_)
                                            : type::VoidTy::Instance();
    type::TyList *formals_ty =
        function->params_->MakeFormalTyList(tenv, errormsg);

    std::list<tr::Access *> *access_list = function_level->Formals();

    auto formal_ty = formals_ty->GetList().cbegin();
    auto access = access_list->cbegin();
    for (absyn::Field *param : function->params_->GetList()) {
      venv->Enter(param->name_, new env::VarEntry(*access, *formal_ty));
      ++formal_ty;
      ++access;
    }

    tr::ExpAndTy *body_exp_and_ty = function->body_->Translate(
        venv, tenv, function_level, entry->label_, errormsg);

    if (!body_exp_and_ty->ty_->IsSameType(result_ty)) {
      errormsg->Error(pos_, "procedure returns value");
    }

    venv->EndScope();

    frags->PushBack(new frame::ProcFrag(
        frame::ProcEntryExit1(
            function_level->frame_,
            new tree::MoveStm(new tree::TempExp(reg_manager->ReturnValue()),
                              body_exp_and_ty->exp_->UnEx())),
        function_level->frame_));
  }
  return tr::getVoidExp();
}

tr::Exp *VarDec::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                           tr::Level *level, temp::Label *label,
                           err::ErrorMsg *errormsg) const {

  tr::ExpAndTy *init_exp_and_ty =
      init_->Translate(venv, tenv, level, label, errormsg);
  auto init_ty = init_exp_and_ty->ty_;

  if (typ_) {

    type::Ty *type_id = tenv->Look(typ_);

    if (!type_id) {
      errormsg->Error(pos_, "undefined type %s", typ_->Name().data());
      return new tr::NxExp(tr::getVoidStm());
    }

    if (!init_ty->ActualTy()->IsSameType(type_id)) {
      errormsg->Error(init_->pos_, "type mismatch");
      return new tr::NxExp(tr::getVoidStm());
    }

    tr::Access *access = tr::Access::AllocLocal(level, escape_);
    venv->Enter(var_, new env::VarEntry(access, type_id));
    return new tr::NxExp(new tree::MoveStm(
        access->access_->ToExp(new tree::TempExp(reg_manager->FramePointer())),
        init_exp_and_ty->exp_->UnEx()));
  } else {

    if (init_ty->ActualTy()->IsSameType(type::NilTy::Instance()) &&
        typeid(*(init_ty->ActualTy())) != typeid(type::RecordTy)) {
      errormsg->Error(pos_, "init should not be nil without type specified");
      return new tr::NxExp(tr::getVoidStm());
    }

    tr::Access *access = tr::Access::AllocLocal(level, escape_);
    venv->Enter(var_, new env::VarEntry(access, init_ty->ActualTy()));
    return new tr::NxExp(new tree::MoveStm(
        access->access_->ToExp(new tree::TempExp(reg_manager->FramePointer())),
        init_exp_and_ty->exp_->UnEx()));
  }
}

tr::Exp *TypeDec::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                            tr::Level *level, temp::Label *label,
                            err::ErrorMsg *errormsg) const {

  auto typesList = types_->GetList();
  for (NameAndTy *type : typesList) {
    for (NameAndTy *anotherType : typesList) {
      if (type != anotherType && type->name_ == anotherType->name_) {
        errormsg->Error(pos_, "two types have the same name");
        return tr::getVoidExp();
      }
    }

    tenv->Enter(type->name_, new type::NameTy(type->name_, nullptr));
  }

  for (NameAndTy *type : typesList) {
    type::NameTy *name_ty = static_cast<type::NameTy *>(tenv->Look(type->name_));
    name_ty->ty_ = type->ty_->Translate(tenv, errormsg);

    if (!name_ty->ty_) {
      errormsg->Error(pos_, "undefined type %s", type->name_);
      break;
    }

    type->name_->Name().data();

    type::Ty *tmp = tenv->Look(type->name_), *next, *start = tmp;
    while (tmp) {
      if (typeid(*tmp) != typeid(type::NameTy))
        break;

      next = (static_cast<type::NameTy *>(tmp))->ty_;
      if (next == start) {
        errormsg->Error(pos_, "illegal type cycle");
        return tr::getVoidExp();
      }

      tmp = next;
    }
  }
  return tr::getVoidExp();
}

type::Ty *NameTy::Translate(env::TEnvPtr tenv, err::ErrorMsg *errormsg) const {
  type::Ty *type = tenv->Look(name_);
  if (!type) {
    errormsg->Error(pos_, "undefined type %s", name_->Name().data());
    return type::NilTy::Instance();
  }
  return type;
}

type::Ty *RecordTy::Translate(env::TEnvPtr tenv, err::ErrorMsg *errormsg) const {
  return new type::RecordTy(record_->MakeFieldList(tenv, errormsg));
}

type::Ty *ArrayTy::Translate(env::TEnvPtr tenv, err::ErrorMsg *errormsg) const {
  type::Ty *type = tenv->Look(array_);
  if (!type) {
    errormsg->Error(pos_, "undefined type %s", array_->Name().data());
    return type::NilTy::Instance();
  }
  return new type::ArrayTy(type);
}

} // namespace absyn
