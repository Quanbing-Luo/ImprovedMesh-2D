#include <iostream>
#include <string>
#include "Mesh.h"

int main()
{		
	
	Mesh m;

	m.Input();

	m.Initialization();	

	m.MeshGeneration();

	m.Output();	
}



