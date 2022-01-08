#include "syscall.h"

int main(){
    int nghiaID, LAID;
    PrintString("Nghia-La test starting...\n");
    CreateSemaphore("nghia", 0);
    CreateSemaphore("lananh", 0);
    LAID = Exec("../test/lananh");
    nghiaID = Exec("../test/nghia");
    PrintString("\r\n---------------------------------\n");
    
    
    Join(LAID);
    Join(nghiaID); 
    Signal("nghia");
    PrintString("this line appears after joining\n"); 
    
    Exit(0);
}