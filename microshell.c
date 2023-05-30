#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void	print_err(char *str, int err)
{
	int i;
	
	i = 0;
	while (str[i])
		++i;
	write(2, str, i);
	if (err == 1)
		exit(1);
}

void	ft_perror(char *str, char *cmd)
{
	print_err(str, 0);
	print_err(cmd, 0);
	print_err("\n", 0);
}

void ft_cd(char **cmd)
{
	if (!cmd[1])
		return (print_err("error: cd: bad arguments\n", 0));
	if (chdir(cmd[1]) == -1)
		return (ft_perror("error: cd: cannot change directory to ", cmd[1]));
}

void ft_exe(char **cmd, char *path, char **env, int last_cmd)
{
	int fd[2];
	int pid;

	if (!cmd[0])
		return ;
	if (!strcmp(cmd[0], "cd"))
		return (ft_cd(cmd));
	if (pipe(fd) == -1)
		print_err("error: fatal\n", 1);
	pid = fork();
	if (pid == -1)
		print_err("error: fatal\n", 1);
	if (pid)
	{
		if (close(fd[1]) == -1)
			print_err("error: fatal\n", 1);
		if (dup2(fd[0], 0) == -1)
			print_err("error: fatal\n", 1);
		if (close(fd[0]) == -1)
			print_err("error: fatal\n", 1);
		if (waitpid(pid, NULL, 0) == -1)
			print_err("error: fatal\n", 1);
	}
	else
	{
		if (close(fd[0]) == -1)
			print_err("error: fatal\n", 1);
		if (!last_cmd)
		{
			if (dup2(fd[1], 1) == -1)
				print_err("error: fatal\n", 1);
		}
		execve(path, cmd, env);
		ft_perror("error: cannot execute ", path);
		exit(1);
	}
}

int main(int argc, char **argv, char **env)
{
	int i;
	int cmd;

	i = 1;
	cmd = 1;
	while (i < argc)
	{
		if (!strcmp(argv[i], ";"))
		{
			argv[i] = NULL;
			ft_exe(&argv[cmd], argv[cmd], env, 1);
			cmd = i + 1;
		}
		else if (!strcmp(argv[i], "|"))
		{
			argv[i] = NULL;
			ft_exe(&argv[cmd], argv[cmd], env, 0);
			cmd = i + 1;
		}
		i++;
	}
	ft_exe(&argv[cmd], argv[cmd], env, 1);
	return (0);
}
