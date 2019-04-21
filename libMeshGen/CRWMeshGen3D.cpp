// libMeshGen.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "libMeshGen.h"
#include "..\MatrixInterface\Matrix.h"

#define _CRT_SECURE_NO_WARNINGS


CRWMeshGen3D::CRWMeshGen3D()
{
	// check the sizes of certain 
	assert(sizeof(CRing) == 24);
	assert(sizeof(CNode) == 48);

	this->height = 0;
	this->vertical_density = 0;

	memset(&this->sparse_points, 0, sizeof(this->sparse_points));
}

CRWMeshGen3D::~CRWMeshGen3D()
{
	if (this->sparse_points.rings_count != 0)
	{
		free(this->sparse_points.rings_radiuses);
		free(this->sparse_points.rings_materials);
	}

	if (this->sparse_points.sparse_count != 0)
	{
		free(this->sparse_points.sparse_radiuses);
		free(this->sparse_points.sparse_values);
	}


}

///<summary>fsfsdf</summary>
double CRWMeshGen3D::GetRadius(void) const
{
	if (this->sparse_points.rings_count == 0)
		return 0;

	double m = *this->sparse_points.rings_radiuses;
	for (int i = 0; i < this->sparse_points.rings_count; i++)
		m = max(m, this->sparse_points.rings_radiuses[i]);

	return m;

}

void CRWMeshGen3D::SetSpatialParameters(double height, double vertical_density)
{
	this->height = height;
	this->vertical_density = vertical_density;
}

void CRWMeshGen3D::SetSparseFunctionPoints(const double *radius_array, const double *sparse_array, int count)
{
	if (this->sparse_points.sparse_count != 0)
	{
		free(this->sparse_points.sparse_radiuses);
		free(this->sparse_points.sparse_values);
	}

	this->sparse_points.sparse_count = count;
	this->sparse_points.sparse_radiuses = (double*)malloc(sizeof(double) * count);
	this->sparse_points.sparse_values = (double*)malloc(sizeof(double) * count);
	memcpy(this->sparse_points.sparse_radiuses, radius_array, sizeof(double) * count);
	memcpy(this->sparse_points.sparse_values, sparse_array, sizeof(double) * count);
}

void CRWMeshGen3D::SetRingsAndMaterials(const double *radius_array, const double *material_array, int count)
{
	if (this->sparse_points.rings_count != 0)
	{
		free(this->sparse_points.rings_radiuses);
		free(this->sparse_points.rings_materials);
	}

	if (count != 4)
		throw NativeExceptions::Exception(_T("At the moment mesh generator does not support number of rings different from 4"));

	this->reconstruction_mesh_radius = radius_array[1];
	this->electrode_surface_radius = radius_array[2];
	
	this->outer_shield_radius = radius_array[3];

	this->sparse_points.rings_count = count;
	this->sparse_points.rings_radiuses = (double*)malloc(sizeof(double) * count);
	this->sparse_points.rings_materials = (double*)malloc(sizeof(double) * count);
	memcpy(this->sparse_points.rings_radiuses, radius_array, sizeof(double) * count);
	memcpy(this->sparse_points.rings_materials, material_array, sizeof(double) * count);

	this->sparse_points.ring_material_min = *this->sparse_points.rings_materials;
	this->sparse_points.ring_material_max = *this->sparse_points.rings_materials;

	for(int i = 0; i < this->sparse_points.rings_count; i++)
	{
		this->sparse_points.ring_material_min = min(this->sparse_points.ring_material_min, this->sparse_points.rings_materials[i]);
		this->sparse_points.ring_material_max = max(this->sparse_points.ring_material_max, this->sparse_points.rings_materials[i]);
	}
}


int CRWMeshGen3D::GetNumberOfRings(void) const
{
	return this->nodes_on_ring.size();
}

int CRWMeshGen3D::GetNumberOfPlainElements(void) const
{
	return this->plain_elements.size();
}

int CRWMeshGen3D::GetNumberOfSpatialElements(void) const
{
	return this->spatial_elements.size();
}

int CRWMeshGen3D::GetNumberOfPlainNodes(void) const
{
	return this->plain_nodes.size();
}

int CRWMeshGen3D::GetNumberOfSpatialNodes(void) const
{
	return this->spatial_nodes.size();
}

int CRWMeshGen3D::GetNumberOfSpatialLayers(void) const
{
	return (int)floor(this->height / this->vertical_density) + 1;
}


int CRWMeshGen3D::GetNumberOfNodesOnRing(int ring_no) const
{
	return this->nodes_on_ring[ring_no].size();
}

CNode * CRWMeshGen3D::GetFirstPlainNode(void)
{
	if (this->plain_nodes.empty())
		return NULL;
	return &this->plain_nodes[0];
}

