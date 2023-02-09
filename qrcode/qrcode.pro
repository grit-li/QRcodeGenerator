TEMPLATE = lib
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += MAJOR_VERSION=4
DEFINES += MINOR_VERSION=0
DEFINES += MICRO_VERSION=0

ROOT_PATH = $$PWD/..
include($$ROOT_PATH/config.pri)

HEADERS += \
    mqrspec.h \
    qrencode.h \
    qrencode_inner.h \
    qrinput.h \
    qrspec.h \
    rsecc.h \
    split.h \
    bitstream.h \
    mask.h \
    mmask.h

SOURCES += \
    mqrspec.c \
    qrencode.c \
    qrinput.c \
    qrspec.c \
    rsecc.c \
    split.c \
    bitstream.c \
    mask.c \
    mmask.c

