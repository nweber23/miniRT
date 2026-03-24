/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   random_vec.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nweber <nweber@student.42Heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 12:00:52 by apregitz          #+#    #+#             */
/*   Updated: 2025/11/12 16:27:34 by nweber           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mini_rt.h"

/**
 * Generate a vector with each component uniformly sampled in [min, max).
 * @param min lower bound (inclusive)
 * @param max upper bound (exclusive)
 * @return t_vec3 with random components
 */
t_vec3	random_vec3(double min, double max)
{
	t_vec3	vec;

	vec.x = random_double_range(min, max);
	vec.y = random_double_range(min, max);
	vec.z = random_double_range(min, max);
	return (vec);
}

/**
 * Fill an existing vector with uniformly distributed components in [min, max).
 * @param vec pointer to vector to write
 * @param min lower bound (inclusive)
 * @param max upper bound (exclusive)
 */
void	randomize_vec3(t_vec3 *vec, double min, double max)
{
	vec->x = random_double_range(min, max);
	vec->y = random_double_range(min, max);
	vec->z = random_double_range(min, max);
}

/**
 * Return a uniformly distributed random unit-length vector.
 * Samples vectors in the unit cube until one falls inside the unit sphere,
 * then normalizes it to length 1. Protects against degenerate (near-zero)
 * vectors using a tiny epsilon.
 * @return unit-length random vector
 */
t_vec3	random_unit_vec3(void)
{
	t_vec3	vec;
	double	lensq;

	while (true)
	{
		randomize_vec3(&vec, -1.0, 1.0);
		lensq = vec3_dot(vec, vec);
		if (lensq > 1e-160 && lensq <= 1)
			return (vec3_divide(vec, sqrt(lensq)));
	}
}

/**
 * Sample a cosine-weighted random direction on the hemisphere around normal.
 * Adding normal to a random unit sphere vector produces a cosine distribution
 * which converges faster for diffuse surfaces than uniform sampling.
 * @param normal pointer to hemisphere center normal
 * @return random unit vector cosine-weighted toward normal
 */
t_vec3	random_on_hemisphere(t_vec3 *normal)
{
	t_vec3	dir;

	dir = vec3_add(*normal, random_unit_vec3());
	if (vec3_dot(dir, dir) < 1e-160)
		return (*normal);
	return (vec3_normalize(dir));
}
