// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LIBMESHGEN_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LIBMESHGEN_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef LIBMESHGEN_EXPORTS
#define LIBMESHGEN_API __declspec(dllexport)
#define FLAGS_ATTRIB
#else
#define LIBMESHGEN_API __declspec(dllimport)
#define FLAGS_ATTRIB [Flags]
#endif

#include "stdafx.h"

#undef TRACE0
#define TRACE0(s) printf("%s\n", (s));


#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

class CNode;
class CElement;
class CRing;
class CPlainElectrodeEdge;

enum MeshSaveMode;
enum MeshingMode;

class NodeFlag
{
public:
	enum Enum : DWORD
	{
		Empty					= 0x00000000,
		//NormalNode				= 0x00000001,
		ReconstructionMeshNode	= 0x00000010,
		
		ElectrodesSurfaceNode	= 0x00000020,
		ElectrodesMeshNode		= 0x00000800,

		OuterShieldSurfaceNode	= 0x00000040,
		CenterOfGeometryNode	= 0x80000000,
	};
};

class ElementFlags
{
public:
	enum Enum : DWORD
	{
		NormalElement			= 0x00000000,
		ReconstructionElement	= 0x80000000,
	};
};

class LIBMESHGEN_API CRWMeshGen3D
{
private:
	bool mesh_generated;

	std::vector<std::vector<int>> nodes_on_ring;
	std::vector<CNode> plain_nodes;
	std::vector<CNode> spatial_nodes;
	std::vector<CElement> plain_elements;
	std::vector<CElement> spatial_elements;
	std::vector<CRing> rings;

	std::vector<CElement> outer_shield_surface;
	std::vector<CElement> electrodes_surface;

	std::vector<std::vector<CPlainElectrodeEdge>> electrodes;

	struct SPARSE_POINTS
	{
		double *sparse_radiuses;
		double *sparse_values;
		int sparse_count;

		double *rings_radiuses;
		double *rings_materials;
		int rings_count;

		double ring_material_min;
		double ring_material_max;
	} sparse_points;

	// vital data for creating the third dimention
	double height, vertical_density;

	int AddNode(int ring_number, double radius, double angle, NodeFlag::Enum flags);
	int GetNode(int ring_number, int node_number);

public:
	double reconstruction_mesh_radius;
	double electrode_surface_radius;
	double outer_shield_radius;

	struct 
	{
		double ring_distance;
		double shield_distance;
		int count;
		int nodes_per_electrode;
	} plain_electrodes;

	CRWMeshGen3D(void);
	~CRWMeshGen3D();

	//void SetDensity(double horizontal, double shield);
	void SetSpatialParameters(double height, double vertical_density);

	//void SetShieldDistance(double distance, const bool shield_active = FALSE);
	//void SetInnerWallDistance(double distance, const bool inner_active = FALSE);
	//void SetOuterWallDistance(double distance);

	void SetSparseFunctionPoints(const double *radius_array, const double *sparse_array, int count);
	void SetRingsAndMaterials(const double *radius_array, const double *material_array, int count);

	void GeneratePlainMesh(void);
	void GenerateSpatialMesh(void);
	//void ExtractReconstructionMesh(std::vector<CNode> & nodes, std::vector<CElement> & elements, std::vector<CNode> & extracted_nodes, std::vector<CElement> & extracted_elements);
	//void ExtractReconstructionMesh(std::vector<CNode> & nodes, std::vector<CElement> & elements, std::vector<CElement> & extracted_elements);
	void ExtractMesh(std::vector<CNode> & nodes, std::vector<CElement> & elements, std::vector<CElement> & extracted_elements, NodeFlag::Enum flag);

	void ClearMesh(void);

	void SaveNodes(std::vector<CNode> & nodes, enum MeshSaveMode save_mode, const TCHAR * file_name);
	void SaveElements(std::vector<CElement> & elements, enum MeshSaveMode save_mode, const TCHAR * file_name);
	void SaveMaterials(std::vector<CElement> & elements, enum MeshSaveMode save_mode, const TCHAR * file_name);


	double GetRadius(void) const;
	//double GetMaxRadius(void) const;

	int GetNumberOfRings(void) const;
	int GetNumberOfNodesOnRing(int ring_no) const;

	int GetNumberOfPlainNodes(void) const;
	int GetNumberOfPlainElements(void) const;
	CNode * GetFirstPlainNode(void);
	CElement * GetFirstPlainElement(void);

	int GetNumberOfSpatialLayers(void) const;
	int GetNumberOfSpatialNodes(void) const;
	int GetNumberOfSpatialElements(void) const;
	CNode* GetFirstSpatialNode(void);
	CElement* GetFirstSpatialElement(void);


