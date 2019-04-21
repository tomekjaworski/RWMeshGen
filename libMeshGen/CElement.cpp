#include "stdafx.h"
#include "libMeshGen.h"

double CElement::GetEdgeRatio3(std::vector<CNode> * node_list) const
{
	CNode * n1 = &node_list->operator [](this->node1);
	CNode * n2 = &node_list->operator [](this->node2);
	CNode * n3 = &node_list->operator [](this->node3);

	double d12x = (n1->GetX() - n2->GetX() );
	double d12y = (n1->GetY() - n2->GetY());
	double d23x = (n2->GetX() - n3->GetX() );
	double d23y = (n2->GetY() - n3->GetY());
	double d31x = (n3->GetX() - n1->GetX() );
	double d31y = (n3->GetY() - n1->GetY());

	double d12 = (d12x * d12x + d12y * d12y);
	double d23 = (d23x * d23x + d23y * d23y);
	double d31 = (d31x * d31x + d31y * d31y);

	double d1 = (d12 > d23) ? d23 / d12 : d12 / d23;
	double d2 = (d31 > d12) ? d12 / d31 : d31 / d12;

	return abs(min(d1, d2));
}

double DotProduct(double x1, double y1, double x2, double y2)
{
	return x1*x2 + y1*y2;
}

double CrossProduct(double x1, double y1, double x2, double y2)
{
	return x1 * y2 - x2 * y1;
}

bool CElement::PointInElement3(CNode * nodes, const double x, const double y) const
{
	CNode * n1 = &nodes[this->node1]; // A
	CNode * n2 = &nodes[this->node2]; // B
	CNode * n3 = &nodes[this->node3]; // C

	double cpBP = CrossProduct(
		n2->GetX() - n1->GetX(), n2->GetY() - n1->GetY(),	// AB
		x - n1->GetX(), y - n1->GetY());					// AP			
	double cpCP = CrossProduct(
		n3->GetX() - n2->GetX(), n3->GetY() - n2->GetY(),	// BC
		x - n2->GetX(), y - n2->GetY());					// BP			
	double cpAP = CrossProduct(
		n1->GetX() - n3->GetX(), n1->GetY() - n3->GetY(),	// CA
		x - n3->GetX(), y - n3->y);							// CP			


	if (cpAP < 0) return false;
	if (cpBP < 0) return false;
	if (cpCP < 0) return false;
		
	return true;
}

double CElement::GetEdgeRatio4(std::vector<CNode> * node_list) const
{
	CNode * n1 = &node_list->operator [](this->node1);
	CNode * n2 = &node_list->operator [](this->node2);
	CNode * n3 = &node_list->operator [](this->node3);
	CNode * n4 = &node_list->operator [](this->node4);

	double d12x = (n1->GetX() - n2->GetX());
	double d12y = (n1->GetY() - n2->GetY());
	double d13x = (n1->GetX() - n3->GetX());
	double d13y = (n1->GetY() - n3->GetY());
	double d14x = (n1->GetX() - n4->GetX());
	double d14y = (n1->GetY() - n4->GetY());

	double d23x = (n2->GetX() - n3->GetX());
	double d23y = (n2->GetY() - n3->GetY());
	double d24x = (n2->GetX() - n4->GetX());
	double d24y = (n2->GetY() - n4->GetY());

	double d34x = (n3->GetX() - n4->GetX());
	double d34y = (n3->GetY() - n4->GetY());


	double d12 = (d12x * d12x + d12y * d12y);
	double d13 = (d13x * d13x + d13y * d13y);
	double d14 = (d14x * d14x + d14y * d14y);

	double d23 = (d23x * d23x + d23y * d23y);
	double d24 = (d24x * d24x + d24y * d24y);

	double d34 = (d34x * d34x + d34y * d34y);

	double d1 = (d12 > d13) ? d13 / d12 : d12 / d13;
	double d2 = (d23 > d24) ? d24 / d23 : d23 / d24;
	double d3 = (d14 > d34) ? d34 / d14 : d14 / d34;

	return abs(min(min(d1, d2), d3));
}

