#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>

char cwd[100];
char **parse_string(char *, int *);
char **form_two_ins(char **, int, char *);
int read_cpu();
int read_proc(float *, float *, char *);
void proc_check(char **);
void checkcpupercentage(char *);
void checkresidentmemory(char *);
void listFiles();
void sortFile(char *);
void two_commands();
void executeCommands(char *);
void term_handler(int);
void int_handler(int);
void command_execution(int, char *);
void redirectionParta();
void redirectionPartb();
int type_check(char **, int);
void ins_check(char **);
void ired_check(char **, int *, int *, int);
void ored_check(char **, int *, int *, int *, int);
void iored(char **, int, int, int);
void instruction_exec(char **, int, int);
void piped_command(char **);
void check_extra_commands(char **);

int main(int argc, char *argv[])
{
    signal(SIGINT, int_handler);
    signal(SIGTERM, term_handler);
    if (getcwd(cwd, 100) == NULL)
        perror(NULL);
    char command[1000], **ins;
    int c, wc;
    while (1)
    {
        write(1, "\n", 2);
        write(1, "myShell>", 8);
        memset(command, '\0', sizeof(command));
        read(0, command, 1000);
        if (strcmp(command, "\n\0") == 0)
            continue;
        ins = parse_string(command, &wc);
        if (strcmp(ins[0], "executeCommands") == 0)
            executeCommands(ins[1]);
        else
            instruction_exec(ins, c, wc);
    }
    return 0;
}
void instruction_exec(char **ins, int c, int wc)
{
    pid_t child;
    int status;

    c = type_check(ins, wc);
    if (c == -1)
    {
        write(1, "Illegal command or arguments\n", 30);
        return;
    }
    else if (c == 1)
    {
        two_commands(form_two_ins(ins, wc, ";"));
    }
    else if (c == 2)
    {
        piped_command(form_two_ins(ins, wc, "|"));
    }
    else
    {
        if ((strcmp(ins[0], "exit") == 0) )
                 exit(0);
        else                 
        if ((strcmp(ins[0], "checkcpupercentage") != 0) && (strcmp(ins[0], "checkresidentmemory") != 0) && (strcmp(ins[0], "listFiles") != 0) && (strcmp(ins[0], "sortFile") != 0) && (strcmp(ins[0], "grep") != 0) && (strcmp(ins[0], "cat") != 0))
        {
            write(1, "Illegal command or arguments\n", 30);
            return;
        }
        child = fork();
        if (child == 0)
        {
            int x = -1, y = 0, u = -1, v = 0, f = 0;
            ored_check(ins, &x, &y, &f, wc);
            ired_check(ins, &u, &v, wc);
            if (y >= 2 || v >= 2)
            {
                write(1, "Illegal command or arguments\n", 30);
                exit(0);
            }
            iored(ins, x, u, f);
            proc_check(ins);
            exit(0);
        }
        else
        {
            wait(&status);
        }
    }
    return;
}
char **parse_string(char *s, int *wc)
{
    int x = strlen(s);
    char st[x];
    strcpy(st, s);
    int i, word_count = 1, ctr = 0, j = 0;
    char **newString;
    char word[100];
    for (i = 0; i <= strlen(st); i++)
        if (st[i] == '\n')
            st[i] = '\0';
    for (i = 0; i <= strlen(st); i++)
        if (st[i] == ' ')
            word_count++;
    if (word_count == 0)
        return NULL;
    i = 0;
    newString = (char **)malloc(word_count * sizeof(char *));
    for (i = 0; i < strlen(st); i++)
    {
        if (st[i] == ' ')
        {
            word[j] = '\0';
            newString[ctr] = (char *)malloc(strlen(word) * sizeof(char));
            strcpy(newString[ctr], word);
            j = 0;
            ctr++;
            memset(word, '\0', sizeof(word));
        }
        else
        {
            word[j] = st[i];
            j++;
        }
    }
    word[j] = '\0';
    newString[ctr] = (char *)malloc(strlen(word) * sizeof(char));
    strcpy(newString[ctr], word);

    *wc = word_count;
    return newString;
}

