// Below are for automatic generated headers. 
#ifndef __RASPMONITOR_LOADABLE_H
#define __RASPNOMITOR_LOADABLE_H


/**************************************************
 * The order of the following header files must
 * be kept. 
 * ************************************************/

#include "command_enum.h"
#include "capability_enum.h"

// type of transition
typedef struct transition {
    capability provide;
    capability require;
    command cmd;
    //struct transition * next;
} transition;


typedef struct weighted_transition {
    capability provide;
    capability require;
    command cmd;
    uint32_t weight;  
} weighted_transition;

#include "cmd2capability_table.h"   // this must be after *_enum.h
#include "transition.h"             // this must be after struct transition, etc.  
#include "cmd2enum_table.h"

// Any transition below this will be considered as non-suspicious operation,
// thus will not be counted when compute the risk score (max_score)
#define SUSPICIOUS_THRESHOLD 20000000
#define COMMAND_WINDOW_SIZE 10
// command command_window[COMMAND_WINDOW_SIZE];

extern command str_to_command(char *cmd);

/******************************************************************************
 * 
 * Definition for command_windows
 * 
 * ***************************************************************************/
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

// create and return a command_window. 
// Note: it's the caller's responsibility to check existence. 
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

#endif // end of __RASPNOMITOR_LOADABLE_H



