/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_get_next_line.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: muarici <muarici@student.42kocaeli.com.tr> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 19:12:31 by muarici           #+#    #+#             */
/*   Updated: 2026/03/21 by muarici                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 42
#endif

static char	storage[BUFFER_SIZE + 1];

static int	ft_strlen(char *s)
{
	int	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i])
		i++;
	return (i);
}

static char	*ft_strjoin(char *s1, char *s2)
{
	char	*result;
	int		len1;
	int		len2;
	int		i;

	if (!s2)
		return (s1);
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	result = malloc(sizeof(char) * (len1 + len2 + 1));
	if (!result)
	{
		free(s1);
		return (NULL);
	}
	i = 0;
	while (i < len1)
	{
		result[i] = s1[i];
		i++;
	}
	i = 0;
	while (i < len2)
	{
		result[len1 + i] = s2[i];
		i++;
	}
	result[len1 + len2] = '\0';
	free(s1);
	return (result);
}

static int	check_storage(void)
{
	int	i;

	i = 0;
	while (storage[i])
	{
		if (storage[i] == '\n')
			return (i);
		i++;
	}
	return (-1);
}

static void	clear_storage(int x)
{
	int	i;

	i = 0;
	while (storage[x] != '\0')
	{
		storage[i] = storage[x];
		i++;
		x++;
	}
	storage[i] = '\0';
}

static char	*ft_get_storage(int i)
{
	char	*returnval;
	int		j;

	j = 0;
	if (storage[0] == '\0')
		return (NULL);
	if (i == -1)
	{
		j = ft_strlen(storage);
		returnval = malloc(sizeof(char) * (j + 1));
		if (!returnval)
			return (NULL);
		j = 0;
		while (storage[j])
		{
			returnval[j] = storage[j];
			j++;
		}
		returnval[j] = '\0';
		storage[0] = '\0';
	}
	else
	{
		returnval = malloc(sizeof(char) * (i + 2));
		if (!returnval)
			return (NULL);
		while (j <= i)
		{
			returnval[j] = storage[j];
			j++;
		}
		returnval[j] = '\0';
		clear_storage(j);
	}
	return (returnval);
}

char	*get_next_line(int fd)
{
	int		bytes_read;
	int		newline_pos;
	char	*chunk;
	char	*result;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	result = NULL;
	while (1)
	{
		newline_pos = check_storage();
		if (newline_pos >= 0)
		{
			chunk = ft_get_storage(newline_pos);
			result = ft_strjoin(result, chunk);
			free(chunk);
			return (result);
		}
		chunk = ft_get_storage(-1);
		if (chunk)
		{
			result = ft_strjoin(result, chunk);
			free(chunk);
		}
		bytes_read = read(fd, storage, BUFFER_SIZE);
		if (bytes_read > 0)
			storage[bytes_read] = '\0';
		else
			break ;
	}
	return (result);
}

int main()
{
	int fd = open("sado.txt", O_RDWR | O_CREAT, 0777);
	printf(get_next_line(fd));
	printf(get_next_line(fd));
	printf(get_next_line(fd));
	printf(get_next_line(fd));
	printf(get_next_line(fd));
}