CNode * CRWMeshGen3D::GetFirstSpatialNode(void)
{
	if (this->spatial_nodes.empty())
		return NULL;
	return &this->spatial_nodes[0];
}

CRing * CRWMeshGen3D::GetFirstRing(void)
{
	if (this->rings.empty())
		return NULL;
	return &this->rings[0];
}

CElement * CRWMeshGen3D::GetFirstPlainElement(void)
{
	if (this->plain_elements.empty())
		return NULL;
	return &this->plain_elements[0];
}

CElement * CRWMeshGen3D::GetFirstSpatialElement(void)
{
	if (this->spatial_elements.empty())
		return NULL;
	return &this->spatial_elements[0];
}

int CRWMeshGen3D::GetNearestFixedRing(double radius) const
{
	if (this->sparse_points.rings_count == 0)
		return -1;

	int min_ring = 0;
	for (int i = 0; i < this->sparse_points.rings_count; i++)
		if (fabs(this->sparse_points.rings_radiuses[i] - radius) < fabs(this->sparse_points.rings_radiuses[min_ring] - radius))
			min_ring = i;

	return min_ring;
}

void CRWMeshGen3D::GeneratePlainMesh(void)
{
	this->rings.push_back(CRing(0.0, 0.0, true));
	double previous_radius = 0.0;
	double current_radius = 0;
	double limit = this->GetRadius();
	int iring = 1;
	bool next_fixed = true; // pierscieñ w œrodku siatki
	while(true)
	{
		if (iring < this->sparse_points.rings_count)
		{
			// -1 usuwa outer shield ring z modelu zwyklego
			if (this->sparse_points.rings_radiuses[iring] < 0) //-1
			{
				iring++;
				continue;
			}
		}

		double radius_delta = this->SparseFunction(current_radius);
		double next_radius = current_radius + radius_delta;

		if (current_radius > limit)
			break;


		if (iring < this->sparse_points.rings_count)
		{
			double fixed_ring_dist = this->sparse_points.rings_radiuses[iring];
			if ((fixed_ring_dist >= current_radius) && (fixed_ring_dist < next_radius))
			{
				radius_delta = fixed_ring_dist - current_radius;
				double sparsity = (fixed_ring_dist - current_radius) / (next_radius - current_radius);
				sparsity = this->SparseFunction(current_radius) + (this->SparseFunction(next_radius) - this->SparseFunction(current_radius)) * sparsity;
				sparsity = this->SparseFunction(current_radius + radius_delta);
				previous_radius = current_radius;
				current_radius += radius_delta;
				this->rings.push_back(CRing(current_radius, sparsity, true));
				iring++;

			} else
			{
				previous_radius = current_radius;
				current_radius += radius_delta;
				next_fixed = false;
				this->rings.push_back(CRing(current_radius, this->SparseFunction(current_radius), false));
			}
		} else
			break;

	}

	// generowanie wêz³ów
	this->nodes_on_ring.resize(this->rings.size());
	this->AddNode(0, 0, 0, static_cast<NodeFlag::Enum>(NodeFlag::ReconstructionMeshNode | NodeFlag::CenterOfGeometryNode | NodeFlag::ElectrodesMeshNode));

	//double reco_point = (this->inner_active) ? this->inner_distance : this->outer_distance;
	//reco_point += 0.001;
	double reco_point = this->reconstruction_mesh_radius + 0.001;
	double electrodes_point = this->electrode_surface_radius + 0.001;

	// wêz³y na kolejnych pierœcieniach
	for (int ring_no = 1; ring_no < this->rings.size(); ring_no++)
	{
		double radius = this->rings[ring_no].radius;
		double density = this->rings[ring_no].density;
	//	if (density == 0.0)
		//	continue;

		double _2PI = 2.0 * M_PI;
		int nodes_on_ring = (int)floor((_2PI * radius) / density);

		NodeFlag::Enum flag = NodeFlag::Empty;

		// mark nodes inside the reconstruction mesh (complete mesh - tube mesh - isolation mesh)
		if (radius <= reco_point)
			flag = static_cast<NodeFlag::Enum>(flag | NodeFlag::ReconstructionMeshNode);
		if (radius <= electrodes_point)
			flag = static_cast<NodeFlag::Enum>(flag | NodeFlag::ElectrodesMeshNode);
		//else
		//	flag = static_cast<NodeFlags::Enum>(flag | NodeFlags::NormalNode);

		double epsilon = 0.01;

		// mark nodes that build the circular surface for electrode placement
		if (fabs(radius - this->electrode_surface_radius) < epsilon)
			flag = static_cast<NodeFlag::Enum>(flag | NodeFlag::ElectrodesSurfaceNode);

		// mark nodes that build the circular surface for electrode placement
		if (fabs(radius - this->outer_shield_radius) < epsilon)
			flag = static_cast<NodeFlag::Enum>(flag | NodeFlag::OuterShieldSurfaceNode);

		for (int inode = 0; inode < nodes_on_ring; inode++)
		{
			double angle = _2PI * ((double)inode / (double)nodes_on_ring);
			this->AddNode(ring_no, radius, angle, flag);
		}
	}

	// przesuniêcie wszystkich wêz³ów tak, aby zajmowa³y I æwiartke uk³adu wsp.;
	double cx = this->GetRadius();
	double cy = this->GetRadius();
	for(std::vector<CNode>::iterator iter = this->plain_nodes.begin(); iter != this->plain_nodes.end(); iter++)
	{
		iter->SetX(iter->GetX() + cx);
		iter->SetY(iter->GetY() + cy);
	}

	// ³¹czenie wêz³ów
	// najpierw po³¹cz wêze³ œrodku
	//this->elements;
	int center_node = this->nodes_on_ring[0][0];
	for (int inode = 0; inode < this->nodes_on_ring[1].size(); inode++)
	{
		int n1 = this->GetNode(1, inode);
		int n2 = this->GetNode(1, inode + 1);
		CElement triangle(center_node, n1, n2);
		triangle.SetTag(1);
		this->plain_elements.push_back(triangle); 
	}


	// i ca³a reszta            
	for (int iring = 1; iring < this->rings.size() - 1; iring++)
	{
		int r = 0; // numer wêz³a na pierœcieniu i
		int r1 = 0; // numer wêz³a na pierœcieniu i+1

		//if (this->nodes_on_ring[iring].size() == 0)
		while ((r < this->nodes_on_ring[iring].size()) || (r1 < this->nodes_on_ring[iring + 1].size()))
		{
			// get node indices
			int n1 = this->GetNode(iring, r);
			int n2 = this->GetNode(iring + 1, r1);
			int n3a = this->GetNode(iring, r + 1);
			int n3b = this->GetNode(iring + 1, r1 + 1);

			// get node pointers
			CNode * pn1 = &this->plain_nodes[n1];
			CNode * pn2 = &this->plain_nodes[n2];
			CNode * pn3a = &this->plain_nodes[n3a];
			CNode * pn3b = &this->plain_nodes[n3b];

			// build triangle on nearest nodes
			double d1 = CNode::Distance(pn1, pn3b);
			double d2 = CNode::Distance(pn2, pn3a);

			if (d1 < d2)
			{
				// przypadek I
				CElement new_elem(n1, n2, n3b);
				new_elem.SetTag(1);
				this->plain_elements.push_back(new_elem);
				r1++;
			}
			else
			{
				// przypadek II
				CElement new_elem(n1, n2, n3a);
				new_elem.SetTag(2);
				this->plain_elements.push_back(new_elem);
				r++;
			}
		}
	}

	// applying the material
	std::vector<CElement>::iterator end = this->plain_elements.end();
	CNode* center_point = &this->plain_nodes[0];
	for(std::vector<CElement>::iterator i = this->plain_elements.begin(); i != end; i++)
	{
		CElement* elem = &(*i);
		CNode * n1 = &this->plain_nodes[elem->GetNode1()];
		CNode * n2 = &this->plain_nodes[elem->GetNode2()];
		CNode * n3 = &this->plain_nodes[elem->GetNode3()];

		double X = (n1->GetX() + n2->GetX() + n3->GetX()) / 3.0;
		double Y = (n1->GetY() + n2->GetY() + n3->GetY()) / 3.0;

		double material = this->sparse_points.rings_materials[0];

		for (int j = 0; j < this->sparse_points.rings_count; j++)
		{
			if (this->sparse_points.rings_radiuses[j] < 0)
				continue;
			if (CNode::Distance(center_point, X, Y) >= this->sparse_points.rings_radiuses[j])
				material = this->sparse_points.rings_materials[j];
		}
		elem->SetMaterial(material);
	}


	// sprawdzenie, czy pierœcienie nie s¹ zbyt blisko siebie
	double min_distance = this->sparse_points.sparse_values[0];
	for(int i = 0; i < this->sparse_points.sparse_count; i++)
		min_distance = min(min_distance, this->sparse_points.sparse_values[i]);
	min_distance /= 2;

	for(int i = 0; i < this->rings.size() - 1; i++)
	{
		CRing *ring = &this->rings[i];
		CRing* next = &this->rings[i + 1];
		if (next->radius - ring->radius < min_distance)
		{
			ring->SetError(true);
			next->SetError(true);
		}
	}
}


