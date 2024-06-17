from type import *
import re

class Instruction(object):
    """
    Abstract base class for different types of assembly instructions.
    """

    def __init__(self, kind):
        self._kind = kind

    def execute(self, state_table):
        """
        Abstract method to execute the instruction.
        Must be implemented by subclasses.
        """
        raise NotImplementedError("Invoke interface in abstract class:Instruction")

    def print_kind(self):
        """
        Print the kind of the instruction.
        """
        print(self._kind)


class MovInstruction(Instruction):
    """
    Class representing the 'MOV' instruction, which moves data from one operand to another.
    """

    def __init__(self, kind, arg0, arg1):
        super(MovInstruction, self).__init__(kind)
        self._arg0 = arg0
        self._arg1 = arg1

    def execute(self, state_table):
        # Get value of first operand
        arg0_val = state_table.get_operand(self._arg0)
        
        # Get value of second operand, it must be mem/register, and store result in it
        if re.match(OperandEnum.Reg.value, self._arg1):
            state_table.store_reg(self._arg1, arg0_val)
        elif re.match(OperandEnum.Imm.value, self._arg1):
            raise AssertionError("Should not use Imm as second operand")
        else:
            mem_add = state_table.get_mem_address(self._arg1)
            state_table.store_mem(mem_add, arg0_val)
        
    def print_kind(self):
        print("Mov Instruction")
        

class AddInstruction(Instruction):
    """
    Class representing the 'ADD' instruction, which adds two operands and stores the result.
    """

    def __init__(self, kind, arg0, arg1):
        super(AddInstruction, self).__init__(kind)
        self._arg0 = arg0
        self._arg1 = arg1

    def execute(self, state_table):
        # Get value of first operand
        arg0_val = state_table.get_operand(self._arg0)
        
        # Get value of second operand, it must be mem/register, and store result in it
        if re.match(OperandEnum.Reg.value, self._arg1):
            arg1_val = state_table.load_reg(self._arg1)
            state_table.store_reg(self._arg1, arg0_val + arg1_val)
        elif re.match(OperandEnum.Imm.value, self._arg1):
            raise AssertionError("Should not use Imm as second operand")
        else:
            arg1_val = state_table.load_mem(self._arg1)
            state_table.store_mem(self._arg1, arg0_val + arg1_val)

    def print_kind(self):
        print("Add Instruction")


class SubInstruction(Instruction):
    """
    Class representing the 'SUB' instruction, which subtracts one operand from another and stores the result.
    """

    def __init__(self, kind, arg0, arg1):
        super(SubInstruction, self).__init__(kind)
        self._arg0 = arg0
        self._arg1 = arg1

    def execute(self, state_table):
        # Get value of first operand
        arg0_val = state_table.get_operand(self._arg0)
        
        # Get value of second operand, it must be mem/register, and store result in it
        if re.match(OperandEnum.Reg.value, self._arg1):
            arg1_val = state_table.load_reg(self._arg1)
            state_table.store_reg(self._arg1, arg1_val - arg0_val)
        elif re.match(OperandEnum.Imm.value, self._arg1):
            raise AssertionError("Should not use Imm as second operand")
        else:
            arg1_val = state_table.load_mem(self._arg1)
            state_table.store_mem(self._arg1, arg1_val - arg0_val)

    def print_kind(self):
        print("Sub Instruction")


class ImulInstruction(Instruction):
    """
    Class representing the 'IMUL' instruction, which multiplies the RAX register with another operand and stores the result in RAX.
    """

    def __init__(self, kind, arg0):
        super(ImulInstruction, self).__init__(kind)
        self._arg0 = arg0

    def execute(self, state_table):
        # Get value of first operand
        arg0_val = state_table.get_operand(self._arg0)
        rax_val = state_table.get_operand("%rax")

        result = arg0_val * rax_val
        mark_bit = ~(1 << 63)
        sign_bit = 1 if result < 0 else 0
        rax_res = (result & mark_bit) | (sign_bit << 63)
        state_table.store_reg('%rax', rax_res)

    def print_kind(self):
        print("Imul Instruction")


class IdivInstruction(Instruction):
    """
    Class representing the 'IDIV' instruction, which divides the RAX register by another operand and stores the quotient in RAX and the remainder in RDX.
    """

    def __init__(self, kind, arg0):
        super(IdivInstruction, self).__init__(kind)
        self._arg0 = arg0

    def execute(self, state_table):
        # Get value of first operand
        arg0_val = state_table.get_operand(self._arg0)
        rax_val = state_table.load_reg('%rax')

        assert arg0_val != 0, "Divide 0"

        rax_res = rax_val // arg0_val
        rdx_res = rax_val %  arg0_val

        # print("%d / %d = %d ··· %d" % (rax_val, arg0_val, rax_res, rdx_res))

        state_table.store_reg('%rax', rax_res)
        state_table.store_reg('%rdx', rdx_res)

    def print_kind(self):
        print("Idiv Instruction")


class LeaInstruction(Instruction):
    """
    Class representing the 'LEA' instruction, which loads the effective address of the source operand into the destination register.
    """

    def __init__(self, kind, arg0, arg1):
        super(LeaInstruction, self).__init__(kind)
        self._arg0 = arg0
        self._arg1 = arg1

    def execute(self, state_table):
        # Get value of first operand
        arg0_val = state_table.get_mem_address(self._arg0)
        
        # Get value of second operand, it must be mem/register, and store result in it
        if re.match(OperandEnum.Reg.value, self._arg1):
            state_table.store_reg(self._arg1, arg0_val)
        else:
            raise AssertionError("Only register is allowed here")
        
    def print_kind(self):
        print("Lea Instruction")


class CallInstruction(Instruction):
    """
    Class representing the 'CALL' instruction, which transfers control to a subroutine.
    """

    def __init__(self, kind, label):
        super(CallInstruction, self).__init__(kind)
        self._label = label

    def execute(self, state_table):
        state_table.call(self._label)
        
    def print_kind(self):
        print("Call Instruction")


class CmpInstruction(Instruction):
    """
    Class representing the 'CMP' instruction, which compares two operands and sets the status flags in the state table.
    """

    def __init__(self, kind, arg0, arg1):
        super(CmpInstruction, self).__init__(kind)
        self._arg0 = arg0
        self._arg1 = arg1

    def execute(self, state_table):
        # Get value of operands
        arg0_val = state_table.get_operand(self._arg0)
        arg1_val = state_table.get_operand(self._arg1)
        state_table.set_comparison(arg0_val, arg1_val)

    def print_kind(self):
        print("Cmp Instruction")


class JmpInstruction(Instruction):
    """
    Class representing the 'JMP' instruction, which performs a conditional or unconditional jump to a specified label.
    """

    def __init__(self, kind, condition, label):
        super(JmpInstruction, self).__init__(kind)
        self._label = label
        self._condition = condition

    def execute(self, state_table):
        # Validate the condition
        if state_table.validate_condition(self._condition):
            state_table.jump_label(self._label)
        
    def print_kind(self):
        print("Jmp Instruction")


class RetInstruction(Instruction):
    """
    Class representing the 'RET' instruction, which returns control from a subroutine to the calling procedure.
    """

    def __init__(self, kind):
        super(RetInstruction, self).__init__(kind)

    def execute(self, state_table):
        state_table.ret()
        
    def print_kind(self):
        print("Ret Instruction")
        