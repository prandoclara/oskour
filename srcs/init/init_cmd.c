/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claprand <claprand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 13:38:11 by claprand          #+#    #+#             */
/*   Updated: 2024/10/24 16:00:34 by claprand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void init_cmd(t_cmd *cmd, char **av)
{
	(void)av;
    cmd->command = NULL;
    cmd->args = NULL;
    cmd->path = NULL;
    cmd->pipe_output = 0;
    cmd->pipe_fd = malloc(2 * sizeof(int));
	if (cmd->pipe_fd == NULL)
	{
		perror("malloc pipe_fd");
		exit(EXIT_FAILURE);
	}
    cmd->io_fds = malloc(sizeof(t_io_fds));
    if (cmd->io_fds)
        init_io(cmd->io_fds);
    cmd->next = NULL;
    cmd->prev = NULL;
}
