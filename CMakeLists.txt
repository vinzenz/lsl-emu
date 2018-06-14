cmake_minimum_required(VERSION 3.0.0)
project(lsl-emu VERSION 0.1.0)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -ggdb")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

link_libraries(stdc++fs sqlite3)

include_directories(
    vsqlite--/include
    .
)

add_executable(lsl-emu
    main.cpp
    double-conversion/src/bignum-dtoa.cc
    double-conversion/src/bignum.cc
    double-conversion/src/cached-powers.cc
    double-conversion/src/diy-fp.cc
    double-conversion/src/double-conversion.cc
    double-conversion/src/fast-dtoa.cc
    double-conversion/src/fixed-dtoa.cc
    double-conversion/src/strtod.cc
    lsl/ast/api.cc
    lsl/ast/printer.cc
    lsl/compiler/analyzer.cc
    lsl/compiler/callgraph.cc
    lsl/filebuf.cc
    lsl/lexer/lexer.cc
    lsl/md5.cc
    lsl/parser/make_string.cc
    lsl/parser/parser.cc
    lsl/runtime/library/functions.cc
    lsl/runtime/library/list.cc
    lsl/runtime/library/math.cc
    lsl/runtime/library/string.cc
    lsl/runtime/world/script/eval.cc
    lsl/runtime/world/script/state.cc
    lsl/runtime/world/script/value.cc
    lsl/runtime/world/script/value_convert.cc
    vsqlite--/src/sqlite/backup.cpp
    vsqlite--/src/sqlite/command.cpp
    vsqlite--/src/sqlite/connection.cpp
    vsqlite--/src/sqlite/execute.cpp
    vsqlite--/src/sqlite/query.cpp
    vsqlite--/src/sqlite/result.cpp
    vsqlite--/src/sqlite/savepoint.cpp
    vsqlite--/src/sqlite/transaction.cpp
    vsqlite--/src/sqlite/view.cpp
)
