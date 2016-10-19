/* 
 * File:   MeshCompletionApplication.cpp
 * Author: allan
 * 
 * Created on April 12, 2016, 11:48 PM
 */

#include "MeshCompletionApplication.h"
#include "OFFMeshLoader.h"
#include "MeshGeometry.h"
#include "WireframeGeometry.h"

#include <osg/Geode>
#include <osg/LineWidth>
#include <osgUtil/SmoothingVisitor>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <map>
#include <tuple>

MeshCompletionApplication* MeshCompletionApplication::_instance = 0;

MeshCompletionApplication::MeshCompletionApplication() :
    _window( new MainWindow( "[GMP] Trabalho 1" ) ),
    _cornerTable( nullptr ),
    _isWireframeEnabled( false )
{
    srand( time( NULL ) );    
            
    _scene = new osg::Group;
    _rootGeode = new osg::Geode;
    
    _scene->addChild( _rootGeode );
    
    osg::ref_ptr< osg::LineWidth > linewidth = new osg::LineWidth( 2.0f );
    _rootGeode->getOrCreateStateSet()->setAttributeAndModes( linewidth, osg::StateAttribute::ON );         
    _rootGeode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::ON );
        
    osg::ref_ptr< osgGA::TrackballManipulator > manipulator = new osgGA::TrackballManipulator();
    
    osg::Vec3d eye, center, up, newEye( 0.0f, 5.0f, 5.0f );
    manipulator->getHomePosition( eye, center, up );    
    manipulator->setHomePosition( newEye, center, up );    
    
    _window->getCanvas().setCameraManipulator( manipulator );
    _window->getCanvas().setSceneData( _scene );
    _window->show();
}


MeshCompletionApplication::~MeshCompletionApplication()
{
    delete _instance;    
    _instance = 0;
}


MeshCompletionApplication* MeshCompletionApplication::getInstance()
{
    if( !_instance )
        _instance = new MeshCompletionApplication();
    
    return _instance;
}


void MeshCompletionApplication::buildGeometries()
{
    _meshGeometry = new MeshGeometry( _cornerTable );   
    _wireframeGeometry = new WireframeGeometry( _cornerTable );  
    
    _boundariesGeode = new osg::Geode;
    
    osg::ref_ptr< osg::LineWidth > linewidth = new osg::LineWidth( 3.0f );
    _boundariesGeode->getOrCreateStateSet()->setAttributeAndModes( linewidth, osg::StateAttribute::ON );   
    _boundariesGeode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    
    for( auto boundary : _boundaries )
    {
        std::vector< double > vertices;
        
        for( auto iVertex : boundary )
        {
            vertices.push_back( _cornerTable->getAttributes()[ 3 * iVertex ] );
            vertices.push_back( _cornerTable->getAttributes()[ 3 * iVertex + 1 ] );
            vertices.push_back( _cornerTable->getAttributes()[ 3 * iVertex + 2 ] );
        }
        
        osg::ref_ptr< BoundaryGeometry > boundaryGeometry = new BoundaryGeometry( vertices ); 
        
        _boundariesGeode->addDrawable( boundaryGeometry );   
    }
    
    _scene->addChild( _boundariesGeode );
    
    _rootGeode->addDrawable( _meshGeometry );   
    
    if( _isWireframeEnabled )
        _rootGeode->addDrawable( _wireframeGeometry );       
    
    // Finalize
    _rootGeode->setInitialBound( _scene->computeBound() );
    
    osgUtil::SmoothingVisitor sv;
    _rootGeode->accept( sv ); 

    _window->getCanvas().realize();    
}


bool MeshCompletionApplication::openFile( std::string file )
{
    _rootGeode->removeDrawables( 0, _rootGeode->getNumDrawables() );            
    
    _cornerTable = OFFMeshLoader().parse( file ); 
    
    if( !_cornerTable )
        return false;
    
    calculateHoleBoundaries();
    
    buildGeometries();
    
    return true;
}


void MeshCompletionApplication::setLightingEnabled( bool isLightingEnabled )
{
    _rootGeode->getOrCreateStateSet()->setMode( GL_LIGHTING, 
            isLightingEnabled ? osg::StateAttribute::ON : osg::StateAttribute::OFF );
}
    