	CRing * GetFirstRing(void);
	CRing * GetNearestRing(double position);
	int GetNearestFixedRing(double radius) const;

	std::vector<CNode> & GetPlainNodes(void);
	std::vector<CNode> & GetSpatialNodes(void);
	std::vector<CElement> & GetPlainElements(void);
	std::vector<CElement> & GetSpatialElements(void);
	std::vector<CElement> & GetElectrodesSurfaceElements(void);
	std::vector<CElement> & GetOuterShieldSurfaceElements(void);

	double SparseFunction(double x) const;
	double NormalizeMaterial(double material) const;

	//
	int GetNumberOfPlainElectrodes(void) const;
	int GetNumberOfPlainElectrodeElements(int electrode_number) const;
	CPlainElectrodeEdge* GetFirstPlainElectrodeElement(int electrode_number);
	std::vector<CPlainElectrodeEdge> & GetElectrode(int electrode_number);
	void SetPlainElectrodesDistributionParameters(double ring_radius, double shield_radius, int electrodes, int nodes_per_electrode);
	void GeneratePlainElectrodesDistribution(void);
	void SaveDistribution(const TCHAR* file_name);
};

class LIBMESHGEN_API CNode
{
private:
		void RecalcCoords(void);

public:
	struct
	{
		double radius;
		double angle;
		
		double x;
		double y;
		double z;

		NodeFlag::Enum flags;
	};

	CNode(const CNode& node)
	{
		this->radius = node.radius;
		this->angle = node.angle;
		this->x = node.x;
		this->y = node.y;
		this->z = node.z;
		this->flags = node.flags;
	}

	CNode& operator=(const CNode &node)
	{
		this->radius = node.radius;
		this->angle = node.angle;
		this->x = node.x;
		this->y = node.y;
		this->z = node.z;
		this->flags = node.flags;
		return *this;
	}


	CNode()	: radius(0), x(0), y(0), z(0), flags(NodeFlag::Empty) { };
	CNode(int ring_number, double radius, double angle, NodeFlag::Enum flags);

	void SetAngle(double angle);
	void SetRadius(double radius);
	double static Distance(const class CNode * node1, const class CNode * node2);
	double static Distance(const class CNode * node1, double x, double y);
	double static Distance(const class CNode * node1, double x, double y, double z);

	void SetX(double x) { this->x = x; }
	void SetY(double y) { this->y = y; }
	void SetZ(double z) { this->z = z; }

	double GetX(void) const throw() { return this->x; }
	double GetY(void) const throw() { return this->y; }
	double GetZ(void) const throw() { return this->z; }

	double GetLength(void) const throw()
	{
		return sqrt(this->GetX() * this->GetX() + this->GetY() * this->GetY() + this->GetZ() * this->GetZ());
	}

	void Normalize(void) throw()
	{
		double l = this->GetLength();
		this->SetX(this->GetX() / l);
		this->SetY(this->GetY() / l);
		this->SetZ(this->GetZ() / l);
	}

	CNode operator- (const CNode& n)
	{
		CNode res;
		res.SetX(this->GetX() - n.GetX());
		res.SetY(this->GetY() - n.GetY());
		res.SetZ(this->GetZ() - n.GetZ());
		return res;
	}

public:
	static CNode Center(const CNode& n1, const CNode& n2, const CNode& n3)
	{
		CNode n;
		n.SetX((n1.GetX() + n2.GetX() + n3.GetX()) / 3.0);
		n.SetY((n1.GetY() + n2.GetY() + n3.GetY()) / 3.0);
		n.SetZ((n1.GetZ() + n2.GetZ() + n3.GetZ()) / 3.0);
		return n;
	}

	static CNode Center(const CNode& n1, const CNode& n2, const CNode& n3, const CNode& n4)
	{
		CNode n;
		n.SetX((n1.GetX() + n2.GetX() + n3.GetX() + n4.GetX()) / 4.0);
		n.SetY((n1.GetY() + n2.GetY() + n3.GetY() + n4.GetY()) / 4.0);
		n.SetZ((n1.GetZ() + n2.GetZ() + n3.GetZ() + n4.GetZ()) / 4.0);
		return n;
	}