int type_check(char **ins, int word_count)
{
    int i, r;
    int two_cmds = -1, pipe_red = -1;
    int count_pipe_red = 0, count_tc = 0;
    for (i = 0; i < word_count; i++)
    {
        if ((strcmp(ins[i], "|") == 0))
        {
            pipe_red = i;
            count_pipe_red++;
        }
        if ((strcmp(ins[i], ";") == 0))
        {
            two_cmds = i;
            count_tc++;
        }
    }
    if (count_tc > 1)
        return -1;
    else if (count_tc == 1)
        if (two_cmds > 0)
            return 1;
        else
            return -1;
    else if (count_pipe_red > 1)
        return -1;
    else if (count_pipe_red == 1)
        if (pipe_red > 0)
            return 2;
        else
            return -1;
    else
        return 0;
}
char **form_two_ins(char **newString, int word_count, char *s)
{
    char **twins;
    int j = 0, l[2], sp[2], k = 0, pos, i;
    l[0] = 0;
    l[1] = 0;
    sp[0] = 0;
    sp[1] = 0;
    twins = malloc(2 * sizeof(char *));
    for (i = 0; i < word_count; i++)
    {
        if (strcmp(newString[i], s) != 0)
        {
            l[j] += strlen(newString[i]);
            sp[j]++;
        }
        else
        {
            j++;
            pos = i;
        }
    }
    j = 0;
    twins = malloc(2 * sizeof(char *));
    twins[0] = malloc((l[0] + sp[0] - 1) * sizeof(char));
    twins[1] = malloc((l[1] + sp[1] - 1) * sizeof(char));
    strcpy(twins[0], newString[0]);
    strcat(twins[0], " ");
    strcpy(twins[1], newString[pos + 1]);
    strcat(twins[1], " ");
    for (i = 1; i < word_count; i++)
    {
        if (i != pos)
        {
            strcat(twins[j], newString[i]);
            strcat(twins[j], " ");
        }
        else
        {
            j++;
            i++;
        }
    }
    twins[0][strlen(twins[0]) - 1] = '\0';
    twins[1][strlen(twins[1]) - 1] = '\0';
    return twins;
}
void ired_check(char **ins, int *ir, int *irc, int word_count)
{
    int i, count = 0;
    for (i = 0; i < word_count; i++)
    {

        if ((strcmp(ins[i], "<") == 0))
        {
            *ir = i;
            count++;
        }
    }
    *irc = count;
    return;
}

void ored_check(char **ins, int * or, int *orc, int *f, int word_count)
{
    int i, count = 0;
    for (i = 0; i < word_count; i++)
    {
        if ((strcmp(ins[i], ">") == 0))
        {
            * or = i;
            count++;
            *f = 0;
        }
        if ((strcmp(ins[i], ">>") == 0))
        {
            * or = i;
            count++;
            *f = 1;
        }
    }
    *orc = count;
    return;
}
void iored(char **ins, int pos_or, int pos_ir, int t)
{
    int out_fp, in_fp;
    if (pos_ir != -1)
    {
        in_fp = open(ins[pos_ir + 1], O_RDONLY);
        dup2(in_fp, 0);
        close(in_fp);
    }
    if (pos_or != -1)
    {
        if (t == 0)
            out_fp = open(ins[pos_or + 1], O_CREAT | O_WRONLY, 0644);
        else
            out_fp = open(ins[pos_or + 1], O_CREAT | O_WRONLY | O_APPEND, 0644);
        dup2(out_fp, 1);
        close(out_fp);
    }
    return;
}
void proc_check(char **ins)
{

    if (strcmp(ins[0], "checkcpupercentage") == 0)
        command_execution(1, ins[1]);
    else if (strcmp(ins[0], "checkresidentmemory") == 0)
        command_execution(2, ins[1]);
    else if (strcmp(ins[0], "listFiles") == 0)
        command_execution(3, ins[1]);
    else if (strcmp(ins[0], "sortFile") == 0)
        command_execution(4, ins[1]);
    else
        check_extra_commands(ins);
    return;
}
void command_execution(int x, char *s)
{
    switch (x)
    {
    case 1:
        checkcpupercentage(s);
        break;
    case 2:
        checkresidentmemory(s);
        break;
    case 3:
        listFiles();
        break;
    case 4:
        sortFile(s);
        break;
    default:
        return;
    }
    return;
}

void int_handler(int num)
{
    char s;
    write(1, "the program is interrupted, do you want to exit [Y/N]", 53);
    read(0, &s, 1);
    if (s == 'Y')
        exit(0);
}

void checkcpupercentage(char *s)
{
    float before, after;
    float u_i, c_i, u_f, c_f;
    int f = 1;
    char user_per[10], cpu_per[10];
    f = read_proc(&u_i, &c_i, s);
    if (f == 0)
    {
        return;
        exit(0);
    }
    before = read_cpu();
    sleep(2);
    f = read_proc(&u_f, &c_f, s);
    after = read_cpu();
    float x = (100 * (u_f - u_i) / (after - before));
    float y = (100 * (c_f - c_i) / (after - before));
    snprintf(user_per, 10, "%f", x);
    snprintf(cpu_per, 10, "%f", y);
    char message_1[] = "user mode cpu percentage: ";
    char message_2[] = "system mode cpu percentage: ";
    write(1, message_1, strlen(message_1));
    write(1, user_per, strlen(user_per));
    write(1, "%\0", 1);
    write(1, "\n\0", 1);
    write(1, message_2, strlen(message_2));
    write(1, cpu_per, strlen(cpu_per));
    write(1, "%\0", 1);
    write(1, "\n\0", 1);
    return;
}
void checkresidentmemory(char *s)
{

    char *eargs[] = {"ps", "-o", "rss=", s, NULL};
    execve("/bin/ps", eargs, NULL);
}

