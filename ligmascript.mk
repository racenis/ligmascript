##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=ligmascript
ConfigurationName      :=Debug
WorkspaceConfiguration :=Debug
WorkspacePath          :=C:/Users/Poga/Desktop/painis/tram
ProjectPath            :=C:/Users/Poga/Desktop/painis/ligmascript
IntermediateDirectory  :=../tram/build-$(WorkspaceConfiguration)/__/ligmascript
OutDir                 :=$(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Poga
Date                   :=04/02/2023
CodeLitePath           :="C:/Program Files/CodeLite"
MakeDirCommand         :=mkdir
LinkerName             :=C:/mingw64/bin/g++.exe
SharedObjectLinkerName :=C:/mingw64/bin/g++.exe -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputDirectory        :=C:/Users/Poga/Desktop/painis/tram/build-$(WorkspaceConfiguration)/bin
OutputFile             :=..\tram\build-$(WorkspaceConfiguration)\bin\$(ProjectName).exe
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :=$(IntermediateDirectory)/ObjectsList.txt
PCHCompileFlags        :=
RcCmpOptions           := 
RcCompilerName         :=C:/mingw64/bin/windres.exe
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)./src 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overridden using an environment variable
##
AR       := C:/mingw64/bin/ar.exe rcu
CXX      := C:/mingw64/bin/g++.exe
CC       := C:/mingw64/bin/gcc.exe
CXXFLAGS :=  -g -O0 -Wall -std=c++98  -pedantic $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := C:/mingw64/bin/as.exe


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
WXWIN:=C:\Users\Poga\Desktop\miskaste\wxwidgets
WXCFG:=gcc_dll\mswu
Objects0=$(IntermediateDirectory)/src_shared_allocator.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_compiler_error.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_compiler_compiler.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_shared_bytecode.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_shared_stack.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_interpreter_word.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_interpreter_stacks.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_shared_instructions.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_interpreter_interpreter.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_interpreter_error.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_compiler_token.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_interpreter_coroutine.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_decompiler_decompiler.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_compiler_state.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: MakeIntermediateDirs $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@if not exist "$(IntermediateDirectory)" $(MakeDirCommand) "$(IntermediateDirectory)"
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@if not exist "$(IntermediateDirectory)" $(MakeDirCommand) "$(IntermediateDirectory)"
	@if not exist "$(OutputDirectory)" $(MakeDirCommand) "$(OutputDirectory)"

$(IntermediateDirectory)/.d:
	@if not exist "$(IntermediateDirectory)" $(MakeDirCommand) "$(IntermediateDirectory)"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/src_shared_allocator.cpp$(ObjectSuffix): src/shared/allocator.cpp $(IntermediateDirectory)/src_shared_allocator.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/ligmascript/src/shared/allocator.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_shared_allocator.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_shared_allocator.cpp$(DependSuffix): src/shared/allocator.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_shared_allocator.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_shared_allocator.cpp$(DependSuffix) -MM src/shared/allocator.cpp

$(IntermediateDirectory)/src_shared_allocator.cpp$(PreprocessSuffix): src/shared/allocator.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_shared_allocator.cpp$(PreprocessSuffix) src/shared/allocator.cpp

$(IntermediateDirectory)/src_compiler_error.cpp$(ObjectSuffix): src/compiler/error.cpp $(IntermediateDirectory)/src_compiler_error.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/ligmascript/src/compiler/error.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_compiler_error.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_compiler_error.cpp$(DependSuffix): src/compiler/error.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_compiler_error.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_compiler_error.cpp$(DependSuffix) -MM src/compiler/error.cpp

$(IntermediateDirectory)/src_compiler_error.cpp$(PreprocessSuffix): src/compiler/error.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_compiler_error.cpp$(PreprocessSuffix) src/compiler/error.cpp

$(IntermediateDirectory)/src_compiler_compiler.cpp$(ObjectSuffix): src/compiler/compiler.cpp $(IntermediateDirectory)/src_compiler_compiler.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/ligmascript/src/compiler/compiler.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_compiler_compiler.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_compiler_compiler.cpp$(DependSuffix): src/compiler/compiler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_compiler_compiler.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_compiler_compiler.cpp$(DependSuffix) -MM src/compiler/compiler.cpp

