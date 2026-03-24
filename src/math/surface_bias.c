/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   surface_bias.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nweber <nweber@student.42Heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 00:00:00 by anakin            #+#    #+#             */
/*   Updated: 2025/11/12 16:24:26 by nweber           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mini_rt.h"

/**
 * Apply a small offset to a surface point along its normal to avoid
 * self-intersection (shadow acne) when spawning new rays.
 * The bias magnitude is a small constant epsilon. The offset direction is
 * chosen based on the sign of the dot product between the outgoing ray
 * direction and the surface normal: if the ray points in the same general
 * hemisphere as the normal the bias is applied along +normal, otherwise
 * along -normal.
 * @param point     original surface point (world-space)
 * @param direction direction of the spawned ray (not required to be unit)
 * @param normal    surface normal at the point (should be unit length)
 * @return new point moved by a small bias along the normal
 */
/**
 * Precomputes the reciprocal of each ray direction component for fast
 * AABB slab tests. Must be called before any BVH traversal with this ray.
 * @param r ray whose inv_dir field will be filled
 */
__attribute__((always_inline)) inline void	ray_compute_inv(t_ray *r)
{
	r->inv_dir.x = 1.0 / r->direction.x;
	r->inv_dir.y = 1.0 / r->direction.y;
	r->inv_dir.z = 1.0 / r->direction.z;
}

__attribute__((always_inline)) inline t_vec3	apply_surface_bias(t_vec3 point,
	t_vec3 direction, t_vec3 normal)
{
	double	eps;
	double	sign;
	t_vec3	bias;

	eps = 1e-4;
	if (vec3_dot(direction, normal) > 0.0)
		sign = 1.0;
	else
		sign = -1.0;
	bias = vec3_multiply(normal, eps * sign);
	return (vec3_add(point, bias));
}
