#include "stdafx.h"
#include "libMeshGen.h"



CNode::CNode(int ring_number, double radius, double angle, NodeFlag::Enum flags)
	: /*id(0),*/ z(0.0)//, flags(NodeFlags::NormalNode)
{
	//this->rring = ring_number;
	this->radius = radius;
	this->angle = angle;
	this->flags = flags;
	this->RecalcCoords();
}

void CNode::SetRadius(double radius)
{
	this->radius = radius;
	this->RecalcCoords();
}

void CNode::SetAngle(double angle)
{
	this->angle = angle;
	this->RecalcCoords();
}


void CNode::RecalcCoords(void)
{
	this->x = cos(this->angle) * this->radius;
	this->y = sin(this->angle) * this->radius;
}

double CNode::Distance(const class CNode * node1, const class CNode * node2)
{                                                                                
	double sqx = (node1->x - node2->x) * (node1->x - node2->x);                                              
	double sqy = (node1->y - node2->y) * (node1->y - node2->y);                                              
	double sqz = (node1->z - node2->z) * (node1->z - node2->z);                                              
                                                                          
	return sqrt(sqx + sqy + sqz);                                                   
} 

double CNode::Distance(const class CNode * node1, double x, double y, double z)
{                                                                                
	double sqx = (node1->x - x) * (node1->x - x);                                              
	double sqy = (node1->y - y) * (node1->y - y);                                              
	double sqz = (node1->z - z) * (node1->z - z);                                              
                                                                          
	return sqrt(sqx + sqy + sqz);                                                   
} 

double CNode::Distance(const class CNode * node1, double x, double y)
{                                                                                
	double sqx = (node1->x - x) * (node1->x - x);                                              
	double sqy = (node1->y - y) * (node1->y - y);                                              
                                                                          
	return sqrt(sqx + sqy);                                                   
} 

/*
CNode::CNode()
{
}

CNode::CNode(const double x, const double y, const double z)
	: X(x), Y(y), Z(z)
{
}

CNode::CNode(const double x, const double y)
	: X(x), Y(y)
{
}

CNode::CNode(const class CNode & node)
	: X(node.X), Y(node.Y), Z(node.Z)
{
}

CNode::~CNode()
{
}

void CNode::DivideBy(double divider)
{
	this->X /= divider;
	this->Y /= divider;
	this->Z /= divider;
}

void CNode::Set(const class CNode & node)                      
{                                                              
	this->X = node.X;                                                
	this->Y = node.Y;                                                
	this->Z = node.Z;                                                
}                                                              
                                                            
void CNode::Set(const double x, const double y, const double z)
{                                                              
	this->X = x;                                                  
	this->Y = y;                                                  
	this->Z = z;                                                  
}     

void CNode::Substract(const class CNode & node)
{                                              
	this->X -= node.X;                               
	this->Y -= node.Y;                               
	this->Z -= node.Z;                               
}                                              
                                            
void CNode::Add(const class CNode & node)      
{                                              
	this->X += node.X;                               
	this->Y += node.Y;                               
	this->Z += node.Z;                               
}   

double CNode::Distance3D(const class CNode & node1, const class CNode & node2)
{                                                                                
	double sqx = (node1.X - node2.X) * (node1.X - node2.X);                                              
	double sqy = (node1.Y - node2.Y) * (node1.Y - node2.Y);                                              
	double sqz = (node1.Z - node2.Z) * (node1.Z - node2.Z);                                              
                                                                              
	return sqrt(sqx + sqy + sqz);                                                   
}  

double CNode::Distance2D(const class CNode & node1, const class CNode & node2)
{                                                                                
	double sqx = (node1.X - node2.X) * (node1.X - node2.X);                                              
	double sqy = (node1.Y - node2.Y) * (node1.Y - node2.Y);                                              
                                                                              
	return sqrt(sqx + sqy);                                                   
}  
*/