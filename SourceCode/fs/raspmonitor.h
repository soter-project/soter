#ifndef __LINUX_RASPMONITOR_H
#define __LINUX_RASPMONITOR_H



#define RASPMONITOR_DBG 0

// If you set this to "1" the raspmonitor will
// block any shell commands that trigger "malicious" flag.
#define RASPMONITOR_ENABLE_DEFENSE 0


// This file defines basic data structure of raspmonitor.
// first parameter is pData; setcond parameter is pReturn
typedef int (*RASPMONITOR_HOOK)(void*, void*);

// Going to store loadable hook, can te extened later. 
struct raspmonitor_hook_struct {
    // The hook function will be set by a loadable module. 
    // That is, a loadable module will set the call_hook pointer. 
    RASPMONITOR_HOOK call_hook;
    RASPMONITOR_HOOK call_hook_on_process_exit;
};


struct raspmonitor_status {
    // 0 is not set; non-zero is set. 
    char hook_is_set;
};

// ----- Any exported functions declared below ---------
// They are defined in exec.c 

// pFunc is a pointer pointing to a function in a loadable module
extern int raspmonitor_set_hook(RASPMONITOR_HOOK pFunc);
// pFunc is a pointer pointing to a function in a loadable module
extern int raspmonitor_set_hook_on_process_exit(RASPMONITOR_HOOK pFunc);
// This function will be called in the built-in kernel.
// It eventually calls the hooked function. 
extern int raspmonitor_invoke_hook(void* pData, void*pReturn);
// This function now don't need any data as input. 
// However, this signature allows this function to be extended in the future. 
// The place to call this function is inside
//      kernel/exit.c    :   do_exit()  function. 
// Basically, every time a process terminates, this function should be invoked. 
extern int raspmonitor_invoke_hook_on_process_exit(void* pData, void*pReturn);


// ------ Any global variables declared below ------------
// They are defined in exec.c 

// This record whether a loadable module has set the hook.
extern struct raspmonitor_status gRaspmonitorStatus;
// The gRaspmonitorHook is a global singleton that is defined 
extern struct raspmonitor_hook_struct gRaspmonitorHook;



#endif
