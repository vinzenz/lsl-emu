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
    lsl/ast/api.cc

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
    lsl/ast/api.hh

