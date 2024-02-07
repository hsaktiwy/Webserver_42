#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

typedef struct list{
    char *arg;
    struct list *next;
}t_list;

t_list  *ft_lstnew(char *arg)
{
    t_list *new;

    new = malloc(sizeof(t_list));
    if (!new)
        return (NULL);
    new->arg = arg;
    new->next = NULL;
    return (new);
}

void ft_lstadd_back(t_list **list, t_list *new)
{
    t_list *curr;

    if (!*list)
    {
        *list = new;
        return ;
    }
    curr = *list;
    while (curr->next)
        curr = curr->next;
    curr->next = new;
}

int     ft_lstsize(t_list *list)
{
    int i = 0;

    while (list)
    {
        list = list->next;
        i++;
    }
    return (i);
}

char **command(t_list *list, int size)
{
    char **cmd;
    int i = 0;

    if (size != 0)
    {
        cmd = (char **)malloc((sizeof(char *) * (size + 1)));
        if (!cmd)
            return (NULL);
        cmd[size] = NULL;
        while (i < size)
        {
            cmd[i] =  list->arg;
            list = list->next;
            i++;
        }
        return (cmd);
    }
    return (NULL);
}

void free_list(t_list **cmd)
{
    t_list *curr;
    t_list *next;

    if (!*cmd)
        return ;
    curr = *cmd;
    while (curr)
    {
        next = curr->next;
        free(curr);
        curr = next;
    }
    *cmd  = NULL;
}
int ft_strlen(char *str)
{
    int i = 0;

    while (str[i])
        i++;
    return (i);
}

int     ft_err(char *str)
{
    write(STDERR_FILENO, str, ft_strlen(str));
    return (1);
}
int     cd(char **cmd, int size)
{
    if (size != 2)
        return (ft_err("error: cd: bad arguments\n"));
    else if (chdir(cmd[1]) == -1)
    {
        ft_err("error: cd: cannot change ");
        ft_err(cmd[1]);
        return (ft_err("\n"));
    }
    return (0);
}

int execute_command(char **cmd, char **env, char **argv, int i)
{
    static int read_fd;
    int     thereis_pipe = 0;
    int status;
    int fd[2];
    int id;

    if (argv[i])
        thereis_pipe = !strcmp(argv[i], "|");
    if (pipe(fd) == -1)
        return (ft_err("error: fatal\n"));
    id = fork();
    if (id == -1)
        return (ft_err("error: fatal\n"));
    else if (id == 0)
    {
        //execute
        if (thereis_pipe)
        {
            if (dup2(fd[1], STDOUT_FILENO) == -1 && close(fd[1]) == -1)
                return (ft_err("error: fatal\n"));
        }
        else if (read_fd != STDIN_FILENO)
        {
            if (dup2(read_fd, STDIN_FILENO) == -1 && close(fd[read_fd]) == -1)
                return (ft_err("error: fatal\n"));
        }
        execve(cmd[0], cmd, env);
        ft_err("error: cannot execute ");
        ft_err(cmd[0]);
        ft_err("\n");
        exit(1);
    }
    waitpid(id, &status, 0);
    if (thereis_pipe)
        read_fd = fd[0];
    else
        close(fd[0]);
    close(fd[1]);
    status = WIFEXITED(status) && WEXITSTATUS(status);
    return (status);
}

// void    display(char **cmd)
// {
//     int i  = 0;
//     ft_err("display :\n     arg: ");
//     if (!cmd)
//         return ;
//     while (cmd[i])
//     {
//         ft_err(" ");
//         ft_err(cmd[i]);
//         i++;
//     }
//     ft_err("\n");
// }
// #include <stdio.h>
int main(int argc, char **argv, char **env)
{
    t_list  *list = NULL;
    int     status = 0;
    char    **cmd;
    int     size;
    int i = 1;

    if (argc != 1)
    {
        while (i <= argc)
        {
            if (!argv[i] || (argv[i] && !strcmp(argv[i], ";")) || (argv[i] && !strcmp(argv[i], "|")))
            {
                size = ft_lstsize(list);
                // printf("size = %d\n", size);
                cmd = command(list, size);
                // display(cmd);
                if (cmd && !strcmp(cmd[0], "cd"))
                    status = cd(cmd, size);
                else if (cmd)
                    status = execute_command(cmd, env, argv, i);
                free_list(&list);
                free(cmd);
                cmd = NULL;
                list = NULL;
            }
            else if(argv[i])
            {
                // ft_err("test\n");
                ft_lstadd_back(&list, ft_lstnew(argv[i]));
            }
            i++;
        }
    }
    return (status);
}
