/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: muarici <muarici@student.42kocaeli.com.tr> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 11:05:33 by muarici           #+#    #+#             */
/*   Updated: 2026/04/07 13:19:33 by muarici          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <stdlib.h>
#include <fcntl.h>

static int	ft_check_newline(char *s)
{
	int	i;

	if (!s)
		return (-1);
	i = 0;
	while (s[i])
	{
		if (s[i] == '\n')
			return (i);
		i++;
	}
	return (-1);
}

static char	*ft_gnl_helper(char *storage, char *result)
{
	int		newline_pos;
	char	*chunk;

	if (!storage || storage[0] == '\0')
		return (result);
	newline_pos = ft_check_newline(storage);
	if (newline_pos >= 0)
		chunk = ft_get_storage(newline_pos, storage);
	else
		chunk = ft_get_storage(-1, storage);
	if (!chunk)
		return (result);
	result = ft_strjoin(result, chunk, 0);
	free(chunk);
	return (result);
}

char	*get_next_line(int fd)
{
	int			bytes_read;
	static char	storage[BUFFER_SIZE + 1];
	char		*result;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	result = NULL;
	result = ft_gnl_helper(storage, result);
	while (ft_check_newline(result) == -1)
	{
		bytes_read = read(fd, storage, BUFFER_SIZE);
		if (bytes_read < 0)
		{
			if (result)
				free(result);
			storage[0] = '\0';
			return (NULL);
		}
		if (bytes_read == 0)
			break ;
		storage[bytes_read] = '\0';
		result = ft_gnl_helper(storage, result);
	}
	return (result);
}
