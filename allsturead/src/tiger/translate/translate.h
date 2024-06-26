#ifndef TIGER_TRANSLATE_TRANSLATE_H_
#define TIGER_TRANSLATE_TRANSLATE_H_

#include <list>
#include <memory>

#include "tiger/absyn/absyn.h"
#include "tiger/env/env.h"
#include "tiger/errormsg/errormsg.h"
#include "tiger/frame/frame.h"
#include "tiger/semant/types.h"


namespace tr {

class Exp;
class ExpAndTy;
class Level;


class PatchList {
public:
  PatchList() = default;
  explicit PatchList(std::list<temp::Label **> patch_list) 
      : patch_list_(std::move(patch_list)) {}

  void DoPatch(temp::Label *label) {
    for (auto &patch : patch_list_) {
      *patch = label;
    }
  }

  [[nodiscard]] const std::list<temp::Label **> &GetList() const {
    return patch_list_;
  }

  static PatchList JoinPatch(const PatchList &first, const PatchList &second) {
    PatchList result(first.GetList());
    result.patch_list_.insert(result.patch_list_.end(), second.patch_list_.begin(), second.patch_list_.end());
    return result;
  }

private:
  std::list<temp::Label **> patch_list_;
};

class Access {
public:
  Level *level_;
  frame::Access *access_;

  Access(Level *level, frame::Access *access)
      : level_(level), access_(access) {}
  static Access *AllocLocal(Level *level, bool escape);
  tree::Exp *ToExp(Level *currentLevel);
};

class Level {
public:
  frame::Frame *frame_;
  Level *parent_;

  Level(Level *parent, temp::Label *name,
        std::list<bool> formals);
  Level(frame::Frame *frame, Level *parent) : frame_(frame), parent_(parent) {}
  std::list<Access *> *Formals();
  tree::Exp *StaticLink(Level *targetLevel);
};

class ProgTr {
public:
  ProgTr(std::unique_ptr<absyn::AbsynTree> absyn_tree,
         std::unique_ptr<err::ErrorMsg> errormsg)
      : absyn_tree_(std::move(absyn_tree)), errormsg_(std::move(errormsg)),
        tenv_(std::make_unique<env::TEnv>()),
        venv_(std::make_unique<env::VEnv>()){};

  /**
   * Translate IR tree
   */
  void Translate();

  /**
   * Transfer the ownership of errormsg to outer scope
   * @return unique pointer to errormsg
   */
  std::unique_ptr<err::ErrorMsg> TransferErrormsg() {
    return std::move(errormsg_);
  }

private:
  std::unique_ptr<absyn::AbsynTree> absyn_tree_;
  std::unique_ptr<err::ErrorMsg> errormsg_;
  std::unique_ptr<Level> main_level_;
  std::unique_ptr<env::TEnv> tenv_;
  std::unique_ptr<env::VEnv> venv_;

  // Fill base symbol for var env and type env
  void FillBaseVEnv();
  void FillBaseTEnv();
};

} // namespace tr

#endif
