/* 
 * File:   MeshHoleBuilder.cpp
 * Author: allanws
 * 
 * Created on October 14, 2016, 7:55 PM
 */

#include "MeshHoleBuilder.h"
#include <algorithm>

MeshHoleBuilder::MeshHoleBuilder( std::shared_ptr< CornerTable > mesh ) :
    _originalMesh( mesh )
{
}

MeshHoleBuilder::~MeshHoleBuilder()
{
}

std::shared_ptr< CornerTable > MeshHoleBuilder::build( unsigned int holeSizeInTriangles )
{    
    /*std::vector< CornerType > removedTriangles;
    
    CornerType currentCorner = rand() % _originalMesh->getNumTriangles();
    
    while( removedTriangles.size() < holeSizeInTriangles )
    {
        removedTriangles.push_back( _originalMesh->cornerTriangle( currentCorner ) );        
        currentCorner = _originalMesh->cornerOpposite( currentCorner );
        
        auto hasRemoved = [ & ]( const CornerType triangle )
        {
            return triangle == _originalMesh->cornerTriangle( currentCorner );
        };
        
        
        while( std::find_if( removedTriangles.begin(), removedTriangles.end(), hasRemoved ) != removedTriangles.end() )
        {            
            currentCorner = _originalMesh->cornerOpposite( _originalMesh->cornerNext( currentCorner ) ); 
        }       
    }*/
    
    return _originalMesh;
}

