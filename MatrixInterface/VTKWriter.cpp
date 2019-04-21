#include "StdAfx.h"
#include "VTKWriter.h"

#pragma warning(disable: 4996) // C4996: 'fopen': This function or variable may be unsafe. Consider using fopen_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.	e:\szkola\dr\projects\rwmeshgen\matrixinterface\vtkwriter.cpp	18	MatrixInterface


CVTKWriter::CVTKWriter(void)
{
	this->output_file = NULL;
}

CVTKWriter::~CVTKWriter(void)
{
	this->CloseFile();
}

int CVTKWriter::OpenOutputFile(const char* file_name)
{
	this->output_file = fopen(file_name, "wb+");
	if (this->output_file = NULL)
		return FALSE;

	setvbuf(this->output_file, NULL, _IOLBF, 1 * 1024 * 1024);
	return TRUE;
}

int CVTKWriter::OpenOutputFile(const wchar_t* file_name)
{
	this->output_file = _wfopen(file_name, L"wb+");
	if (this->output_file == NULL)
		return FALSE;

	setvbuf(this->output_file, NULL, _IOLBF, 1 * 1024 * 1024);
	return TRUE;
}

int CVTKWriter::CloseFile(void)
{
	if (this->output_file == NULL)
		return FALSE;

	int ret = fclose(this->output_file);
	this->output_file = NULL;
	if (ret) // ret != 0 -> something went wrong
		return FALSE;
	return TRUE;
}

int CVTKWriter::WriteHeader(const char* description)
{
	if (this->output_file == NULL)
		return FALSE;

	fprintf(this->output_file, "# vtk DataFile Version 3.0\n");
	fprintf(this->output_file, "%s\n", description);
	fprintf(this->output_file, "ASCII\n");
	fprintf(this->output_file, "DATASET UNSTRUCTURED_GRID\n\n");

	return TRUE;
}

int CVTKWriter::WritePoints(CMatrix * vertices)
{
	if (vertices == NULL)
		return FALSE;
	if (vertices->NumberOfCols() != 3)
		return FALSE; // not X, Y, Z ??
	if (this->output_file == NULL)
		return FALSE;

	fprintf(this->output_file, "POINTS %d float\n", vertices->NumberOfRows());

	for(int i = 0; i < vertices->NumberOfRows(); i++)
		fprintf(this->output_file, "%6.3f %6.3f %6.3f\n",
		vertices->operator [](i)[0],
		vertices->operator [](i)[1],
		vertices->operator [](i)[2]);

	fprintf(this->output_file, "\n");
	return TRUE;
}

int CVTKWriter::WritePoints(int count, GET_FLOAT_DATA_CALLBACK callback)
{
	if (callback == NULL)
		return FALSE;
	if (this->output_file == NULL)
		return FALSE;

	fprintf(this->output_file, "POINTS %d float\n", count);
	for(int i = 0; i < count; i++)
	{
		float xyz[3];
		callback(i, xyz);
		fprintf(this->output_file, "%6.3f %6.3f %6.3f\n", xyz[0], xyz[1], xyz[2]);
	}

	fprintf(this->output_file, "\n");
	return TRUE;
}

int CVTKWriter::WriteCells(int count, GET_INTEGER_DATA_CALLBACK callback, int vtk_cell_type)
{
	if (callback == NULL)
		return FALSE;
	if (this->output_file == NULL)
		return FALSE;

	if ((vtk_cell_type != VTK_TETRA) && (vtk_cell_type != VTK_TRIANGLE))
		return FALSE;

	if (vtk_cell_type == VTK_TETRA)
	{
		fprintf(this->output_file, "CELLS %d %d\n", count, count * 5);
		for(int i = 0; i < count; i++)
		{
			int abcd[4];
			callback(i, abcd);
			fprintf(this->output_file, "4 %d %d %d %d\n", abcd[0], abcd[1], abcd[2], abcd[3]);
		}
	}

	if (vtk_cell_type == VTK_TRIANGLE)
	{
		fprintf(this->output_file, "CELLS %d %d\n", count, count * 4);
		for(int i = 0; i < count; i++)
		{
			int abcd[4];
			callback(i, abcd);
			fprintf(this->output_file, "3 %d %d %d\n", abcd[0], abcd[1], abcd[2]);
		}
	}

	fprintf(this->output_file, "\n");
	return TRUE;
}

int CVTKWriter::WriteCells(CMatrix * elements, int vtk_cell_type)
{
	if (elements == NULL)
		return FALSE;
	if (elements->NumberOfCols() != 4)
		return FALSE; // not A, B, C, D ??
	if (this->output_file == NULL)
		return FALSE;

	if (vtk_cell_type == VTK_TETRA)
	{
		fprintf(this->output_file, "CELLS %d %d\n", elements->NumberOfRows(), elements->NumberOfRows() * 5);
		for(int i = 0; i < elements->NumberOfRows(); i++)
			fprintf(this->output_file, "4 %d %d %d %d\n", 
			(int)elements->operator [](i)[0] - 1,
			(int)elements->operator [](i)[1] - 1,
			(int)elements->operator [](i)[2] - 1,
			(int)elements->operator [](i)[3] - 1);
	}
	if (vtk_cell_type == VTK_TRIANGLE)
	{
		fprintf(this->output_file, "CELLS %d %d\n", elements->NumberOfRows(), elements->NumberOfRows() * 4);
		for(int i = 0; i < elements->NumberOfRows(); i++)
			fprintf(this->output_file, "3 %d %d %d\n", 
			(int)elements->operator [](i)[0] - 1,
			(int)elements->operator [](i)[1] - 1,
			(int)elements->operator [](i)[2] - 1);
	}

	fprintf(this->output_file, "\n");
	return TRUE;
}

