// This is the main DLL file.

#include "stdafx.h"

#include "MeshGeneratorWrapper.h"
#include "..\libMeshGen\libMeshGen.h"
#include "..\MatrixInterface\Matrix.h"
#include "..\MatrixInterface\VTKWriter.h"


using namespace System;
using namespace System::Runtime::InteropServices;

namespace Geometry {

	public value struct RingStruct
	{
	public:
		double radius;
		double density;
		bool fixed;

		bool error;
	};

	public value struct PlainElectrodeEdge
	{
	public:
		int node1;
		int node2;
		int electrode;
	};


	public enum NodeFlags
	{
		Empty					= 0x00000000,
		//NormalNode				= 0x00000001,
		ReconstructionMeshNode	= 0x00000010,
		
		ElectrodesSurfaceNode	= 0x00000020,
		ElectrodesMeshNode		= 0x00000800,

		OuterShieldSurfaceNode	= 0x00000040,
		CenterOfGeometryNode	= 0x80000000,
	};

	public value struct NodeStruct
	{
	public:
		double radius;
		double angle;
		
		double x;
		double y;
		double z;

		NodeFlags flags;
	};

	public value struct ElementStruct
	{
	public:
		int node1;
		int node2;
		int node3;
		int node4;
		double material;
		int tag;
	};

