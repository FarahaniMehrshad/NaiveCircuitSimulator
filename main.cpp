#include <iostream>
#include "CircuitCore.h"
#include "CircuitGui.h"

// Hide cmd
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

int main()
{
	CircuitGui gui;

	gui.Start();

	return 0;
}