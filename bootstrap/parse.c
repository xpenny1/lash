#include <stdio.h>
#include <string.h>
#include <sys/types.h>

typedef struct {
    char *start;
    char *end;
} StringView;
typedef enum {
    Int,
    Char
} PrimitiveType;
typedef struct {
    int pinterLevel;
    int arrayLevel;
    PrimitiveType primitiveType;
} Type;
typedef struct {
    StringView name;
    Type type;
} Arg;
typedef struct {
    StringView name;
    Arg *args;
    int argc;
    Type ret;
    StringView code;
} Func;

char *manyP(char *str, char *parser(char *)) {
    char *tmp;
    while ((tmp=parser(str))) str=tmp;
    return str;
}

char *manyOneP(char *str, char *parser(char *)) {
    if (!parser(str)) return NULL;
    return manyP(str, parser);
}

char *whitespaceP(char *str) {
	switch (*str) {
		case ' ':  return ++str; 
		case '\t': return ++str; 
		case '\n': return ++str; 
		default: return NULL;
	}
}

char *letterP(char *str){
    if ('a' <= *str && *str <= 'z') {
    	return ++str; 
    } else if ('A' <= *str && *str <= 'Z') {
    	return ++str; 
    } else {
    	return NULL;
    }
}

char *wordP(char *str) {
	char *tmp;
	if (!letterP(str++)) return NULL;
	while ((tmp = letterP(str))) {str=tmp;}
	return str;
}

char *intP(char *str) {
	if (*str++ != 'i') return NULL; 
	if (*str++ != 'n') return NULL; 
	if (*str++ != 't') return NULL; 
	return str;
}
char *charP(char *str) {
	if (*str++ != 'c') return NULL; 
	if (*str++ != 'h') return NULL; 
	if (*str++ != 'a') return NULL; 
	if (*str++ != 'r') return NULL; 
	return str;
}

char *typeP(char *str, Type *type) {
	char *tmp = NULL;
	type->pinterLevel = 0;
	if      ((tmp=charP(str))) {type->primitiveType=Char;str=tmp;}
	else if ((tmp=intP(str)))  {type->primitiveType=Int; str=tmp;}
	else                       {return NULL;}
	while ((tmp=whitespaceP(str))) str=tmp;
	while (*str == '*') {type->pinterLevel++;str++;}
	type->arrayLevel = 0;
	return str;
}

char *hashBraceOpen(char *str, int *hashesPtr) {
	int hashes = 0;
	while (*str++ == '#') hashes++; 
	if (*(str-1) != '{') return NULL;
	*hashesPtr = hashes;
	return str;
}
char *hashBraceClose(char *str, int hashes) {
    if (*str++ != '}') return NULL;
    for (int i=0; i<hashes; i++) if (*str++ != '#') return NULL; 
    return str;
}

char *funcP(char *str, Func *func){
	char *tmp;
	if (*str++ != 'f') return NULL; 
	if (*str++ != 'u') return NULL; 
	if (*str++ != 'n') return NULL; 
	if (*str++ != 'c') return NULL; 
	if (!(str=manyP(str,whitespaceP))) return NULL;
	func->name.start = str;
	if (!(str=manyOneP(str, letterP))) return NULL;
	func->name.end = str;
	if (!(str=manyP(str,whitespaceP))) return NULL;

	if (*str++ != '(') return NULL;
	if (!(str=manyP(str,whitespaceP))) return NULL;
	int argIndex = 0;
	while (*str != ')') {
		while ((tmp = whitespaceP(str))) {str=tmp;}
		if (!(str = typeP(str, &(func->args[argIndex].type)))) return NULL;
		func->args[argIndex].name.start=str;
		if (!(str = wordP(str))) return NULL;
		func->args[argIndex].name.end=str;
		if (!(str=manyP(str,whitespaceP))) return NULL;
		argIndex++;
		switch (*str) {
			case ',': str++; break;
			case ')': break;
			default: return NULL;
		}
	}
	func->argc = argIndex;
	str++;
	str=manyP(str, whitespaceP);
	if (*str++ != '-') return NULL;
	if (*str++ != '>') return NULL;
	str=manyP(str, whitespaceP);
	if(!(str=typeP(str, &func->ret))) return NULL; 
	int hashes;
	if (!(str=hashBraceOpen(str, &hashes))) return NULL;
	func->code.start = str;
	while(!(tmp=hashBraceClose(str++, hashes))) if (!*str) return NULL;
	func->code.end = str-1;
	str = tmp;
	return str;
}

void printSV(StringView sv) {
	fwrite(sv.start, sizeof(char), sv.end - sv.start, stdout); 
}

char svBuffer[1024] = {0};
char *sPrintView(StringView sv) {
	memcpy(svBuffer, sv.start, sv.end - sv.start);
	svBuffer[1 + sv.end - sv.start] = 0;
	return svBuffer;
}

void printType(Type type) {
	switch (type.primitiveType) {
               	case Int: printf("Type: Int\n");
               		break;
               	case Char: printf("Type: Char\n");
			break;
               }
	printf("Pointer: %d\n", type.pinterLevel); 
	printf("Array %d\n", type.arrayLevel);
}
void printArg(Arg arg) {
    printf("Name: %s\n", sPrintView(arg.name));
    printType(arg.type);
}

char buffer[1024] = {0};
int main(int argc, char **args) {
	char *input = buffer;
	fread(input, sizeof(char), 1024, stdin); 
	printf("%s\n", input); 

	Arg funcArgs[100] = {0};
	Func func = {0};
	func.args = funcArgs;

	if ((input = funcP(input, &func))) {
		printf("Func!\n");  
		printf("%s\n", input);

		printf("Name: %s\n", sPrintView(func.name));
		printf("Argc: %d\n", func.argc);
		printf("Args\n");
		for (int i=0; i<func.argc; i++) {
			printArg(func.args[i]); 
		}
		printf("Ret\n");
		printType(func.ret); 
		printf("%s\n",sPrintView(func.code));
        } else {
		printf("No Func!\n");  
	}


}

