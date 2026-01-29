//#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include "./lib/tcc-0.9.26/tcc.h"
#include "lib/tcc-0.9.26/libtcc.h"

typedef struct {
    void *call;
    TCCState *tccState;
} RawFn;

TCCState *initTCC() {
    TCCState *tccState;
    // 1. TCC Instanz erstellen
    tccState = tcc_new();
    if (!tccState) {
        fprintf(stderr, "TCC konnte nicht initialisiert werden\n");
        exit(1); 
    }

    // WICHTIG: TCC sagen, dass wir Code direkt im Speicher ausführen wollen (JIT)
    tcc_set_output_type(tccState, TCC_OUTPUT_MEMORY);

    return tccState;
}

void *tccCompile(TCCState *tccState, char *name, char *fn){

    // 3. Code kompilieren
    if (tcc_compile_string(tccState, fn) == -1) {
        fprintf(stderr, "Fehler beim Kompilieren des C-Strings\n");
        exit(1); 
    }

    // 4. Symbole auflösen und Speicher für die Ausführung vorbereiten
    if (tcc_relocate(tccState, TCC_RELOCATE_AUTO) < 0) {
        fprintf(stderr, "Fehler beim Relocating des Codes\n");
        exit(1); 
    }

    // 5. Den Funktionspointer aus dem Symbol "add" extrahieren
    void *ptr = tcc_get_symbol(tccState, name);
    if (!ptr) {
        fprintf(stderr, "Symbol 'add' konnte nicht gefunden werden\n");
        exit(1); 
    }
    return ptr;
}

void deinitTCC(TCCState *tccState){
    tcc_delete(tccState); 
}

RawFn lambda(char *name, char *impl) {
    TCCState *tccState = initTCC();
    void *fnPtr = tccCompile(tccState, name, impl);
    RawFn rfn = {fnPtr, tccState};
    return rfn; 
}

#define lam(name,expr) lambda(#name,"")

int main() {
    int (*add_func)(int, int); // Der Funktionspointer für unser Ergebnis
    
    TCCState *tccState = initTCC();

    // 2. Der C-Code als String
    char *code = 
        "int add(int a, int b) {\n"
        "    return a * b;\n"
        "}";

    add_func = (int (*)(int,int)) tccCompile(tccState, "add", code);

    int result = add_func(22, 20);
    printf("Ergebnis der JIT-kompilierten Funktion: %d\n", result);

    printf("Test: %d\n", ((int (*)(int))tccCompile(tccState, "test", "int test(int i){return i*2;}"))(21)); 

    deinitTCC(tccState); 

    return 0;
}