void CRWMeshGen3D::GenerateSpatialMesh(void)
{
	// generowanie wêz³ów w przestrzeni ze zbioru wêz³ów na p³aszczyŸnie
	int number_of_levels = this->GetNumberOfSpatialLayers();
	this->spatial_nodes.resize(this->plain_nodes.size() * number_of_levels);
	std::vector<CNode>::iterator dest_iter  = this->spatial_nodes.begin();
	for (int i = 0; i < number_of_levels; i++)
	{
		float vertical_position = (this->height / (float)(number_of_levels - 1)) * (float)i;
		std::vector<CNode>::iterator end_iter = this->plain_nodes.end();
		for(std::vector<CNode>::iterator iter = this->plain_nodes.begin(); iter != end_iter; iter++)
		{
			*dest_iter = *iter;
			CNode * node_ptr = &*dest_iter;
			node_ptr->SetZ(vertical_position);
			dest_iter++;
		}
	}

	this->spatial_elements.reserve((number_of_levels - 1) * this->plain_elements.size() * 3);
	int nodes_per_level = this->GetNumberOfPlainNodes();
	for (int i = 0; i < number_of_levels - 1; i++)
	{
		int current_level_first_node = i * nodes_per_level;
		int next_level_first_node = (i + 1) * nodes_per_level;
		std::vector<CElement>::iterator end = this->plain_elements.end();
		for(std::vector<CElement>::iterator ie = this->plain_elements.begin(); ie != end; ie++)
		{
			int tag = ie->GetTag();
			int n1 = current_level_first_node + ie->GetNode1();
			int n2 = current_level_first_node + ie->GetNode2();
			int n3 = current_level_first_node + ie->GetNode3();
			int n1p = next_level_first_node + ie->GetNode1();
			int n2p = next_level_first_node + ie->GetNode2();
			int n3p = next_level_first_node + ie->GetNode3();

			CNode* pA = &this->spatial_nodes[n1];
			CNode* pB = &this->spatial_nodes[n2];
			CNode* pC = &this->spatial_nodes[n3];
			CNode* pAp = &this->spatial_nodes[n1p];
			CNode* pBp = &this->spatial_nodes[n2p];
			CNode* pCp = &this->spatial_nodes[n3p];

			// build one spatial cell divided onto 3 tetrahedrons
			if (tag == 1)
			{
				this->spatial_elements.push_back(CElement(n1, n1p, n2p, n3p));
				this->spatial_elements.push_back(CElement(n1, n2, n2p, n3p));
				this->spatial_elements.push_back(CElement(n1, n2, n3, n3p));
				//this->spatial_elements.push_back(CElement(n1, n1p, n2p, n3p));
				//this->spatial_elements.push_back(CElement(n1, n2, n2p, n3p));
				//this->spatial_elements.push_back(CElement(n1, n2, n3, n3p));
			} else
			{
				this->spatial_elements.push_back(CElement(n1, n2, n2p, n3));
				this->spatial_elements.push_back(CElement(n1, n2p, n3p, n1p));
				this->spatial_elements.push_back(CElement(n1, n2p, n3p, n3));

				//this->spatial_elements.push_back(CElement(n1, n1p, n2p, n3p));
				//this->spatial_elements.push_back(CElement(n1, n2, n2p, n3p));
				//this->spatial_elements.push_back(CElement(n1, n2, n3, n3p));
			}

			// apply a material value to the newly cerated tetrahedrons
			this->spatial_elements[this->spatial_elements.size() - 3].SetMaterial(ie->GetMaterial());
			this->spatial_elements[this->spatial_elements.size() - 2].SetMaterial(ie->GetMaterial());
			this->spatial_elements[this->spatial_elements.size() - 1].SetMaterial(ie->GetMaterial());



#define IS_ELEC(ptr) (((ptr)->flags & NodeFlag::ElectrodesSurfaceNode) == NodeFlag::ElectrodesSurfaceNode)
#define IS_SHLD(ptr) (((ptr)->flags & NodeFlag::OuterShieldSurfaceNode) == NodeFlag::OuterShieldSurfaceNode)

			if (IS_ELEC(pA) && IS_ELEC(pAp) && IS_ELEC(pBp))	electrodes_surface.push_back(CElement(n1, n1p, n2p));	
			if (IS_ELEC(pA) && IS_ELEC(pB) && IS_ELEC(pBp))		electrodes_surface.push_back(CElement(n1, n2, n2p));	
			if (IS_ELEC(pA) && IS_ELEC(pAp) && IS_ELEC(pC))		electrodes_surface.push_back(CElement(n1, n1p, n3));	
			if (IS_ELEC(pAp) && IS_ELEC(pC) && IS_ELEC(pCp))	electrodes_surface.push_back(CElement(n1p, n3, n3p));	
			if (IS_ELEC(pB) && IS_ELEC(pC) && IS_ELEC(pCp))		electrodes_surface.push_back(CElement(n2, n3, n3p));	
			if (IS_ELEC(pB) && IS_ELEC(pBp) && IS_ELEC(pCp))	electrodes_surface.push_back(CElement(n2, n2p, n3p));	

			if (IS_SHLD(pA) && IS_SHLD(pAp) && IS_SHLD(pBp))	outer_shield_surface.push_back(CElement(n1, n1p, n2p));	
			if (IS_SHLD(pA) && IS_SHLD(pB) && IS_SHLD(pBp))		outer_shield_surface.push_back(CElement(n1, n2, n2p));	
			if (IS_SHLD(pA) && IS_SHLD(pAp) && IS_SHLD(pC))		outer_shield_surface.push_back(CElement(n1, n1p, n3));	
			if (IS_SHLD(pAp) && IS_SHLD(pC) && IS_SHLD(pCp))	outer_shield_surface.push_back(CElement(n1p, n3, n3p));	
			if (IS_SHLD(pB) && IS_SHLD(pC) && IS_SHLD(pCp))		outer_shield_surface.push_back(CElement(n2, n3, n3p));	
			if (IS_SHLD(pB) && IS_SHLD(pBp) && IS_SHLD(pCp))	outer_shield_surface.push_back(CElement(n2, n2p, n3p));	

#undef IS_ELEC(ptr)
#undef IS_SHLD(ptr)

		}
	}

	// porz¹dkowanie normalnych
	// 26.05.2010:
	for(std::vector<CElement>::iterator i = this->spatial_elements.begin(); i != this->spatial_elements.end(); i++)
	{
		CNode* n1 = &this->spatial_nodes[i->GetNode1()];
		CNode* n2 = &this->spatial_nodes[i->GetNode2()];
		CNode* n3 = &this->spatial_nodes[i->GetNode3()];
		CNode* n4 = &this->spatial_nodes[i->GetNode4()];

		CNode center123 = CNode::Center(*n1, *n2, *n3);
        CNode center324 = CNode::Center(*n3, *n2, *n4);
        CNode center341 = CNode::Center(*n3, *n4, *n1);
        CNode center142 = CNode::Center(*n1, *n4, *n2);
        CNode center = CNode::Center(*n1, *n2, *n3, *n4);

		CNode norm123 = CNode::CrossProduct(*n1, *n2, *n3);
        norm123.Normalize();

		CNode norm324 = CNode::CrossProduct(*n3, *n2, *n4);
        norm324.Normalize();

		CNode norm341 = CNode::CrossProduct(*n3, *n4, *n1);
        norm341.Normalize();

		CNode norm142 = CNode::CrossProduct(*n1, *n4, *n2);
        norm142.Normalize();

		double dir1 = CNode::DotProduct(norm123, *n4 - center123);
        double dir2 = CNode::DotProduct(norm324, *n1 - center324);
        double dir3 = CNode::DotProduct(norm341, *n2 - center341);
        double dir4 = CNode::DotProduct(norm142, *n3 - center142);

		if (dir1 == 0 || dir2 == 0 || dir3 == 0 || dir4 == 0)
			throw NativeExceptions::Exception(_T("dir 1/2/3/4 is equal to zero"));

		if (dir1 < 0 && dir2 < 0 && dir3 < 0 && dir4 < 0)
		{
			// normalne s¹ okej, nie trzeba ich korygowac
			continue;
		}

		if (dir1 > 0 && dir2 > 0 && dir3 > 0 && dir4 > 0)
		{
			// normalne s¹ do wewnêtrz, potrzebna korekcja
			int in1 = i->GetNode1();
			int in2 = i->GetNode2();
			i->SetNode1(in2);
			i->SetNode2(in1);
			continue;
		}

		throw NativeExceptions::Exception(_T("Some of the triangle's normals are directed inside and in the same time some of them are directed outside"));

	}
}

