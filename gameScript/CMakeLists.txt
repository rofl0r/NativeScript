file(GLOB_RECURSE GS_HEADERS *.h *.hpp)
file(GLOB_RECURSE GS_SOURCES *.cpp *.c)


# Export interfaces
add_definitions(-DGS_BUILD_DLL)


# Workaround for the bug which sets in MSVC Linker->CommandLine->AdditionalOptions /machine:x64 regardless of architecture
set(CMAKE_SHARED_LINKER_FLAGS_INIT "")
 

# FLEX and BISON
find_package(FLEX)
find_package(BISON)

# Create parser directory in build path
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/parser) 

# Run flex and bison
BISON_TARGET(GSParser ./parser/bison.y ${CMAKE_CURRENT_BINARY_DIR}/parser/bison.cpp)
FLEX_TARGET(GSScanner ./parser/flex.l  ${CMAKE_CURRENT_BINARY_DIR}/parser/flex.cpp)
ADD_FLEX_BISON_DEPENDENCY(GSScanner GSParser)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/parser)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/parser)


# Create library
add_library(gameScript SHARED ${GS_HEADERS} ${GS_SOURCES} ${BISON_GSParser_OUTPUTS} ${FLEX_GSScanner_OUTPUTS})


# Link LLVM
find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
#llvm_map_components_to_libnames(llvm_libs support core irreader)
llvm_map_components_to_libnames(llvm_libs
	Core
	ExecutionEngine
	MCJIT
	MC
	Support
	X86CodeGen
	X86Desc
	X86Info
	X86AsmParser
	X86Disassembler
	RuntimeDyld
	MCDisassembler
	AsmPrinter
	SelectionDAG
	CodeGen
	Target
	Instrumentation
	ScalarOpts
	InstCombine
	ProfileData
	Object
	BitReader
	MCParser
	TransformUtils
	ipa
	Analysis
	X86AsmPrinter
	X86Utils
)

# Link against LLVM libraries
target_link_libraries(gameScript ${llvm_libs})
