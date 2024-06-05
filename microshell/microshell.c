#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void	ft_error(char *str, char *str2)
{
	write(2, "error: ", 7);
	write(2, str, strlen(str));
	write(2, str2, strlen(str2));
	write(2, "\n", 1);
}

int	ft_cd(char **av, int i)
{
	if (i != 2)
	{
		ft_error("cd: bad arguments", "");
		return (1);
	}
	if (chdir(av[1]))
	{
		ft_error("cd: cannot change directory to ", av[1]);
		return (1);
	}
	return (0);
}

int	ft_handle_fds(int *fd, int is_child, int pipes)
{
	if (pipes)
	{
		if (is_child)
		{
			if (dup2(fd[1], 1) == -1 || close(fd[0]) == -1 || close(fd[1]) ==
				-1)
			{
				ft_error("fatal", "");
				return (1);
			}
		}
		else
		{
			if (dup2(fd[0], 0) == -1 || close(fd[0]) == -1 || close(fd[1]) ==
				-1)
			{
				ft_error("fatal", "");
				return (1);
			}
		}
	}
	return (0);
}

int	ft_execute(char *symbol, char *path, char **av, char **ev, int i)
{
	int	fd[2];
	int	status;
	int	pid;
	int	pipes;

	pipes = 0;
	if (symbol)
		pipes = !strcmp(symbol, "|");
	if (pipe(fd) == -1)
	{
		ft_error("fatal", "");
		return (1);
	}
	pid = fork();
	if (pid == 0)
	{
		av[i] = 0;
		if (ft_handle_fds(fd, 1, pipes))
			return (1);
		execve(path, av, ev);
		ft_error("cannot execute ", path);
		return (1);
	}
	waitpid(pid, &status, 0);
	if (ft_handle_fds(fd, 0, pipes))
		return (1);
	return (WEXITSTATUS(status));
}

int	main(int ac, char **av, char **ev)
{
	int		i;
	int		status;
	char	*path;
	char	*symbol;

	i = 0;
	status = 0;
	if (ac < 2)
		return (0);
	while (av[i] && av[++i])
	{
		av = &av[i];
		i = 0;
		while (av[i] && strcmp(av[i], ";") && strcmp(av[i], "|"))
			i++;
		symbol = av[i];
		path = av[0];
		if (!strcmp(path, "cd"))
			status = ft_cd(av, i);
		else if (i)
			status = ft_execute(symbol, path, av, ev, i);
	}
	return (status);
}