	static CNode CrossProduct(const CNode& base, const CNode& nA, const CNode& nB)
	{
		CNode n;
	    n.SetX((nA.GetY() - base.GetY()) * (nB.GetZ() - base.GetZ()) - (nA.GetZ() - base.GetZ()) * (nB.GetY() - base.GetY()));
        n.SetY((nA.GetZ() - base.GetZ()) * (nB.GetX() - base.GetX()) - (nA.GetX() - base.GetX()) * (nB.GetZ() - base.GetZ()));
        n.SetZ((nA.GetX() - base.GetX()) * (nB.GetY() - base.GetY()) - (nA.GetY() - base.GetY()) * (nB.GetX() - base.GetX()));
		return n;
	}

	static double DotProduct(const CNode& n1, const CNode& n2) throw()
	{
		return n1.GetX() * n2.GetX() + n1.GetY() * n2.GetY() + n1.GetZ() * n2.GetZ();
	}

};


class LIBMESHGEN_API CElement
{
public:
	struct
	{
		int node1;
		int node2;
		int node3;
		int node4;
		double material;
	};

	int tag;


	CElement()
		: node1(0), node2(0), node3(0), node4(0), material(0) { };

	// constructor for triangles (plain elements)
	CElement(const int n1, const int n2, const int n3, const int n4)
		: node1(n1), node2(n2), node3(n3), node4(n4), material(0) { };

	// constructor for simplexes (spatial elements)
	CElement(const int n1, const int n2, const int n3)
		: node1(n1), node2(n2), node3(n3), node4(0), material(0) { };

	// get edge ratio for a triangle based of first 3 nodes
	double GetEdgeRatio3(std::vector<CNode> * node_list) const;

	// get an edge ratio for a simplex (tetrahedron) based on all four nodes
	double GetEdgeRatio4(std::vector<CNode> * node_list) const;

	// indicates cursor presence in triangle based on first 3 nodes
	bool PointInElement3(CNode * nodes, const double x, const double y) const;

	void SetMaterial(double material)
	{
		this->material = material;
	}

	double GetMaterial(void) const
	{
		return this->material;
	}

	int GetNode1(void) const throw() { return this->node1; }	
	int GetNode2(void) const throw() { return this->node2; }	
	int GetNode3(void) const throw() { return this->node3; }	
	int GetNode4(void) const throw() { return this->node4; }	

	void SetNode1(int node) throw() { this->node1 = node; }
	void SetNode2(int node) throw() { this->node2 = node; }
	void SetNode3(int node) throw() { this->node3 = node; }
	void SetNode4(int node) throw() { this->node4 = node; }

	void SetTag(int tag) throw() { this->tag = tag; }
	int GetTag(void) const throw() { return this->tag; }
};

class LIBMESHGEN_API CRing
{
public:
	struct
	{
		double radius;
		double density;
		bool fixed;

		bool error;
	};

	CRing()
		: radius(0.0), fixed(false), density(0.0), error(false)
	{
	}

	CRing(const double radius, const double density, bool fixed = false)
		: radius(radius), density(density), fixed(fixed), error(false)
	{
	}

	void SetRadius(double new_radius)
	{
		this->radius = new_radius;
	}

	void SetError(bool error_flag)
	{
		this->error = error_flag;
	}

	bool GerError(void)
	{
		return this->error;
	}

};

enum MeshSaveMode
{
	SavePlainData,
	SaveSpatialData
};

class LIBMESHGEN_API CPlainElectrodeEdge
{
	struct
	{
		int node1;
		int node2;
		int electrode;
	};

public:
	CPlainElectrodeEdge()
		: node1(0), node2(0), electrode(0)
	{
	}

	CPlainElectrodeEdge(int n1, int n2, int el_id)
		: node1(n1), node2(n2), electrode(el_id)
	{
	}


	int GetNode1(void) const throw() { return this->node1; }
	int GetNode2(void) const throw() { return this->node2; }
	int GetElectrode(void) const throw() { return this->electrode; }

};

//enum MeshingMode
//{
//	// Normal: RADIUS_DELTA = shield_density, DENSITY = shield_density
//	MeshingMode_Normal,
//
//	// Linear: RADIUS_DELTA = const, DENSITY = coef * current_radius
//	MeshingMode_Linear_Rconst_Dvar,
//
//	// Linear: RADIUS_DELTA = coef * current_radius, DENSITY = coef * current_radius
//	MeshingMode_Linear_Rvar_Dvar,
//
//	// Exponential: RADIUS_DELTA = const, DENSITY = coef * current_radius ^ exponent
//	MeshingMode_Exponential_Rconst_Dvar,
//
//	// Exponential: RADIUS_DELTA = coef * current_radius ^ exponent, DENSITY = coef * current_radius ^ exponent
//	MeshingMode_Exponential_Rvar_Dvar,
//};

