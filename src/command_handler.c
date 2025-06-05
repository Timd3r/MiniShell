/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_handler.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 17:59:28 by tde-raev          #+#    #+#             */
/*   Updated: 2025/06/05 17:59:40 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Creates a new empty command structure
 * @return Pointer to newly allocated command structure or NULL on failure
 */
t_command *new_command(void)
{
    t_command *cmd;

    cmd = (t_command *)malloc(sizeof(t_command));
    if (!cmd)
        return (NULL);
    cmd->name = NULL;
    cmd->args = NULL;
    cmd->redirections = NULL;
    cmd->input_pipe = 0;
    cmd->output_pipe = 0;
    cmd->next = NULL;
    return (cmd);
}

/**
 * Creates a new redirection structure
 * @param type Type of redirection (IN, OUT, APPEND, HEREDOC)
 * @param file Filename or delimiter string
 * @return Pointer to newly allocated redirection structure or NULL on failure
 */
t_redir *new_redirection(t_token_type type, char *file)
{
    t_redir *redir;

    if (!file)
        return (NULL);
    redir = (t_redir *)malloc(sizeof(t_redir));
    if (!redir)
        return (NULL);
    redir->type = type;
    redir->file = ft_strdup(file);
    if (!redir->file)
    {
        free(redir);
        return (NULL);
    }
    return (redir);
}

/**
 * Adds a redirection to a command
 * @param cmd Command to add redirection to
 * @param redir Redirection to add
 * @return 1 on success, 0 on failure
 */
int add_redirection(t_command *cmd, t_redir *redir)
{
    t_redir **new_redirections;
    int count;

    if (!cmd || !redir)
        return (0);
    
    count = 0;
    if (cmd->redirections)
    {
        while (cmd->redirections[count])
            count++;
    }
    
    new_redirections = (t_redir **)malloc(sizeof(t_redir *) * (count + 2));
    if (!new_redirections)
        return (0);
    
    for (int i = 0; i < count; i++)
        new_redirections[i] = cmd->redirections[i];
    
    new_redirections[count] = redir;
    new_redirections[count + 1] = NULL;
    
    if (cmd->redirections)
        free(cmd->redirections);
    
    cmd->redirections = new_redirections;
    return (1);
}

/**
 * Adds an argument to a command
 * @param cmd Command to add argument to
 * @param arg Argument to add
 * @return 1 on success, 0 on failure
 */
int add_argument(t_command *cmd, char *arg)
{
    char **new_args;
    int count;

    if (!cmd || !arg)
        return (0);
    
    if (!cmd->name)
    {
        cmd->name = ft_strdup(arg);
        return (cmd->name != NULL);
    }
    count = 0;
    if (cmd->args)
    {
        while (cmd->args[count])
            count++;
    }
    
    new_args = (char **)malloc(sizeof(char *) * (count + 2));
    if (!new_args)
        return (0);
    
    for (int i = 0; i < count; i++)
        new_args[i] = cmd->args[i];
    
    new_args[count] = ft_strdup(arg);
    if (!new_args[count])
    {
        free(new_args);
        return (0);
    }
    
    new_args[count + 1] = NULL;
    
    if (cmd->args)
        free(cmd->args);
    
    cmd->args = new_args;
    return (1);
}

/**
 * Frees a single command structure
 * @param cmd Command to free
 */
void free_command(t_command *cmd)
{
    if (!cmd)
        return;
    
    if (cmd->name)
        free(cmd->name);
    
    if (cmd->args)
    {
        for (int i = 0; cmd->args[i]; i++)
            free(cmd->args[i]);
        free(cmd->args);
    }
    
    if (cmd->redirections)
    {
        for (int i = 0; cmd->redirections[i]; i++)
        {
            if (cmd->redirections[i]->file)
                free(cmd->redirections[i]->file);
            free(cmd->redirections[i]);
        }
        free(cmd->redirections);
    }
    
    free(cmd);
}

/**
 * Frees a linked list of command structures
 * @param cmd First command in the list
 */
