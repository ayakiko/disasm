main: disasm/disasm.obj disasm/arithm.obj disasm/logic.obj
	$(CXX) /EHsc /O2 main.cpp disasm.obj arithm.obj logic.obj

disasm/disasm.obj:
	$(CXX) /c $*.cpp

disasm/arithm.obj:
	$(CXX) /c $*.cpp

disasm/logic.obj:
	$(CXX) /c $*.cpp