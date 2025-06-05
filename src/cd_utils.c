/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 17:02:59 by tde-raev          #+#    #+#             */
/*   Updated: 2025/06/05 17:03:01 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void    execute_cd(t_token **tokens)
{
    //char *path;

    if (!tokens || !tokens[0] || !tokens[0]->c)
    {
        fprintf(stderr, "minishell: cd: missing argument\n");
        return;
    }
}