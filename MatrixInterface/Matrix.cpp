#include "StdAfx.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include "matrix.h"

using namespace std;
#include <omp.h>

#define TINY 1.0e-20;

//=================================================================
// CMatrix constructors
//=================================================================
CMatrix::CMatrix(char *FileName)
{
	reallocationStatus = 1;
	data = NULL;
	cols = 0;
	rows = 0;
	Error = 0;
	InFromFile(FileName);
	Temp = NULL;
}

CMatrix::CMatrix(wchar_t * FileName)
{
	reallocationStatus = 1;
	data = NULL;
	cols = 0;
	rows = 0;
	Error = 0;
	InFromFile(FileName);
	Temp = NULL;
}

////---------------------------------------------------------------------------
//CMatrix::CMatrix(int _rows, int _cols)
//{
//	reallocationStatus = 1;
//	data = NULL;
//	cols = 0;
//	rows = 0;
//	Error = 0;
//
//	AllocateMemory(_rows, _cols, 0.0);
//	Temp = NULL;
//}

//---------------------------------------------------------------------------
//utworzenie i wype³nienie macierzy dowoln?wartoœci?
CMatrix::CMatrix(int _rows, int _cols, double val)
{
	reallocationStatus = 1;
	data = NULL;
	cols = 0;
	rows = 0;
	Error = 0;
	AllocateMemory(_rows, _cols, val);
	Temp = NULL;
}

//---------------------------------------------------------------------------
CMatrix::CMatrix(CMatrix *A)
{
	reallocationStatus = 1;
	data = NULL;
	cols = 0;
	rows = 0;
	Error = 0;
	Temp = NULL;
	if (!A)
		AllocateMemory(2, 2, 0.0);
	else
		(*this) = (*A);
}

// utworzenie macierzy jednokolumnowej z COUNT elementami
CMatrix::CMatrix(double *data, int count)
{
	this->reallocationStatus = 1;
	this->data = NULL;
	this->cols = this->rows = 0;
	this->Error = 0;
	this->Temp = NULL;

	this->AllocateMemory(count, 1, 0.0);
	for(int i = 0; i < count; i++)
		this->operator[](i)[0] = data[i];
}

// utworzenie macierzy jednokolumnowej z COUNT elementami
CMatrix::CMatrix(int *data, int count)
{
	this->reallocationStatus = 1;
	this->data = NULL;
	this->cols = this->rows = 0;
	this->Error = 0;
	this->Temp = NULL;

	this->AllocateMemory(count, 1, 0.0);
	for(int i = 0; i < count; i++)
		this->operator[](i)[0] = data[i];

}

//---------------------------------------------------------------------------
CMatrix::~CMatrix(void)
{
	FreeMemory();
	delete Temp;
	Temp = NULL;
}

int CMatrix::NewSize(int _rows, int _cols, double val)		
{
	reallocationStatus = 0;
	return AllocateMemory(_rows, _cols, val);
}

int CMatrix::GetReallocationStatus()
{
	return reallocationStatus;
}


//=================================================================
// CMatrix Serialization
//=================================================================
//the first line in file represents the row and column size of the read matrix
//the following lines represents the matrix data ordered in rows and columns 
//separated with space or tabulator
//it returns 0 in case of error otherwise 1

int CMatrix::InFromFile(char *FileName)
{
	ifstream ifs(FileName);
	return this->InFromFileInternal(ifs);
}

int CMatrix::InFromFile(wchar_t *FileName)
{
	ifstream ifs(FileName);
	return this->InFromFileInternal(ifs);
}

int CMatrix::InFromFileInternal(ifstream & stream)
{
	if (!stream.is_open())
	{
		
		//CString buff;
		//buff.Format("Error while opening file: %s", FileName); 
		//AfxMessageBox(buff);
		return 0;
	}
	int _r, _c;
	stream >> _r >> _c;
	
	if (_r <= 0 || _c <= 0)
		return 0;
	
	if (_r != rows || _c != cols)
		AllocateMemory(_r, _c, 0.0);	

	for (int j = 0; j < rows; j++)
		for (int i = 0; i < cols; i++)
			stream >> data[j][i];
	stream.close();
	return 1;
}

