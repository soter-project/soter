#include <stdio.h>
#include <string.h>

// Just hack the uint64_t 
#define uint64_t long long int

#include "rasploadable.h"


// This function returns a pointer that points to
// the beginning of the real command string. 
// E.g., if "/bin/bash" is given, then "bash" is return. 
// 
// Input    :   
//  cmd : the string wants to be dealt with. 
//  length  :   the length of the whole string, '\0' not included.
//
// return   :   a pointer pointing to the beginning of the wanted string.
//              If a '/' is not found, then return the original pointer. 
char* get_real_command_string(char* cmd, int length) {
    while (length > 0) {
        if (*(cmd + length) != '/') {
            length -= 1;
        } else {
            return (cmd+length);
        }
    }
    return cmd;
}


void init_command_window(void) {
    int i = 0;
    for (i = 0; i < COMMAND_WINDOW_SIZE; i++) {
        command_window[i] = none_command;
    }
}

// update the comamnd_window with a given command, cmdstr. 
// Return the length of current window. 
int update_command_window(char* cmdstr) {
    int i = 0;
    for (i = 1; i < COMMAND_WINDOW_SIZE; i++) {
        if (none_command != command_window[i]) {
            command_window[i-1] = command_window[i];
        } else {
            break;
        }
    }
    if (COMMAND_WINDOW_SIZE == i) {
        command_window[i-1] = str_to_command(cmdstr);
    } else {
        command_window[i] = str_to_command(cmdstr);
    }
    return i;
} 


// Tools to translate char* into command type. 
command str_to_command(char *cmd) {
    int i = 0;
    // TODO: 
    // This is a shortcut to check whether it's a binary execution. 
    // I would suggest to check whether it's a binary execution in 
    // the built-in kernel rather than here. 
    if (strncmp("./", cmd, 2) == 0) {
        return BINARY_COMMAND_uniq_name_flag;
    }
    int length = strlen(cmd);
    for (; i < MAX_COMMAND; i++) {
        if (strncmp(enum2cmd_table[i], cmd, length) == 0) {
            return (command)i;
        }
    }
    return UNKNOWN_uniq_name_flag;
}


// The major function that computes the max score given 
// a list of commands. 
// Input    :   array of command, and length of the array
// Output   :   the max score of the given command list. 
long long compute_max_score_path(command cmds[], int length) {
    long long max_score = 0;
    int real_length = 0;
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
        return max_score / real_length;
    }
}



// The major function that computes the max score given 
// a list of commands. 
// Input    :   array of command, and length of the array
// Output   :   the max score of the given command list. 
long long compute_max_score_capability(command cmds[], int length) {
    long long score = 0;
    long long max_score = 0;
    int real_length = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    command cmd1;
    command cmd2;
    struct transition tran;
    // provides is an array.
    // the subscript if the array is the capability. 
    // E.g., if provides[0] != 0, then FP is in the set 
    // If provides[0] == 0, then FP is not in the set.   
    capability provides[MAX_CAPABILITY] = {0};
    if (length < 2) {
        return 0;
    }
    // Go through all the commands in the list. 
    for (i=1; i < length; i++) {
        cmd1 = cmds[i-1];
        cmd2 = cmds[i];
        // add cmd1 into the provides. 
        provides[(int)cmd2capability[(int)cmd1]] = 1;
        max_score = 0;
        // For each cmd2, go thourgh all the possible transitions between 
        // cmd2 and existing commands (i.e., already in the provides.)
        for (j = 0; j < MAX_CAPABILITY; j++) {
            // if command 'j' is in the provides.
            if (provides[j] != 0) {
                // make a transition between cmd2 and 'j' for comparison purpose 
                tran.provide = (capability)j;
                tran.require = cmd2capability[(int)cmd2];
                tran.cmd = cmd2;
                // check whether tran is in the global transition list. 
                for (k = 0; k < MAX_TRANSITION; k++) {
                    if (tran.provide == ALL_TRANSITIONS[k].provide && 
                        tran.require == ALL_TRANSITIONS[k].require && 
                        tran.cmd == ALL_TRANSITIONS[k].cmd) 
                    {
                        // if in the list, then check whether the weight is larger 
                        // than max_score, then update accordingly. 
                        // printf("transition found!\n");
                        if (!(ALL_TRANSITIONS[k].weight < max_score)) {
                            // printf("score updated! %f\n", ALL_TRANSITIONS[k].weight);
                            max_score = ALL_TRANSITIONS[k].weight;
                        }            
                        break;
                    }
                }
            } 
        }
        if (max_score > SUSPICIOUS_THRESHOLD) {
            real_length += 1;
            score += max_score;
        }
    }
    if (0 == real_length) {
        return 0;
    } else {
        return score/real_length;
    }
}


/*********************************************************************
 *  Check the process tree and the exe file paths. 
 *   
 ***********************************************************************/









/*********************************************************************
 *  For test.  
 *   
 ***********************************************************************/

#define CMD_LENGTH 3


int main() {
    int i = 0;
    long long s = 0;
    int current_window_length = 0;
    // hard-coded test purpose. 
    // In the kernel, each comamnd string will be provided by the in-kernel hook. 
    command cmds[CMD_LENGTH] = {
        str_to_command("cd"), 
        str_to_command("wget"), 
        str_to_command("chmod")
    };
    // initialize - everytime an alert is detected, this will be invoked.
    init_command_window();
    for (i = 0; i < CMD_LENGTH; i++) {
        printf("command '%s' is received!\n", enum2cmd_table[cmds[i]]);
        current_window_length = update_command_window(enum2cmd_table[cmds[i]]);
        s = compute_max_score_path(command_window, current_window_length);
        printf("current score: %lld\n", s);
        printf("--------------------------\n");
    }
    // initialize - everytime an alert is detected, this will be invoked.
    init_command_window();
    for (i = 0; i < CMD_LENGTH; i++) {
        printf("command '%s' is received!\n", enum2cmd_table[cmds[i]]);
        current_window_length = update_command_window(enum2cmd_table[cmds[i]]);
        s = compute_max_score_capability(command_window, current_window_length);
        printf("current score: %lld\n", s);
        printf("--------------------------\n");
    }
}
