#include <stdio.h>
#include <stdlib.h>
#include <sys/sysctl.h>
#include <libproc.h>
#include <signal.h> 

void list_processes() {
    int num_processes;
    struct kinfo_proc *process_list = NULL;
    size_t length;

    int mib[3] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL };

    // get the len of the data
    sysctl(mib, 3, NULL, &length, NULL, 0);

    // alloc memory for the processes
    process_list = malloc(length);

    // get the process list
    sysctl(mib, 3, process_list, &length, NULL, 0);

    num_processes = length / sizeof(struct kinfo_proc);

    printf("PID\tName\n");
    for (int i = 0; i < num_processes; i++) {
        printf("%d\t%s\n", process_list[i].kp_proc.p_pid, process_list[i].kp_proc.p_comm);
    }

    free(process_list);
}

void get_process_info(pid_t pid) {
    struct proc_taskinfo info;
    struct proc_threadinfo thinfo; 
    int status = proc_pidinfo(pid, PROC_PIDTASKINFO, 0, &info, sizeof(info));
    
    if (status != sizeof(info)) {
        fprintf(stderr, "Failed to get process info for PID %d\n", pid);
        return;
    }
    
    printf("PID: %d\n", pid);
    printf("CPU Usage: %llu -> %d%%\n", info.pti_total_user + info.pti_total_system, thinfo.pth_cpu_usage); 
    printf("Memory Usage: %lluKB\n", info.pti_resident_size / 1024);
}

void kill_process(pid_t pid) {
    if (kill(pid, SIGKILL) == 0) {
        printf("Process %d terminated successfully\n", pid);
    } else {
        perror("Failed to terminate process");
    }
}

void print_menu() {
    printf("\n");
    printf("1. List Processes\n");
    printf("2. Get Process Info\n");
    printf("3. Kill Process\n");
    printf("4. Exit\n");
    printf("Choose an option: ");
}

int main() {
    int choice;
    pid_t pid;

    while (1) {
        print_menu();
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                list_processes();
                break;
            case 2:
                printf("Enter PID: ");
                scanf("%d", &pid);
                get_process_info(pid);
                break;
            case 3:
                printf("Enter PID to kill: ");
                scanf("%d", &pid);
                kill_process(pid);
                break;
            case 4:
                exit(0);
            default:
                printf("Invalid choice\n");
        }
    }

    return 0;
}