void free_commands(t_command *cmd)
{
    t_command *next;
    
    while (cmd)
    {
        next = cmd->next;
        free_command(cmd);
        cmd = next;
    }
}

/**
 * Builds commands from tokenized input
 * @param tokens Array of tokens
 * @return Pointer to first command in linked list or NULL on failure
 */
t_command *build_commands(t_token **tokens)
{
    t_command *first_cmd = NULL;
    t_command *current_cmd = NULL;
    t_command *new_cmd = NULL;
    int i;
    
    if (!tokens)
        return (NULL);
    
    i = 0;
    while (tokens[i])
    {
        if (tokens[i]->type == PIPE)
        {
            // Set output pipe flag for current command
            if (current_cmd)
                current_cmd->output_pipe = 1;
            
            // Create a new command
            new_cmd = new_command();
            if (!new_cmd)
            {
                free_commands(first_cmd);
                return (NULL);
            }
            
            // Set input pipe flag for new command
            new_cmd->input_pipe = 1;
            
            // Add to command list
            if (!first_cmd)
                first_cmd = new_cmd;
            else
                current_cmd->next = new_cmd;
            
            current_cmd = new_cmd;
        }
        else if (tokens[i]->type == IN || tokens[i]->type == OUT || 
                tokens[i]->type == APPEND || tokens[i]->type == HEREDOC)
        {
            // Ensure we have a command to add redirection to
            if (!current_cmd)
            {
                current_cmd = new_command();
                if (!current_cmd)
                {
                    free_commands(first_cmd);
                    return (NULL);
                }
                if (!first_cmd)
                    first_cmd = current_cmd;
            }
            
            // Make sure there's a next token for the filename
            if (tokens[i + 1] && tokens[i + 1]->type == WORD)
            {
                t_redir *redir = new_redirection(tokens[i]->type, tokens[i + 1]->c);
                if (!redir || !add_redirection(current_cmd, redir))
                {
                    if (redir)
                        free(redir);
                    free_commands(first_cmd);
                    return (NULL);
                }
                i++; // Skip the filename token
            }
        }
        else if (tokens[i]->type == WORD)
        {
            // Ensure we have a command to add the word to
            if (!current_cmd)
            {
                current_cmd = new_command();
                if (!current_cmd)
                {
                    free_commands(first_cmd);
                    return (NULL);
                }
                if (!first_cmd)
                    first_cmd = current_cmd;
            }
            
            // Add the word as an argument
            if (!add_argument(current_cmd, tokens[i]->c))
            {
                free_commands(first_cmd);
                return (NULL);
            }
        }
        
        i++;
    }
    
    return (first_cmd);
}

/**
 * Debug function to print all commands
 * @param commands First command in the list
 */
void print_commands(t_command *commands)
{
    t_command *cmd = commands;
    int cmd_index = 0;
    
    while (cmd)
    {
        printf("Command %d:\n", cmd_index++);
        printf("  Name: %s\n", cmd->name ? cmd->name : "(null)");
        printf("  Args: ");
        if (cmd->args)
        {
            for (int i = 0; cmd->args[i]; i++)
                printf("%s ", cmd->args[i]);
        }
        printf("\n");
        
        printf("  Redirections:\n");
        if (cmd->redirections)
        {
            for (int i = 0; cmd->redirections[i]; i++)
            {
                printf("    Type: ");
                switch (cmd->redirections[i]->type)
                {
                    case IN: printf("IN"); break;
                    case OUT: printf("OUT"); break;
                    case APPEND: printf("APPEND"); break;
                    case HEREDOC: printf("HEREDOC"); break;
                    default: printf("UNKNOWN"); break;
                }
                printf(", File: %s\n", cmd->redirections[i]->file);
            }
        }
        
        printf("  Input from pipe: %d\n", cmd->input_pipe);
        printf("  Output to pipe: %d\n", cmd->output_pipe);
        
        cmd = cmd->next;
        if (cmd)
            printf("\n");
    }
}


