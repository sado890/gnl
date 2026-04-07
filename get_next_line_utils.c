/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: muarici <muarici@student.42kocaeli.com.tr> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 11:27:34 by muarici           #+#    #+#             */
/*   Updated: 2026/04/07 13:23:38 by muarici          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <stdlib.h>

size_t	ft_strlen(char *s)
{
	size_t	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i])
		i++;
	return (i);
}

char	*ft_strjoin(char *s1, char *s2, size_t i)
{
	char	*result;
	size_t	j;

	if (!s2)
		return (s1);
	result = malloc(ft_strlen(s1) + ft_strlen(s2) + 1);
	if (!result)
	{
		if (s1)
			free(s1);
		return (NULL);
	}
	i = 0;
	while (s1 && s1[i])
	{
		result[i] = s1[i];
		i++;
	}
	j = 0;
	while (s2[j])
		result[i++] = s2[j++];
	result[i] = '\0';
	if (s1)
		free(s1);
	return (result);
}

void	ft_clear_storage(int x, char *storage)
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

char	*ft_get_storage(int i, char *storage)
{
	char	*returnval;
	int		j;
	int		len;

	if (storage[0] == '\0')
		return (NULL);
	if (i == -1)
		len = ft_strlen(storage);
	else
		len = i + 1;
	returnval = malloc(sizeof(char) * (len + 1));
	if (!returnval)
		return (NULL);
	j = 0;
	while (j < len)
	{
		returnval[j] = storage[j];
		j++;
	}
	returnval[j] = '\0';
	if (i == -1)
		storage[0] = '\0';
	else
		ft_clear_storage(len, storage);
	return (returnval);
}
