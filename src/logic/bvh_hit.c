/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_hit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nweber <nweber@student.42Heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/28 00:00:00 by anakin            #+#    #+#             */
/*   Updated: 2025/11/12 13:41:02 by nweber           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mini_rt.h"

/**
 * Tests all objects stored in a BVH leaf against the ray and returns the
 * closest hit within the provided range.
 * @param node BVH leaf node containing objects and obj_count
 * @param r ray to cast
 * @param range valid t range (tmin/tmax)
 * @param rec out hit record of the closest hit (if any)
 * @return 1 if any object in the leaf was hit, 0 otherwise
 */
static int	test_leaf_objects(t_bvh_node *node, t_ray *r,
	t_hit_range range, t_hit_record *rec)
{
	t_hit_record	tmp;
	t_hit_range		cur_range;
	int				hit_any;
	int				i;

	cur_range.tmax = range.tmax;
	hit_any = 0;
	i = 0;
	while (i < node->obj_count)
	{
		cur_range.tmin = range.tmin;
		if (hittable_hit(node->objects[i], r, cur_range, &tmp))
		{
			hit_any = 1;
			cur_range.tmax = tmp.t;
			*rec = tmp;
		}
		i++;
	}
	return (hit_any);
}

/**
 * Pushes non-null BVH children onto the traversal stack.
 * Left is pushed last so it is popped (tested) first.
 * @param stk explicit traversal stack
 * @param sp stack pointer (in/out)
 * @param nd current interior node
 */
static void	bvh_push_children(t_bvh_node **stk, int *sp, t_bvh_node *nd)
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
 * Tests a leaf node and updates the range if a closer hit is found.
 * @param node BVH leaf node
 * @param r ray to cast
 * @param range valid t range pointer (tmax updated on hit)
 * @param rec out hit record
 * @return 1 if hit found, 0 otherwise
 */
static int	bvh_update_leaf(t_bvh_node *node, t_ray *r,
	t_hit_range *range, t_hit_record *rec)
{
	if (test_leaf_objects(node, r, *range, rec))
	{
		range->tmax = rec->t;
		return (1);
	}
	return (0);
}

/**
 * Iterative BVH traversal using an explicit stack.
 * Avoids recursion overhead. Tightens tmax after each hit to prune
 * far branches early via AABB rejection.
 * @param node BVH root
 * @param r ray to cast (must have inv_dir precomputed)
 * @param range valid t range (tmin/tmax)
 * @param rec out hit record for the closest hit
 * @return 1 if a hit was found, 0 otherwise
 */
int	bvh_hit(t_bvh_node *node, t_ray *r, t_hit_range range, t_hit_record *rec)
{
	t_bvh_node	*stk[64];
	int			sp;
	int			hit;

	if (!node)
		return (0);
	sp = 0;
	hit = 0;
	stk[sp++] = node;
	while (sp > 0)
	{
		node = stk[--sp];
		if (!aabb_hit(&node->box, r, range.tmin, range.tmax))
			continue ;
		if (node->obj_count > 0)
		{
			hit |= bvh_update_leaf(node, r, &range, rec);
			continue ;
		}
		bvh_push_children(stk, &sp, node);
	}
	return (hit);
}
