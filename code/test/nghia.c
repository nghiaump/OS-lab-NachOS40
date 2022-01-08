#include "syscall.h"

int main(){
    int i;
    for(i = 0; i < 5; ++i){
        Wait("nghia");
        PrintString("Nghia ");
        PrintNum(i);
        PrintChar('\n');
        Signal("lananh");
    }
    Exit(0);
}