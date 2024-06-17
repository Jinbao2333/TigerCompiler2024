from cmd import Cmd
from os import error
import sys
from debugger import Debugger

class Client(Cmd):
    """
    The Client class provides a command-line interface for interacting with the Tiger Interpreter.
    It extends the Cmd class and implements various commands to control the debugging process.
    
    Attributes:
        prompt (str): The command prompt string.
        intro (str): The introductory message displayed when the interpreter starts.
        _debugger (Debugger): The Debugger instance used for managing the debugging process.
    """
    prompt = '(tiger) '
    intro = 'Using Tiger Interpreter'

    def __init__(self, filename):
        """
        Initializes the Client with the specified filename.
        
        Args:
            filename (str): The name of the file to be debugged.
        """
        print(filename)
        # reload(sys)
        # sys.setdefaultencoding('utf-8')
        self._debugger = Debugger(filename)
        Cmd.__init__(self)

    def help_show(self):
        """
        Displays help information for the 'show' command.
        """
        print("help")

    def help_disable(self):
        """
        Displays help information for the 'disable' command.
        """
        print("disable <i>")
        print("<i> is the index of the breakpoint to disable")

    def help_breakpoints(self):
        """
        Displays help information for the 'breakpoints' command.
        """
        print("b <line>/func")
        print("<line> is the line number of your assembly you want to break, and <func> is the function name")

    def do_show(self, arg):
        """
        Executes the 'show' command to display various debugging information.
        
        Args:
            arg (str): The argument specifying what to show (registers, memory, breakpoints, etc.).
        """
        args = arg.split()
        if len(args) < 1:
            self.help_show()
        else:
            if args[0].startswith('r'):
                self._debugger.show_registers()
            elif args[0].startswith('t'):
                self._debugger.show_temper_registers()
            elif args[0].startswith('ma'):
                self._debugger.show_machine_registers()
            elif args[0].startswith('memtable'):
                self._debugger.show_memory_by_table()
            elif args[0].startswith('me'):
                self._debugger.show_memory_in_order()
            elif args[0].startswith('b'):
                self._debugger.show_breakpoints()
            else:
                self.help_show()

    def do_ni(self, arg):
        """
        Executes the 'ni' (next instruction) command to move to the next instruction.
        
        Args:
            arg (str): The number of instructions to execute. If not specified, executes one instruction.
        """
        print(arg)
        if len(arg) > 0 and arg.isdigit():
            for i in range(int(arg)):
                self._debugger.next_instruction()
        else:
            self._debugger.next_instruction()

    def do_continue(self, arg):
        """
        Executes the 'continue' command to continue execution until the next breakpoint or error.
        
        Args:
            arg (str): Not used.
        """
        try:
            self._debugger.con()
        except BaseException:
            print('Error occurred at:')
            self._debugger.disassemble()

    def do_b(self, arg):
        """
        Executes the 'b' (breakpoints) command to set breakpoints.
        
        Args:
            arg (str): The line number or function name where the breakpoint should be set.
        """
        args = arg.split()
        if len(args) < 1:
            self.help_breakpoints()
        else:
            if args[0].isdigit():
                self._debugger.breakpoints_line(int(args[0]))
            else:
                self._debugger.breakpoints_func(args[0])

    def do_disable(self, arg):
        """
        Executes the 'disable' command to disable breakpoints.
        
        Args:
            arg (str): The index of the breakpoint to disable. If not specified, disables all breakpoints.
        """
        if len(arg) < 1:
            self._debugger.disable_all()
        else:
            if arg[0].isdigit():
                self._debugger.disable(int(arg[0]))
            else:
                self.help_breakpoints()
            
    def do_disassemble(self, arg):
        """
        Executes the 'disassemble' command to disassemble the current code and display it.
        
        Args:
            arg (str): Not used.
        """
        self._debugger.disassemble()

    def do_exit(self, arg):
        """
        Executes the 'exit' command to exit the Tiger Interpreter.
        
        Args:
            arg (str): Not used.
        
        Returns:
            bool: True to exit the interpreter.
        """
        print('Exit Tiger Interpreter')
        return True
