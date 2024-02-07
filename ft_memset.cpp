/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ft_memset.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaktiwy <hsaktiwy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 11:14:16 by hsaktiwy          #+#    #+#             */
/*   Updated: 2024/02/07 11:19:15 by hsaktiwy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.server.hpp"

void	*ft_memset(void *b, int c, size_t len)
{
	unsigned char	x;
	size_t			i;
	char			*str;

	x = (unsigned char)c;
	i = 0;
	str = (char *)b;
	while (i < len)
	{
		str[i] = x;
		i++;
	}
	return (b);
}