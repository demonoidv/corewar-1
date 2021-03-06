/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsporer <vsporer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/17 18:29:25 by vsporer           #+#    #+#             */
/*   Updated: 2018/03/25 15:10:45 by vsporer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vm.h"

static t_process	*recycle_process(t_vm *env)
{
	t_process	*new;

	if (env->killed_process)
	{
		new = env->killed_process;
		env->killed_process = env->killed_process->next;
		ft_bzero((void*)new, sizeof(t_process));
	}
	else if (!(new = (t_process*)malloc(sizeof(t_process))))
		ft_exit(strerror(errno));
	return (new);
}

t_process			*new_process(t_process *src, unsigned int new_pc, t_vm *env)
{
	int				i;
	t_process		*new;
	static long		id = 0;

	i = -1;
	new = recycle_process(env);
	if (src)
	{
		ft_memcpy((void*)new, (void*)src, sizeof(t_process));
		new->pc = new_pc % MEM_SIZE;
	}
	else
	{
		while (++i < REG_NUMBER)
			new->reg[i] = 0;
		new->pc = new_pc % MEM_SIZE;
		new->carry = 0;
		new->cooldown = 0;
		new->last_live = 0;
		new->next = NULL;
	}
	new->id = ++id;
	return (new);
}

void				add_process(t_process **lst, t_process *new)
{
	if (*lst)
	{
		new->next = *lst;
		*lst = new;
	}
	else
	{
		new->next = NULL;
		*lst = new;
	}
}

void				store_process(t_process **lst, t_process *todel, \
t_process **st, t_process *prev)
{
	if (todel)
	{
		if (*lst == todel)
			*lst = todel->next;
		else
			prev->next = todel->next;
		add_process(st, todel);
	}
}
