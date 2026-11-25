# justfile

DE_DIR:= "gramado"
CMDS_DIR:= "cmds"
BASE_DIR := "base"

# Default recipe
default: build

build:
    cd {{DE_DIR}} && make
    cd {{CMDS_DIR}} && make
    cd {{BASE_DIR}} && make

all:
    cd {{DE_DIR}}    && make all
    cd {{CMDS_DIR}}  && make all
    cd {{BASE_DIR}}  && make all

all-base:
    cd {{BASE_DIR}}  && make all-base

de:
    cd {{DE_DIR}} && make all

cmds:
    cd {{CMDS_DIR}} && make all

base:
    cd {{BASE_DIR}} && make all

clean:
    cd {{DE_DIR}}    && make clean
    cd {{CMDS_DIR}}  && make clean
    cd {{BASE_DIR}}  && make clean

clean-all:
    cd {{DE_DIR}}    && make clean-all
    cd {{CMDS_DIR}}  && make clean-all
    cd {{BASE_DIR}}  && make clean-all

run:
    cd {{BASE_DIR}} && ./run

