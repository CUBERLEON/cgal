#ifndef CGAL_SURFACE_MESH_SEGMENTATION_APPROX_DECOMPOSITION_H

// Copyright (c) 2018  GeometryFactory Sarl (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
// SPDX-License-Identifier: GPL-3.0+
//
// Author(s)     : Liubomyr Piadyk

#define CGAL_SURFACE_MESH_SEGMENTATION_APPROX_DECOMPOSITION_H

#include <CGAL/license/Surface_mesh_segmentation.h>

/**
 * @file approx_decomposition.h
 * @brief The API which contains template functions for concavity value computation and approximate convex decomposition
 */
#include <CGAL/internal/Approximate_convex_decomposition/decomposition.h>
#include <CGAL/internal/Approximate_convex_decomposition/concavity.h>
#include <CGAL/boost/graph/named_function_params.h>
#include <CGAL/boost/graph/named_params_helper.h>

namespace CGAL
{


/*!
 * \ingroup PkgSurfaceSegmentation
 * @brief Function computing concavity value of a cluster described by an id in a triangle mesh.
 *
 * Cluster is a subset of connected faces in a triangle mesh.
 *
 * @pre `is_triangle_mesh(mesh)`
 *
 * @tparam ConcurrencyTag enables sequential versus parallel algorithm (possible values are `Sequential_tag` and `Parallel_tag`)
 * @tparam TriangleMesh a model of `FaceListGraph`
 * @tparam FacePropertyMap a `ReadWritePropertyMap` with the `boost::graph_traits<TriangleMesh>::%face_descriptor` key type and an integer value type
 * @tparam NamedParameters a sequence of Named Parameters
 *
 * @param mesh clustered triangle mesh
 * @param face_ids property map with per face cluster ids
 * @param cluster_id id of the target cluster on which concavity value is computed
 * @param np optional sequence of `Named Parameters` among the ones listed below
 *
 * \cgalNamedParamsBegin
 *    \cgalParamBegin{vertex_point_map} the property map with the points associated to the vertices of `mesh` \cgalParamEnd
 *    \cgalParamBegin{geom_traits} a geometric traits class instance \cgalParamEnd
 * \cgalNamedParamsEnd
 *
 * @return concativity value, the largest distance from a vertex in a cluster to its projection onto the convex hull of a triangle mesh
 */
template <class ConcurrencyTag, class TriangleMesh, class FacePropertyMap, class NamedParameters>
double
concavity_value(const TriangleMesh& mesh,
                FacePropertyMap face_ids,
                std::size_t cluster_id,
                const NamedParameters& np)
{
    typedef typename Polygon_mesh_processing::GetVertexPointMap<TriangleMesh, NamedParameters>::const_type Vpm;
    typedef typename Polygon_mesh_processing::GetGeomTraits<TriangleMesh, NamedParameters>::type Geom_traits;

    Geom_traits geom_traits = boost::choose_param(boost::get_param(np, internal_np::geom_traits), Geom_traits());
    Vpm vpm = boost::choose_param(boost::get_param(np, internal_np::vertex_point),
                                  get_const_property_map(boost::vertex_point, mesh));

    CGAL_precondition(is_triangle_mesh(mesh));

    internal::Concavity<TriangleMesh, Vpm, Geom_traits, ConcurrencyTag> algorithm(mesh, vpm, geom_traits);
    return algorithm.compute(face_ids, cluster_id);
}


#ifndef DOXYGEN_RUNNING
template <class ConcurrencyTag, class TriangleMesh, class FacePropertyMap>
double
concavity_value(const TriangleMesh& mesh,
                FacePropertyMap face_ids,
                std::size_t cluster_id)
{
    return concavity_value<ConcurrencyTag>(mesh, face_ids, cluster_id, Polygon_mesh_processing::parameters::all_default());
}
#endif


/*!
 * \ingroup PkgSurfaceSegmentation
 * @brief Function computing concavity value of a triangle mesh.
 *
 * @pre `is_triangle_mesh(mesh)`
 *
 * @tparam ConcurrencyTag enables sequential versus parallel algorithm (possible values are `Sequential_tag` and `Parallel_tag`)
 * @tparam TriangleMesh a model of `FaceListGraph`
 * @tparam NamedParameters a sequence of Named Parameters
 *
 * @param mesh triangle mesh on which concavity value is computed
 * @param np optional sequence of `Named Parameters` among the ones listed below
 *
 * \cgalNamedParamsBegin
 *    \cgalParamBegin{vertex_point_map} the property map with the points associated to the vertices of `mesh` \cgalParamEnd
 *    \cgalParamBegin{geom_traits} a geometric traits class instance \cgalParamEnd
 * \cgalNamedParamsEnd
 *
 * @return concativity value, the largest distance from a vertex in a cluster to its projection onto the convex hull of a triangle mesh
 */
template <class ConcurrencyTag, class TriangleMesh, class NamedParameters>
double
concavity_value(const TriangleMesh& mesh,
                const NamedParameters& np)
{
    typedef typename Polygon_mesh_processing::GetVertexPointMap<TriangleMesh, NamedParameters>::const_type Vpm;
    typedef typename Polygon_mesh_processing::GetGeomTraits<TriangleMesh, NamedParameters>::type Geom_traits;

    Geom_traits geom_traits = boost::choose_param(boost::get_param(np, internal_np::geom_traits), Geom_traits());
    Vpm vpm = boost::choose_param(boost::get_param(np, internal_np::vertex_point),
                                  get_const_property_map(boost::vertex_point, mesh));

    CGAL_precondition(is_triangle_mesh(mesh));

    internal::Concavity<TriangleMesh, Vpm, Geom_traits, ConcurrencyTag> algorithm(mesh, vpm, geom_traits);
    return algorithm.compute();
}


#ifndef DOXYGEN_RUNNING
template <class ConcurrencyTag, class TriangleMesh>
double
concavity_value(const TriangleMesh& mesh)
{
    return concavity_value<ConcurrencyTag>(mesh, Polygon_mesh_processing::parameters::all_default());
}
#endif


/*!
 * \ingroup PkgSurfaceSegmentation
 * @brief Function computing approximate convex decomposition of a triangle mesh, which is the smallest set of clusters satisfying concavity constraint. 
 *
 * Clusters are subsets of connected faces in a triangle mesh which concavity values are less or equal to 'concavity_threshold'.
 *
 * This function fills a property map which associates a cluster-id (in the range [0, 'number_of_clusters'-1]) to each face.
 *
 * @pre `is_triangle_mesh(mesh)`
 * @pre `num_face(mesh) >= min_number_of_clusters`
 *
 * @tparam ConcurrencyTag enables sequential versus parallel algorithm (possible values are `Sequential_tag` and `Parallel_tag`)
 * @tparam TriangleMesh a model of `FaceListGraph`
 * @tparam FacePropertyMap a `ReadWritePropertyMap` with the `boost::graph_traits<TriangleMesh>::%face_descriptor` key type and an integer value type
 * @tparam NamedParameters a sequence of Named Parameters
 *
 * @param mesh triangle mesh on which approximate convex decomposition is computed
 * @param[out] face_ids property map with per face cluster ids
 * @param concavity_threshold maximal concavity value of a cluster constraint
 * @param min_number_of_clusters minimal number of clusters constraint (default=1)
 * @param np optional sequence of `Named Parameters` among the ones listed below
 *
 * \cgalNamedParamsBegin
 *    \cgalParamBegin{vertex_point_map} the property map with the points associated to the vertices of `mesh` \cgalParamEnd
 *    \cgalParamBegin{geom_traits} a geometric traits class instance \cgalParamEnd
 * \cgalNamedParamsEnd
 *
 * @return number of clusters
 */
template <class ConcurrencyTag, class TriangleMesh, class FacePropertyMap, class NamedParameters>
std::size_t
convex_decomposition(const TriangleMesh& mesh,
                     FacePropertyMap face_ids,
                     double concavity_threshold,
                     std::size_t min_number_of_clusters,
                     const NamedParameters& np)
{
    typedef typename Polygon_mesh_processing::GetVertexPointMap<TriangleMesh, NamedParameters>::const_type Vpm;
    typedef typename Polygon_mesh_processing::GetGeomTraits<TriangleMesh, NamedParameters>::type Geom_traits;

    Geom_traits geom_traits = boost::choose_param(boost::get_param(np, internal_np::geom_traits), Geom_traits());
    Vpm vpm = boost::choose_param(boost::get_param(np, internal_np::vertex_point),
                                  get_const_property_map(boost::vertex_point, mesh));

    CGAL_precondition(is_triangle_mesh(mesh));
    CGAL_precondition(num_faces(mesh) >= min_number_of_clusters);

    internal::Approx_decomposition<TriangleMesh, Vpm, Geom_traits, ConcurrencyTag> algorithm(mesh, vpm, geom_traits);
    return algorithm.decompose(face_ids, concavity_threshold, min_number_of_clusters);
}


#ifndef DOXYGEN_RUNNING
template <class ConcurrencyTag, class TriangleMesh, class FacePropertyMap>
std::size_t
convex_decomposition(const TriangleMesh& mesh,
                     FacePropertyMap face_ids,
                     double concavity_threshold = 0.05,
                     std::size_t min_number_of_clusters = 1)
{
    return convex_decomposition<ConcurrencyTag>(mesh, face_ids, concavity_threshold, min_number_of_clusters, Polygon_mesh_processing::parameters::all_default());
}
#endif


} //namespace CGAL

#endif //CGAL_SURFACE_MESH_SEGMENTATION_APPROX_DECOMPOSITION_H
