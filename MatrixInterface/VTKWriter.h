#pragma once
//#include "stdafx.h"
#include "Matrix.h"
#include "vtkCellType.h"

typedef void (CALLBACK *GET_FLOAT_DATA_CALLBACK)(int index, float* data_pointer);
typedef void (CALLBACK *GET_INTEGER_DATA_CALLBACK)(int index, int* data_pointer);

#if defined(_DLL_EXPORT)
class __declspec(dllexport) CVTKWriter
#else
class  __declspec(dllimport) CVTKWriter
#endif
{
private:
	//CMatrix* vertices;
	//CMatrix* simples;

	FILE* output_file;

public:
	CVTKWriter(void);
	~CVTKWriter(void);

	// Sets the source for vertices (3 columns, n rows)
//int SetVertexMatrix(CMatrix * vertices);

	// Sets the source for simplexes (4 columns, n rows)
	// Value of each element should be rounded to integer
//	int SetSimplexMatrix(CMatrix * simples);


	// Two functions, which will create and open file for output
	int OpenOutputFile(const char* file_name);
	int OpenOutputFile(const wchar_t* file_name);

	// Close the output file
	int CloseFile(void);

	// Write VTK text header to the file. Should be called
	// just after OpenOutputFile method
	int WriteHeader(const char* description);


	// Writes points by coordinates
	int WritePoints(CMatrix * vertices);
	int WritePoints(int count, GET_FLOAT_DATA_CALLBACK callback);

	// Writes cells by node indices
	int WriteCells(CMatrix * elements, int vtk_cell_type);
	int WriteCells(int count, GET_INTEGER_DATA_CALLBACK callback, int vtk_cell_type);

	// Writes cell types
	int WriteCellTypes(int count, int type);
	int WriteCellTypes(CMatrix * column_vector);

	// Writes POINT_DATA section
	int WritePointData(const char* scalars_name, const char* lookup_table_name, CMatrix * column_vector);
	int WritePointData(const char* scalars_name, const char* lookup_table_name, int count, float value);
	int WritePointData(const char* scalars_name, const char* lookup_table_name, int count, GET_FLOAT_DATA_CALLBACK callback);

	// Writes CELL_DATA section
	int WriteCellData(const char* scalars_name, const char* lookup_table_name, CMatrix * column_vector);
	int WriteCellData(const char* scalars_name, const char* lookup_table_name, int count, float value);
	int WriteCellData(const char* scalars_name, const char* lookup_table_name, int count, GET_FLOAT_DATA_CALLBACK callback);

	int WriteColorTable(const char *lookup_table_name, float r0, float g0, float b0, float r1, float g1, float b1);

private:
	int WritePointDataHeader(const char* scalars_name, const char* lookup_table_name, int count);
	int CVTKWriter::WriteCellDataHeader(const char* scalars_name, const char* lookup_table_name, int count);
};
