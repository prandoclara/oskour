/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claprand <claprand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 13:52:16 by claprand          #+#    #+#             */
/*   Updated: 2024/10/24 16:02:44 by claprand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	execute(t_data *data)
{
    if (data->cmd->path == NULL)
    {
	    data->cmd->path = get_cmd_path(data, data->cmd->command);
	    if (access(data->cmd->path, F_OK | X_OK) != 0)
	    {
		    ft_putstr_fd("minishell: command not found: ", 2);
		    ft_putendl_fd(data->cmd->args[0], 2);
		    freetab(data->cmd->args);
		    free(data->cmd->path);
		    exit(EXIT_FAILURE);
	    }
    }
	if (execve(data->cmd->path, &data->cmd->command, data->env) == -1)
	{
		ft_putstr_fd("minishell: command not found: ", 2);
		ft_putendl_fd(data->cmd->args[0], 2);
        ft_putendl_fd(strerror(errno), 2); // Affiche l'erreur réelle
		freetab(data->cmd->args);
        
		free(data->cmd->path);
		exit(EXIT_FAILURE);
	}
	return (1);
}

void	get_here_doc_lines(t_data *data)
{
	char	*line;

	close(data->cmd->pipe_fd[0]);
	while (1)
	{
		ft_putstr_fd("> ", 1);
		line = get_next_line(0);
		if (ft_strncmp(line, data->cmd->args[2], ft_strlen(data->cmd->args[2] + 1)) == 0)
		{
			free(line);
			exit(0);
		}
		ft_putstr_fd(line, data->cmd->pipe_fd[1]);
		free(line);
	}
}

void	get_here_doc(t_data *data)
{
	if (pipe(data->cmd->pipe_fd) == -1)
		exit(0);
	data->pid = fork();
	if (data->pid == -1)
		exit(0);
	if (data->pid == 0)
		get_here_doc_lines(data);
	else
	{
		close(data->cmd->pipe_fd[1]);
		dup2(data->cmd->pipe_fd[0], 0);
		wait(NULL);
	}
}

void	do_pipe(t_data *data)
{
	if (pipe(data->cmd->pipe_fd) == -1)
		error_exit(EXIT_FAILURE, strerror(errno));
	data->pid = fork();
	if (data->pid == -1)
		error_exit(EXIT_FAILURE, strerror(errno));
	if (data->pid == 0)
	{
		close(data->cmd->pipe_fd[0]);
		dup2(data->cmd->pipe_fd[1], STDOUT_FILENO);
        close(data->cmd->pipe_fd[1]);
        io_redirections(data);
		execute(data);
	}
	else
	{
		close(data->cmd->pipe_fd[1]);
		dup2(data->cmd->pipe_fd[0], STDIN_FILENO);
        close(data->cmd->pipe_fd[0]);
	}
}

void     io_redirections(t_data *data)
{
	if (data->cmd->io_fds->infile)
	{
		data->cmd->io_fds->fd_in = open(data->cmd->io_fds->infile, O_RDONLY);
		if (data->cmd->io_fds->fd_in < 0)
			error_exit(EXIT_FAILURE, strerror(errno));
		dup2(data->cmd->io_fds->fd_in, STDIN_FILENO);
        close(data->cmd->io_fds->fd_in);
	}

	if (data->cmd->io_fds->outfile)
	{
		data->cmd->io_fds->fd_out = open(data->cmd->io_fds->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (data->cmd->io_fds->fd_out < 0)
			error_exit(EXIT_FAILURE, strerror(errno));
		dup2(data->cmd->io_fds->fd_out, STDOUT_FILENO);
        close(data->cmd->io_fds->fd_out);
	}
}

void run_pipeline(t_data *data)
{
	while (data->cmd)
	{
		if (data->cmd->next)
			do_pipe(data);
		else
		{
			io_redirections(data);
			execute(data);
		}
		data->cmd = data->cmd->next;
	}
}