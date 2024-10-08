#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "shadow_stack.h"

void *shadow_stack = NULL;
int shadow_stack_sp = 0;

#define SHADOW_STACK_SIZE 1024
#if defined(__x86_64__) || defined(_M_X64) // 64 bits
  #define BYTES_ARCH 8
#else // 32 bits
  #define BYTES_ARCH 4
#endif

__attribute__((no_instrument_function)) void activate_shadow_stack_protection(void *unaligned_address, int prot)
{
  /* This function does a mprotect call to a memory area. 
     Before reading the shadow stack, PROT_READ and PROT_WRITE protections are ON (activate_protection == 1).
     When finished, these previous protections are OFF (activate_protection == 0). 
     The unaligned address is automatically aligned to a page size.
  */
  int page_size = getpagesize();
  void *aligned_address = (void*)((size_t)unaligned_address & ((size_t)(-1) & ~(page_size - 1)));
  mprotect(aligned_address, SHADOW_STACK_SIZE * BYTES_ARCH, prot);
}

void __cyg_profile_func_enter(void *this_fn, void *call_site) {
  /* Shadow stack push */

  void *ret_address = __builtin_return_address(1);
  if(shadow_stack == NULL)
  { 
    shadow_stack = sbrk(SHADOW_STACK_SIZE * BYTES_ARCH);
    if(shadow_stack == NULL)
    {
      perror("Error allocating memory.");
      exit(EXIT_FAILURE);
    }
  }

  activate_shadow_stack_protection(shadow_stack,PROT_READ | PROT_WRITE);
  ((void **)shadow_stack)[shadow_stack_sp++] = ret_address; // copy return_address in shadow_stack
  activate_shadow_stack_protection(shadow_stack,PROT_NONE);  
}

void __cyg_profile_func_exit(void *this_fn, void *call_site) {
  /* Shadow stack pop + compare */
  void *ret_addr = __builtin_return_address(1);
  activate_shadow_stack_protection(shadow_stack,PROT_READ);
  if (shadow_stack_sp <= 0 || ((void **)shadow_stack)[--shadow_stack_sp] != ret_addr) {
      printf("Ayo ! Potential security breach detected... Leaving the program !\n");
      exit(1);
  }
  activate_shadow_stack_protection(shadow_stack,PROT_NONE);
}