void listFiles()
{
    strcat(cwd, "/files.txt");
    int fp = open(cwd, O_CREAT | O_WRONLY, 0644);
    dup2(fp, 1);
    close(fp);
    char *eargs[] = {"ls", NULL};
    execve("/bin/ls", eargs, NULL);
}

void sortFile(char *s)
{
    int fd = open(s, O_RDONLY);
    if (fd == -1)
    {
        perror(NULL);
        exit(0);
    }
    dup2(fd, 0);
    close(fd);
    char *eargs[] = {"sort", NULL};
    execve("/usr/bin/sort", eargs, NULL);
}

void executeCommands(char *s)
{
    int status;
    int fd = open(s, O_RDONLY);
    if (fd == -1)
    {

        perror(NULL);
        exit(0);
    }
    pid_t child;
    char ins[30][1000], c;
    int i = 0, j = 0, r;
    while (1)
    {
        r = read(fd, &c, sizeof(c));
        if (r == 0)
            break;
        if (c == '\n')
        {
            c = '\0';
            ins[i][j] = '\0';
            i++;
            j = 0;
        }
        else
        {
            ins[i][j] = c;
            j++;
        }
    }
    int v, wc;
    for (int j = 0; j < i; j++)
    {
        child = fork();
        if (child == 0)
        {
            char **comm = parse_string(ins[j], &wc);
            c = type_check(comm, wc);
            instruction_exec(comm, c, wc);
            free(comm);
            exit(0);
        }
        else
            wait(&status);
    }
    return;
}
void term_handler(int num)
{
    write(1, "Got SIGTERM-Leaving\n", 21);
    exit(0);
}

