# justfile

os_dir := "os"

# Default recipe
default: build

build:
    cd {{os_dir}} && make

all:
    cd {{os_dir}} && make all

clean:
    cd {{os_dir}} && make clean

clean-all:
    cd {{os_dir}} && make clean-all

run:
    cd {{os_dir}} && ./run

