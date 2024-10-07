/* Headers */
#include <stdio.h>

int main()
{
    printf("size of int pointer = %ld byte \n", sizeof(int));
    printf("size of float = %ld byte \n", sizeof(float));
    printf("size of double = %ld byte \n", sizeof(int));
    printf("size of void = %ld byte \n", sizeof(void));

    printf("size of int pointer = %ld byte \n", sizeof(int*));
    printf("size of float pointer = %ld byte \n", sizeof(float*));
    printf("size of double pointer = %ld byte \n", sizeof(int*));
    printf("size of void pointer = %ld byte \n", sizeof(void*));

    return 0;

}
