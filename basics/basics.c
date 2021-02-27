/*
 * =====================================================================================
 *
 *       Filename:  basics.c
 *
 *    Description:  Learning the basics of C
 *
 *        Version:  1.0
 *        Created:  02/22/21 19:57:46
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Gabriel Malaquias (),
 *   Organization:
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>

void print_hello_using_switch(int times) {
  int i = 0;
  puts("====> Switch");
  while(i < times) {
    switch(i % 2) {
      case 0 :
        printf("Hello world even %d\n", i);
        break;
      case 1 :
        printf("Hello world odd %d\n", i);
        break;
    }

    i++;
  }
}

void print_hello_using_do(){
  int i = 0;
  puts("====> Do");
  do {
    puts("Hello World");
    i++;
  } while(i < 5);
}

void print_hello_times(int times){
  puts("====> Times");
  for(int i = 0; i < times; i++){
    puts("Hello World");
  }
}

void print_hello_using_while(){
  int i = 0;
  puts("====> While");
  while(i < 5) {
    puts("Hello world");
    i++;
  }
}

void print_hello_using_for(){
  puts("====> For");
  for(int i = 0; i < 5; i++){
    puts("Hello world");
  }
}

int main(int argc, char** argv) {
  print_hello_using_for();
  print_hello_using_while();
  print_hello_times(2);
  print_hello_using_do();
  print_hello_using_switch(10);
  return 0;
}