// d:\Projekty\VTK\Filtering\vtkCellType.h 
int CVTKWriter::WriteCellTypes(int count, int vtk_cell_type)
{
	if (this->output_file == NULL)
		return FALSE;

	fprintf(this->output_file, "CELL_TYPES %d\n", count);
	for(int i = 0; i < count; i++)
		fprintf(this->output_file, "%d\n", vtk_cell_type);

	fprintf(this->output_file, "\n");
	return TRUE;
}

int CVTKWriter::WriteCellTypes(CMatrix * column_vector)
{
	if (column_vector == NULL)
		return FALSE;
	if (column_vector->NumberOfCols() != 1)
		return FALSE;
	if (this->output_file == NULL)
		return FALSE;

	fprintf(this->output_file, "CELL_TYPES %d\n", column_vector->NumberOfRows());
	for(int i = 0; i < column_vector->NumberOfRows(); i++)
		fprintf(this->output_file, "%d\n", (int)column_vector->operator[](i)[0]);

	fprintf(this->output_file, "\n");
	return TRUE;
}


int CVTKWriter::WriteColorTable(const char *lookup_table_name, float r0, float g0, float b0, float r1, float g1, float b1)
{
	if (this->output_file == NULL)
		return FALSE;

	fprintf(this->output_file, "LOOKUP_TABLE %s %d\n", lookup_table_name, 2);
	fprintf(this->output_file, "%d %.2f %.2f %.2f\n", 0, r0, g0, b0);
	fprintf(this->output_file, "%d %.2f %.2f %.2f\n", 1, r1, g1, b1);

	fprintf(this->output_file, "\n");
	return TRUE;
}

// ###########################################################################
// #
// # 
// #
// #


int CVTKWriter::WritePointDataHeader(const char* scalars_name, const char* lookup_table_name, int count)
{
	if (this->output_file == NULL)
		return FALSE;

	fprintf(this->output_file, "POINT_DATA %d\n", count);
	fprintf(this->output_file, "SCALARS %s float\n", scalars_name);
	fprintf(this->output_file, "LOOKUP_TABLE %s\n", lookup_table_name);

	return TRUE;
}

int CVTKWriter::WritePointData(const char* scalars_name, const char* lookup_table_name, CMatrix * column_vector)
{
	if (column_vector == NULL)
		return FALSE;
	if (column_vector->NumberOfCols() != 1)
		return FALSE;

	if (!this->WritePointDataHeader(scalars_name, lookup_table_name, column_vector->NumberOfRows()))
		return FALSE;

	for(int i = 0; i < column_vector->NumberOfRows(); i++)
		fprintf(this->output_file, "%.3f\n", column_vector->operator[](i)[0]);

	fprintf(this->output_file, "\n");
	return TRUE;
}

int CVTKWriter::WritePointData(const char* scalars_name, const char* lookup_table_name, int count, float value)
{
	if (!this->WritePointDataHeader(scalars_name, lookup_table_name, count))
		return FALSE;

	while(count-- > 0)
		fprintf(this->output_file, "%.3f\n", value);

	fprintf(this->output_file, "\n");
	return TRUE;
}

int CVTKWriter::WritePointData(const char* scalars_name, const char* lookup_table_name, int count, GET_FLOAT_DATA_CALLBACK callback)
{
	if (callback == NULL)
		return FALSE;

	if (!this->WritePointDataHeader(scalars_name, lookup_table_name, count))
		return FALSE;

	for (int i = 0; i < count; i++)
	{
		float value;
		callback(i, &value);
		fprintf(this->output_file, "%.3f\n", value);
	}

	fprintf(this->output_file, "\n");
	return TRUE;
}
// ###########################################################################
// #
// # 
// #
// #

int CVTKWriter::WriteCellDataHeader(const char* scalars_name, const char* lookup_table_name, int count)
{
	if (this->output_file == NULL)
		return FALSE;

	fprintf(this->output_file, "CELL_DATA %d\n", count);
	fprintf(this->output_file, "SCALARS %s float\n", scalars_name);
	fprintf(this->output_file, "LOOKUP_TABLE %s\n", lookup_table_name);

	return TRUE;
}

int CVTKWriter::WriteCellData(const char* scalars_name, const char* lookup_table_name, CMatrix * column_vector)
{
	if (column_vector == NULL)
		return FALSE;
	if (column_vector->NumberOfCols() != 1)
		return FALSE;

	if (!this->WriteCellDataHeader(scalars_name, lookup_table_name, column_vector->NumberOfRows()))
		return FALSE;

	for(int i = 0; i < column_vector->NumberOfRows(); i++)
		fprintf(this->output_file, "%.3f\n", column_vector->operator[](i)[0]);

	fprintf(this->output_file, "\n");
	return TRUE;
}

int CVTKWriter::WriteCellData(const char* scalars_name, const char* lookup_table_name, int count, float value)
{
	if (!this->WriteCellDataHeader(scalars_name, lookup_table_name, count))
		return FALSE;

	while(count-- > 0)
		fprintf(this->output_file, "%.3f\n", value);

	fprintf(this->output_file, "\n");
	return TRUE;
}

int CVTKWriter::WriteCellData(const char* scalars_name, const char* lookup_table_name, int count, GET_FLOAT_DATA_CALLBACK callback)
{
	if (callback == NULL)
		return FALSE;

	if (!this->WriteCellDataHeader(scalars_name, lookup_table_name, count))
		return FALSE;

	for (int i = 0; i < count; i++)
	{
		float value;
		callback(i, &value);
		fprintf(this->output_file, "%.3f\n", value);
	}

	fprintf(this->output_file, "\n");
	return TRUE;
}