int CRWMeshGen3D::GetNode(int ring_number, int node_number)
{
	int node_id = this->nodes_on_ring[ring_number][node_number % (this->nodes_on_ring[ring_number].size())];
	return node_id;
}

int CRWMeshGen3D::AddNode(int ring_number, double radius, double angle, NodeFlag::Enum flags)
{
	CNode node(ring_number, radius, angle, flags);
	int id = this->plain_nodes.size();
	this->plain_nodes.push_back(node);
	this->nodes_on_ring[ring_number].push_back(id);
	return id;
}

void CRWMeshGen3D::SaveNodes(std::vector<CNode> & nodes, enum MeshSaveMode save_mode, const TCHAR * file_name)
{
#if !defined(_UNICODE)
	FILE *node_file = fopen(file_name, _T("w+"));
#else
	FILE *node_file = _wfopen(file_name, _T("w+"));
#endif

	if (node_file == NULL)
		throw NativeExceptions::IOException(file_name, errno, _T("Error creating nodes files"));

	setvbuf(node_file, NULL, _IOFBF, 1024 * 1024); 
	// 20.02.2010: Dopasowanie do CMatrix
	//fprintf(node_file, "%d\n", nodes.size());
	fprintf(node_file, "%d\t%d\n", nodes.size(), 3);

	std::vector<CNode>::iterator iter = nodes.begin();
	std::vector<CNode>::iterator end = nodes.end();
	if (save_mode == MeshSaveMode::SavePlainData)
		while (iter != end)
		{
			CNode * n = &*iter;
			fprintf(node_file,  "%10.5f %10.5f %10.5f\n", n->GetX(), n->GetY(), 0.0f);
			iter++;
		}
	else
		// MeshSaveMode::SaveSpatialData
		while (iter != end)
		{
			CNode * n = &*iter;
			fprintf(node_file,  "%10.5f %10.5f %10.5f\n", n->GetX(), n->GetY(), n->GetZ());
			iter++;
		}

	fflush(node_file);
	fclose(node_file);
}

