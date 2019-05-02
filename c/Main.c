#include <stdio.h>
void print(int i);
extern int asmfunc();
int main(int argc, char const *argv[]) {
  printf("Hello");
  print(3);
  printf("%d",asmfunc());
  return 0;
}
