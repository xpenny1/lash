#include <stdio.h>
#include <stdlib.h>
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


char *whitespace(char *str) {
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
	while ((tmp=whitespace(str))) str=tmp;
	while (*str == '*') {type->pinterLevel++;str++;}
	type->arrayLevel = 0;
	return str;
}

char *funcP(char *str, Arg (*args)[100]){
	char *tmp;
	if (*str++ != 'f') return NULL; 
	if (*str++ != 'u') return NULL; 
	if (*str++ != 'n') return NULL; 
	if (*str++ != 'c') return NULL; 
	while ((tmp = whitespace(str))) str=tmp;
	(*(args[0])).name.start = str;
	while ((tmp = letterP(str))) str=tmp;
	(*args[0]).name.end = str;
	while ((tmp = whitespace(str))) {str=tmp;}

	if (*str++ != '(') return NULL;

	while ((tmp = whitespace(str))) {str=tmp;}
	if (!(str = typeP(str, &(args[0]->type)))) return NULL;
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

int main(int argc, char **args) {
    	Array *arr = newArray();
    	int d;
    	d = 3;
    	Push(int, &d, arr); 
	d = 7;
    	Push(int, &d, arr); 
	return 0;
    	for (int i=0; i<Length(int, arr); i++) {
    		printf("%d: %d\n", i, At(int, i, arr));  
    	}
	char buffer[1024] = {0};
	char *input = buffer;
	fread(input, sizeof(char), 1024, stdin); 
	printf("%s\n", input); 

	Arg (*funcArgs)[100] = {0};
	StringView name = {0};
	Type type = {0};

	if ((input = funcP(input, funcArgs))) {
		printf("Func!\n");  
		printf("%s\n", input);
		Arg *arg;
		//while((arg = *funcArgs_++)) {
    		//	printArg(*arg); 
		//}
        } else {
		printf("No Func!\n");  
	}


}