int read_proc(float *user_time, float *cpu_time, char *s)
{
    char filename[100] = "/proc/";
    int i, ctr = 0;
    char path[6] = "/stat";
    char c, word[2][10];
    int length, j;
    strcat(filename, s);
    length = strlen(filename);
    for (j = 0; path[j] != '\0'; j++, length++)
        filename[length] = path[j];
    filename[length] = '\0';
    int fd1 = open(filename, O_RDONLY);
    if (fd1 == -1)
    {
        perror(NULL);
        return 0;
    }
    read(fd1, &c, sizeof(c));
    j = 0;

    while (c != '\n')
    {
        if (c == ' ')
        {
            if (ctr == 13 || ctr == 14)
                word[ctr - 13][j] = '\0';
            ctr++;
            j = 0;
        }
        if (ctr >= 15)
            break;
        else
        {
            if (ctr == 13 || ctr == 14)
                word[ctr - 13][j] = c;
            j++;
        }
        read(fd1, &c, sizeof(c));
    }
    *user_time = atoi(word[0]);
    *cpu_time = atoi(word[1]);
    return 1;
}
int read_cpu()
{
    int j = 0, i, ctr = 0;
    float x = 0;
    char *path = "/proc/stat";
    char val[12][20];
    int fd2 = open(path, O_RDONLY);
    char c;
    read(fd2, &c, sizeof(c));
    while (c != '\n')
    {

        if (c == ' ')
        {
            val[ctr][j] = '\0';
            ctr++;
            j = 0;
        }
        else
        {
            val[ctr][j] = c;
            j++;
        }
        read(fd2, &c, sizeof(c));
    }
    val[ctr][j] = '\0';
    ctr++;
    for (j = 2; j < ctr; j++)
    {
        x += atoi(val[j]);
    }
    return x;
}
void piped_command(char **twins)
{
    int i = 0, comm_pipe[2];
    int child, status, second_child;
    int fd_1[2];
    pipe(fd_1);
    pipe(comm_pipe);
    child = fork();
    if (child == 0)
    {
        int x = -1, y = 0, u = -1, v = 0, f = 0, i, wc;
        close(comm_pipe[0]);
        dup2(comm_pipe[1], 1);
        close(comm_pipe[1]);
        char **ns;
        ns = parse_string(twins[0], &wc);
        close(fd_1[0]);
        if (strcmp(ns[0], "exit") == 0)
        {
            write(fd_1[1], "exit\0", 5);
            close(fd_1[1]);
            exit(0);
        }
        close(fd_1[1]);
        
        if (strcmp(ns[0], "executeCommands") == 0)
            executeCommands(ns[1]);
        else
        {
            ored_check(ns, &x, &y, &f, wc);
            ired_check(ns, &u, &v, wc);
            iored(ns, x, u, f);
            proc_check(ns);
            exit(0);
        }
    }
    else
    {
        close(comm_pipe[1]);
        wait(&status);
        char rep_1[5];
        read(fd_1[0], rep_1, 5);
        close(fd_1[0]);
        if (strcmp(rep_1, "exit") == 0)
                exit(0);
        int fd_2[2];
        pipe(fd_2);
        close(fd_1[1]);
        second_child = fork();
        if (second_child == 0)
        {
            dup2(comm_pipe[0], 0);
            close(comm_pipe[0]);
            int x = -1, y = 0, u = -1, v = 0, f = 0, i, wc;
            char **ns;
            ns = parse_string(twins[1], &wc);
            close(fd_2[0]);
            close(fd_1[0]);
            if (strcmp(ns[0], "exit") == 0)
            {
                write(fd_2[1], "exit\0", 5);
                close(fd_2[1]);
                exit(0);
            }
            close(fd_2[1]);
            if (strcmp(ns[0], "executeCommands") == 0)
                executeCommands(ns[1]);
            else
            {
                ored_check(ns, &x, &y, &f, wc);
                ired_check(ns, &u, &v, wc);
                iored(ns, x, u, f);
                proc_check(ns);
                exit(0);
            }
        }
        else
        {
            close(comm_pipe[0]);
            close(fd_2[1]);
            wait(&status);
            char rep_2[5];
            read(fd_2[0], rep_2, 5);
            close(fd_2[0]);
            if (strcmp(rep_2, "exit") == 0)
                exit(0);
            return;
        }
    }
}
void two_commands(char **twins)
{
    int child, second_child, status,pid;
    int fd_1[2];
    pipe(fd_1);
    child = fork();
    if (child == 0)
    {
        int x = -1, y = 0, u = -1, v = 0, f = 0, i, wc;
        char **ns;
        ns = parse_string(twins[0], &wc);
        close(fd_1[0]);
        if (strcmp(ns[0], "exit") == 0)
        {
            write(fd_1[1], "exit\0", 5);
            close(fd_1[1]);
            exit(0);
        }
        close(fd_1[1]);
        int c = type_check(ns, wc);
        if (strcmp(ns[0], "executeCommands") == 0)
            executeCommands(ns[1]);
        else
            instruction_exec(ns, c, wc);
        exit(0);
    }
    else
    {
        int fd_2[2];
        pipe(fd_2);
        close(fd_1[1]);
        second_child = fork();
        if (second_child == 0)
        {
            int x = -1, y = 0, u = -1, v = 0, f = 0, i, wc;
            char **ns;
            ns = parse_string(twins[1], &wc);
            close(fd_2[0]);
            close(fd_1[0]);
            if (strcmp(ns[0], "exit") == 0)
            {
                write(fd_2[1], "exit\0", 5);
                close(fd_2[1]);
                exit(0);
            }
            close(fd_2[1]);
            int c = type_check(ns, wc);
            if (strcmp(ns[0], "executeCommands") == 0)
                executeCommands(ns[1]);
            else
                instruction_exec(ns, c, wc);
            exit(0);
        }
        else
        {
            close(fd_2[1]);
            while ((pid = waitpid(-1, NULL, 0)))
            {
                if (errno == ECHILD)
                {
                    break;
                }
            }
            char rep_1[5], rep_2[5];
            read(fd_1[0], rep_1, 5);
            read(fd_2[0], rep_2, 5);
            close(fd_1[0]);
            close(fd_2[0]);
            if ((strcmp(rep_1, "exit") == 0) || (strcmp(rep_2, "exit") == 0))
                exit(0);
        }
    }
    return;
}
void check_extra_commands(char **ins)
{
    int f=0;
    char *eargs_1[]={ins[0],ins[1],NULL};
    char *eargs_2[]={ins[0],NULL};
    if ((strcmp(ins[1], "<") == 0) || (strcmp(ins[1], ">") == 0) || (strcmp(ins[1], ">>") == 0))
        f=1;
    if (strcmp(ins[0], "grep") == 0)
    { 
        if(f==0)
                execve("/bin/grep", eargs_1, NULL);

        else
                execve("/bin/grep", eargs_2, NULL);

    }
    else if (strcmp(ins[0], "cat") == 0)
    {
        if(f==0)
            execve("/bin/cat", eargs_1, NULL);
        else
            execve("/bin/cat", eargs_2, NULL);
        
    }
    else
        write(1, "Illegal command or arguments\n", 30);
    return;
}