//---------------------------------------------------------------------------
void CMatrix::Out()
{
	cout << rows << "\t" << cols << endl;
	for (int j = 0; j < rows; j++)
	{
		for (int i = 0; i < cols; i++)
		{
			if (data[j][i] < 0.00000000001 && data[j][i] > -0.00000000001)
				data[j][i] = 0.0;
			cout << data[j][i] << "\t";
		}
		cout << endl;
	}
}

//---------------------------------------------------------------------------

void CMatrix::OutToFile(char *FileName)
{
	ofstream ofs(FileName);
	this->OutToFileInternal(ofs);
	ofs.close();
}


void CMatrix::OutToFile(wchar_t *FileName)
{
	ofstream ofs(FileName);
	this->OutToFileInternal(ofs);
	ofs.close();
}


void CMatrix::OutToFileInternal(ofstream & stream)
{
	//if (!stream.is_open())
	stream << rows << "\t" << cols << endl;
	
	for (int j = 0; j < rows; j++)
	{
		for (int i = 0; i < cols; i++)
		{
			if (data[j][i] < 0.00000000001 && data[j][i] > -0.00000000001)
				data[j][i] = 0.0;
			stream << data[j][i] << "\t";
		}
		stream << endl;
	}
}

//---------------------------------------------------------------------------
//FileName, VariableName
void CMatrix::OutToFileMatlab(char *FileName, char *VariableName)
{
	ofstream ofs(FileName);
	ofs.precision(2);
	//ofs.setf(ios::scientific); 
	ofs << VariableName << "=[";

	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
			ofs << " " << data[r][c];
		if (r != rows - 1)
			ofs << ";"<< endl;
	}
	ofs << "]";
	ofs.close();
}

//---------------------------------------------------------------------------
void CMatrix::OutToVTKFile_AsImage(char *FileName, CMatrix &Nodes, CMatrix &Simplices)
{
	// to do
}

void CMatrix::SetValue(double val)
{
	if (!data)
		return;
	
	int i, j;
	for (i = 0; i < rows; i++)
		for (j = 0; j < cols; j++)	
			data[i][j] = val;
}

void CMatrix::SetValueInCol(int col, double val)
{
	if (!data)
		return;
	
	for (int i = 0; i < rows; i++)
		data[i][col] = val;
}

void CMatrix::SetIdentity()
{
	long i,j;
	CMatrix *Temporary = this;
	int r = rows;
	int c = cols;

	#pragma omp parallel for shared(Temporary,r,c) private(i,j)
	for (i = 0; i < r; i++)
		for (j = 0; j < c; j++)
		{
			if (i==j)
			{
				Temporary->data[i][j] = 1.0;
			}
		}

	*this = *Temporary;
}

//=================================================================
// CMatrix Linear Algebra
//=================================================================
//double CMatrix::Determinant();         

//---------------------------------------------------------------------------
CMatrix &CMatrix::Transpose(void)
{
//   CMatrix *Temp = new CMatrix(cols, rows);
	if (Temp == NULL)
		Temp = new CMatrix(cols, rows, 0.0);
	else if (Temp->rows != cols || Temp->cols != rows)
		Temp->AllocateMemory(cols, rows, 0.0);
	
   long i,j;
   int r = rows;
   int c = cols;
   CMatrix *Temporary = Temp;

   #pragma omp parallel for shared(Temporary,r,c) private(i,j)
   for (i = 0; i < r; i++)		
	   for (j = 0; j < c; j++)
			Temporary->data[j][i] = data[i][j];

   return *Temporary;
}

//---------------------------------------------------------------------------
double *CMatrix::operator[](int index)		//[row][col]
{
   if(index >= rows)
   {
      //strcpy(Error, "CMatrix::operator[] index out of range!\nIndex = ");// + IntToStr(index) + "  >=  Cols = " + IntToStr(cols));
	   return NULL;
   }
   return data[index];	
}

//---------------------------------------------------------------------------
CMatrix &CMatrix::operator=(CMatrix &M)
{
   if (cols != M.cols || rows != M.rows)
      AllocateMemory(M.rows, M.cols, 0.0);

   long i,j;
   CMatrix *Temporary = this;
   int r = rows;
   int c = cols;

   #pragma omp parallel for shared(Temporary,r,c,M) private(i,j)

   for (i = 0; i < c; i++)
		for (j = 0; j < r; j++)
			Temporary->data[j][i] = M.data[j][i];

   return *Temporary;
}

