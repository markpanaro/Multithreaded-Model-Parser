#include "SDLGraphicsProgram.hpp"
#include "ObjectManager.hpp"
#include "Camera.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <thread>


// Initialization function
// Returns a true or false value based on successful completion of setup.
// Takes in dimensions of window.
SDLGraphicsProgram::SDLGraphicsProgram(int w, int h, char* argv[]):m_screenWidth(w),m_screenHeight(h){
	// Initialization flag
	bool success = true;
	// String to hold any errors that occur.
	std::stringstream errorStream;
	// The window we'll be rendering to
	m_window = NULL;
	// Render flag

	// Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO)< 0){
		errorStream << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
		success = false;
	}
	else{
		//Use OpenGL 3.3 core
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
		// We want to request a double buffer for smooth updating.
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		//Create window
		m_window = SDL_CreateWindow( "Final Project",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                m_screenWidth,
                                m_screenHeight,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

		// Check if Window did not create.
		if( m_window == NULL ){
			errorStream << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}

		//Create an OpenGL Graphics Context
		m_openGLContext = SDL_GL_CreateContext( m_window );
		if( m_openGLContext == NULL){
			errorStream << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}

		// Initialize GLAD Library
		if(!gladLoadGLLoader(SDL_GL_GetProcAddress)){
			errorStream << "Failed to iniitalize GLAD\n";
			success = false;
		}

		//Initialize OpenGL
		if(!InitGL()){
			errorStream << "Unable to initialize OpenGL!\n";
			success = false;
		}
  	}

    // If initialization did not work, then print out a list of errors in the constructor.
    if(!success){
        errorStream << "SDLGraphicsProgram::SDLGraphicsProgram - Failed to initialize!\n";
        std::string errors=errorStream.str();
        SDL_Log("%s\n",errors.c_str());
    }else{
        SDL_Log("SDLGraphicsProgram::SDLGraphicsProgram - No SDL, GLAD, or OpenGL, errors detected during initialization\n\n");
    }

	// SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN); // Uncomment to enable extra debug support!
	GetOpenGLVersionInfo();




    std::string fileName = argv[1];

    // Setup geometry
    // We are using a new abstraction which allows us
    // to create triangles shapes on the fly
    // Position and Texture coordinate 
    
    // Store object position data
    std::vector<float> positions;
    // Store object texture coordinate data
    std::vector<float> texture_coord;
    // Store object normal data
    std::vector<float> normal_positions;


    std::vector<std::string> allObjects;

    std::ifstream inputFile;
    inputFile.open(fileName);

    if (inputFile.is_open()){
            // Temp for each time
            std::string line;
            
            // Read in data
            std::ofstream outFile;

            // Current vertex
            int curVertex = 0;

            // current m_geometry vertex=
            int addToGeometry = 0;

            while(std::getline(inputFile, line)){
                    // Construct a string stream from line
                    std::stringstream stream(line);
                    // Break out each individual component based off of spaces
                    std::string chunk;

                    // Get usemtl lines
                    if(line[0] == 'u' and line[1] == 's' and line[2] == 'e' and line[3] == 'm' and line[4] == 't' and line[5] == 'l'){
                        allObjects.push_back(std::string(line));
                    }

                    // Position data
                    if(line[0] == 'v' and line[1] == ' '){
                            while(stream >> chunk){
                                    if(chunk[0] == 'v'){
                                            continue;
                                    }else{
                                            positions.push_back(std::stof(chunk));
                                    }
                            }
                    }

                    // Texture coordinates
                    if(line[0] == 'v' and line[1] == 't'){
            
                            bool flag = false;
                            float prev;
                            while(stream >> chunk){
                                    if(chunk[0] == 'v'){
                                            continue;
                                    }else{
                                            texture_coord.push_back(std::stof(chunk));
                                    }
                            }
                    }

                    // Normal coordinates
                    if(line[0] == 'v' and line[1] == 'n'){
            
                            bool flag = false;
                            float prev;
                            while(stream >> chunk){
                                    if(chunk[0] == 'v'){
                                            continue;
                                    }else{
                                            normal_positions.push_back(std::stof(chunk));
                                    }
                            }
                    }
            }
    }

    inputFile.close();


	// Setup our objects
    for(int i= 0; i < allObjects.size(); ++i){ 
        Object* temp = new Object;

        temp->MakeTexturedOBJ(fileName, positions, texture_coord, normal_positions, allObjects[i]);

        ObjectManager::Instance().AddObject(temp);
    }
}


