

#ifndef __COMMAND_WINDOW_H
#define __COMMAND_WINDOW_H


// #include "rasploadable.h"



struct command_window_key {
    int pid;
};

struct command_window {
    command command_window[COMMAND_WINDOW_SIZE];
    struct command_window_key key;
    struct command_window* next;
};


// Return NULL if not found
struct command_window* find_command_window(struct command_window_key*);

// Create and return a command window. If already exist, it is the same as 
// find_command_window. 
struct command_window* new_command_window(struct command_window_key*);

// Remove and return the removed command window. If not found, return NULL
struct command_window* remove_command_window(struct command_window_key*);

// Initialize the "command_windows" global variable
void init_command_windows(void);

// Destroy the linked list and free all memory.
void destroy_command_windows(void);



// The head pointer to a linked list. 
// The linked list contains a command_window as its each element. 
extern struct command_window* command_windows;



#endif   // end of __COMMAND_WINDOW_H