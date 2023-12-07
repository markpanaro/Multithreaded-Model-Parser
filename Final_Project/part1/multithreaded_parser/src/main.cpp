// Support Code written by Michael D. Shah
// Last Updated: 6/10/21
// Please do not redistribute without asking permission.

// Functionality that we created
#include "SDLGraphicsProgram.hpp"

#include <iostream>
#include <string>

int main(int argc, char* argv[]){

	if (argc < 2){
		std::cout << "Please specify a .obj model" << std::endl;
		return 0;
	}

	// Create an instance of an object for a SDLGraphicsProgram
	SDLGraphicsProgram mySDLGraphicsProgram(1280,720,argv);
	// May need to change SDLGraphicsProgram to take extra
	// arg to give file path to object file

	// User needs to specify at least one argument, detected by argc size
	if (argc < 2){
		std::cout << "Please specify a .obj model" << std::endl;
		return 0;
	}

	

	// Run our program forever
	mySDLGraphicsProgram.Loop();
	// When our program ends, it will exit scope, the
	// destructor will then be called and clean up the program.
	return 0;
}
