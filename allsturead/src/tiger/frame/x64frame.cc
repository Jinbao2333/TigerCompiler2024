#include "tiger/frame/x64frame.h"

extern frame::RegManager *reg_manager;

namespace frame {

class InFrameAccess : public Access {
  /* TODO: Put your lab5 code here */

public:
  int offset;
  explicit InFrameAccess(int offset) : offset(offset) {}
  
  /* End for lab5 code */
};

class InRegAccess : public Access {
public:
  temp::Temp *reg;

  explicit InRegAccess(temp::Temp *reg) : reg(reg) {}
  /* TODO: Put your lab5 code here */

  /* End for lab5 code */
};

class X64Frame : public Frame {
  /* TODO: Put your lab5 code here */

  /* End for lab5 code */
};

/* End for lab5 code */

} // namespace frame