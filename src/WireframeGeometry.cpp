/* 
 * File:   WireframeGeometry.cpp
 * Author: allan
 * 
 * Created on April 15, 2016, 5:06 PM
 */

#include "WireframeGeometry.h"
#include "CornerTable.h"

WireframeGeometry::WireframeGeometry( std::shared_ptr< CornerTable > cornerTable )
{
    buildGeometry( cornerTable );
}

WireframeGeometry::~WireframeGeometry() 
{
}

void WireframeGeometry::buildGeometry( std::shared_ptr< CornerTable > cornerTable )
{
    int nTriangles = cornerTable->getNumTriangles();
    const int* triangles = cornerTable->getTriangleList();
    int nVertices = cornerTable->getNumberVertices();
    double* vertices = cornerTable->getAttributes();
    
    osg::ref_ptr< osg::Vec3Array > vertexArray = new osg::Vec3Array;
    osg::ref_ptr< osg::Vec4Array > colorArray = new osg::Vec4Array;
    
    for( int iVertex = 0; iVertex < nVertices; iVertex++ )
    {
        double x, y, z;
        
        x = vertices[ 3 * iVertex ];
        y = vertices[ 3 * iVertex + 1 ];
        z = vertices[ 3 * iVertex + 2 ];
        
        vertexArray->push_back( osg::Vec3f( x, y, z ) );
    }
    
    for( int iTriangle = 0; iTriangle < nTriangles; iTriangle++ )
    {
        osg::ref_ptr< osg::DrawElementsUInt > indexArray = new osg::DrawElementsUInt( osg::PrimitiveSet::LINE_LOOP, 0 );
        
        indexArray->push_back( triangles[ 3 * iTriangle ] );
        indexArray->push_back( triangles[ 3 * iTriangle+1 ] );
        indexArray->push_back( triangles[ 3 * iTriangle+2 ] );  
        
        addPrimitiveSet( indexArray );
        
        colorArray->push_back( osg::Vec4( 0.0f, 0.0f, 0.0f, 1.0f ) );
    }
    
    setVertexArray( vertexArray );
    setColorArray( colorArray );
    setColorBinding( BIND_PER_PRIMITIVE_SET );
}