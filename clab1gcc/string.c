#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 100

int main() {
  char first[MAX];
  char second[MAX];
  char name[MAX];
  char str[MAX];
  int  year;
  
  printf("Enter your birthyear:");
  scanf("%d", &year);
  printf("Enter your first name: ");
  scanf("%s", first);
  printf("Enter your second name: ");
  scanf("%s", second);
  
  for (int i = 0; i < strlen(second); i++) {
        str[i] = toupper(second[i]);
    }
    str[strlen(second)] = '\0'; // Add null terminator to 'str'
  
  printf("Birthyear: %d \n", year);
  printf("First Name: %s \n", first);
  printf("Second Name: %s \n", second);
  printf("Second Name in Uppercase: %s\n", str);

  snprintf(name, sizeof(name), "%s %s %d", first, second, year); // Use  snprintf for safety
  //read year value directly 
  printf("Full name and year with snprintf: %s \n", name);

  strcat(first, second);
  printf("Full name with strcat: %s \n", first);
  
  printf("Comparision: %d\n",strcmp(second,str));
    
  sscanf(name,"%s",first,second,&year);
  //need the memoery address of where year is to find the value of year -->&year
    printf("Full name and year from sscanf: %s %s, Birth year: %d\n", first, second, year);

  return 0;
  
}
