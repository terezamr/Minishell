/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvicente <mvicente@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/28 16:53:49 by mvicente          #+#    #+#             */
/*   Updated: 2023/04/27 12:52:33 by mvicente         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./executor.h"

int	get_com(t_command_list *lst)
{
	int	i;

	i = 0;
	while (lst)
	{
		i++;
		lst = lst->next;
	}
	return (i);
}

void	error_function(t_command_list *lst, int **fd)
{
	perror(lst->command);
	if (fd)
		free_pipes(fd, get_com(lst));
	free_lst(lst);
	g_exit_s = 127;
	exit(g_exit_s);
}

void	command(int **fd, t_command_list *lst, int i, int com)
{
	t_command_list	*node;

	node = get_lst(lst, i);
	if (i == 0)
		command_one(fd, lst, i);
	else if (i == com - 1)
		command_final(fd, lst, i);
	else
		command_middle(fd, lst, i);
	check_builtin(node);
	execve(node->path, node->param, *env());
	error_function(lst, fd);
}

void	do_fork(t_command_list *lst, int **id, int i, int com)
{
	int	pa;

	pa = fork();
	if (pa == -1)
		return ;
	else if (pa == 0)
		command(id, lst, i, com);
}

void	execute_one(t_command_list *lst)
{
	int	pa;

	pa = fork();
	if (pa == -1)
		error_function(lst, 0);
	else if (pa == 0)
	{
		if (lst->inf != 0)
			dup2(lst->inf, STDIN_FILENO);
		if (lst->outf != 0)
			dup2(lst->outf, STDOUT_FILENO);
		execve(lst->path, lst->param, *env());
		error_function(lst, 0);
	}
}

void	execute(t_command_list *lst, int com)
{
	int	i;
	int	f;
	int	**id;
	int	pid;
	int	status;

	(void)f;
	i = 0;
	f = 3;
	id = 0;
	pid = 0;
	if (com == 1)
	{
		if (check_builtin_one(lst) == -1)
			execute_one(lst);
	}
	else
	{
		id = create_pipes(com);
		while (i < com)
		{
			if (i != com - 1)
				pipe(id[i]);
			do_fork(lst, id, i, com);
			if (i != com - 1)
				close(id[i][1]);
			i++;
		}
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		g_exit_s = WEXITSTATUS(status);
	free_pipes(id, com);
	//printf("exit status %i\n", g_exit_s);
}