	public ref class MeshGeneratorWrapper
	{
	public:
		CRWMeshGen3D *mesher;
		//int x;
		MeshGeneratorWrapper()
		{
			// translation between:
			// UNMANAGED				MANAGED	
			assert(sizeof(CRing) == sizeof(RingStruct));
			assert(sizeof(CNode) == sizeof(NodeStruct));
			assert(sizeof(CElement) == sizeof(ElementStruct));
			assert(sizeof(CPlainElectrodeEdge) == sizeof(PlainElectrodeEdge));

			this->mesher = NULL;
			this->Cleanup();

			//int a = sizeof(NodeFlags);
			//a = 

		}

		void Cleanup(void)
		{
			if (this->mesher != NULL)
				delete this->mesher;

			this->mesher = new CRWMeshGen3D();
		}

		void SetSparseFunctionPoints(array<double>^ rpositions , array<double>^ svalues)
		{
			if (rpositions->Length != svalues->Length)
				throw gcnew ArgumentException("Both arrays should have equal number of elements");

			pin_ptr<double> r_ptr = &rpositions[0];
			pin_ptr<double> s_ptr = &svalues[0];
			this->mesher->SetSparseFunctionPoints(r_ptr, s_ptr, svalues->Length);
		}

		void SetRingsAndMaterials(array<double>^ rings , array<double>^ permittivities)
		{
			if (rings->Length != permittivities->Length)
				throw gcnew ArgumentException("Both arrays should have equal number of elements");

			pin_ptr<double> r_ptr = &rings[0];
			pin_ptr<double> p_ptr = &permittivities[0];
			this->mesher->SetRingsAndMaterials(r_ptr, p_ptr, permittivities->Length);
		}

		void SetSpatialParameters(double height, double vertical_density)
		{
			this->mesher->SetSpatialParameters(height, vertical_density);
		}

		void GenerateSpatialMesh(void)
		{
			this->mesher->GeneratePlainMesh();
			this->mesher->GenerateSpatialMesh(); // yeah, it's easy now :D
		}

		void GeneratePlainMesh(void)
		{
			this->mesher->GeneratePlainMesh();
		}
	
		void InternalSaveData(System::String^ nodes_file, System::String^ simples_file, System::String^ materials_file, std::vector<CNode> & nodes, std::vector<CElement> & elements, MeshSaveMode save_mode)
		{
			if (!System::String::IsNullOrEmpty(nodes_file))
			{
				wchar_t* fnodes = (wchar_t*)Marshal::StringToHGlobalUni(nodes_file).ToPointer();
				this->mesher->SaveNodes(nodes, save_mode, fnodes);
				Marshal::FreeHGlobal(System::IntPtr(fnodes));
			}

			if (!System::String::IsNullOrEmpty(simples_file))
			{
				wchar_t* fsamples = (wchar_t*)Marshal::StringToHGlobalUni(simples_file).ToPointer();
				this->mesher->SaveElements(elements, save_mode, fsamples);
				Marshal::FreeHGlobal(System::IntPtr(fsamples));
			}

			if (!String::IsNullOrEmpty(materials_file))
			{
				wchar_t* fmaterials = (wchar_t*)Marshal::StringToHGlobalUni(materials_file).ToPointer();
				this->mesher->SaveMaterials(elements, save_mode, fmaterials);
				Marshal::FreeHGlobal(System::IntPtr(fmaterials));
			}
		}

		delegate void GetFloatDataDelegate(int index, float* coords);
		delegate void GetIntegerDataDelegate(int index, int* nodes);
		static CNode* __points;
		static CElement* __cells;

		static void GetPointCallback(int index, float* coords)
		{
			CNode* node_ptr = &__points[index];
			coords[0] = node_ptr->GetX();
			coords[1] = node_ptr->GetY();
			coords[2] = node_ptr->GetZ();
		}

		static void GetElementCallback(int index, int* nodes)
		{
			CElement* element_ptr = &__cells[index];
			nodes[0] = element_ptr->GetNode1();
			nodes[1] = element_ptr->GetNode2();
			nodes[2] = element_ptr->GetNode3();
			nodes[3] = element_ptr->GetNode4();
		}

		static void GetMaterialCallback(int index, float* material)
		{
			*material = __cells[index].GetMaterial();
		}

		void InternalSaveDataVTK(System::String^ vtk_file, std::vector<CNode> & nodes, std::vector<CElement> & elements, MeshSaveMode save_mode, Boolean save_materials)
		{
			wchar_t* fname = (wchar_t*)Marshal::StringToHGlobalUni(vtk_file).ToPointer();

			CVTKWriter wr;
			if (!wr.OpenOutputFile(fname))
				throw gcnew System::IO::IOException("An error has occured during creationof VTK file");

			__points = &nodes[0];
			__cells = &elements[0];

			wr.WriteHeader("Ca³a siatka");

			TRACE0("InternalSaveDataVTK - checkpoint 1");
			GetFloatDataDelegate^ point_del = gcnew GetFloatDataDelegate(&GetPointCallback);
			GetFloatDataDelegate^ mat_del = gcnew GetFloatDataDelegate(&GetMaterialCallback);
			GetIntegerDataDelegate^ cell_del = gcnew GetIntegerDataDelegate(&GetElementCallback);

			TRACE0("InternalSaveDataVTK - checkpoint 2");
			wr.WritePoints(nodes.size(), static_cast<GET_FLOAT_DATA_CALLBACK>(Marshal::GetFunctionPointerForDelegate(point_del).ToPointer()));

			TRACE0("InternalSaveDataVTK - checkpoint 3");
			wr.WriteCells(elements.size(), static_cast<GET_INTEGER_DATA_CALLBACK>(Marshal::GetFunctionPointerForDelegate(cell_del).ToPointer()), VTK_TETRA);

			TRACE0("InternalSaveDataVTK - checkpoint 4");
			wr.WriteCellTypes(elements.size(), VTK_TETRA);

			TRACE0("InternalSaveDataVTK - checkpoint 5a");
			wr.WritePointData("Elementy_siatki", "ElectrodesColor", nodes.size(), 1.0f);

			if (save_materials)
			{
				TRACE0("InternalSaveDataVTK - checkpoint 5b");
				wr.WriteCellData("Materialy", "default", elements.size(), static_cast<GET_FLOAT_DATA_CALLBACK>(Marshal::GetFunctionPointerForDelegate(mat_del).ToPointer()));
			}

			TRACE0("InternalSaveDataVTK - checkpoint 6");
			wr.WriteColorTable("ElectrodesColor", 0, 0, 0, 0.25, 0.25, 0.25);


			wr.CloseFile();
			Marshal::FreeHGlobal(System::IntPtr(fname));

			GC::KeepAlive(point_del);
			GC::KeepAlive(mat_del);
			GC::KeepAlive(cell_del);
		}

		void InternalSaveSpatialPlainVTK(System::String^ vtk_file, std::vector<CNode> & nodes, std::vector<CElement> & elements)
		{
			wchar_t* fname = (wchar_t*)Marshal::StringToHGlobalUni(vtk_file).ToPointer();

			CVTKWriter wr;
			if (!wr.OpenOutputFile(fname))
				throw gcnew System::IO::IOException("An error has occured during creationof VTK file");

			__points = &nodes[0];
			__cells = &elements[0];

			wr.WriteHeader("Powierzchnia plaska");

			TRACE0("InternalSaveDataVTK - checkpoint 1");
			GetFloatDataDelegate^ point_del = gcnew GetFloatDataDelegate(&GetPointCallback);
			GetIntegerDataDelegate^ cell_del = gcnew GetIntegerDataDelegate(&GetElementCallback);

			TRACE0("InternalSaveDataVTK - checkpoint 2");
			wr.WritePoints(nodes.size(), static_cast<GET_FLOAT_DATA_CALLBACK>(Marshal::GetFunctionPointerForDelegate(point_del).ToPointer()));

			TRACE0("InternalSaveDataVTK - checkpoint 3");
			wr.WriteCells(elements.size(), static_cast<GET_INTEGER_DATA_CALLBACK>(Marshal::GetFunctionPointerForDelegate(cell_del).ToPointer()), VTK_TRIANGLE);

			TRACE0("InternalSaveDataVTK - checkpoint 4");
			wr.WriteCellTypes(elements.size(), VTK_TRIANGLE);

			TRACE0("InternalSaveDataVTK - checkpoint 5a");
			wr.WritePointData("Elementy_siatki", "ElectrodesColor", nodes.size(), 1.0f);

			wr.WriteCellData("Materialy", "default", elements.size(), 1.0f);

			TRACE0("InternalSaveDataVTK - checkpoint 6");
			wr.WriteColorTable("ElectrodesColor", 0, 0, 0, 0.25, 0.25, 0.25);

			wr.CloseFile();
			Marshal::FreeHGlobal(System::IntPtr(fname));

			GC::KeepAlive(point_del);
			GC::KeepAlive(cell_del);
		}

		void SaveSpatialDataVTK(String^ vtk_file, String^ shield_file, String^ electrodes_file, Boolean save_materials)
		{
			this->InternalSaveDataVTK(vtk_file, 
				this->mesher->GetSpatialNodes(), this->mesher->GetSpatialElements(),
				MeshSaveMode::SaveSpatialData, save_materials);

			this->InternalSaveSpatialPlainVTK(shield_file,
				this->mesher->GetSpatialNodes(),
				this->mesher->GetOuterShieldSurfaceElements());

			this->InternalSaveSpatialPlainVTK(electrodes_file,
				this->mesher->GetSpatialNodes(),
				this->mesher->GetElectrodesSurfaceElements());
		}

		void SaveSpatialReconstructionDataVTK(System::String^ vtk_file, Boolean save_materials) 
		{
			std::vector<CNode> nodes;
			std::vector<CElement> elements;
			TRACE0("SaveSpatialReconstructionDataVTK");
			this->mesher->ExtractMesh(this->mesher->GetSpatialNodes(), this->mesher->GetSpatialElements(), elements, NodeFlag::ReconstructionMeshNode);
			this->InternalSaveDataVTK(vtk_file, this->mesher->GetSpatialNodes(), elements, MeshSaveMode::SaveSpatialData, save_materials);
		}

		void SaveSpatialElectrodesDataVTK(System::String^ vtk_file, Boolean save_materials) 
		{
			std::vector<CNode> nodes;
			std::vector<CElement> elements;
			TRACE0("SaveSpatialReconstructionDataVTK");
			this->mesher->ExtractMesh(this->mesher->GetSpatialNodes(), this->mesher->GetSpatialElements(), elements, NodeFlag::ElectrodesMeshNode);
			this->InternalSaveDataVTK(vtk_file, this->mesher->GetSpatialNodes(), elements, MeshSaveMode::SaveSpatialData, save_materials);
		}


		void SavePlainData(System::String^ nodes_file, System::String^ simples_file, System::String^ materials_file)
		{
			this->InternalSaveData(nodes_file, simples_file, materials_file, 
				this->mesher->GetPlainNodes(), this->mesher->GetPlainElements(),
				MeshSaveMode::SavePlainData);
		}

		void SaveSpatialData(System::String^ nodes_file, System::String^ simples_file, System::String^ materials_file)
		{
			this->InternalSaveData(nodes_file, simples_file, materials_file, 
				this->mesher->GetSpatialNodes(), this->mesher->GetSpatialElements(),
				MeshSaveMode::SaveSpatialData);
		}

		void SaveSpatialReconstructionData(System::String^ nodes_file, System::String^ simples_file, System::String^ materials_file) 
		{
			std::vector<CElement> elements;
			this->mesher->ExtractMesh(this->mesher->GetSpatialNodes(), this->mesher->GetSpatialElements(), elements, NodeFlag::ReconstructionMeshNode);
			this->InternalSaveData(nodes_file, simples_file, materials_file, this->mesher->GetSpatialNodes(), elements, MeshSaveMode::SaveSpatialData);
		}

		void SaveSpatialElectrodesData(System::String^ nodes_file, System::String^ simples_file, System::String^ materials_file) 
		{
			std::vector<CElement> elements;
			this->mesher->ExtractMesh(this->mesher->GetSpatialNodes(), this->mesher->GetSpatialElements(), elements, NodeFlag::ElectrodesMeshNode);
			this->InternalSaveData(nodes_file, simples_file, materials_file, this->mesher->GetSpatialNodes(), elements, MeshSaveMode::SaveSpatialData);
		}


		void SavePlainReconstructionData(System::String^ nodes_file, System::String^ simples_file, System::String^ materials_file) 
		{
			std::vector<CElement> elements;
			this->mesher->ExtractMesh(this->mesher->GetPlainNodes(), this->mesher->GetPlainElements(), elements, NodeFlag::ReconstructionMeshNode);
			this->InternalSaveData(nodes_file, simples_file, materials_file, this->mesher->GetPlainNodes(), elements, MeshSaveMode::SavePlainData);
		}

		void SavePlainElectrodesData(System::String^ nodes_file, System::String^ simples_file, System::String^ materials_file) 
		{
			std::vector<CElement> elements;
			this->mesher->ExtractMesh(this->mesher->GetPlainNodes(), this->mesher->GetPlainElements(), elements, NodeFlag::ElectrodesMeshNode);
			this->InternalSaveData(nodes_file, simples_file, materials_file, this->mesher->GetPlainNodes(), elements, MeshSaveMode::SavePlainData);
		}


		double GetRadius(void)
		{
			return this->mesher->GetRadius();
		}

#pragma region GetNumberOf...

		int GetNumberOfPlainNodes(void)
		{
			return this->mesher->GetNumberOfPlainNodes();
		}

		int GetNumberOfPlainElements(void)
		{
			return this->mesher->GetNumberOfPlainElements();
		}

		int GetNumberOfSpatialNodes(void)
		{
			return this->mesher->GetNumberOfSpatialNodes();
		}

		int GetNumberOfSpatialElements(void)
		{
			return this->mesher->GetNumberOfSpatialElements();
		}

		int GetNumberOfRings(void)
		{
			return this->mesher->GetNumberOfRings();
		}

		int GetNumberOfNodesOnRing(int ring_number)
		{
			return this->mesher->GetNumberOfNodesOnRing(ring_number);
		}

		int GetNumberOfSpatialLayers(void)
		{
			return this->mesher->GetNumberOfSpatialLayers();
		}

#pragma endregion


		RingStruct* GetFirstRing(void)
		{
			return  (RingStruct*)this->mesher->GetFirstRing();
		}

		NodeStruct* GetFirstPlainNode(void)
		{
			return (NodeStruct*)this->mesher->GetFirstPlainNode();
		}

		NodeStruct* GetFirstSpatialNode(void)
		{
			return (NodeStruct*)this->mesher->GetFirstSpatialNode();
		}

		ElementStruct* GetFirstPlainElement(void)
		{
			return (ElementStruct*)this->mesher->GetFirstPlainElement();
		}

		double SparseFunction(double X)
		{
			return this->mesher->SparseFunction(X);
		}

		double NormalizeMaterial(double material)
		{
			return this->mesher->NormalizeMaterial(material);
		}


		////////////

		void SetPlainElectrodesDistributionParameters(double ring_radius, double shield_radius, int electrodes, int nodes_per_electrode)
		{
			if (nodes_per_electrode < 2)
				throw gcnew ArgumentException("Nodes per electrode should be at least of value 2");

			this->mesher->SetPlainElectrodesDistributionParameters(ring_radius, shield_radius, electrodes, nodes_per_electrode);
		}

		int GetNumberOfPlainElectrodes(void)
		{
			return this->mesher->GetNumberOfPlainElectrodes();
		}

		int GetNumberOfPlainElectrodeElements(int electrode_number)
		{
			return this->mesher->GetNumberOfPlainElectrodeElements(electrode_number);
		}

		PlainElectrodeEdge* GetFirstPlainElectrodeElement(int electrode_number)
		{
			return (PlainElectrodeEdge*)this->mesher->GetFirstPlainElectrodeElement(electrode_number);
		}

		void GeneratePlainElectrodesDistribution(void)
		{
			this->mesher->GeneratePlainElectrodesDistribution();
		}

		void SaveDistribution(String^ file)
		{
			wchar_t* fname = (wchar_t*)Marshal::StringToHGlobalUni(file).ToPointer();
			this->mesher->SaveDistribution(fname);
			Marshal::FreeHGlobal(System::IntPtr(fname));
		}
	};



}
