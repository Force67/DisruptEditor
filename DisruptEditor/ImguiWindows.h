#pragma once

namespace UI {
	void displayTopMenu();
	void displayTempWindows();
	void displayWindows();

#define DEFINE_WINDOW(x) void display ##x ();
#include "Windows.def"
#undef DEFINE_WINDOW
}
