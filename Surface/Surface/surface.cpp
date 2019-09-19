#include<iostream>
#include<fstream>
#include<vector>

// ---------------------openvolumemesh
// Include vector classes
#include <OpenVolumeMesh/Geometry/VectorT.hh>
// Make some typedefs to facilitate your life

#include <OpenVolumeMesh/Mesh/HexahedralMesh.hh>
typedef OpenVolumeMesh::GeometricHexahedralMeshV3d MyHexMesh;




// Include vector classes
#include <OpenVolumeMesh/Geometry/VectorT.hh>

// Include polyhedral mesh kernel
#include <OpenVolumeMesh/Mesh/PolyhedralMesh.hh>

// Make some typedefs to facilitate your life
typedef OpenVolumeMesh::Geometry::Vec3d         Vec3d;

typedef OpenVolumeMesh::Geometry::Vec3f         Vec3f;
typedef OpenVolumeMesh::GeometryKernel<Vec3f>   PolyhedralMeshV3f;



using namespace std;

bool ReadVTKFile(MyHexMesh &mesh, string fname){
	fname.append(".vtk");
	ifstream fin(fname.c_str());
	string strtmp;
	for(;strtmp!="POINTS";) fin >> strtmp;
	int num_p;
	fin >> num_p >> strtmp;
	double x, y, z;
	vector<OpenVolumeMesh::VertexHandle> vhs;
	for (int i = 0; i < num_p; i++) {
		fin >> x >> y >> z;
		vhs.push_back(mesh.add_vertex(Vec3d(x, y, z)));
	}
	for (; strtmp != "CELLS";) fin >> strtmp;
	int num_c, vertex;
	fin >> num_c >> x >> x;
	vector<OpenVolumeMesh::VertexHandle> vhs1;
	for (int i = 0; i < num_c; i++) {
		fin >> x;//8个顶点
		vhs1.clear();
		for (int j = 0; j < 8; j++) {
			fin >> vertex;
			vhs1.push_back(vhs[vertex]);
		}
		mesh.add_cell(vhs1);
	}
	return true;
}
void FindSurface(MyHexMesh &mesh, MyHexMesh &surface) {
	for (auto f_it = mesh.faces_begin(); f_it.valid(); ++f_it) {
		auto ff_it = f_it; ff_it++;
		int flag = 0;
		Vec3d v[4];
		int i = 0;
		for (auto fv_it = mesh.fv_iter(*f_it); fv_it.valid(); ++fv_it, ++i) {
			v[i] = mesh.vertex(*fv_it);
		}
		for (; ff_it.valid(); ff_it++) {
			int v_flag = 0;
			for (auto fv_it = mesh.fv_iter(*f_it); fv_it.valid(); ++fv_it) {
				for (auto ffv_it = mesh.fv_iter(*ff_it); ffv_it.valid(); ++ffv_it) {
					if (fv_it->idx() == ffv_it->idx())
						v_flag++;
				}
			}
			if (v_flag == 4)flag++;
		}
		if (flag < 2) {
			vector<OpenVolumeMesh::VertexHandle> vhs;
			for (int i = 0; i < 4; i++) {
				vhs.push_back(surface.add_vertex(v[i]));
			}
			surface.add_face(vhs);
		}
	}
}
void WriteVTKFile(MyHexMesh &mesh, string fname) {
	fname.append("_surface.vtk");
	ofstream file(fname.c_str());
	file << "# vtk DataFile Version 3.0\n";
	file << "Mesquite Mesh\n";
	file << "ASCII\n";
	file << "DATASET UNSTRUCTURED_GRID\n";
	file << "POINTS " << mesh.n_vertices() << " double\n";
	for (auto v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); v_it++)
	{
		file << mesh.vertex(*v_it)[0] << " " << mesh.vertex(*v_it)[1] << " " << mesh.vertex(*v_it)[2] << endl;
	}
	file << "CELLS " << mesh.n_faces() << " " << mesh.n_faces() * 4 << endl;
	for (auto f_it = mesh.faces_begin(); f_it != mesh.faces_end(); f_it++)
	{
		file << 3;
		for (auto fv_it = mesh.fv_iter(*f_it); fv_it != mesh.fv_iter(*f_it); fv_it++)
		{
			file << " " << (*fv_it).idx();
		}
		file << endl;
	}
	file << "CELL_TYPES " << mesh.n_faces() << endl;
	for (int i = 0; i < mesh.n_faces(); i++)
		file << 5 << endl;
	file << "POINT_DATA " << mesh.n_vertices() << endl;
	file << "SCALARS fixed int\n";
	file << "LOOKUP_TABLE default\n";
	for (auto v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); v_it++)
	{
		if (mesh.is_boundary(*v_it))
			file << 1 << endl;
		else
			file << 0 << endl;
	}
}
void main(){
	string fname = "fandisk";
	MyHexMesh mymesh;
	MyHexMesh surface;
	//OpenVolumeMesh::IO::FileManager fileManager;
	//fileManager.readFile("fandisk.ovm", mymesh);
	ReadVTKFile(mymesh, fname);
	FindSurface(mymesh, surface);
	WriteVTKFile(surface, fname);
}

