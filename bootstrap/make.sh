#!/usr/bin/sh

gcc ./parse.c -o ./build/parse
./build/parse <<CC
func test (char *bla, int times, char *str) -> int {
    for (int i = 0; i < times; i++) {
        printf("i: %s", str);
    }
    return strlen(str);
}
CC

#gcc main.c \
#    -I./lib/tcc-0.9.26-build/include \
#    -L./lib/tcc-0.9.26-build/lib \
#    -ltcc -ldl -lm -lpthread \
#    -o ./build/main
