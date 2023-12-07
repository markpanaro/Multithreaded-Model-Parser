#include "ObjectManager.hpp"
#include <thread>
#include <iostream>

// Constructor is empty
ObjectManager::ObjectManager(){

}

ObjectManager& ObjectManager::Instance(){
    static ObjectManager* instance = new ObjectManager();
    return *instance;
}

// Retrieve a reference to an object
Object& ObjectManager::GetObject(unsigned int index){
    return *m_objects[index];
}

void ObjectManager::AddObject(Object*& o){
    m_objects.push_back(o);
}

void ObjectManager::RemoveAll(){
    for(int i=0; i < m_objects.size(); i++){
        delete m_objects[i];
    }
}


void ObjectManager::UpdateAll(unsigned int screenWidth, unsigned int screenHeight){
    for(int i=0; i < m_objects.size(); i++){

        static float rot = 0;
        rot+=0.01;
        if(rot>360){rot=0;}

        m_objects[i]->Update(screenWidth,screenHeight);
        ObjectManager::Instance().GetObject(i).GetTransform().LoadIdentity();
        // Push back our wall a bit
        ObjectManager::Instance().GetObject(i).GetTransform().Translate(0.55f,-0.9f,-8.0f);   
        // Rotate
        ObjectManager::Instance().GetObject(i).GetTransform().Rotate(rot,0.0f,1.0f,0.0f);
        // Change size
        ObjectManager::Instance().GetObject(i).GetTransform().Scale(0.8f,0.8f,0.8f);
    }
}




void ObjectManager::RenderAll(){
    for(int i=0; i < m_objects.size(); i++){
        m_objects[i]->Render();
    }
}
