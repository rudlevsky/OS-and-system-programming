#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <fcntl.h>
#include <wait.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define PROC_COUNT (9)
#define STARTING_PROC_ID (1)
#define MAX_CHILDS_COUNT (3)
#define MAX_USR_COUNT (101)

const unsigned char CHILDS_COUNT[PROC_COUNT] =
{
/*  0  1  2  3  4  5  6  7  8  */
    1, 3, 2, 0, 0, 0, 1, 1, 0
};

const unsigned char CHILDS_IDS[PROC_COUNT][MAX_CHILDS_COUNT] =
{
    {1},        /* 0 */
    {2, 3, 4},  /* 1 */
    {5, 6},     /* 2 */
    {0},        /* 3 */
    {0},        /* 4 */
    {0},        /* 5 */
    {7},        /* 6 */
    {8},        /* 7 */
    {0}         /* 8 */
};

/* Group types:
 *
 * 0 = pid;
 * 1 = parent's pgid
 * 2 = previous child group
 */
const unsigned char GROUP_TYPE[PROC_COUNT] =
{
/*  0  1  2  3  4  5  6  7  8  */
    0, 1, 0, 2, 0, 0, 0, 1, 1
};

/* Whome to send signal:
 *
 * 0 = none
 * positive = pid
 * negative = processes group
 *
 * f/ex: "-x" means, that signal is sent to the group, process with pid = x is member of
 */
const char SEND_TO[PROC_COUNT] =
{
/*  0,  1,  2,  3,  4,  5,  6,  7,  8  */
    0, -6,  1,  0, -2,  0,  4,  4,  4
};

const int SEND_SIGNALS[PROC_COUNT] =
{
    0,          /* 0 */
    SIGUSR1,    /* 1 */
    SIGUSR2,    /* 2 */
    0,          /* 3 */
    SIGUSR1,    /* 4 */
    0,          /* 5 */
    SIGUSR1,    /* 6 */
    SIGUSR2,    /* 7 */
    SIGUSR1     /* 8 */
};

const char RECV_SIGNALS_COUNT[2][PROC_COUNT] =
{
/*    0, 1, 2, 3, 4, 5, 6, 7, 8  */
    { 0, 0, 1, 1, 2, 0, 1, 1, 1 },  /* SIGUSR1 */
    { 0, 1, 0, 0, 1, 0, 0, 0, 0 }   /* SIGUSR2 */
};

void sig_handler(int signum);
void set_sig_handler(void (*handler)(void *), int sig_no, int flags);
void print_error_exit(const char *s_name, const char *msg, const int proc_num);
void forker(int curr_number, int childs_count);
void kill_wait_for_children();
void wait_for_children();

char *exec_name = NULL;
int proc_id = 0;
pid_t *pids_list = NULL;
char *tmp_file_name = "/tmp/pids.log";