void CRWMeshGen3D::SaveElements(std::vector<CElement> & elements, enum MeshSaveMode save_mode, const TCHAR * file_name)
{
#if !defined(_UNICODE)
	FILE *simples_file = fopen(file_name, _T("w+"));
#else
	FILE *simples_file = _wfopen(file_name, _T("w+"));
#endif

	if (simples_file == NULL)
		throw NativeExceptions::IOException(file_name, errno, _T("Error creating simples files"));

	setvbuf(simples_file, NULL, _IOFBF, 1024 * 1024); 
	//fprintf(simples_file, "%d\n", elements.size());
	//20.02.2010: Dopasowanie do CMatrix
	fprintf(simples_file, "%d\t%d\n", elements.size(), 4);

	std::vector<CElement>::iterator iter = elements.begin();
	std::vector<CElement>::iterator end = elements.end();
	if (save_mode == MeshSaveMode::SavePlainData)
		while (iter != end)
		{
			CElement * e = &*iter;
			fprintf(simples_file, "%5d %5d %5d %5d\n",
				e->GetNode1() + 1,
				e->GetNode2() + 1,
				e->GetNode3() + 1,
				//01/03/2010: Radek
				1);
			iter++;
		}
	else
		// MeshSaveMode::SaveSpatialData
		while (iter != end)
		{
			CElement * e = &*iter;
			fprintf(simples_file,  "%5d %5d %5d %5d\n", e->GetNode1() + 1, e->GetNode2() + 1, e->GetNode3() + 1, e->GetNode4() + 1);
			iter++;
		}
	fclose(simples_file);
}

