/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_tree.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcollett <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/22 14:46:34 by gcollett          #+#    #+#             */
/*   Updated: 2018/03/22 14:47:35 by gcollett         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <libft.h>
#include <asm.h>

void print_token(int index, int deepness)
{
  if (deepness > 0)
    ft_printf("%*Type = ", deepness);
  if (index == NONE)
   ft_printf("NONE\n");
  else if (index == PROG)
   ft_printf("PROG\n");
  else if (index == HEADER)
   ft_printf("HEADER\n");
  else if (index == CODE)
   ft_printf("CODE\n");
  else if (index == LINE)
   ft_printf("LINE\n");
  else if (index == INST)
   ft_printf("INST\n");
  else if (index == PARAM)
   ft_printf("PARAM\n");
  else if (index == CMD_NAME)
   ft_printf("CMD_NAME\n");
  else if (index == CMD_CMT)
   ft_printf("CMD_CMT\n");
  else if (index == LABEL)
   ft_printf("LABEL\n");
  else if (index == FCT)
   ft_printf("FCT\n");
  else if (index == SEP)
   ft_printf("SEP\n");
  else if (index == REG)
   ft_printf("REG\n");
  else if (index == DIRECT)
   ft_printf("DIRECT\n");
  else if (index == IND)
   ft_printf("IND\n");
  else if (index == ENDCODE)
   ft_printf("ENCODE\n");
  else
    ft_printf("ERROR\n");
}


static void recursive_tree(t_node *elem, int deepness)
{
  t_child *child;

  child = elem->children;
  print_token(elem->type, deepness);
  ft_printf("%*Name = |%s| %p\n", deepness, elem->name, elem);
  if (child)
    recursive_tree(child->elem, deepness + 4);
  while (child && child->next)
  {
      recursive_tree(child->next->elem, deepness + 4);
      child = child->next;
  }
}

void	print_tree(t_node *tree)
{
  t_node *tmp_tree;

  tmp_tree = tree;
  recursive_tree(tmp_tree, 0);
}
