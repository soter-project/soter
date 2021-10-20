#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/fdtable.h>
#include <linux/net.h>
#include <net/sock.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("_");
MODULE_DESCRIPTION("A test of raspmonitor");

// This is the path of raspmonitor.h
#include "../linux-4.19.91/fs/raspmonitor.h"

// These are headers that define the data structure used by 
// this module.
#include "rasploadable.h"


#define RASPMONITOR_DBG 0
#define RASPMONITOR_VERBOSE 1


/******************************************************************************
 * 
 * Definition of data structure of command_windows - linked list
 * 
 * ***************************************************************************/

// The head pointer to a linked list. 
// The linked list contains a command_window as its each element. 
struct command_window* command_windows = NULL;
static int table_size = 0;

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


void init_single_command_window(struct command_window* current_window) {
    int i = 0;
    for (i = 0; i < COMMAND_WINDOW_SIZE; i++) {
        current_window->command_window[i] = none_command;
    }
}

// create and return a command_window. 
// Note: it's the caller's responsibility to check existence. 
struct command_window* new_command_window(struct command_window_key* key) {
    struct command_window* tmp = kmalloc(sizeof(*tmp), GFP_ATOMIC);
    if (tmp) {
        init_single_command_window(tmp);
        tmp->key.pid = key->pid; 
        tmp->next = command_windows;
        command_windows = tmp;
        table_size += 1;
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








/******************************************************************************
 * 
 * Definition of classifier 
 * 
 * ***************************************************************************/

// Return:  
//  0   if "descendant" is NOT a descendant of ancestor
//  1   if "descendant" is a descendant of ancestor
int is_descendant(struct task_struct* ancestor, struct task_struct* descendant)
{
    struct task_struct* p_tmp;
    // all process is a descendant of process 0.
    if (ancestor->pid == 0) return 1;
    for (p_tmp = descendant; p_tmp->pid; p_tmp = p_tmp->real_parent) {
        if (p_tmp == ancestor) return 1;
    }
    return 0;
}


/***
 *  Input   :       
 *      key :   the key that contains the pid, which is used for command window retrival 
 *
 *  Return	:	
 * 	    this function will return a pointer to the command_window   
 * ***/
struct command_window* get_current_command_window(struct command_window_key* key) {

    struct command_window* ret = NULL; 
    ret = find_command_window(key);
    if (NULL == ret) {
        ret = new_command_window(key);
#if RASPMONITOR_DBG
            printk(KERN_INFO "[raspmonitor_dbg] : creating new command window with pid: %d\n", key->pid);
#endif
#if RASPMONITOR_VERBOSE
        printk(KERN_INFO "[raspinfo] : new command window: for pid: %d. parallel_login_size=%d\n", key->pid, table_size);
#endif
    } 
    return ret;

}


/**
 *  This will trace back all parent pids. 
 * 
 *  Return   :   
 * 
 *      0   if current process is not spawned from remote;
 *      pid of dropbear/telnetd that will be used for command window key if current process is spawned from remote; 
 * 
 *      I've proved that the parent of init process (pid=0) is init process its own. 
 **/ 

int get_remote_pid(void) {
    struct task_struct* parent, *child, *child_child;
    int ret = 0;
    child_child = current;
    child = current->real_parent;
    // search while 'child' is not init process
    while (child->pid != 0) {
        parent = child->real_parent;
        // from dropbear
        if (strncmp("dropbear", child->comm, 8) == 0) {
            // if 'child' is the very inital 'dropbear' process, i.e., its parent is not dropbear
            if (strncmp("dropbear", parent->comm, 8) != 0) {
                ret = child_child->pid;
                break;
            }
        // from telnetd
        } else if (strncmp("telnetd", child->comm, 7) == 0) {
            // if 'child' is the very inital 'telnetd' process, i.e., its parent is not telnetd
            if (strncmp("telnetd", parent->comm, 7) != 0) {
                ret = child_child->pid;
                break;
            }
        } else {
            // continue
        }
        child_child = child;
        child = parent;
    } 
#if RASPMONITOR_DBG
    printk(KERN_INFO "[raspmonitor_dbg] Should not reach here, its not local command.\n");
#endif
    return ret;
}





// update the comamnd_window with a given command, cmdstr. 
// Return the length of current window. 
int update_command_window(char* cmdstr, struct command_window* current_window) {
    int i = 0;

    // If it's full
    if (none_command != current_window->command_window[COMMAND_WINDOW_SIZE-1]) {
        // shift the window.
        for (i = 1; i < COMMAND_WINDOW_SIZE; i++) {
            current_window->command_window[i-1] = current_window->command_window[i];
        }
        // Put the new command in the last spot.
        current_window->command_window[i-1] = str_to_command(cmdstr);
#if RASPMONITOR_DBG 
        if (command_window[i-1] < MAX_COMMAND) {
            printk(KERN_INFO "[raspmonitor_dbg] : updated window with %s command!\n", enum2cmd_table[command_window[i-1]]);
        } else {
            printk(KERN_INFO "[raspmonitor_dbg] : command out of range!\n");
        }
#endif
        return COMMAND_WINDOW_SIZE;
    }

    // else, add the new command in the first available spot.
    for (i = 0; i < COMMAND_WINDOW_SIZE; i++) {
        if (none_command == current_window->command_window[i]) {
            current_window->command_window[i] = str_to_command(cmdstr);
#if RASPMONITOR_DBG 
            if (command_window[i] < MAX_COMMAND) {
                printk(KERN_INFO "[raspmonitor_dbg] : updated window with %s command!\n", enum2cmd_table[command_window[i]]);
            } else {
                printk(KERN_INFO "[raspmonitor_dbg] : command out of range!\n");
            }
#endif
            return i+1;
        }
    }

    // should not reach here. 
#if RASPMONITOR_DBG 
    printk(KERN_INFO "[raspmonitor_dbg] : ERROR: update_command_window.\n");
#endif
    // init_single_command_window(current_window);
    return 0;
} 


// This function returns a pointer that points to
// the beginning of the real command string. 
// E.g., if "/bin/bash" is given, then "bash" is return. 
// 
// Input    :   
//  cmd : the string wants to be dealt with. 
//  length  :   the length of the whole string, '\0' not included.
//
// return   :   a pointer pointing to the beginning of the wanted string.
//              If a '/' is not found, then return the original pointer + 1. 
//
// The reason I return origin + 1 instead of origin is because of the case 
// like this :  "/bash".   This command locates under "/" directly. 
// So, the "bash" is returned instead of "/bash"
//
char* get_real_command_string(char* cmd, int length) {
    while (length > 0) {
        if (*(cmd + length) != '/') {
            length -= 1;
        } else {
            return (cmd+length+1);
        }
    }
    return cmd;
}



// Tools to translate char* into command type. 
command str_to_command(char *cmd) {
    int i = 0;
    int length = strlen(cmd);
    char * real_name;
    int real_length;
    // TODO: 
    // This is a shortcut to check whether it's a binary execution. 
    // I would suggest to check whether it's a binary execution in 
    // the built-in kernel rather than here. 
    if (strncmp("./", cmd, 2) == 0) {
        return BINARY_COMMAND_uniq_name_flag;
    }
    real_name = get_real_command_string(cmd, length);
    real_length = strlen(real_name);
    for (; i < MAX_COMMAND; i++) {
        if (strncmp(enum2cmd_table[i], real_name, real_length) == 0) {
            return (command)i;
        }
    }
#if RASPMONITOR_DBG
    printk(KERN_INFO "[raspmonitor_dbg] : real_name : %s\n", real_name);
#endif
    return UNKNOWN_uniq_name_flag;
}



// The major function that computes the max score given 
// a list of commands. 
// Input    :   array of command, and length of the array
// Output   :   the max score of the given command list. 
uint32_t compute_max_score_path(command cmds[], int length) {
    uint32_t max_score = 0;
    uint32_t real_length = 0;
    int i = 0;
    int k = 0;
    command cmd1;
    command cmd2;
    capability current_step;
    capability next_step;
    struct transition tran;
    if (length < 2) {
        return 0;
    }
    for (i=1; i<length; i++) {
        cmd1 = cmds[i-1];
        cmd2 = cmds[i];
        current_step = cmd2capability[(int)cmd1];
        next_step = cmd2capability[(int)cmd2];
        tran.provide = current_step;
        tran.require = next_step;
        tran.cmd = cmd2;
        // check whether tran is in the global transition list. 
        for (k = 0; k < MAX_TRANSITION; k++) {
            if (tran.provide == ALL_TRANSITIONS[k].provide && 
                tran.require == ALL_TRANSITIONS[k].require && 
                tran.cmd == ALL_TRANSITIONS[k].cmd) 
            {
                if (ALL_TRANSITIONS[k].weight > SUSPICIOUS_THRESHOLD) {
                    max_score += ALL_TRANSITIONS[k].weight;
                    real_length += 1;
                }
                break;
            }
        }
    }
    if (real_length == 0) {
        return 0;
    } else {
        real_length = real_length > 3 ? real_length : 3;
        return max_score / real_length;
        //max_score = do_div(max_score, real_length);
    }
}


/**
 *  Will be called when process exit. 
 *  The two parameter is not used.  
 * */
static int raspmonitor_on_process_exit(void* pData, void* pReturn) {
    int ret = 0;
    struct command_window* window_to_free = NULL;
    struct command_window_key key;
    key.pid = current->pid;

#if RASPMONITOR_DBG
    printk(KERN_INFO "[raspmonitor] : pid: %d is going to exit!\n", key.pid);
#endif
    window_to_free = remove_command_window(&key);
    if (NULL != window_to_free) {
        table_size -= 1;
#if RASPMONITOR_VERBOSE
        printk(KERN_INFO "[raspinfo] : remove command window: for pid: %d. parallel_login_size=%d\n", key.pid, table_size);
#endif
        kfree(window_to_free);
    }
    return ret;
}



static int raspmonitor_main(void* pData, void* pReturn) {
    int ret = 0;
    uint32_t max_score;
    int wsize;
    char* real_name;
    int cmd_len = strlen(pData);
    const char* special_command = NULL;
    int remote_pid = 0;
    struct command_window* current_window = NULL;
    struct command_window_key current_window_key;
#if RASPMONITOR_DBG
    printk(KERN_INFO "[raspmonitor_dbg] : %s is called!\n", (const char*)pData);
#endif 

    // This is designed to handle the "VALIDATE_SH_EXECUTABLE", "EndofAScript", "EndofAllScripts", and "BeginofAllScripts"
    // Those commands are special commands that are only used for test purpose. 
    if (strlen(pData) > 5) {
        // check VALIDATE_SH_EXECUTABLE
        special_command = pData;
        if (strncmp("VALIDATE_SH_EXECUTABLE", special_command, 22) == 0) {
            printk(KERN_INFO "[raspmonitor] : score: %u, %s benign!\n", 0, (const char*)pData);
            // init_single_command_window();
            return ret;
        }
        // This is whitelist, a benign routine for openwrt
        if (strncmp("/sbin/askfirst", special_command, 14) == 0) {
            // printk(KERN_INFO "[raspmonitor] : score: %u, %s benign!\n", 0, (const char*)pData);
            return ret;
        }
        // Handle busybox - will ignore all busybox cause we care about its arguments.
        real_name = get_real_command_string(pData, cmd_len);
        if (strncmp("busybox", real_name, 7) == 0) {
            // printk(KERN_INFO "[raspmonitor] : going to ignore buxybox. \n");
            return ret;
        }
        special_command = pData + 5;
        if (strncmp("EndofAScript", special_command, 12) == 0) {
            printk(KERN_INFO "[raspmonitor] : score: %u, %s benign!\n", 0, (const char*)pData);
            return ret;
        }
        if (strncmp("EndofAllScripts", special_command, 15) == 0) {
            printk(KERN_INFO "[raspmonitor] : score: %u, %s benign!\n", 0, (const char*)pData);
            return ret;
        }
        if (strncmp("BeginofAllScripts", special_command, 17) == 0) {
            printk(KERN_INFO "[raspmonitor] : score: %u, %s benign!\n", 0, (const char*)pData);
            return ret;
        }
    }

    remote_pid = get_remote_pid(); 
    // check if current process is spwaned from remote exexution (dropbear/telnetd)
    if (0 == remote_pid) {
#if RASPMONITOR_VERBOSE
        printk(KERN_INFO "[raspinfo] : local execution: %s\n", (const char*)pData);
#endif
        return ret; 
    } else {
        current_window_key.pid = remote_pid;
	    current_window = get_current_command_window(&current_window_key);
        // potentially out of memory
        if (NULL == current_window) {
            printk(KERN_INFO "[raspmonitor_dbg] : can't handle new command window for %s, out of memory.\n", (const char*)pData);
            return ret;
        }

    }

    wsize = update_command_window(pData, current_window);
    max_score = compute_max_score_path(current_window->command_window, wsize);
#if RASPMONITOR_VERBOSE
    printk(KERN_INFO "[raspinfo] : %s from remote pid: %d. parallel_infection_size=%d\n", (const char*)pData, current_window->key.pid, table_size);
#endif
    if (max_score > 63000000) {
    //if (max_score > 67000000) {
#if RASPMONITOR_VERBOSE
        printk(KERN_INFO "[raspmonitor] : score: %u, %s malicious!\n", max_score, (const char*)pData);
#endif
        init_single_command_window(current_window);
        ret = 1;
    } else {
#if RASPMONITOR_VERBOSE
        printk(KERN_INFO "[raspmonitor] : score: %u, %s benign!\n", max_score, (const char*)pData);
#endif
    }
    return ret; 
}




static int __init raspmonitor_init(void) {
    int ret = 0;
    gRaspmonitorStatus.hook_is_set = 1;
    table_size = 0;
    raspmonitor_set_hook(raspmonitor_main);
    raspmonitor_set_hook_on_process_exit(raspmonitor_on_process_exit);
    // init_single_command_window();
    init_command_windows();
#if RASPMONITOR_DBG
    printk(KERN_INFO "[raspmonitor_dbg] : initalizing command windows.\n");
#endif
    printk(KERN_INFO "[raspmonitor] : initializing.\n");
    return ret;
}



static void __exit raspmonitor_exit(void) {
    gRaspmonitorStatus.hook_is_set = 0;
    table_size = 0;
    destroy_command_windows();
#if RASPMONITOR_DBG
    printk(KERN_INFO "[raspmonitor_dbg] : cleaning up command windows.\n");
#endif
    printk(KERN_INFO "[raspmonitor] : going to exit.\n");
}


module_init(raspmonitor_init);
module_exit(raspmonitor_exit);


