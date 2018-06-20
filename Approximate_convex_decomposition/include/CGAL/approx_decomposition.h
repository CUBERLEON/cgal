#ifndef CGAL_APPROXIMATE_CONVEX_DECOMPOSITION_APPROX_DECOMPOSITION_H
#define CGAL_APPROXIMATE_CONVEX_DECOMPOSITION_APPROX_DECOMPOSITION_H

#include <CGAL/internal/Approximate_convex_decomposition/approx_decomposition.h>
#include <CGAL/internal/Approximate_convex_decomposition/concavity.h>

namespace CGAL
{


/*
 * @brief Function computing concavity value of a cluster.
 * 
 * Cluster is a subset of connected faces in a triangle mesh.
 * Concavity value is the largest distance from a vertex in a cluster to the projected point onto the convex hull of a mesh.
 *
 * @return concativity value.
 */
template <class TriangleMesh, class FacePropertyMap, class NamedParameters>
double
concavity_value(const TriangleMesh& mesh,
                FacePropertyMap face_ids,
                std::size_t cluster_id,
                const NamedParameters& np)
{
    typedef typename Polygon_mesh_processing::GetVertexPointMap<TriangleMesh, NamedParameters>::const_type VertexPointMap;
    typedef typename Kernel_traits<typename boost::property_traits<VertexPointMap>::value_type>::Kernel GeomTraits;

    CGAL_precondition(is_triangle_mesh(mesh));

    internal::Concavity<TriangleMesh, GeomTraits> algorithm(mesh, GeomTraits());
    return algorithm.compute(face_ids, cluster_id);
}


/*
 * @brief Function computing concavity value of a triangle mesh.
 * 
 * Concavity value is the largest distance from a vertex in a mesh to the projected point onto the convex hull of a mesh.
 *
 * @return concativity value.
 */
template <class TriangleMesh, class NamedParameters>
double
concavity_value(const TriangleMesh& mesh,
                const NamedParameters& np)
{
    typedef typename Polygon_mesh_processing::GetVertexPointMap<TriangleMesh, NamedParameters>::const_type VertexPointMap;
    typedef typename Kernel_traits<typename boost::property_traits<VertexPointMap>::value_type>::Kernel GeomTraits;

    CGAL_precondition(is_triangle_mesh(mesh));

    internal::Concavity<TriangleMesh, GeomTraits> algorithm(mesh, GeomTraits());
    return algorithm.compute();
}


/*
 * @brief Function computing the approximate convex decomposition of a triangle mesh.
 *
 * This function fills a property map which associates a cluster-id (in the range [0, 'number_of_clusters'-1]) to each face.
 * Clusters are subsets of connected faces in a triangle mesh which concavity values are less than 'concavity_threshold'.
 *
 * @return number of clusters.
 */
template <class TriangleMesh, class FacePropertyMap, class NamedParameters>
std::size_t
convex_decomposition(const TriangleMesh& mesh,
                     FacePropertyMap face_ids,
                     const NamedParameters& np,
                     double concavity_threshold = 0.05,
                     std::size_t min_number_of_clusters = 1)
{
    typedef typename Polygon_mesh_processing::GetVertexPointMap<TriangleMesh, NamedParameters>::const_type VertexPointMap;
    typedef typename Kernel_traits<typename boost::property_traits<VertexPointMap>::value_type>::Kernel GeomTraits;

    CGAL_precondition(is_triangle_mesh(mesh));
    CGAL_precondition(num_faces(mesh) >= min_number_of_clusters);

    internal::Approx_decomposition<TriangleMesh, GeomTraits> algorithm(mesh, GeomTraits());
    return algorithm.decompose(face_ids, concavity_threshold, min_number_of_clusters);
}


} //namespace CGAL

#endif //CGAL_APPROXIMATE_CONVEX_DECOMPOSITION_APPROX_DECOMPOSITION_H
