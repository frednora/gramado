# justfile

projects_dir := "ead"
os_dir := "os"

# Default recipe
default: build

build:
    cd {{projects_dir}} && make
    cd {{os_dir}} && make

all:
    cd {{projects_dir}} && make all
    cd {{os_dir}} && make all

clean:
    cd {{projects_dir}} && make clean
    cd {{os_dir}} && make clean

clean-all:
    cd {{projects_dir}} && make clean-all
    cd {{os_dir}} && make clean-all

run:
    cd {{os_dir}} && ./run

