#include <stdio.h>

/*
    Without shadow stack protection : gcc main.c -o prog -g -fno-stack-protector -Wno-all
    With shadow stack protection : gcc main.c shadow_stack.c -o prog -finstrument-functions -g -fno-stack-protector -Wno-all
*/

void vulnerable_function(void)
{
    char buffer[5];
    gets(buffer); // gets function is vulnerable. Never use it.
}

int main(void)
{
    vulnerable_function();
    return 1;
}
