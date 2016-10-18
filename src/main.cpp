#include <iostream>

#include <ctime>

#include "OFFMeshLoader.h"
#include "MeshCompletionApplication.h"

int main( int argc, char** argv )
{
    gtk_init( &argc, &argv );
    gtk_gl_init( &argc, &argv );
    
    MeshCompletionApplication::getInstance();
    
    gtk_main();    
    return 0;
}