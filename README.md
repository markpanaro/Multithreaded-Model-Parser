## Multithreaded Object Model Parser

## YouTube Link: https://youtu.be/3b2h28nAfhU

![Alt text](Final_Project/part1/media/Image_of_Project_2.png?raw=true "Parser output")

## Description
This project is a multithreaded object model parser capable of loading .obj files that contain multiple textured, normal mapped objects.
## Setup
To try this project, simply navigate to multithreaded_parser, compile with "Python3 build.py", and run the generated prog file with the path to "Combined.obj" as an argument.
## Highlighted Technical achievement
A particular technical achievement of this project was the face loader wtihin the object class. When the parser encounters face data, it launches a thread for each vertex in the face. The position, texture, and normal data is retrieved and converted to a tuple. The complete_vertices hashmap uses these tuples as keys, pointing to the integer location of that vertex within the index buffer storage. If a key is not present, then the actual vertex data is calculated and the vertex is added to the geometry. Adding a vertex is an atomic section, and is locked with mutex.
## Acknowledgements
Special thanks to Dr. Mike Shah for his excellent Computer Graphics course and passionate teaching, without whom this project would not have been possible. The parser was built (with permission) on top of his assignment starter code.