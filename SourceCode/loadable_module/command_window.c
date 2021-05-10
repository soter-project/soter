
#include <linux/types.h>
#include <linux/slab.h> // kfree, kmalloc


// This is the path of raspmonitor.h
#include "../fs/linux-4.19.91/raspmonitor.h"

#include "rasploadable.h"
#include "command_window.h"



// The head pointer to a linked list. 
// The linked list contains a command_window as its each element. 
struct command_window* command_windows = NULL;


// Initialize the "command_windows" global variable
void init_command_windows(void) {
    command_windows = NULL;

}

// Destroy the linked list and free all memory.
void destroy_command_windows(void) {
    struct command_window* current_window = command_windows;
    struct command_window* tmp = NULL;
    while (current_window) {
        tmp = current_window->next;
        kfree(current_window);
        current_window = tmp;
    }
}

// Return 1 if they are equal, 
// Return 0 if not equal. 
int is_command_window_key_equal(struct command_window_key* key1, struct command_window_key* key2) {
    if (NULL == key1 || NULL == key2) {
        return 0;
    }
    if (key1->pid == key2->pid) {
        return 1;
    } else {
        return 0;
    }
}

// Return NULL if not found
struct command_window* find_command_window(struct command_window_key *key) {
    struct command_window* current_window = command_windows;
    struct command_window* tmp = NULL;
    while (current_window) {
        tmp = current_window->next;
        if (is_command_window_key_equal(key, &(current_window->key))) {
            return current_window;
        } else {
            current_window = tmp;
        }
    }
    return NULL;
}


static void init_command_window(struct command_window* new_window) {
    int i = 0;
    for (i = 0; i < COMMAND_WINDOW_SIZE; i++) {
        new_window->command_window[i] = none_command;
    }
}

// create and return a command_window. 
// Note: it's the caller's responsibility to check existence. 
struct command_window* new_command_window(struct command_window_key* key) {
    struct command_window* tmp = kmalloc(sizeof(struct command_window*), GFP_KERNEL);
    if (tmp) {
        init_command_window(tmp);
        tmp->key = *key; 
        tmp->next = command_windows;
        command_windows = tmp;
        return tmp;
    } else {
        return NULL;
    }

}

// Remove and return the removed command window. If not found, return NULL
struct command_window* remove_command_window(struct command_window_key* key) {
    struct command_window* current_window = command_windows;
    struct command_window* next_window = NULL;
    if (NULL == current_window) {
        return NULL;
    }
    if (is_command_window_key_equal(key, &(current_window->key))) {
        command_windows = current_window->next;
        return current_window;
    }
    next_window = current_window->next;
    while (next_window) {
        if (is_command_window_key_equal(key, &(next_window->key))) {
            current_window->next = next_window->next;
            return next_window;
        }
        current_window = next_window;
        next_window = current_window->next;
    }
    return NULL;
}