// Proper shutdown of SDL and destroy initialized objects
SDLGraphicsProgram::~SDLGraphicsProgram(){
    // Reclaim all of our objects
    ObjectManager::Instance().RemoveAll();

    //Destroy window
	SDL_DestroyWindow( m_window );
	// Point m_window to NULL to ensure it points to nothing.
	m_window = nullptr;
	//Quit SDL subsystems
	SDL_Quit();
}


// Initialize OpenGL
// Setup any of our shaders here.
bool SDLGraphicsProgram::InitGL(){
	//Success flag
	bool success = true;

	return success;
}


// Update OpenGL
void SDLGraphicsProgram::Update(){
    // Update all of the objects
    ObjectManager::Instance().UpdateAll(m_screenWidth,m_screenHeight);
}


// Render
// The render function gets called once per loop
void SDLGraphicsProgram::Render(){
	// Setup our OpenGL State machine
    // TODO: Read this
    // The below command is new!
    // What we are doing, is telling opengl to create a depth(or Z-buffer) 
    // for us that is stored every frame.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D); 

    // Initialize clear color
    // This is the background of the screen.
    glViewport(0, 0, m_screenWidth, m_screenHeight);
    glClearColor( 0.2f, 0.2f, 0.2f, 1.f );
    // TODO: Read this
    // Clear color buffer and Depth Buffer
    // Remember that the 'depth buffer' is our
    // z-buffer that figures out how far away items are every frame
    // and we have to do this every frame!
  	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // Nice way to debug your scene in wireframe!
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    // Render all of our objects in a simple loop
    ObjectManager::Instance().RenderAll();

 
	// Delay to slow things down just a bit!
    SDL_Delay(50); 
}


//Loops forever!
void SDLGraphicsProgram::Loop(){
    // Main loop flag
    // If this is quit = 'true' then the program terminates.
    bool quit = false;
    // Event handler that handles various events in SDL
    // that are related to input and output
    SDL_Event e;
    // Enable text input
    SDL_StartTextInput();

    // Set the camera speed for how fast we move.
    float cameraSpeed = 0.05f;

    // Camera - not added
    //Camera* defaultCamera = new Camera();
    //defaultCamera->SetCameraEyePosition(0.0f,0.0f,0.0f);

    // While application is running
    while(!quit){
     	     	 //Handle events on queue
		while(SDL_PollEvent( &e ) != 0){
        	// User posts an event to quit
	        // An example is hitting the "x" in the corner of the window.
    	    if(e.type == SDL_QUIT){
        		quit = true;
	        }
            if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE){
			std::cout << "ESC: Goodbye! (Leaving MainApplicationLoop())" << std::endl;
                quit = true;
        }
            // Handle keyboard input for the camera class
            if(e.type==SDL_MOUSEMOTION){
                // Handle mouse movements
                int mouseX = e.motion.x;
                int mouseY = e.motion.y;
                //Camera::Instance().MouseLook(mouseX, mouseY);
            }
            switch(e.type){
                // Handle keyboard presses
                case SDL_KEYDOWN:
                    switch(e.key.keysym.sym){
                        case SDLK_LEFT:
                        //    Camera::Instance().MoveLeft(cameraSpeed);
                            break;
                        case SDLK_RIGHT:
                        //    Camera::Instance().MoveRight(cameraSpeed);
                            break;
                        case SDLK_UP:
                        //    Camera::Instance().MoveForward(cameraSpeed);
                            break;
                        case SDLK_DOWN:
                        //    Camera::Instance().MoveBackward(cameraSpeed);
                            break;
                        case SDLK_RSHIFT:
                        //    Camera::Instance().MoveUp(cameraSpeed);
                            break;
                        case SDLK_RCTRL:
                        //    Camera::Instance().MoveDown(cameraSpeed);
                            break;
                    }
                break;
            }
      	} // End SDL_PollEvent loop.
		
		// Update our scene
		Update();
		// Render using OpenGL
	    Render(); 	// TODO: potentially move this depending on your logic
					// for how you handle drawing a triangle or rectangle.
      	//Update screen of our specified window
      	SDL_GL_SwapWindow(GetSDLWindow());
	}
    //Disable text input
    SDL_StopTextInput();
}


// Get Pointer to Window
SDL_Window* SDLGraphicsProgram::GetSDLWindow(){
  return m_window;
}

// Helper Function to get OpenGL Version Information
void SDLGraphicsProgram::GetOpenGLVersionInfo(){
	SDL_Log("(Note: If you have two GPU's, make sure the correct one is selected)");
	SDL_Log("Vendor: %s",(const char*)glGetString(GL_VENDOR));
	SDL_Log("Renderer: %s",(const char*)glGetString(GL_RENDERER));
	SDL_Log("Version: %s",(const char*)glGetString(GL_VERSION));
	SDL_Log("Shading language: %s",(const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
}