void CRWMeshGen3D::SaveMaterials(std::vector<CElement> & elements, enum MeshSaveMode save_mode, const TCHAR * file_name)
{
#if !defined(_UNICODE)
	FILE *simples_file = fopen(file_name, _T("w+"));
#else
	FILE *simples_file = _wfopen(file_name, _T("w+"));
#endif

	if (simples_file == NULL)
		throw NativeExceptions::IOException(file_name, errno, _T("Error creating materials files"));

	setvbuf(simples_file, NULL, _IOFBF, 1024 * 1024); 
	// 20.02.2010: Dopasowanie do CMAtrix
	//fprintf(simples_file, "%d\n", elements.size());
	fprintf(simples_file, "%d\t%d\n", elements.size(), 1);

	std::vector<CElement>::iterator iter = elements.begin();
	std::vector<CElement>::iterator end = elements.end();
	if (save_mode == MeshSaveMode::SavePlainData)
		while (iter != end)
		{
			fprintf(simples_file, "%10.5f\n" , iter->GetMaterial());
			iter++;
		}
	else
		// MeshSaveMode::SaveSpatialData
		while (iter != end)
		{
			fprintf(simples_file, "%10.5f\n" , iter->GetMaterial());
			iter++;
		}
	fclose(simples_file);
}


void CRWMeshGen3D::ClearMesh(void)
{
	this->mesh_generated = false;
	//this->shield_active = false;
	//this->inner_active = false;

	this->nodes_on_ring.clear();
	this->plain_nodes.clear();
	this->spatial_nodes.clear();
	this->plain_elements.clear();
	this->rings.clear();
	this->electrodes.clear();
}

CRing * CRWMeshGen3D::GetNearestRing(double position)
{
	if (this->rings.empty())
		return NULL;
	
	std::vector<CRing>::iterator end = this->rings.end();
	CRing * nearest = &this->rings[0];
	for (std::vector<CRing>::iterator i = this->rings.begin(); i < end; i++)
	{
		if (abs((*i).radius - position) >= abs(nearest->radius - position))
			continue;
		nearest = &(*i);
	}
	
	return nearest;
}

