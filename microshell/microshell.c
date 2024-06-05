#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int ft_error(char *s1, char *s2)
{
    write(2, "error: ", 7);
    write(2, s1, strlen(s1));
    write(2, s2, strlen(s2));
    write(2, "\n", 1);
    return(1);
}

int ft_cd(char **av, int i)
{
    if(i != 2)
        return(ft_error("cd: bad arguments", ""));
    if(chdir(av[1]))
        return(ft_error("cd: cannot change directory to ", av[1]));
    return(0);
}

int ft_fds(int *fd, int i, int pipes)
{
    if(pipes)
    {
        if(dup2(fd[i], i) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1)
            return(ft_error("fatal",""));
    }
    return(0);
}

int ft_execute(char **av, char**ev, char *path, char *symbol, int i)
{
    int fd[2];
    int pid;
    int status = 0;
    int pipes = 0;
    if(symbol)
        pipes = !strcmp(symbol, "|");
    if(pipe(fd) == -1)
        return(ft_error("fatal", ""));
    pid = fork();
    if(pid == 0)
    {
        av[i] = 0;
        if(ft_fds(fd, 1, pipes))
            return(1);
        execve(path, av, ev);
        return(ft_error("cannot execute ", path));
    }
    waitpid(pid, &status, 0);
    if(ft_fds(fd, 0, pipes))
        return(1);
    return(WEXITSTATUS(status));
}

int main(int ac, char **av, char **ev)
{
    int i = 0;
    int status = 0;
    char *path;
    char *symbol;
    if(ac < 2)
        return(0);
    while(av[i] && av[++i])
    {
        av = &av[i];
        i = 0;
        while(av[i] && strcmp(av[i], ";") && strcmp(av[i], "|"))
            i++;
        path = av[0];
        symbol = av[i];
        if(!strcmp(path, "cd"))
            status = ft_cd(av, i);
        else if(i)
            status = ft_execute(av, ev, path, symbol, i);
    }
    return(status);
}
