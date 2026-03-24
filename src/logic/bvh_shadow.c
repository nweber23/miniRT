/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_shadow.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nweber <nweber@student.42Heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 16:00:00 by nweber            #+#    #+#             */
/*   Updated: 2025/11/15 16:00:00 by nweber           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mini_rt.h"

/**
 * Tests leaf objects for any intersection (shadow early exit).
 * Returns immediately on first hit instead of finding closest.
 * @param node BVH leaf node
 * @param r ray to cast
 * @param range valid t range
 * @return 1 if any hit, 0 otherwise
 */
static int	test_leaf_any(t_bvh_node *node, t_ray *r, t_hit_range range)
{
	t_hit_record	tmp;
	int				i;

	i = 0;
	while (i < node->obj_count)
	{
		if (hittable_hit(node->objects[i], r, range, &tmp))
			return (1);
		i++;
	}
	return (0);
}

/**
 * Pushes non-null BVH children onto the shadow traversal stack.
 * @param stk explicit traversal stack
 * @param sp stack pointer (in/out)
 * @param nd current interior node
 */
static void	shadow_push_children(t_bvh_node **stk, int *sp, t_bvh_node *nd)
{
	if (nd->right)
	{
		stk[*sp] = nd->right;
		*sp += 1;
	}
	if (nd->left)
	{
		stk[*sp] = nd->left;
		*sp += 1;
	}
}

/**
 * Iterative any-hit BVH traversal for shadow rays.
 * Returns 1 on first intersection found without finding the closest.
 * Uses explicit stack to avoid recursion overhead.
 * @param node BVH root
 * @param r ray to cast (must have inv_dir precomputed)
 * @param range valid t range
 * @return 1 if any hit, 0 otherwise
 */
int	bvh_hit_any(t_bvh_node *node, t_ray *r, t_hit_range range)
{
	t_bvh_node	*stk[64];
	int			sp;

	if (!node)
		return (0);
	sp = 0;
	stk[sp++] = node;
	while (sp > 0)
	{
		node = stk[--sp];
		if (!aabb_hit(&node->box, r, range.tmin, range.tmax))
			continue ;
		if (node->obj_count > 0)
		{
			if (test_leaf_any(node, r, range))
				return (1);
			continue ;
		}
		shadow_push_children(stk, &sp, node);
	}
	return (0);
}

/**
 * Checks planes for any intersection (shadow early exit).
 * Returns 1 on first plane hit instead of finding closest.
 * @param objects object list to scan for planes
 * @param r ray to cast
 * @param range valid t range
 * @return 1 if any plane hit, 0 otherwise
 */
static int	check_plane_any(t_obj_list *objects, t_ray *r, t_hit_range range)
{
	t_hit_record	tmp;
	t_obj			*cur;

	cur = objects->head;
	while (cur)
	{
		if (cur->type == PLANE && hittable_hit(cur, r, range, &tmp))
			return (1);
		cur = cur->next;
	}
	return (0);
}

/**
 * Any-hit world test using BVH. Optimized for shadow occlusion queries.
 * Tests BVH first, then planes. Returns on first hit found.
 * @param ctx BVH query context (root, objects, ray, range)
 * @return 1 if any hit found, 0 otherwise
 */
int	world_hit_any_bvh(t_bvh_ctx *ctx)
{
	if (ctx->bvh && bvh_hit_any(ctx->bvh, ctx->r, ctx->range))
		return (1);
	if (ctx->objects)
		return (check_plane_any(ctx->objects, ctx->r, ctx->range));
	return (0);
}