double CRWMeshGen3D::SparseFunction(double x) const
{
	if (this->sparse_points.sparse_count == 0)
		return 0.0;

	if (x < this->sparse_points.sparse_radiuses[0])
		return 0.0; // lewa granica funkcji
	if (x > this->sparse_points.sparse_radiuses[this->sparse_points.sparse_count - 1])
		return 0.0; // prawa granica funkcji

	for (int i = 0; i < this->sparse_points.sparse_count - 1; i++)
		if ((x >= this->sparse_points.sparse_radiuses[i]) &&
			(x <= this->sparse_points.sparse_radiuses[i + 1]))
		{
			double al = this->sparse_points.sparse_values[i + 1] - this->sparse_points.sparse_values[i];
			double am = this->sparse_points.sparse_radiuses[i + 1] - this->sparse_points.sparse_radiuses[i];
			double a = al / am;

			return (al / am) * (x - this->sparse_points.sparse_radiuses[i]) + this->sparse_points.sparse_values[i];
		}

	throw NativeExceptions::Exception(_T("Cos jest nie tak..."));

}

std::vector<CNode> & CRWMeshGen3D::GetPlainNodes(void)
{
	return this->plain_nodes;
}

std::vector<CNode> & CRWMeshGen3D::GetSpatialNodes(void)
{
	return this->spatial_nodes;
}

std::vector<CElement> & CRWMeshGen3D::GetPlainElements(void)
{
	return this->plain_elements;
}

std::vector<CElement> & CRWMeshGen3D::GetSpatialElements(void)
{
	return this->spatial_elements;
}

std::vector<CElement> & CRWMeshGen3D::GetElectrodesSurfaceElements(void)
{
	return this->electrodes_surface;
}

std::vector<CElement> & CRWMeshGen3D::GetOuterShieldSurfaceElements(void)
{
	return this->outer_shield_surface;
}

void CRWMeshGen3D::ExtractMesh(std::vector<CNode> & nodes, std::vector<CElement> & elements, std::vector<CElement> & extracted_elements, NodeFlag::Enum flag)
{
	// extract the elements from whole mesh
	// each extracted element should have all 3/4 nodes marked as ReconstructionNodes
	TRACE0("ExtractReconstructionMesh");
	std::vector<CElement>::iterator elem = elements.begin();
	for(; elem != elements.end(); elem++)
	{
		CNode* n1 = &nodes[elem->GetNode1()];
		CNode* n2 = &nodes[elem->GetNode2()];
		CNode* n3 = &nodes[elem->GetNode3()];
		CNode* n4 = &nodes[elem->GetNode4()];
		//TODO: for 2D plain meshes, n4 is always zero, so it points to central node in (0, 0) - first entry in NODES collection
		//It's marked as ReconstructionNode every time,
		//but assuming this, in some circumstances, can lead to erroneous output

		if ((n1->flags & flag) == 0) continue;
		if ((n2->flags & flag) == 0) continue;
		if ((n3->flags & flag) == 0) continue;
		if ((n4->flags & flag) == 0) continue;
		//if ((n1->flags & NodeFlags::ReconstructionMeshNode) == 0) continue;
		//if ((n2->flags & NodeFlags::ReconstructionMeshNode) == 0) continue;
		//if ((n3->flags & NodeFlags::ReconstructionMeshNode) == 0) continue;
		//if ((n4->flags & NodeFlags::ReconstructionMeshNode) == 0) continue;

		// and update it with new node indices
		CElement element(elem->GetNode1(), elem->GetNode2(), elem->GetNode3(), elem->GetNode4());
		element.SetMaterial(elem->GetMaterial());
		extracted_elements.push_back(element);
	}
	TRACE0("ExtractReconstructionMesh - finished");
}

double CRWMeshGen3D::NormalizeMaterial(double material) const
{
	if (this->sparse_points.rings_count == 0)
		return min(1, max(0, material));

	return (material - this->sparse_points.ring_material_min) / (this->sparse_points.ring_material_max - this->sparse_points.ring_material_min);
}

//
//
//
int CRWMeshGen3D::GetNumberOfPlainElectrodes(void) const
{
	return this->electrodes.size();
}

int CRWMeshGen3D::GetNumberOfPlainElectrodeElements(int electrode_number) const
{
	if (electrode_number >= this->electrodes.size())
		return 0;
	return this->electrodes[electrode_number].size();
}

std::vector<CPlainElectrodeEdge> & CRWMeshGen3D::GetElectrode(int electrode_number)
{
	return this->electrodes[electrode_number];
}

