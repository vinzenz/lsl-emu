TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS=-std=c++11 -I. -O0 -ggdb

SOURCES += main.cpp \
    lsl/lexer/lexer.cc \
    lsl/filebuf.cc \
    double-conversion/src/bignum-dtoa.cc \
    double-conversion/src/bignum.cc \
    double-conversion/src/cached-powers.cc \
    double-conversion/src/diy-fp.cc \
    double-conversion/src/double-conversion.cc \
    double-conversion/src/fast-dtoa.cc \
    double-conversion/src/fixed-dtoa.cc \
    double-conversion/src/strtod.cc \
    lsl/parser/parser.cc \
    lsl/ast/api.cc \
    lsl/ast/printer.cc \
    lsl/parser/make_string.cc \
    lsl/runtime/world/script/value.cc \
    lsl/runtime/world/script/eval.cc \
    lsl/runtime/world/script/state.cc \
    lsl/runtime/library/functions.cc \
    lsl/runtime/library/math.cc \
    lsl/runtime/library/string.cc

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    lsl/lexer/delim.hh \
    lsl/lexer/keyword.hh \
    lsl/lexer/lexer.hh \
    lsl/lexer/op.hh \
    lsl/lexer/tokendef.hh \
    lsl/lexer/tokens.hh \
    lsl/filebuf.hh \
    lsl/types.hh \
    lsl/ast/ast.hh \
    lsl/parser/apply.hh \
    lsl/parser/error.hh \
    lsl/parser/state.hh \
    lsl/parser/parser.hh \
    double-conversion/src/bignum-dtoa.h \
    double-conversion/src/bignum.h \
    double-conversion/src/cached-powers.h \
    double-conversion/src/diy-fp.h \
    double-conversion/src/double-conversion.h \
    double-conversion/src/fast-dtoa.h \
    double-conversion/src/fixed-dtoa.h \
    double-conversion/src/ieee.h \
    double-conversion/src/strtod.h \
    double-conversion/src/utils.h \
    lsl/ast/api.hh \
    double-conversion/test/cctest/cctest.h \
    double-conversion/test/cctest/checks.h \
    double-conversion/test/cctest/gay-fixed.h \
    double-conversion/test/cctest/gay-precision.h \
    double-conversion/test/cctest/gay-shortest-single.h \
    double-conversion/test/cctest/gay-shortest.h \
    lsl/ast/printer.hh \
    lsl/runtime/library/functions.hh \
    lsl/runtime/world/inventory.hh \
    lsl/runtime/world/object.hh \
    lsl/runtime/world/prim.hh \
    lsl/runtime/world/script.hh \
    lsl/runtime/world/simulator.hh \
    lsl/runtime/quaternion.hh \
    lsl/runtime/types.hh \
    lsl/runtime/vector.hh \
    lsl/runtime/world/script/call.hh \
    lsl/runtime/world/script/script.hh \
    lsl/runtime/world/script/state.hh \
    lsl/runtime/world/script/value.hh \
    lsl/runtime/world/script/value_convert.hh \
    lsl/runtime/world/script/constants.hh \
    lsl/runtime/world/script/type_info.hh \
    lsl/runtime/world/script/error.hh \
    lsl/runtime/world/script/context.hh \
    lsl/utils.hh \
    lsl/runtime/world/script/function.hh \
    lsl/runtime/world/script_fwd.hh \
    lsl/runtime/world/script/eval.hh

