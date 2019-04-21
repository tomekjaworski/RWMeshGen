#pragma once

#include <fstream>
#include <iostream>

class __declspec(dllexport) CMatrix
{
//Attributes
public:
    double **data;
	double **resized_data;
    int rows;
    int cols;
    char *Error;
	
// Construction
public:
	CMatrix(char *);
	CMatrix(wchar_t *);

//    CMatrix(int, int);
	CMatrix(int _rows = 0, int _cols = 0, double val = 0.0);    //wypeienie macierzy dowoln?wartoi?
	CMatrix(CMatrix *);

	CMatrix(double *data, int count);	// z tablicy double
	CMatrix(int *data, int count);		// z tablicy intów

    ~CMatrix();
	int NewSize(int, int, double);		//rows, cols
	int GetReallocationStatus();

//Serialization
public:
    int InFromFile(char *);       
	int InFromFile(wchar_t *);

    void Out();       
    void OutToFile(char *); 
	void OutToFile(wchar_t *);

    void OutToFileMatlab(char *, char *);	//FileName, VariableName
	void OutToVTKFile_AsImage(char *, CMatrix&, CMatrix&);
	void SetValue(double val);
	void SetValueInCol(int col, double val);
	void SetIdentity();

//LinearAlgebra
public:
    double Determinant();         
    CMatrix &Transpose(void);     
    double *operator[](int);		//[row][col]
    CMatrix &operator=(CMatrix&);
	CMatrix &operator+(CMatrix&); 
	CMatrix &operator+(double); 
	CMatrix &operator-(CMatrix&);
	CMatrix &operator-(double); 
	CMatrix &operator*(CMatrix&);
	CMatrix &operator*(double); 
    CMatrix &operator+=(CMatrix&);
	CMatrix &operator+=(double);
    CMatrix &operator-=(CMatrix&);
	CMatrix &operator-=(double);
	CMatrix &operator*=(CMatrix&); 
    CMatrix &operator*=(double);
	CMatrix &operator%=(CMatrix&); //mnozenie algebraiczne
	CMatrix &operator/=(CMatrix&); //dzielenie algebraiczne

//other
public:
	void FindMinMax(double *min, double *max);
	void FindMinMaxWithoutZero(double *min, double *max);
	double Norm();
	double MeanValue();
	double SumOfRow(int);
	double SumOfCol(int);
	int NumberOfRows();
	int NumberOfCols();
	CMatrix &SensMatrixNormlisation(void);   // Normalizacja Macierzy Czui
	CMatrix &MatrixNormlisation(CMatrix&, CMatrix&);		// Normalizacja liniowa dowolnej macierzy
	int Resize(int,int);  // nie dziala poprawnie
	double GetAmountOfMemory();

	CMatrix &GetColumnVector(int);   // Normalizacja Macierzy Czui
	CMatrix &GetRowVector(int);   // Normalizacja Macierzy Czui

	void ThresholdValues(double, double); // Progowanie wartoi w macierzy
	int FreeMemory();

	// Matrix inversion via LU
/*	void LUDecomposition(double *);
	void LUBacksubstitution(CMatrix&,double []);
	void MatrixInverse();*/
	CMatrix &SimpleInverse();


private:
	int AllocateMemory(int, int, double);
	int *indx;  
	CMatrix *Temp;
	CMatrix *Inv;
	int reallocationStatus;	//1 if pole data is beeing modyfied and should not be referenced

	int InFromFileInternal(std::ifstream & stream);
	void OutToFileInternal(std::ofstream & stream);
};