CPlainElectrodeEdge* CRWMeshGen3D::GetFirstPlainElectrodeElement(int electrode_number)
{
	if (electrode_number >= this->electrodes.size())
		return NULL;
	if (this->electrodes[electrode_number].size() == 0)
		return NULL;

	return &this->electrodes[electrode_number][0];
}

void CRWMeshGen3D::GeneratePlainElectrodesDistribution(void)
{
	if (this->rings.empty())
		return;

	CRing* ring_ptr = this->GetNearestRing(this->plain_electrodes.ring_distance);
	CNode* node_ptr = this->GetFirstPlainNode();
	int ring = ring_ptr - &this->rings[0];

	// czy starczy miejsca na tyle elektrod?
	if ((this->plain_electrodes.nodes_per_electrode /*+ 1*/) * this->plain_electrodes.count > this->GetNumberOfNodesOnRing(ring))
		return; // nie.. nie starczy

	std::vector<int> & ring_nodes = this->nodes_on_ring[ring];

	int nodes_in_use = this->plain_electrodes.nodes_per_electrode * this->plain_electrodes.count;
	int free_nodes = ring_nodes.size() - nodes_in_use;

	int base = (int)((double)free_nodes / (double)this->plain_electrodes.count);
	int rest = free_nodes - base * this->plain_electrodes.count;
	double delta = (double)rest / (double)this->plain_electrodes.count;

	this->electrodes.resize(this->plain_electrodes.count);

	#define NODE(n) ring_nodes[(n) % ring_nodes.size()]

	int current_node = 0;
	double current_delta = 0.0;
	for (int ielectrode = 0; ielectrode < this->plain_electrodes.count; ielectrode++)
	{
		for (int inode = 0; inode < this->plain_electrodes.nodes_per_electrode - 1; inode++)
		{
			int n1 = NODE(current_node + inode);
			int n2 = NODE(current_node + inode + 1);
			this->electrodes[ielectrode].push_back(CPlainElectrodeEdge(n1, n2, ielectrode + 1));
		}
		current_node += this->plain_electrodes.nodes_per_electrode;
		current_node += base;

		current_delta += delta;
		if (current_delta >= 1)
		{
			current_delta -= 1;
			current_node++;
		}
	}

	// a teraz generacja ekranu, jeœli taki jest
	if (this->plain_electrodes.shield_distance <= 0)
		return;

	this->electrodes.resize(this->plain_electrodes.count + 1);
	ring_ptr = this->GetNearestRing(this->plain_electrodes.shield_distance);
	node_ptr = this->GetFirstPlainNode();
	ring = ring_ptr - &this->rings[0];

	ring_nodes = this->nodes_on_ring[ring];

	for (int inode = 0; inode < ring_nodes.size(); inode++)
	{
		int n1 = NODE(inode);
		int n2 = NODE(inode + 1);

		this->electrodes[this->electrodes.size() - 1].push_back(CPlainElectrodeEdge(n1, n2, 1000));
	}

}

void CRWMeshGen3D::SetPlainElectrodesDistributionParameters(double ring_radius, double shield_radius, int electrodes, int nodes_per_electrode)
{
	this->plain_electrodes.ring_distance = ring_radius;
	this->plain_electrodes.shield_distance = shield_radius;
	this->plain_electrodes.count = electrodes;
	this->plain_electrodes.nodes_per_electrode = nodes_per_electrode;
}

void CRWMeshGen3D::SaveDistribution(const TCHAR* file_name)
{

#if !defined(_UNICODE)
	FILE *file = fopen(file_name, _T("w+"));
#else
	FILE *file = _wfopen(file_name, _T("w+"));
#endif

	int max_count = 0;
	std::vector<std::vector<CPlainElectrodeEdge>>::iterator i;

	for(i = this->electrodes.begin(); i != this->electrodes.end(); i++)
		max_count = max(max_count, (*i).size());

	// 20.02.2010: Dopasowanie do CMatrix
	//fprintf(file, "%d\t%d\n", max_count * 2, this->plain_electrodes.count);
	//fprintf(file, "%d\t%d\n", this->plain_electrodes.count, max_count * 2);
	fprintf(file, "%d\t%d\n", this->electrodes.size(), max_count * 2);
	for(int i = 0; i < this->electrodes.size(); i++)
	{
		for(int j = 0; j < this->electrodes[i].size(); j++)
			fprintf(file, "%d %d  ", this->electrodes[i][j].GetNode1() + 1, this->electrodes[i][j].GetNode2() + 1);
		for (int j = this->electrodes[i].size(); j < max_count; j++)
			fprintf(file, "%d %d  ", 0, 0);

		fprintf(file, "\n");
	}

	fclose(file);
}