//---------------------------------------------------------------------------
CMatrix &CMatrix::operator+(CMatrix &M) 
{
   if (cols != M.cols || rows != M.rows)
   {
      //strcpy(Error, "Added matrices are different size");
      //return NULL;
   }
//   CMatrix *Temp = new CMatrix(rows, cols, 0.0);
	if (Temp == NULL)
		Temp = new CMatrix(rows, cols, 0.0);
	else if (Temp->rows != rows || Temp->cols != cols)
		Temp->AllocateMemory(rows, cols, 0.0);

	long i,j;
	CMatrix *Temporary = Temp;
	int r = rows;
	int c = cols;

	#pragma omp parallel for shared(Temporary,r,c,M) private(i,j)
    for (i = 0; i < c; i++)
		for (j = 0; j < r; j++)
			Temporary->data[j][i] = data[j][i] + M.data[j][i];
   return *Temporary;
}

//---------------------------------------------------------------------------
CMatrix &CMatrix::operator+(double d)
{
   //CMatrix *Temp = new CMatrix(rows, cols, 0.0);
	if (Temp == NULL)
		Temp = new CMatrix(rows, cols, 0.0);
	else if (Temp->rows != rows || Temp->cols != cols)
		Temp->AllocateMemory(rows, cols, 0.0);

	long i,j;
    CMatrix *Temporary = Temp;
	int r = rows;
	int c = cols;
	
	#pragma omp parallel for shared(Temporary,r,c,d) private(i,j)
    for (i = 0; i < c; i++)
		for (j = 0; j < r; j++)
			Temporary->data[j][i] = data[j][i] + d;
   
	return *Temporary;
}

//---------------------------------------------------------------------------
CMatrix &CMatrix::operator-(CMatrix &M) 
{
   if (cols != M.cols || rows != M.rows)
   {
      //strcpy(Error, "Subtracted matrices are different size");
      //return NULL;
   }
//   CMatrix *Temp = new CMatrix(rows, cols, 0.0);
	if (Temp == NULL)
		Temp = new CMatrix(rows, cols, 0.0);
	else if (Temp->rows != rows || Temp->cols != cols)
		Temp->AllocateMemory(rows, cols, 0.0);
   
	long i,j;
	CMatrix *Temporary = Temp;
	int r = rows;
	int c = cols;

	#pragma omp parallel for shared(Temporary,r,c,M) private(i,j)
   for (i = 0; i < c; i++)
		for (j = 0; j < r; j++)
			Temporary->data[j][i] = data[j][i] - M.data[j][i];
   
   return *Temporary;
}

//---------------------------------------------------------------------------
CMatrix &CMatrix::operator-(double d)
{
//   CMatrix *Temp = new CMatrix(rows, cols, 0.0);
	if (Temp == NULL)
		Temp = new CMatrix(rows, cols, 0.0);
	else if (Temp->rows != rows || Temp->cols != cols)
		Temp->AllocateMemory(rows, cols, 0.0);
   
	long i,j;
	CMatrix *Temporary = Temp;
	int r = rows;
	int c = cols;

   #pragma omp parallel for shared(Temporary,r,c,d) private(i,j)
   for (i = 0; i < c; i++)
		for (j = 0; j < r; j++)
			Temporary->data[j][i] = data[j][i] - d;
   
   return *Temporary;
}

//---------------------------------------------------------------------------
CMatrix &CMatrix::operator*(CMatrix &M)
{
//   CMatrix *Temp = new CMatrix(rows, M.cols, 0.0);
	if (Temp == NULL)
		Temp = new CMatrix(rows, M.cols, 0.0);
	else if (Temp->rows != rows || Temp->cols != M.cols)
		Temp->AllocateMemory(rows, M.cols, 0.0);
	else
		Temp->SetValue(0.0);

	if (cols != M.rows)
    {
      //strcpy(Error, "Multiplied matrices are different size");
      //return NULL;
      printf("Multiplied matrices are of different size");
    }

	long i,j,p;

	int a = M.cols;
	CMatrix *Temporary = Temp;
	int r = rows;
	int c = cols;

	#pragma omp parallel for shared(Temporary,M,a,r,c) private(i,j,p)
	for (i = 0; i < a; i++)			
	{
		for (j = 0; j < r; j++)
		{
		   for (p = 0; p < c; p++)
		   {
				Temporary->data[j][i] = Temporary->data[j][i] + data[j][p] * M.data[p][i];
		   }
		}
	}
   
   return *Temporary;
}

