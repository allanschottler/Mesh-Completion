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
#include "MeshHoleBuilder.h"

#include <osg/Geode>
#include <osg/LineWidth>
#include <osgUtil/SmoothingVisitor>
#include <fstream>
#include <iostream>
#include <assert.h>

MeshCompletionApplication* MeshCompletionApplication::_instance = 0;

MeshCompletionApplication::MeshCompletionApplication() :
    _window( new MainWindow( "[GMP] Trabalho 1" ) ),
    _cornerTable( nullptr ),
    _isWireframeEnabled( false )
{
    srand( time( NULL ) );    
            
    _scene = new osg::Geode; 
    
    osg::ref_ptr< osg::LineWidth > linewidth = new osg::LineWidth( 2.0f );
    _scene->getOrCreateStateSet()->setAttributeAndModes( linewidth, osg::StateAttribute::ON );         
    _scene->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::ON );
        
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


bool MeshCompletionApplication::openFile( std::string file )
{
    _scene->removeDrawables( 0, _scene->getNumDrawables() );            
    
    _cornerTable = MeshHoleBuilder( OFFMeshLoader().parse( file ) ).build( 1000 );    
    
    if( !_cornerTable )
        return false;
    
    _meshGeometry = new MeshGeometry( _cornerTable );   
    _wireframeGeometry = new WireframeGeometry( _cornerTable );    
    
    _scene->addDrawable( _meshGeometry );   
    
    if( _isWireframeEnabled )
        _scene->addDrawable( _wireframeGeometry );   
    
    _scene->setInitialBound( _scene->computeBound() );
    
    osgUtil::SmoothingVisitor sv;
    _scene->accept( sv ); 

    _window->getCanvas().realize();
    
    return true;
}


void MeshCompletionApplication::setLightingEnabled( bool isLightingEnabled )
{
    _scene->getOrCreateStateSet()->setMode( GL_LIGHTING, 
            isLightingEnabled ? osg::StateAttribute::ON : osg::StateAttribute::OFF );
}
    

void MeshCompletionApplication::setWireframeEnabled( bool isWireframeEnabled )
{
    _isWireframeEnabled = isWireframeEnabled;
    
    if( isWireframeEnabled )
        _scene->addDrawable( _wireframeGeometry );
    else
        _scene->removeDrawable( _wireframeGeometry );
}

