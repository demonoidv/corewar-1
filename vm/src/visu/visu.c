/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   visu.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ygaude <ygaude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/13 22:22:52 by ygaude            #+#    #+#             */
/*   Updated: 2018/03/01 16:36:23 by ygaude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>

#include "libft.h"

#include "corewar.h"
#include "visu.h"

void				panic(const char *str, const char *str2)
{
	ft_dprintf(2, "%s: %s\n", str, str2);
	exit(-1);
}

t_winenv			*getsdlenv(t_vm *vm)
{
	static t_winenv	*winenv = NULL;

	if (!winenv && vm)
		if ((winenv = (t_winenv *)ft_memalloc(sizeof(t_winenv))))
		{
			winenv->vm = vm;
			winenv->quit = 0;
		}
	return (winenv);
}

////////////////////////////////////////////////////////////////////////////////
void				initsdl(t_winenv *env)
{
	if (!env || SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
		panic("Error while initializing SDL", SDL_GetError());
	if (SDL_GetDesktopDisplayMode(0, &(env->dispmode)))
		panic("SDL_GetDesktopDisplayMode failed", SDL_GetError());
	env->win = SDL_CreateWindow("Corewar",
				SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
				env->dispmode.w, env->dispmode.h, SDL_WINDOW_FULLSCREEN);
	if (!env->win)
		panic("Error while creating window", SDL_GetError());
	env->render = SDL_CreateRenderer(env->win, -1, SDL_RENDERER_ACCELERATED);
	if (!env->render)
		panic("Error while creating renderer", SDL_GetError());
	if (TTF_Init() == -1 || !(env->font = TTF_OpenFont("roboto.ttf", 15)))
		panic("Error while initializing SDL_TTF", TTF_GetError());
	SDL_SetRenderDrawColor(env->render, 9, 11, 16, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(env->render);
}

SDL_Texture			*getnewtex(t_winenv *env, int access, int w, int h)
{
	SDL_Texture		*tex;

	tex = SDL_CreateTexture(env->render, env->dispmode.format, access, w, h);
	if (!tex)
		panic("Failed creating texture", SDL_GetError());
	return (tex);
}

void				cleartex(SDL_Renderer *render, SDL_Texture *tex)
{
	SDL_SetRenderTarget(render, tex);
	SDL_RenderClear(render);
}

SDL_Texture			*strtotex(char *str, t_winenv *env, SDL_Color color)
{
	SDL_Texture		*tex;
	SDL_Surface		*surf;

	surf = TTF_RenderText_Blended(env->font, str, color);
	if (!surf)
		panic("Failed creating text surface", SDL_GetError());
	tex = SDL_CreateTextureFromSurface(env->render, surf);
	if (!tex)
		panic("Failed converting surface to texture", SDL_GetError());
	SDL_FreeSurface(surf);
	return (tex);
}

SDL_Texture			*memtotex(t_winenv *env, int line, int col)
{
	const char		*digits;
	char			str[3];

	digits = "0123456789abcdef";
	str[0] = digits[env->vm->memory[line * 64 + col] / 16];
	str[1] = digits[env->vm->memory[line * 64 + col] % 16];
	str[2] = '\0';
	return (strtotex(str, env, (SDL_Color){150, 150, 150, 255}));
}

void				dispmemline(t_winenv *env, int line)
{
	SDL_Texture		*tex;
	SDL_Rect		dst;
	int				i;
	int				wunit;

	i = 0;
	SDL_QueryTexture(env->memtex, NULL, NULL, &wunit, NULL);
	dst.y = env->dispmode.h * line / 64;
	dst.h = env->dispmode.h / 64;
	while (i < 64)
	{
		dst.x = wunit * i / 64;
		tex = memtotex(env, line, i);
		SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
		SDL_RenderCopy(env->render, tex, NULL, &dst);
		SDL_DestroyTexture(tex);
		i++;
	}
}

void				dispproc(t_winenv *env)
{
	SDL_Rect	rect;
	SDL_Rect	memrect;
	t_process	*cur;

	SDL_QueryTexture(env->memtex, NULL, NULL, &memrect.w, &memrect.h);
	rect.w = memrect.w / 64;
	rect.h = memrect.h / 64;
	cur = env->vm->process;
	while (cur)
	{
		rect.x = (cur->pc % 64) * memrect.w / 64;
		rect.y = cur->pc / 64 * memrect.h / 64;
		if (cur->champ == env->vm->champion)
			SDL_SetRenderDrawColor(env->render, 50, 100, 50, SDL_ALPHA_OPAQUE);
		else
			SDL_SetRenderDrawColor(env->render, 50, 50, 100, SDL_ALPHA_OPAQUE);
		SDL_RenderFillRect(env->render, &rect);
		cur = cur->next;
	}
}

void				memdisp(t_winenv *env)
{
	int				i;

	i = 0;
	SDL_SetRenderDrawColor(env->render, 10, 10, 10, SDL_ALPHA_OPAQUE);
	cleartex(env->render, env->memtex);
	dispproc(env);
	while (i < MEM_SIZE / 64)
	{
		dispmemline(env, i);
		i++;
	}
}

void				visu_update(t_winenv *env)
{
	SDL_Rect		rect;

	SDL_SetRenderTarget(env->render, env->wintex);
	rect = (SDL_Rect){0,0,0,0};
	rect.h = env->dispmode.h;
	rect.w = env->dispmode.w * 4 / 5;
	SDL_RenderCopy(env->render, env->memtex, NULL, &rect);
	rect.x = rect.w;
	rect.w =  env->dispmode.w * 1 / 5;
	SDL_RenderCopy(env->render, env->hudtex, NULL, &rect);
	SDL_SetRenderTarget(env->render, NULL);
	SDL_RenderCopy(env->render, env->wintex, NULL, NULL);
}

void				events(t_winenv *env)
{
	(void)env;
}
////////////////////////////////////////////////////////////////////////////////

void				visu_init(t_vm *vm)
{
	t_winenv		*env;
	SDL_DisplayMode	dm;

	env = getsdlenv(vm);
	initsdl(env);
	SDL_GetDesktopDisplayMode(0, &env->dispmode);
	dm = env->dispmode;
	env->wintex = getnewtex(env, TEXTARGET, dm.w, dm.h);
	env->memtex = getnewtex(env, TEXTARGET, dm.w * 4 / 5, dm.h);
	env->hudtex = getnewtex(env, TEXTARGET, dm.w * 1 / 5, dm.h);
	SDL_SetRenderDrawColor(env->render, 100, 50, 50, SDL_ALPHA_OPAQUE);
	cleartex(env->render, env->wintex);
	SDL_SetRenderDrawColor(env->render, 50, 100, 50, SDL_ALPHA_OPAQUE);
	cleartex(env->render, env->memtex);
	SDL_SetRenderDrawColor(env->render, 50, 50, 100, SDL_ALPHA_OPAQUE);
	cleartex(env->render, env->hudtex);
	visu_update(env);
}

int					quitvisu(t_winenv *env)
{
	if (!env || env->quit || SDL_QuitRequested())
	{
		SDL_Quit();
		return (0);
	}
	return (1);
}

int					visu(void)
{
	t_winenv		*env;

	env = getsdlenv(NULL);
	env->ticks = SDL_GetTicks();
	memdisp(env);
	SDL_RenderPresent(env->render);
	while (!(env->quit |= SDL_QuitRequested()) && SDL_GetTicks() < env->ticks)
	{}
		events(env);
		visu_update(env);
		SDL_RenderPresent(env->render);
	//}
	return (quitvisu(env));
}