//---------------------------------------------------------------------------
CMatrix &CMatrix::operator*(double d)
{
//   CMatrix *Temp = new CMatrix(rows, cols, 0.0);
	if (Temp == NULL)
		Temp = new CMatrix(rows, cols, 0.0);
	else if (Temp->rows != rows || Temp->cols != cols)
		Temp->AllocateMemory(rows, cols, 0.0);
 
   
	long i,j;
	CMatrix *Temporary = Temp;
	int r = rows;
	int c = cols;

	#pragma omp parallel for shared(Temporary,r,c,d) private(i,j)
	for (i = 0; i < c; i++)
		for (j = 0; j < r; j++)
			Temporary->data[j][i] = data[j][i] * d;
   
	return *Temporary;
}

//---------------------------------------------------------------------------
CMatrix &CMatrix::operator+=(CMatrix &M)
{
   if (cols != M.cols || rows != M.rows)
   {
      //strcpy(Error, "Added matrices are different size");
      //return NULL;
   }
 
   for (int i = 0; i < cols; i++)
		for (int j = 0; j < rows; j++)
			data[j][i] += M.data[j][i];
   return *this;
}

//---------------------------------------------------------------------------
CMatrix &CMatrix::operator+=(double d)
{

	for (int i = 0; i < cols; i++)
		for (int j = 0; j < rows; j++)
			data[j][i] += d;
   return *this;
}
//---------------------------------------------------------------------------
CMatrix &CMatrix::operator-=(CMatrix &M)
{
   if (cols != M.cols || rows != M.rows)
   {
      //strcpy(Error, "Subtracted matrices are different size");
      //return NULL;
   }

   for (int i = 0; i < cols; i++)
		for (int j = 0; j < rows; j++)
			data[j][i] -= M.data[j][i];
   return *this;
}

//---------------------------------------------------------------------------
CMatrix &CMatrix::operator-=(double d)
{
   for (int i = 0; i < cols; i++)
		for (int j = 0; j < rows; j++)
			data[j][i] -= d;
   return *this;
}

//---------------------------------------------------------------------------
CMatrix &CMatrix::operator*=(CMatrix &M)
{
//   CMatrix *Temp = new CMatrix(rows, M.cols, 0.0);
	if (Temp == NULL)
		Temp = new CMatrix(rows, M.cols, 0.0);
	else if (Temp->rows != rows || Temp->cols != M.cols)
		Temp->AllocateMemory(rows, M.cols, 0.0);
	else
		Temp->SetValue(0.0);

	if (cols != M.rows)
   {
      //strcpy(Error, "Multiplied matrices are different size");
      //return NULL;
   }
   M.cols;

   for (int i = 0; i < M.cols; i++)
	   for (int j = 0; j < rows; j++)
		   for (int p = 0; p < cols; p++)
			   Temp->data[j][i] += data[j][p] * M.data[p][i];		//[row][col]
   (*this) = (*Temp);
   delete Temp;
   Temp = NULL;
   return *this;
}

//---------------------------------------------------------------------------
CMatrix &CMatrix::operator*=(double d)
{
   for (int i = 0; i < cols; i++)
		for (int j = 0; j < rows; j++)
			data[j][i] *= d;
   return *this;
}

CMatrix &CMatrix::operator%=(CMatrix &M)  /// mnozenie algebraiczne macierzy
{

	for (int i = 0; i < cols; i++)
		for (int j = 0; j < rows; j++)
			this->data[i][j] = this->data[i][j] * M.data[i][j];
   
	return *this;
}

CMatrix &CMatrix::operator/=(CMatrix &M)  /// dzielenie algebraiczne macierzy
{

	for (int i = 0; i < cols; i++)
		for (int j = 0; j < rows; j++)
			this->data[i][j] = this->data[i][j] / M.data[i][j];
   
	return *this;
}


