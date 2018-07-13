#include <CGAL/Polyhedron_3.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/property_map.h>

#include <CGAL/approximate_convex_decomposition.h>

#include <string>
#include "Utils.h" 

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
typedef CGAL::Surface_mesh<Kernel::Point_3> Surface_mesh;

template <typename Concurrency_tag, typename Mesh>
bool test_on_mesh(const std::string& path, double concavity_threshold, int min_number_of_clusters, std::size_t expected_clusters_num)
{
  Mesh mesh;
  if (!read_to_polyhedron(path.c_str(), mesh)) return false;

  typedef typename boost::graph_traits<Mesh>::face_descriptor face_descriptor;
  
  typedef std::map<face_descriptor, std::size_t> Clusters_map;
  Clusters_map cluster_ids;
  boost::associative_property_map<Clusters_map> clusters_pmap(cluster_ids);

  std::size_t clusters_num = CGAL::approximate_convex_segmentation<Concurrency_tag>(mesh, clusters_pmap, concavity_threshold, min_number_of_clusters);

  std::cout << "Number of clusters: " << clusters_num << std::endl;

  if (clusters_num != expected_clusters_num)
  {
    std::cerr << "The number of clusters doesn't match with expected value" << std::endl;
    return false;
  }
  
  BOOST_FOREACH(face_descriptor face, faces(mesh))
  {
    int id = get(clusters_pmap, face);
    std::cout << id << " ";

    if (id < 0 || std::size_t(id) >= clusters_num)
    {
      std::cerr << "A cluster's id should be in the range [0, clusters_num-1]" << std::endl;
      return false; 
    }
  }
  std::cout << std::endl;

  return true;
}

void test_on_mesh(const std::string& path, double concavity_threshold, int min_number_of_clusters, std::size_t expected_clusters_num)
{
  expect_or_fail(test_on_mesh<CGAL::Sequential_tag, Polyhedron>(path, concavity_threshold, min_number_of_clusters, expected_clusters_num)); 
  expect_or_fail(test_on_mesh<CGAL::Sequential_tag, Surface_mesh>(path, concavity_threshold, min_number_of_clusters, expected_clusters_num)); 
    
#ifdef CGAL_LINKED_WITH_TBB
  expect_or_fail(test_on_mesh<CGAL::Parallel_tag, Polyhedron>(path, concavity_threshold, min_number_of_clusters, expected_clusters_num)); 
  expect_or_fail(test_on_mesh<CGAL::Parallel_tag, Surface_mesh>(path, concavity_threshold, min_number_of_clusters, expected_clusters_num)); 
#endif
}

int main()
{
  test_on_mesh("data/cube.off", 0.01, 1, 1); 
  test_on_mesh("data/cube.off", 0.01, 10, 10);
  test_on_mesh("data/sword.off", 0.3, 1, 12);
  test_on_mesh("data/sword.off", 0.3, 400, 400);

  return EXIT_SUCCESS;
}
