#include <iostream>
#include "chip-8.h"
#include <GL/gl.h>
#include <fstream>
#include <windows.h>
#include <string>

using namespace std;

Chip8 myChip8;
/*std::string getexepath()
{
    char result[ MAX_PATH ];
    return std::string( result, GetModuleFileName( NULL, result, MAX_PATH ) );
}*/
void setupGraphics(){

}
void setupInput(){

}
void drawGraphics(){

}
void display() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
    glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer

    // Draw a Red 1x1 Square centered at origin
    glBegin(GL_QUADS);              // Each set of 4 vertices form a quad
    glColor3f(1.0f, 0.0f, 0.0f); // Red
    glVertex2f(-0.5f, -0.5f);    // x, y
    glVertex2f( 0.5f, -0.5f);
    glVertex2f( 0.5f,  0.5f);
    glVertex2f(-0.5f,  0.5f);
    glEnd();

    glFlush();  // Render now
}
int main(int argc, char **argv)
{
    char* buffer;
    streampos size;

    string source_name = "C:\\Users\\link938\\CLionProjects\\Chip-8\\src\\c8_test.c8";//Tofix later
    ifstream sourcefile(source_name,ios::binary|ios::ate);
    if(sourcefile.is_open()){

        size = sourcefile.tellg();
        buffer = new char[size];
        sourcefile.seekg(0,ios::beg);
        sourcefile.read(buffer,size);
        sourcefile.close();
    }
    else{
        printf("not working");
        return 0;
    }


    // Set up render system and register input callbacks
    setupGraphics();
    setupInput();

    // Initialize the Chip8 system and load the game into the memory
    myChip8.initialize();
    for(int i = 0;i<size;i++){
        myChip8.memory[i+0x200] = buffer[i];
    }

    //myChip8.loadGame("pong");
    // Game loop
    while(true){
        // Emulate one cycle
        myChip8.emulateCycle();
        cout<<myChip8.pc<<endl;
        // If the draw flag is set, update the screen
        if(myChip8.drawFlag)
            drawGraphics();

        // Store key press state (Press and Release)
        //myChip8.setKeys();
    }

    return 0;
}