//=================================================================
// CMatrix other methods
//=================================================================
void CMatrix::FindMinMax(double *min, double *max)
{
	long i, j;
	double Min = data[0][0], Max = data[0][0];
	int r = rows;
	int c = cols;
	
	#pragma omp parallel for shared(r,c,Min,Max) private(i,j)
	for (i = 0; i < c; i++)
		for (j = 0; j < r; j++)
			if (data[j][i] < Min)
				Min = data[j][i];
			else if (data[j][i] > Max)
				Max = data[j][i];
	if (min) *min = Min;
	if (max) *max = Max;
}

//---------------------------------- -----------------------------------------
void CMatrix::FindMinMaxWithoutZero(double *min, double *max)
{
	int i, j;
	double Min = data[0][0], Max = data[0][0];
	for (i = 0; i < cols; i++)
		for (j = 0; j < rows; j++)
			if (data[i][j] < Min)
			{
				if (data[i][j] > 0)
					Min = data[i][j];
			}
			else if (data[i][j] > Max)
				Max = data[i][j];
	if (min) *min = Min;
	if (max) *max = Max;
}
//---------------------------------------------------------------------------
double CMatrix::Norm()
{
	double norma = 0.0;

	for (int c = 0; c < cols; c++)
		for (int r = 0; r < rows; r++) 
			norma += data[r][c]*data[r][c];
	norma = sqrt(norma);
	return norma;
}

//---------------------------------------------------------------------------
double CMatrix::MeanValue()
{
	double mean = 0.0;

	for (int c = 0; c < cols; c++)
		for (int r = 0; r < rows; r++) 
			mean += data[r][c];
	mean /= (cols * rows);
	return mean;
}

double CMatrix::SumOfRow(int row)
{
	double sum = 0;

	for (int c = 0; c < cols; c++)
			sum += data[row][c];
	return sum;
}

double CMatrix::SumOfCol(int col)
{
	double sum = 0;

	for (int r = 0; r < rows; r++)
			sum += data[r][col];
	return sum;
}

int CMatrix::NumberOfRows()
{
	return this->rows;
}

int CMatrix::NumberOfCols()
{
	return this->cols;
}

//=================================================================
// CMatrix private section
//=================================================================
int CMatrix::AllocateMemory(int x, int y, double val)
{
	int i;//, j;
	if (data)
		FreeMemory();

	try
	{
  		data = new double*[x];

		for (i = 0; i < x; i++)
		{
			//TRACE2("Alokowanie pamiie %5d / %d\n", i, x);
			data[i] = new double[y];
		}
	}
	catch (...)
	{
		CString s;
		s.Format(_T("Problemy z pamiecia w module CMatrix\nZaalokowano tylko %d wierszy z %d"), i, x);
		AfxMessageBox(s, MB_OK, 0);
		rows = i;
		cols = y;
		SetValue(val);
		reallocationStatus = 0;
		return 0;
	}
	rows = x;
	cols = y;
	SetValue(val);
	reallocationStatus = 0;
	return 1;
}
//---------------------------------------------------------------------------
int CMatrix::FreeMemory(void)
{
	//this->data[1][1];
	reallocationStatus = 1;
	if(data)
	{
	  for (int i = 0; i < rows; i++)
      {
         if (data[i])
   			delete [](data[i]);
         data[i] = NULL;
      }
		delete []data;
		data = NULL;
	
	}
	cols = 0;
	rows = 0;
   return 1;
}

CMatrix &CMatrix::MatrixNormlisation(CMatrix &MatrixMin, CMatrix &MatrixMax )
{
//   CMatrix *Temp = new CMatrix(rows, cols, 0.0);
	if (Temp == NULL)
		Temp = new CMatrix(rows, cols, 0.0);
	else if (Temp->rows != rows || Temp->cols != cols)
		Temp->AllocateMemory(rows, cols, 0.0);

	if ((this->rows != MatrixMin.rows) || (this->cols != MatrixMin.cols) || (this->rows != MatrixMax.rows) || (this->cols != MatrixMax.cols))
	{
		cout << "Error: Matrices dimensions are different!!\n";
	}
	else
	{
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				Temp->data[i][j] = (this->data[i][j] - MatrixMin.data[i][j]) / (MatrixMax.data[i][j] - MatrixMin.data[i][j]);
	}

	return *Temp;
}

