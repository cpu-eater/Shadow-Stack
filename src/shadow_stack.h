#ifndef SHADOW_STACK
#define SHADOW_STACK

/* Activate or desactivate shadow stack protection (read / write / none) */
__attribute__((no_instrument_function)) void activate_shadow_stack_protection(void *unaligned_address, int prot);

/* Shadow stack push */
void __cyg_profile_func_enter(void *this_fn, void *call_site) __attribute__((no_instrument_function));

/* Shadow stack pop + compare */
void __cyg_profile_func_exit(void *this_fn, void *call_site) __attribute__((no_instrument_function));

#endif
