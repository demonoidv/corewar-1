/* ************************************************************************** */ /*                                                                            */
/*                                                        :::      ::::::::   */
/*   visu_finish.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ygaude <ygaude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/25 15:21:01 by ygaude            #+#    #+#             */
/*   Updated: 2018/03/25 19:53:59 by ygaude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <SDL.h>
#include <stdlib.h>
#include "corewar.h"
#include "visu.h"

void		setcolor(SDL_Renderer *render, SDL_Color color, unsigned char alpha)
{
	SDL_SetRenderDrawColor(render, color.r, color.g, color.b, alpha);
}

void		end_blur(t_winenv *env, int squares[45][80], int color)
{
	SDL_Rect	rect;
	int			i;
	int			j;

	rect = (SDL_Rect){0, 0, 32, 32};
	rect.x = rand() % 80 * 32;
	rect.y = rand() % 45 * 32;
	i = 0;
	while (i < 45 * 80)
	{
		j = 0;
		while ((i < 45 * 80) && j < 100)
		{
			while (squares[rect.y / 32][rect.x / 32])
			{
				if ((env->quit |= SDL_QuitRequested()))
					return ;
				rect.x = rand() % 80 * 32;
				rect.y = rand() % 45 * 32;
			}
			SDL_RenderClear(env->render);
			if (color)
				setcolor(env->render, env->palette[color], 50 + (i / 100) * 2);
			else
				setcolor(env->render, (SDL_Color){200, 200, 200, 0}, 50 + (i / 100) * 2);
			SDL_SetRenderTarget(env->render, env->wintex);
			SDL_RenderFillRect(env->render, &rect);
			SDL_SetRenderTarget(env->render, NULL);
			SDL_RenderCopy(env->render, env->wintex, NULL, NULL);
			squares[rect.y / 32][rect.x / 32] = 1;
			i++;
			SDL_RenderPresent(env->render);
		}
	}
	SDL_RenderPresent(env->render);
}

SDL_Texture	*getendsplash(t_winenv *env, t_header *winner)
{
	SDL_Rect		rect;
	SDL_Texture		*splash;
	SDL_Texture		*name;
	SDL_Texture		*tmp;

	if (winner)
	{
		rect = (SDL_Rect){0, 0, env->dispmode.w / 2, env->dispmode.h / 4};
		splash = getnewtex(env, TEXTARGET, env->dispmode.w / 2, env->dispmode.h / 2);
		SDL_SetRenderTarget(env->render, splash);
		name = strtotex(winner->prog_name, env, env->palette[winner->index_color + 1], 1);
		SDL_QueryTexture(name, NULL, NULL, &rect.w, NULL);
		rect.x = (env->dispmode.w / 2 - rect.w) / 2;
		SDL_RenderCopy(env->render, name, NULL, &rect);
		tmp = strtotex("WINS", env, env->palette[winner->index_color + 1], 1);
		rect.y = rect.h;
		SDL_QueryTexture(tmp, NULL, NULL, &rect.w, NULL);
		rect.x = (env->dispmode.w / 2 - rect.w) / 2;
		SDL_RenderCopy(env->render, tmp, NULL, &rect);
		SDL_DestroyTexture(name);
		SDL_DestroyTexture(tmp);
	}
	else
		splash = strtotex("No one wins", env, (SDL_Color){255, 255, 255, 255}, 1);
	return (splash);
}

void		putsplash(t_winenv *env, SDL_Texture *splash, SDL_Texture *tex, SDL_Rect rect)
{
	SDL_SetTextureBlendMode(splash, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(env->render, tex);
	setcolor(env->render, (SDL_Color){0, 0, 0, 0}, 0);
	SDL_RenderClear(env->render);
	SDL_RenderCopy(env->render, splash, NULL, &rect);
	SDL_SetRenderTarget(env->render, NULL);
	SDL_RenderCopy(env->render, env->wintex, NULL, NULL);
	SDL_RenderCopy(env->render, tex, NULL, NULL);
	SDL_RenderPresent(env->render);
}

void		end_splash(t_winenv *env, SDL_Texture *tex, t_header *winner)
{
	SDL_Texture	*splash;
	SDL_Rect	rect;
	int			i;

	splash = getendsplash(env, winner);
	i = 0;
	while (i < env->dispmode.w * 19 / 20)
	{
		rect.w = i;
		rect.h = env->dispmode.h * i / env->dispmode.w / 2;
		rect.x = env->dispmode.w / 2 - i / 2;
		rect.y = env->dispmode.w / 32 - rect.h / 2 + env->dispmode.h / 4;
		putsplash(env, splash, tex, rect);
		i += 5;
	}
}

void		visu_finish(t_header *winner)
{
	int				squares[45][80];
	t_winenv		*env;
	SDL_Texture		*tex;

	env = getsdlenv(NULL);
	sranddev();
	tex = getnewtex(env, TEXTARGET, env->dispmode.w, env->dispmode.h);
	SDL_SetRenderDrawBlendMode(env->render, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
	ft_bzero(squares, sizeof(squares));
	env->ticks = SDL_GetTicks();
	SDL_RenderPresent(env->render);
	end_blur(env, squares, (winner) ? winner->index_color + 1 : 0);
	end_splash(env, tex, winner);
	while (!(env->quit |= SDL_QuitRequested()))
		;
}