void MeshCompletionApplication::setWireframeEnabled( bool isWireframeEnabled )
{
    _isWireframeEnabled = isWireframeEnabled;
    
    if( isWireframeEnabled )
        _rootGeode->addDrawable( _wireframeGeometry );
    else
        _rootGeode->removeDrawable( _wireframeGeometry );
}

void MeshCompletionApplication::calculateHoleBoundaries()
{    
    std::map< CornerType, CornerType > boundaryEdges;
    
    std::vector< bool > visitedTriangles;
    visitedTriangles.resize( _cornerTable->getNumTriangles(), false );
        
    CornerType currentCorner = 0;
    CornerType currentTriangle = _cornerTable->cornerTriangle( currentCorner );
        
    std::queue< CornerType > bufferedTriangles;
    bufferedTriangles.push( currentTriangle );
        
    auto processCorner = [ & ]( const CornerType corner )
    {
        CornerType oppositeCorner = _cornerTable->cornerOpposite( corner );

        if( oppositeCorner == CornerTable::BORDER_CORNER )
        {
            std::vector< CornerType > neighbourCorners = {    
                _cornerTable->cornerNext( corner ), _cornerTable->cornerPrevious( corner )
            };  

            assert( !boundaryEdges.count( _cornerTable->cornerToVertexIndex( neighbourCorners[ 0 ] ) ) );

            boundaryEdges[ _cornerTable->cornerToVertexIndex( neighbourCorners[ 0 ] ) ] = 
                    _cornerTable->cornerToVertexIndex( neighbourCorners[ 1 ] );

            return;
        }

        bufferedTriangles.push( _cornerTable->cornerTriangle( oppositeCorner ) );
    };
        
    // BFS on triangles  
    while( !bufferedTriangles.empty() )
    {
        currentTriangle = bufferedTriangles.front();
        bufferedTriangles.pop();
        
        if( visitedTriangles[ currentTriangle ] )
            continue;
        
        visitedTriangles[ currentTriangle ] = true; 
        
        processCorner( 3 * currentTriangle );
        processCorner( 3 * currentTriangle + 1 );
        processCorner( 3 * currentTriangle + 2 );
    }    
    
    for( auto t : visitedTriangles )
        assert( t );
    
    // Connectivity
    while( !boundaryEdges.empty() )
    {
        auto oldIt = boundaryEdges.begin();
        auto currentIt = boundaryEdges.find( oldIt->second );
        
        HoleBoundary hole = { oldIt->first };        
            
        while( currentIt != boundaryEdges.end() )
        {            
            hole.push_back( currentIt->first );
            boundaryEdges.erase( oldIt );
            
            oldIt = currentIt;
            currentIt = boundaryEdges.find( oldIt->second );
        }      
        
        boundaryEdges.erase( oldIt );
        
        _boundaries.push_back( hole );
    }    
}

std::vector< unsigned int > MeshCompletionApplication::calculateMinimumPatchMesh( HoleBoundary boundary )
{
    std::map< std::tuple< unsigned int, unsigned int >, int > weightSet;
    
    auto weightFunction = [ & ]( unsigned int i, unsigned int j, unsigned int k )
    {
        return 0;
    };
    
    for( unsigned int i = 0; i < boundary.size() - 2; i++ )
    {
        weightSet[ std::make_tuple( i, i + 1 ) ] = 0;
    }
    
    for( unsigned int i = 0; i < boundary.size() - 3; i++ )
    {
        weightSet[ std::make_tuple( i, i + 2 ) ] = weightFunction( i, i + 1, i + 2 );
    }
    
    unsigned int j = 2;
    int minIndex = -1;
    
    while( j < boundary.size() - 1 )
    {
        j++;
        
        for( unsigned int i = 0; i < boundary.size() - j - 1; i++ )
        {
            unsigned int k = i + j;
            
            int minWeight = INT_MAX;
            
            for( unsigned int m = i + 1; m < k - 1; m++ )
            {
                int wim = weightSet[ std::make_tuple( i, m ) ];
                int wmk = weightSet[ std::make_tuple( m, k ) ];
                int f = weightFunction( i, m, k );
                int total = wim + wmk + f;
                
                if( total < minWeight )
                {
                    minWeight = total;
                    minIndex = m;
                }
            }
            
            weightSet[ std::make_tuple( i, k ) ] = minWeight;
        }
    }
    
    std::cout << "INDEX (" << minIndex << ") MINIMUM WEIGHT = " << weightSet[ std::make_tuple( 0, boundary.size() - 1 ) ] << "\n";
    return {};
}
