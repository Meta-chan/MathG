/*
	Part of the MathG Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#define IR_IMPLEMENT
#include "mathg.h"

int main()
{
	if (MathG::init(true))
	{
		bool ok;
		MatrixG a(2, 2, &ok);
		VectorG b(2, &ok);
		VectorG c(2, &ok);
		float adata[4] = { 0.25f, 0.5f, 0.75f, 1.0f };
		a.store(adata);
		float bdata[2] = { 0.5f, 1.0f };
		b.store(bdata);
		MathG::multiply_mvv(&a, &b, &c);
		float cdata[2];
		c.load(cdata);
		MathG::free();
	}
};