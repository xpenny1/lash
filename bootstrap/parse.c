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

typedef char *(*Parser)(char *);
char *many(char *str, Parser parser) {
	char *tmp;
	while ((tmp=parser(str))) str++; 
	return str;
}
char *manyOne(char *str, Parser parser) {
    	if (!(str=parser(str))) return NULL;
    	return many(str, parser);
}
char *whitespaceCharP(char *str) {
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
	char *tmp;
	type->pinterLevel = 0;
	if      ((tmp=charP(str))) {type->primitiveType=Char;str=tmp;}
	else if ((tmp=intP(str)))  {type->primitiveType=Int; str=tmp;}
	else                       {return NULL;}
	str=many(str, whitespaceCharP);
	while (*str == '*') {type->pinterLevel++;str++;}
	type->arrayLevel = 0;
	return str;
}
char *argP(char *str, Arg *arg) {
    if (!(str=typeP(str, &arg->type))) return NULL;
    arg->name.start=str;
	    if (!(str=manyOne(str, letterP)))  return NULL;
    arg->name.end=str;
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
	str=many(str, whitespaceCharP); 
	if (*str++ != 'f') return NULL; 
	if (*str++ != 'u') return NULL; 
	if (*str++ != 'n') return NULL; 
	if (*str++ != 'c') return NULL; 
	if (!(str=manyOne(str, whitespaceCharP))) return NULL;
	name->start = str;
		while ((tmp = letterP(str))) str=tmp;
	name->end = str;
	if (!(str=manyOne(str, whitespaceCharP))) return NULL;
	if (*str++ != '(') return NULL;
	int argCounter = 	0;
	while (*str != ')') {
    		if (argCounter!=0 && *str++!=',') return NULL;
		str=many(str, whitespaceCharP); 
		if (!(str = argP(str, &args[argCounter]))) return NULL;
		str=many(str, whitespaceCharP); 
		argCounter+=1;
	}
	str++;
	str=many(str, whitespaceCharP); 
	if (*str++ != '-') return NULL; 
	if (*str++ != '>') return NULL;
	str=many(str, whitespaceCharP);
	if (!(str=typeP(str, type))) return NULL;
	str=many(str, whitespaceCharP);
	code->start=str+1;
	if (*str++ != '{') return NULL;
	int braceLevel = 1;
	while (braceLevel) {
    		switch (*str++) {
    			case '{': braceLevel++;
    				break;
    			case '}': braceLevel--;
    				break;
    			default:
        			break;
    		}
	}
	code->end=str-1;
	str=many(str, whitespaceCharP); 

	//if (!(str = wordP(str))) return NULL;
	//while ((tmp = whitespace(str))) {str=tmp;}

	return str;
}

void printSV(StringView sv) {
	fwrite(sv.start, sizeof(char), sv.end - sv.start, stdout); 
}

char svBuffer[1024] = {0};
char *sPrintView(StringView sv) {
	memcpy(svBuffer, sv.start, sv.end - sv.start);
	svBuffer[sv.end - sv.start] = 0;
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
	char buffer[1024] = {0};
	char *input = buffer;
	fread(input, sizeof(char), 1024, stdin); 
	printf("%s\n", input); 

	StringView name = {0};
	Arg funcArgs[100] = {0};
	Type type = {0};
	StringView code = {0};

	if ((input = funcP(input, &name, funcArgs, &type, &code))) {
		printf("Func!\n");  
		printf("%s\n", input);
		//Arg *arg;
		printf("###Name: %s\n", sPrintView(name)); 
		int argCounter = 0;
		Arg arg;
		while ((arg=funcArgs[argCounter]).name.start) {
			printf("###Arg %d\n", argCounter);
			printArg(funcArgs[argCounter]); 
			argCounter++;
		}
		printf("###Returns\n");
		printType(type); 
		printf("###Code\n");
		printSV(code);
        } else {
		printf("No Func!\n");  
	}


}