CMatrix &CMatrix::SensMatrixNormlisation(void)
{
	// to do
	int r = this->rows;
	int c = this->cols;
	
	double suma=0;
 
	for (int i = 0; i < rows; i++)
	{
		suma = this->SumOfRow(i);
		suma = 1.0 / suma;
		for (int j = 0; j < cols; j++)
		{
			data[i][j] = data[i][j] * suma;
		}
	}
	return *this;
}

int CMatrix::Resize(int new_rows, int new_cols)
{
	int i, j;
	if (resized_data)
		FreeMemory();

	if (new_rows < this->rows || new_cols < this->cols)
	{
		cout << " Error: The new dimensions are smaller than current!! " << endl;
	}
	else
	{
		try
		{
			resized_data = new double*[new_rows];

			for (i = 0; i < new_rows; i++)
			{
				resized_data[i] = new double[new_cols]; //Miltos addes
			}
		}
		catch (...)
		{
		}
		
		for (i = 0; i < new_rows; i++)
			for (j = 0; j < new_cols; j++)	
				resized_data[i][j] = 0.0;

		for (i = 0; i < rows; i++)
			for (j = 0; j < cols; j++)	
				resized_data[i][j] = data[i][j];
	}

	data = resized_data;

	rows = new_rows;
	cols = new_cols;

	this->reallocationStatus=0; //Miltos Added
	return 1;
}

double CMatrix::GetAmountOfMemory()
{
	double mem;
		mem = ((this->cols * this->rows) + (this->cols * this->rows)) * 8;
		cout << "Mesh has " << mem/1024 << " KBytes" << endl ;
	return mem/1024;
}

CMatrix &CMatrix::GetColumnVector(int row)
{
	int c = this->cols;
//   CMatrix *Temp = new CMatrix(1, cols, 0.0);
	if (Temp == NULL)
		Temp = new CMatrix(rows, cols, 0.0);
	else if (Temp->rows != 1 || Temp->cols != cols)
		Temp->AllocateMemory(1, cols, 0.0);


	for (int i = 0; i < c; i++)
		Temp->data[1][i] = this->data[row][i];

	return *Temp;
}

CMatrix &CMatrix::GetRowVector(int col)
{
	int r = this->rows;
//   CMatrix *Temp = new CMatrix(rows, 1, 0.0);
	if (Temp == NULL)
		Temp = new CMatrix(rows, 1, 0.0);
	else if (Temp->rows != rows || Temp->cols != 1)
		Temp->AllocateMemory(rows, 1, 0.0);

	for (int i = 0; i < r; i++)
		Temp->data[i][0] = this->data[i][col];

	return *Temp;
}

void CMatrix::ThresholdValues(double min, double max)
{


	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
		{
			if (this->data[i][j] <= min) this->data[i][j] = min;
			if (this->data[i][j] >= max) this->data[i][j] = max;
		}
}


CMatrix &CMatrix::SimpleInverse()
{
	Temp = new CMatrix(this->rows,this->cols,0.0);
	CMatrix *inv = new CMatrix(this->rows,this->cols,0.0);
	long i,j,k;
	double Sum,m;

	CMatrix *Temporary = Temp;
	int r = this->rows;
	int c = this->cols;

	#pragma omp parallel for shared(Temporary,r,c) private(i,j)
		for (i=0;i<r;i++)
		{
			for(j=0;j<c;j++)
			{
				if (i==j)
				{
					Temporary->data[i][j] = 1.0;
				}
			}
		}

	
	for (k=0;k<r-1;k++)
	{
	//	#pragma omp parallel for shared(Temporary,r,c) private(i,j)
		for (i=k+1;i<r;i++)
		{
			m=data[i][k]/data[k][k];
			for (j=0;j<c;j++)
			{
				data[i][j] = data[i][j] - m*data[k][j];
				Temporary->data[i][j] = Temporary->data[i][j] - m*Temporary->data[k][j];
			}
		}
	}

	for (i=r-1;i>=0;i--)
	{
		//#pragma omp parallel for shared(Temporary,inv,Sum,r,c) private(j,k)
			for (j=0;j<r;j++)
			{
				Sum=0;
				for (k=i;k<r;k++)
					Sum = Sum + this->data[i][k]*inv->data[k][j];
				inv->data[i][j]=(Temporary->data[i][j]-Sum)/this->data[i][i];
			}
	}

return *inv;
}
