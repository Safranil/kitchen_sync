include(TestBigEndian)
include(CheckSymbolExists)
include(CheckCCompilerFlag)

# endianness test
test_big_endian(BIG_ENDIAN)
if(NOT BIG_ENDIAN)
	add_definitions(-DNATIVE_LITTLE_ENDIAN)
endif()

# function tests
check_symbol_exists(explicit_bzero  "stddef.h;stdint.h;stdlib.h;string.h" HAVE_EXPLICIT_BZERO)
if(HAVE_EXPLICIT_BZERO)
	add_definitions(-DHAVE_EXPLICIT_BZERO)
endif()

check_symbol_exists(explicit_memset "stddef.h;stdint.h;stdlib.h;string.h" HAVE_EXPLICIT_MEMSET)
if(HAVE_EXPLICIT_MEMSET)
	add_definitions(-DHAVE_EXPLICIT_MEMSET)
endif()

check_symbol_exists(memset_s        "stddef.h;stdint.h;stdlib.h;string.h" HAVE_MEMSET_S)
if(HAVE_MEMSET_S)
	add_definitions(-DHAVE_MEMSET_S)
endif()

# define the library we're actually trying to build
add_compile_options(-O3)
add_library(blake2-static STATIC src/blake2-dispatch.c)

# build and add the reference implementation
add_library(blake2-ref OBJECT src/blake2b-ref.c src/blake2s-ref.c)
target_compile_definitions(blake2-ref PRIVATE SUFFIX=_ref)
target_sources(blake2-static PRIVATE $<TARGET_OBJECTS:blake2-ref>)

# build and add the optimized implementations when supported by the compiler; blake2-dispatch will take care of choosing the best one at runtime
check_c_compiler_flag("-msse2" COMPILER_SUPPORTS_SSE2_FLAG)
if(COMPILER_SUPPORTS_SSE2_FLAG)
	add_library(blake2-sse2 OBJECT src/blake2b.c src/blake2s.c)
	target_compile_definitions(blake2-sse2 PRIVATE SUFFIX=_sse2)
	target_compile_options(blake2-sse2 PRIVATE -msse2)
	target_sources(blake2-static PRIVATE $<TARGET_OBJECTS:blake2-sse2>)
endif()

check_c_compiler_flag("-mssse3" COMPILER_SUPPORTS_SSSE3_FLAG)
if(COMPILER_SUPPORTS_SSSE3_FLAG)
	add_library(blake2-ssse3 OBJECT src/blake2b.c src/blake2s.c)
	target_compile_definitions(blake2-ssse3 PRIVATE SUFFIX=_ssse3)
	target_compile_options(blake2-ssse3 PRIVATE -msse2 -mssse3)
	target_sources(blake2-static PRIVATE $<TARGET_OBJECTS:blake2-ssse3>)
endif()

check_c_compiler_flag("-msse4.1" COMPILER_SUPPORTS_SSE41_FLAG)
if(COMPILER_SUPPORTS_SSE41_FLAG)
	add_library(blake2-sse41 OBJECT src/blake2b.c src/blake2s.c)
	target_compile_definitions(blake2-sse41 PRIVATE SUFFIX=_sse41)
	target_compile_options(blake2-sse41 PRIVATE -msse2 -mssse3 -msse4.1)
	target_sources(blake2-static PRIVATE $<TARGET_OBJECTS:blake2-sse41>)
endif()

check_c_compiler_flag("-mavx" COMPILER_SUPPORTS_AVX_FLAG)
if(COMPILER_SUPPORTS_AVX_FLAG)
	add_library(blake2-avx OBJECT src/blake2b.c src/blake2s.c)
	target_compile_definitions(blake2-avx PRIVATE SUFFIX=_avx)
	target_compile_options(blake2-avx PRIVATE -msse2 -mssse3 -msse4.1 -mavx)
	target_sources(blake2-static PRIVATE $<TARGET_OBJECTS:blake2-avx>)
endif()

check_c_compiler_flag("-mxop" COMPILER_SUPPORTS_XOP_FLAG)
if(COMPILER_SUPPORTS_XOP_FLAG)
	add_library(blake2-xop OBJECT src/blake2b.c src/blake2s.c)
	target_compile_definitions(blake2-xop PRIVATE SUFFIX=_xop)
	target_compile_options(blake2-xop PRIVATE -msse2 -mssse3 -msse4.1 -mavx -mxop)
	target_sources(blake2-static PRIVATE $<TARGET_OBJECTS:blake2-xop>)
endif()
