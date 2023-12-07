#include "Object.hpp"
#include "Error.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <tuple>
#include <thread>
#include <mutex>

std::mutex gLock;

Object::Object(){
}

Object::~Object(){
    
}


// Initialization of object as a 'quad'
//
// This could be called in the constructor or
// otherwise 'explicitly' called this
// so we create our objects at the correct time
void Object::MakeTexturedQuad(std::string fileName){

        // Setup geometry
        // We are using a new abstraction which allows us
        // to create triangles shapes on the fly
        // Position and Texture coordinate 

        m_geometry.AddVertex(-1.0f,-1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
        m_geometry.AddVertex( 1.0f,-1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f);
    	m_geometry.AddVertex( 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f);
        m_geometry.AddVertex(-1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f);
            
        // Make our triangles and populate our
        // indices data structure	
        m_geometry.MakeTriangle(0,1,2);
        m_geometry.MakeTriangle(2,3,0);

        // This is a helper function to generate all of the geometry
        m_geometry.Gen();

        // Create a buffer and set the stride of information
        // NOTE: How we are leveraging our data structure in order to very cleanly
        //       get information into and out of our data structure.
        m_vertexBufferLayout.CreateNormalBufferLayout(m_geometry.GetBufferDataSize(),
                                        m_geometry.GetIndicesSize(),
                                        m_geometry.GetBufferDataPtr(),
                                        m_geometry.GetIndicesDataPtr());

        // Load our actual texture
        // We are using the input parameter as our texture to load
        m_textureDiffuse.LoadTexture(fileName.c_str());

        // Load the normal map texture
        m_normalMap.LoadTexture("normal.ppm");

        
        // Setup shaders
        std::string vertexShader = m_shader.LoadShader("./shaders/vert.glsl");
        std::string fragmentShader = m_shader.LoadShader("./shaders/frag.glsl");
        // Actually create our shader
        m_shader.CreateShader(vertexShader,fragmentShader);
}

// Initialization of object from OBJ file
//
// This could be called in the constructor or
// otherwise 'explicitly' called this
// so we create our objects at the correct time

//(std::string fileName, std::vector<float>& PositionData, std::vector<float>& TextureData, std::vector<float>& NormalData, std::string mtlToUse){
void Object::MakeTexturedOBJ(std::string fileName, std::vector<float>& positions, std::vector<float>& texture_coord, std::vector<float>& normal_positions, std::string mtlToUse){
        // Hashmap with face (tuple) key and (int) vertex position value
        std::map<std::tuple<int, int, int>, int> complete_vertices;

        // mtl file
        std::string mtlFile = fileName.substr(0, fileName.find_last_of("/\\"));
        // Kd map
        std::string map_Kd = fileName.substr(0, fileName.find_last_of("/\\"));
        // normal map
        std::string normal_map = fileName.substr(0, fileName.find_last_of("/\\"));
        normal_map.append("/" + mtlToUse.substr(mtlToUse.find(" ") + 1) + "_Normal.ppm");

        std::ifstream inputFile;
        inputFile.open(fileName);

        if (inputFile.is_open()){
                // Temp for each time
                std::string line;
                
                // Read in data
                std::ofstream outFile;

                // Current vertex
                int curVertex = 0;

                // current m_geometry vertex
                int addToGeometry = 0;
                std::vector<std::tuple<int, int, int>> vertexIndicesTuple(3);
                std::vector<std::string> vertexToParse;
                bool curObject = false;

                while(std::getline(inputFile, line)){
                        // Construct a string stream from line
                        std::stringstream stream(line);
                        // Break out each individual component based off of spaces
                        std::string chunk;
                        
                        // mtlfile
                        if(line[0] == 'm'){
                                while(stream >> chunk){
                                        if(chunk == "mtllib"){
                                                continue;
                                        }else{
                                                mtlFile.append("/");
                                                mtlFile.append(chunk);
                                        }
                                }
                        }


                        if (line == mtlToUse){
                                curObject = true;
                        }

                        // Parse object and add position, texture and normal data to respective
                        // vector. If creating face, check if each vertex exists in complete_vertices.
                        // Get the index and push if present, else create vertex.

                        if (curObject) {
                                // Utilizing hashmap with tuple key and vertex value
                                // make a tuple from face, if not in vertex map, add and create new vertex

                                // Lambda function to load each vertex
                                auto lambda=[this, &complete_vertices, &addToGeometry, &vertexIndicesTuple, positions, texture_coord, normal_positions](std::string chunk, int vertexIndex) {

                                        std::cout << "Hello from thread " << std::this_thread::get_id() << std::endl;

                                        // store vertex information
                                        int vertexPosition;
                                        int vertexTexture;
                                        int vertexNormal;

                                        //load vertex position info
                                        std::string curInd = "";
                                        int i = 0;
                                        while(chunk[i] != '/'){
                                                curInd += chunk[i];
                                                i += 1;
                                        }

                                        vertexPosition = std::stoi(curInd);

                                        //load texture position info
                                        i += 1;
                                        curInd = "";
                                        while(chunk[i] != '/'){
                                                curInd += chunk[i];
                                                i += 1;
                                        }
                                        
                                        vertexTexture = std::stoi(curInd);

                                        //load normal position info
                                        i += 1;
                                        curInd = "";
                                        while(chunk[i]){
                                                curInd += chunk[i];
                                                i += 1;
                                        }

                                        vertexNormal = std::stoi(curInd);

                                        // Group vertex info as tuple
                                        std::tuple <int, int, int> vertexToAdd(vertexPosition, vertexTexture, vertexNormal);

                                        // If vertex tuple is not in complete_vertices hashmap, create vertex in m_geometry, add index to hashmap, 
                                        // increment current geometry location pointer
                                        if (complete_vertices.find(vertexToAdd) == complete_vertices.end()) {
                                                int vertexPositionActual = (vertexPosition - 1) * 3;
                                                int vertexTextureActual = (vertexTexture - 1) * 2;
                                                int vertexNormalActual = (vertexNormal - 1) * 3;
                                                
                                                // Atomic section
                                                gLock.lock();
                                                        m_geometry.AddVertex(positions[vertexPositionActual], positions[vertexPositionActual + 1], positions[vertexPositionActual + 2], texture_coord[vertexTextureActual], texture_coord[vertexTextureActual + 1], normal_positions[vertexNormalActual], normal_positions[vertexNormalActual + 1], normal_positions[vertexNormalActual + 2]);
                                                

                                                        complete_vertices[vertexToAdd] = addToGeometry;
                                                        addToGeometry += 1;
                                                gLock.unlock();
                                        }
                                        // Add generated tuple to current face vector
                                        vertexIndicesTuple.at(vertexIndex) = vertexToAdd;
                                };


                                // load face information
                                if(line[0] == 'f'){                                      
                                        while(stream >> chunk){
                                                if(chunk[0] == 'f'){
                                                        continue;
                                                }else{
                                                        vertexToParse.push_back(chunk);
                                                        vertexIndicesTuple.resize(3);                                                      
                                                }
                                                
                                        }  

                                        // Create vector of threads
                                        std::vector<std::thread> threads;
                                        // Launch a thread for each vertex
                                        for(int i = 0; i < 3; i++){
                                                threads.push_back (std::thread(lambda, vertexToParse[i],i));
                                        }

                                        // Wait until all threads finish
                                        for(int i = 0; i < 3; i++){
                                                threads[i].join();
                                        }
     
                                // Make triangle using tuples as hashmap keys to get correct vertex information
                                m_geometry.MakeTriangle(complete_vertices[vertexIndicesTuple[0]], complete_vertices[vertexIndicesTuple[1]], complete_vertices[vertexIndicesTuple[2]]);
                                
                                // Empty associated memory
                                vertexIndicesTuple.clear();
                                vertexToParse.clear();
                                
                                } else if(line[0] == 's' or line == mtlToUse){
                                        continue;
                                } else {
                                        break;
                                }
                        }
                }
        }

        inputFile.close();


        // Convert to mtl file tag
        std::string mtlActual = "new" + mtlToUse.substr(3);

        bool mtlFlag = false;

        std::ifstream mtlInputFile;
        mtlInputFile.open(mtlFile);
        if (mtlInputFile.is_open()){
                // Temp for each time
                std::string line;

                // Read in data
                std::ofstream outFile;

                while(std::getline(mtlInputFile, line)){
                        // Flag if equal to expected tag
                        if (line == mtlActual){
                                mtlFlag = true;
                        }
                        if (mtlFlag){
                                // Create file path
                                if (line.substr(0, line.find(" ")) == "map_Kd"){
                                        map_Kd.append("/");
                                        std::string tempstr = line.substr(line.find(" "));
                                        map_Kd.append(tempstr.substr(1));
                                        
                                        break;
                                }
                        }
                }
        }
        mtlInputFile.close();

        // Check for trailing whitespace
        if (isspace(map_Kd.back())){
                map_Kd.pop_back();
        }


        // This is a helper function to generate all of the geometry
        m_geometry.Gen();

        // Create a buffer and set the stride of information
        // NOTE: How we are leveraging our data structure in order to very cleanly
        //       get information into and out of our data structure.
        m_vertexBufferLayout.CreateNormalBufferLayout(m_geometry.GetBufferDataSize(),
                                        m_geometry.GetIndicesSize(),
                                        m_geometry.GetBufferDataPtr(),
                                        m_geometry.GetIndicesDataPtr());

        // Load our actual texture
        // We are using the input parameter as our texture to load
        //m_textureDiffuse.LoadTexture(fileName.c_str());
        //fileName = "brick.ppm";
        


        
        //fileName = map_Kd;//"/Users/markpanaro/finalproject-mark/common/objects/Tree/Leaves.ppm";   //map_Kd  // /Users/markpanaro/finalproject-mark/common/objects/windmill/windmill_diffuse.ppm";//"/Users/markpanaro/finalproject-mark/common/objects/house/house_diffuse.ppm";
        m_textureDiffuse.LoadTexture(map_Kd.c_str());//map_Kd.c_str());





        // Load the normal map texture
        //m_normalMap.LoadTexture("normal.ppm");
        //m_normalMap.LoadTexture("/Users/markpanaro/finalproject-mark/common/objects/house/house_normal.ppm");
        m_normalMap.LoadTexture(normal_map);

        
        // Setup shaders
        std::string vertexShader = m_shader.LoadShader("./shaders/vert.glsl");
        std::string fragmentShader = m_shader.LoadShader("./shaders/frag.glsl");
        // Actually create our shader
        m_shader.CreateShader(vertexShader,fragmentShader);
}


// TODO: In the future it may be good to 
// think about loading a 'default' texture
// if the user forgets to do this action!
void Object::LoadTexture(std::string fileName){
        // Load our actual textures
        m_textureDiffuse.LoadTexture(fileName);
}


// Bind everything we need in our object
// Generally this is called in update() and render()
// before we do any actual work with our object
void Object::Bind(){
        // Make sure we are updating the correct 'buffers'
        m_vertexBufferLayout.Bind();
        // Diffuse map is 0 by default, but it is good to set it explicitly
        m_textureDiffuse.Bind(0);
        // We need to set the texture slot explicitly for the normal map  
        m_normalMap.Bind(1);
        // Select our appropriate shader
        m_shader.Bind();
}

void Object::Update(unsigned int screenWidth, unsigned int screenHeight){
        // Call our helper function to just bind everything
        Bind();
        // TODO: Read and understand
        // For our object, we apply the texture in the following way
        // Note that we set the value to 0, because we have bound
        // our texture to slot 0.
        m_shader.SetUniform1i("u_DiffuseMap",0);
        // If we want to load another texture, we assign it to another slot
        m_shader.SetUniform1i("u_NormalMap",1);  
         // Here we apply the 'view' matrix which creates perspective.
        // The first argument is 'field of view'
        // Then perspective
        // Then the near and far clipping plane.
        // Note I cannot see anything closer than 0.1f units from the screen.
        // TODO: In the future this type of operation would be abstracted away
        //       in a camera class.
        m_projectionMatrix = glm::perspective(glm::radians(45.0f),((float)screenWidth)/((float)screenHeight),0.1f,20.0f);

        // Set the uniforms in our current shader
        m_shader.SetUniformMatrix4fv("modelTransformMatrix",m_transform.GetTransformMatrix());
        m_shader.SetUniformMatrix4fv("projectionMatrix", &m_projectionMatrix[0][0]);

        // Create a first 'light'
        // Set in a light source position
        m_shader.SetUniform3f("lightPos",0.0f,0.0f,0.0f);	
        // Set a view and a vector
        m_shader.SetUniform3f("viewPos",0.0f,0.0f,0.0f);

}

// Render our geometry
void Object::Render(){
    // Call our helper function to just bind everything
    Bind();
	//Render data
    glDrawElements(GL_TRIANGLES,
                   m_geometry.GetIndicesSize(), // The number of indices, not triangles.
                   GL_UNSIGNED_INT,             // Make sure the data type matches
                        nullptr);               // Offset pointer to the data. 
                                                // nullptr because we are currently bound
}

// Returns the actual transform stored in our object
// which can then be modified
Transform& Object::GetTransform(){
    return m_transform; 
}