$(IntermediateDirectory)/src_compiler_compiler.cpp$(PreprocessSuffix): src/compiler/compiler.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_compiler_compiler.cpp$(PreprocessSuffix) src/compiler/compiler.cpp

$(IntermediateDirectory)/src_shared_bytecode.cpp$(ObjectSuffix): src/shared/bytecode.cpp $(IntermediateDirectory)/src_shared_bytecode.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/ligmascript/src/shared/bytecode.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_shared_bytecode.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_shared_bytecode.cpp$(DependSuffix): src/shared/bytecode.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_shared_bytecode.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_shared_bytecode.cpp$(DependSuffix) -MM src/shared/bytecode.cpp

$(IntermediateDirectory)/src_shared_bytecode.cpp$(PreprocessSuffix): src/shared/bytecode.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_shared_bytecode.cpp$(PreprocessSuffix) src/shared/bytecode.cpp

$(IntermediateDirectory)/src_shared_stack.cpp$(ObjectSuffix): src/shared/stack.cpp $(IntermediateDirectory)/src_shared_stack.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/ligmascript/src/shared/stack.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_shared_stack.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_shared_stack.cpp$(DependSuffix): src/shared/stack.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_shared_stack.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_shared_stack.cpp$(DependSuffix) -MM src/shared/stack.cpp

$(IntermediateDirectory)/src_shared_stack.cpp$(PreprocessSuffix): src/shared/stack.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_shared_stack.cpp$(PreprocessSuffix) src/shared/stack.cpp

$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix): src/main.cpp $(IntermediateDirectory)/src_main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/ligmascript/src/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_main.cpp$(DependSuffix): src/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_main.cpp$(DependSuffix) -MM src/main.cpp

$(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix): src/main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix) src/main.cpp

$(IntermediateDirectory)/src_interpreter_word.cpp$(ObjectSuffix): src/interpreter/word.cpp $(IntermediateDirectory)/src_interpreter_word.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/ligmascript/src/interpreter/word.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_interpreter_word.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_interpreter_word.cpp$(DependSuffix): src/interpreter/word.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_interpreter_word.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_interpreter_word.cpp$(DependSuffix) -MM src/interpreter/word.cpp

$(IntermediateDirectory)/src_interpreter_word.cpp$(PreprocessSuffix): src/interpreter/word.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_interpreter_word.cpp$(PreprocessSuffix) src/interpreter/word.cpp

$(IntermediateDirectory)/src_interpreter_stacks.cpp$(ObjectSuffix): src/interpreter/stacks.cpp $(IntermediateDirectory)/src_interpreter_stacks.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/ligmascript/src/interpreter/stacks.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_interpreter_stacks.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_interpreter_stacks.cpp$(DependSuffix): src/interpreter/stacks.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_interpreter_stacks.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_interpreter_stacks.cpp$(DependSuffix) -MM src/interpreter/stacks.cpp

$(IntermediateDirectory)/src_interpreter_stacks.cpp$(PreprocessSuffix): src/interpreter/stacks.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_interpreter_stacks.cpp$(PreprocessSuffix) src/interpreter/stacks.cpp

$(IntermediateDirectory)/src_shared_instructions.cpp$(ObjectSuffix): src/shared/instructions.cpp $(IntermediateDirectory)/src_shared_instructions.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/ligmascript/src/shared/instructions.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_shared_instructions.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_shared_instructions.cpp$(DependSuffix): src/shared/instructions.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_shared_instructions.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_shared_instructions.cpp$(DependSuffix) -MM src/shared/instructions.cpp

$(IntermediateDirectory)/src_shared_instructions.cpp$(PreprocessSuffix): src/shared/instructions.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_shared_instructions.cpp$(PreprocessSuffix) src/shared/instructions.cpp

