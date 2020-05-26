#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[])
{
	int aux = ftell(stdin);
	char msg[300];
	scanf("%[^\n]s", msg);
	int tam = strlen(msg);
	tam += 1;//incluindo \n

	fseek(stdin, aux, SEEK_SET);
	fwrite(msg, sizeof(char), 1, stdin);
	return 0;
}