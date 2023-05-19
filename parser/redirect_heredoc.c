/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_heredoc.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvicente <mvicente@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/21 16:14:29 by asepulve          #+#    #+#             */
/*   Updated: 2023/05/19 15:00:03 by mvicente         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

char	*get_delimitador(char *token, char ***tokens)
{
	int		i;
	char	*delimitador;

	i = 0;
	i += jump_white_spaces(&token[i]);
	i += ft_isredirects(&token[i]);
	i += jump_white_spaces(&token[i]);
	delimitador = ft_strdup(&token[i]);
	if (tokens)
		free_tokens(tokens);
	return (delimitador);
}

void	close_heredoc(int fd, char *delimitador, char *line)
{
	(void)fd;
	if (!line)
		ft_printf(ERR_CTRL_D, delimitador);
	else if (line)
		free(line);
	if (delimitador)
		free(delimitador);
	free_envp(*env());
	exit(EXIT_SUCCESS);
}

static void	heredoc_process(int fd, char *token, char ***tokens)
{
	char	*line;
	int		expand_flag;
	char	*delimitador;

	expand_flag = 1;
	(void)expand_flag;
	delimitador = get_delimitador(token, tokens);
	// delimitador = formatter(delimitador);
	while (1)
	{
		ft_printf("heredoc:>");
		line = get_next_line(0);
		if (line == NULL || !ft_strncmp(line, delimitador, \
		ft_strlen(line) - (line[ft_strlen(line) - 1] == '\n')))
			close_heredoc(fd, delimitador, line);
		write(fd, line, ft_strlen(line));
		free(line);
	}
}

int	heredoc(char *token, t_com_list *new, t_com_list *lst, char **paths, char ***tokens)
{
	int		pid;
	char	*pathname;
	int		fd;

	pathname = take_avaible_filename();
	(void)new;
	(void)lst;
	fd = open(pathname, O_WRONLY | O_CREAT, 0644);
	pid = fork();
	if (pid == -1)
		ft_putendl_fd("Forked failed\n", 2);
	else if (pid == 0)
	{
		signal(SIGINT, NULL);
		signal(SIGQUIT, SIG_IGN);
		if (pathname)
			free(pathname);
		if (new)
		{
			free(new->command);
			if (new->path)
				free(new->path);
			free_double(new->param);
			free(new);
		}
		if (lst)
			free_lst(lst);
		if (paths)
			free_double(paths);
		heredoc_process(fd, token, tokens);
	}
	wait(NULL);
	close(fd);
	fd = open(pathname, O_RDONLY, 0444);
	if (pathname)
		free(pathname);
	return (fd);
}
