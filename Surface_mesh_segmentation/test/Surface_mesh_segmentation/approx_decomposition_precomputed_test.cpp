#include <CGAL/approximate_convex_decomposition.h>

#include <CGAL/Surface_mesh.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include "Utils.h"
#include <vector>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef CGAL::Surface_mesh<Kernel::Point_3> Mesh;
typedef boost::graph_traits<Mesh>::face_descriptor face_descriptor;

#ifndef CGAL_LINKED_WITH_TBB
typedef CGAL::Sequential_tag Concurrency_tag;
#else
typedef CGAL::Parallel_tag Concurrency_tag;
#endif

int main()
{
  Mesh mesh;
  if (!read_to_polyhedron("data/sword.off", mesh)) return EXIT_FAILURE;

  typedef Mesh::Property_map<face_descriptor, int> Clusters_map;
  Clusters_map cluster_ids = mesh.add_property_map<face_descriptor, int>("f:cluster").first;

  std::size_t clusters_num = CGAL::approximate_convex_segmentation<Concurrency_tag>(mesh, cluster_ids, 0.3);

  std::vector<int> precomputed;
  std::ifstream file("data/sword_0.3_precomputed.txt");
  int id;
  while (file >> id)
  {
    precomputed.push_back(id);
  }
  file.close();

  if (precomputed.size() != num_faces(mesh))
  {
    std::cerr << "Invalid precomputed file" << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Number of clusters: " << clusters_num << std::endl;
  
  int i = 0;
  BOOST_FOREACH(face_descriptor face, faces(mesh))
  {
    std::cout << cluster_ids[face] << " ";

    if (cluster_ids[face] != precomputed[i])
    {
      std::cerr << "The cluster-id of a face doesn't match with the precomputed value" << std::endl;
      return EXIT_FAILURE;
    }
    ++i;
  }
  std::cout << std::endl;

  return EXIT_SUCCESS;
}