int main(int argc, char *argv[])
{
    exec_name = basename(argv[0]);

    pids_list = (pid_t*)mmap(pids_list, (2*PROC_COUNT)*sizeof(pid_t), PROT_READ | PROT_WRITE,
                             MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    int i = 0;                           // initialize pids list with zeros 
    for (i = 0; i < 2*PROC_COUNT; ++i) {
        pids_list[i] = 0;
    }

    forker(0, CHILDS_COUNT[0]);          // create processes tree 

    set_sig_handler(&kill_wait_for_children, SIGTERM, 0);

    if (proc_id == 0) {                  // main process waits 
        wait_for_children();
        munmap(pids_list, (2*PROC_COUNT)*sizeof(pid_t));
        return 0;
    }

    on_exit(&wait_for_children, NULL);

    pids_list[proc_id] = getpid();          // save pid to list

    if (proc_id == STARTING_PROC_ID) {                  // starter waits for all pids to be available 

        FILE *tmp = fopen(tmp_file_name, "wt");
        if (tmp == NULL) {
            print_error_exit(exec_name, "Can't create temp file", 0);
        }

        for (i = 1; i < PROC_COUNT; ++i) {
            fprintf(tmp, "%d\n", pids_list[i]);
        }

        fclose(tmp);

        pids_list[0] = 1;           // all pids are set

        set_sig_handler(&sig_handler, 0, 0);

        do{
            for (i = 1+PROC_COUNT; (i < 2*PROC_COUNT)  && (pids_list[i] != 0); ++i) {

                if (pids_list[i] == -1) {
                    print_error_exit(exec_name, "Error: not all processes forked or initialized.\nHalting.", 0);
                    exit(1);
                }
            }
        } while (i < 2*PROC_COUNT);

        for (i = PROC_COUNT+1; i < 2*PROC_COUNT; ++i) {   
            pids_list[i] = 0;
        }

        sig_handler(0); // start signal-flow


    } else {    // other processes

        do {
            // wait for all pids to be written
        } while (pids_list[0] == 0);

        set_sig_handler(&sig_handler, 0, 0);
    }

    while (1000) {
        pause();  // start cycle
    }
    return 0;
}  


void print_error_exit(const char *s_name, const char *msg, const int proc_num) {
    fprintf(stderr, "%s: %s %d\n", s_name, msg, proc_num);
    fflush(stderr);
    pids_list[proc_num] = -1;
    exit(1);
} 


void wait_for_children() {
    int i = CHILDS_COUNT[proc_id];
    while (i > 0) {
        wait(NULL);
        --i;
    }
}   


long long current_time() {
    struct timeval time;
    gettimeofday(&time, NULL);
    return time.tv_usec;
}   



volatile int usr_recv[2] = {0, 0};

volatile int usr_amount[2][2] =
{
    {0, 0}, 
    {0, 0}  
};


void kill_wait_for_children() {
    int i = 0;

    for (i = 0; i < CHILDS_COUNT[proc_id]; ++i) {
        kill(pids_list[CHILDS_IDS[proc_id][i]], SIGTERM);
    }

    wait_for_children();

    if (proc_id != 0)
    	printf("%d %d завершил работу после %d SIGUSR1 и %d SIGUSR2\n",
           getpid(), getppid(), usr_amount[0][1], usr_amount[1][1]);

    fflush(stdout);
    exit(0);
}   


void sig_handler(int signum) {

    if (signum == SIGUSR1) {
        signum = 0;
    } else if (signum == SIGUSR2) {
        signum = 1;
    } else {
        signum = -1;
    }

    if (signum != -1) {
        ++usr_amount[signum][0];
        ++usr_recv[signum];
        printf("%d %d %d получил %s%d %lld\n", proc_id, getpid(), getppid(),
               "USR", signum+1, current_time() );

        fflush(stdout);

    	
    	if (proc_id == 1) {
        	if (usr_amount[0][0] + usr_amount[1][0] == MAX_USR_COUNT) {
            		kill_wait_for_children();
        	}

        	pids_list[PROC_COUNT + 6] = pids_list[PROC_COUNT + 4] = 0;
    	}

    	if (proc_id == 8) {
        	do{
            	// wait for 6th and 4th processes to send signal
        	}while ( (pids_list[PROC_COUNT + 6] + pids_list[PROC_COUNT + 4]) != 2 );
    	}

    	if (! ( (usr_recv[0] == RECV_SIGNALS_COUNT[0][proc_id]) &&
        	(usr_recv[1] == RECV_SIGNALS_COUNT[1][proc_id]) ) ) {
            	if ( (proc_id == 4) && (signum == 0) ) {
                	pids_list[PROC_COUNT + 4] = 1;
            	}
            	return;
        }
        usr_recv[0] = usr_recv[1] = 0;
    }

    char to = SEND_TO[proc_id];

    if (to != 0) {
        signum = ( (SEND_SIGNALS[proc_id] == SIGUSR1) ? 1 : 2);
        ++usr_amount[signum-1][1];
    }

    if (signum != 0) {
    	printf("%d %d %d послал %s%d %lld\n", proc_id, getpid(), getppid(),
         	"USR", signum, current_time() );
    }

    fflush(stdout);

    if (to > 0) {
        kill(pids_list[to], SEND_SIGNALS[proc_id]);
    } else if (to < 0) {
        kill(-getpgid(pids_list[-to]), SEND_SIGNALS[proc_id]);
    } else {
        return;
    }
    
    if (proc_id == 6) {
        pids_list[PROC_COUNT + 6] = 1;
    }
}   


void set_sig_handler(void (*handler)(void *), int sig_no, int flags) {

    sigset_t block_mask;
    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGUSR1);
    sigaddset(&block_mask, SIGUSR2);

    struct sigaction sa, oldsa;         // set sighandler 
    sa.sa_mask = block_mask;
    sa.sa_flags = flags;
    sa.sa_handler = handler;

    //ustan lubomu 
    if (sig_no != 0) {
        sigaction(sig_no, &sa, &oldsa);
        return;
    }

    //ustan ne vsem
    int i = 0;
    for (i = 0; i < PROC_COUNT; ++i) {
        char to = SEND_TO[i];
                                                                         /* someone sends me a signal: */
        if ( ( (to > 0) && (to == proc_id) )  ||                             /* <- directly */
             ( (to < 0) && (getpgid(pids_list[-to]) == getpgid(0)) ) ) {      /* <- or through the group */

            if (SEND_SIGNALS[i] != 0) { // signal is really sent
                if (sigaction(SEND_SIGNALS[i], &sa, &oldsa) == -1) {
                    print_error_exit(exec_name, "Can't set sighandler!", proc_id);
                }
            }
        }
    }  
    pids_list[proc_id + PROC_COUNT] = 1;
}   


void forker(int curr_number, int childs_count) {
    pid_t pid = 0;
    int i = 0;

    for (i = 0; i < childs_count; ++i) {

        int chld_id = CHILDS_IDS[curr_number][i];
        if ( (pid = fork() ) == -1) {
            print_error_exit(exec_name, "Can't fork!", chld_id);
        } else if (pid == 0) {  /*  child    */
            proc_id = chld_id;

            if (CHILDS_COUNT[proc_id] != 0) {
                forker(proc_id, CHILDS_COUNT[proc_id]);         // fork children
            }

            break;

        } else {    // pid != 0 (=> parent)
            static int prev_chld_grp = 0;
            int grp_type = GROUP_TYPE[chld_id];

            if (grp_type == 0) {
                if (setpgid(pid, pid) == -1) {
                    print_error_exit(exec_name, "Can't set group", chld_id);
                } else {
                    prev_chld_grp = pid;
                }

            } else if (grp_type == 1) {
                if (setpgid(pid, getpgid(0)) == -1) {
                    print_error_exit(exec_name, "Can't set group", chld_id);
                }
            } else if (grp_type == 2) {
                if (setpgid(pid, prev_chld_grp) == -1) {
                    print_error_exit(exec_name, "Can't set group", chld_id);
                }
            }
        }   
    }  
}   

