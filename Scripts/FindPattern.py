ProcName = raw_input("Process name: ")
MyMem = PyHadesMem.MemoryMgr(ProcName)
MyFindPat = PyHadesMem.FindPattern(MyMem)
Pattern = raw_input("Pattern: ")
Mask = raw_input("Mask: ")
Address = MyFindPat.Find(Pattern, Mask)
print("Address: " + hex(Address))