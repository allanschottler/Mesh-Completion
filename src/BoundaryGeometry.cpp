/* 
 * File:   BoundaryGeometry.cpp
 * Author: allanws
 * 
 * Created on October 18, 2016, 9:22 PM
 */

#include <vector>

#include "BoundaryGeometry.h"

BoundaryGeometry::BoundaryGeometry( std::shared_ptr< CornerTable > cornerTable ) 
{
    buildGeometry( cornerTable );
}


BoundaryGeometry::~BoundaryGeometry() 
{
}


void BoundaryGeometry::buildGeometry( std::shared_ptr< CornerTable > cornerTable )
{
    double* vertexBuffer = cornerTable->getAttributes();
    std::vector< double > vertices( vertexBuffer, vertexBuffer + cornerTable->getNumberVertices() * 3 );
    osg::ref_ptr< osg::Vec3Array > vertexArray = new osg::Vec3Array;
    osg::ref_ptr< osg::Vec4Array > colorArray = new osg::Vec4Array;
    osg::ref_ptr< osg::DrawElementsUInt > indexArray = new osg::DrawElementsUInt( osg::PrimitiveSet::LINE_LOOP, 0 );
    
    unsigned int index = 0;
    
    for( unsigned int iVertex = 0; iVertex < vertices.size(); iVertex += 3 )
    {
        double x, y, z;
        
        x = vertices[ iVertex ];
        y = vertices[ iVertex + 1 ];
        z = vertices[ iVertex + 2 ];
        
        vertexArray->push_back( osg::Vec3f( x, y, z ) );
        indexArray->push_back( index++ );        
    }
    
    colorArray->push_back( osg::Vec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
    
    addPrimitiveSet( indexArray );
    setVertexArray( vertexArray );
    setColorArray( colorArray );
    setColorBinding( BIND_OVERALL );
}