$(IntermediateDirectory)/src_interpreter_interpreter.cpp$(ObjectSuffix): src/interpreter/interpreter.cpp $(IntermediateDirectory)/src_interpreter_interpreter.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/ligmascript/src/interpreter/interpreter.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_interpreter_interpreter.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_interpreter_interpreter.cpp$(DependSuffix): src/interpreter/interpreter.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_interpreter_interpreter.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_interpreter_interpreter.cpp$(DependSuffix) -MM src/interpreter/interpreter.cpp

$(IntermediateDirectory)/src_interpreter_interpreter.cpp$(PreprocessSuffix): src/interpreter/interpreter.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_interpreter_interpreter.cpp$(PreprocessSuffix) src/interpreter/interpreter.cpp

$(IntermediateDirectory)/src_interpreter_error.cpp$(ObjectSuffix): src/interpreter/error.cpp $(IntermediateDirectory)/src_interpreter_error.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/ligmascript/src/interpreter/error.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_interpreter_error.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_interpreter_error.cpp$(DependSuffix): src/interpreter/error.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_interpreter_error.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_interpreter_error.cpp$(DependSuffix) -MM src/interpreter/error.cpp

$(IntermediateDirectory)/src_interpreter_error.cpp$(PreprocessSuffix): src/interpreter/error.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_interpreter_error.cpp$(PreprocessSuffix) src/interpreter/error.cpp

$(IntermediateDirectory)/src_compiler_token.cpp$(ObjectSuffix): src/compiler/token.cpp $(IntermediateDirectory)/src_compiler_token.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/ligmascript/src/compiler/token.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_compiler_token.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_compiler_token.cpp$(DependSuffix): src/compiler/token.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_compiler_token.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_compiler_token.cpp$(DependSuffix) -MM src/compiler/token.cpp

$(IntermediateDirectory)/src_compiler_token.cpp$(PreprocessSuffix): src/compiler/token.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_compiler_token.cpp$(PreprocessSuffix) src/compiler/token.cpp

$(IntermediateDirectory)/src_interpreter_coroutine.cpp$(ObjectSuffix): src/interpreter/coroutine.cpp $(IntermediateDirectory)/src_interpreter_coroutine.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/ligmascript/src/interpreter/coroutine.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_interpreter_coroutine.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_interpreter_coroutine.cpp$(DependSuffix): src/interpreter/coroutine.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_interpreter_coroutine.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_interpreter_coroutine.cpp$(DependSuffix) -MM src/interpreter/coroutine.cpp

$(IntermediateDirectory)/src_interpreter_coroutine.cpp$(PreprocessSuffix): src/interpreter/coroutine.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_interpreter_coroutine.cpp$(PreprocessSuffix) src/interpreter/coroutine.cpp

$(IntermediateDirectory)/src_decompiler_decompiler.cpp$(ObjectSuffix): src/decompiler/decompiler.cpp $(IntermediateDirectory)/src_decompiler_decompiler.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/ligmascript/src/decompiler/decompiler.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_decompiler_decompiler.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_decompiler_decompiler.cpp$(DependSuffix): src/decompiler/decompiler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_decompiler_decompiler.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_decompiler_decompiler.cpp$(DependSuffix) -MM src/decompiler/decompiler.cpp

$(IntermediateDirectory)/src_decompiler_decompiler.cpp$(PreprocessSuffix): src/decompiler/decompiler.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_decompiler_decompiler.cpp$(PreprocessSuffix) src/decompiler/decompiler.cpp

$(IntermediateDirectory)/src_compiler_state.cpp$(ObjectSuffix): src/compiler/state.cpp $(IntermediateDirectory)/src_compiler_state.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/ligmascript/src/compiler/state.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_compiler_state.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_compiler_state.cpp$(DependSuffix): src/compiler/state.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_compiler_state.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_compiler_state.cpp$(DependSuffix) -MM src/compiler/state.cpp

$(IntermediateDirectory)/src_compiler_state.cpp$(PreprocessSuffix): src/compiler/state.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_compiler_state.cpp$(PreprocessSuffix) src/compiler/state.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r $(IntermediateDirectory)


