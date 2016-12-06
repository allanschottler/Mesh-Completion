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
#include <queue>
#include <functional>
#include <math.h>

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
        
        auto indexArray = calculateMinimumPatchMesh( boundary );
        
        auto patchCornerTable = std::make_shared< CornerTable >
            ( indexArray.data(), vertices.data(), indexArray.size() / 3, vertices.size() / 3, 3 );
        
        auto patchRefinedCornerTable = calculateRefinedPatchMesh( patchCornerTable, boundary );
        
        osg::ref_ptr< BoundaryGeometry > boundaryGeometry = new BoundaryGeometry( patchCornerTable ); 
        osg::ref_ptr< MeshGeometry > patchMeshGeometry = new MeshGeometry( patchRefinedCornerTable ); 
        osg::ref_ptr< WireframeGeometry > patchWireframGeometry = new WireframeGeometry( patchRefinedCornerTable ); 
//        osg::ref_ptr< MeshGeometry > patchMeshGeometry = new MeshGeometry( patchCornerTable ); 
//        osg::ref_ptr< WireframeGeometry > patchWireframGeometry = new WireframeGeometry( patchCornerTable ); 
        
        _boundariesGeode->addDrawable( boundaryGeometry );   
        _rootGeode->addDrawable( patchMeshGeometry );   
        _rootGeode->addDrawable( patchWireframGeometry );   
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
        
        std::reverse( hole.begin(), hole.end() );
        
        _boundaries.push_back( hole );
    }    
}

double MeshCompletionApplication::calculateDihedralAngle( CornerType vi, CornerType vj, CornerType vk,
                                                          CornerType vl, CornerType vm, CornerType vn )
{
    osg::Vec3d v1( 
        _cornerTable->getAttributes()[ 3 * vi ],
        _cornerTable->getAttributes()[ 3 * vi + 1 ],
        _cornerTable->getAttributes()[ 3 * vi + 2 ] );

    osg::Vec3d v2( 
        _cornerTable->getAttributes()[ 3 * vj ],
        _cornerTable->getAttributes()[ 3 * vj + 1 ],
        _cornerTable->getAttributes()[ 3 * vj + 2 ] );

    osg::Vec3d v3( 
        _cornerTable->getAttributes()[ 3 * vk ],
        _cornerTable->getAttributes()[ 3 * vk + 1 ],
        _cornerTable->getAttributes()[ 3 * vk + 2 ] );
    
    osg::Vec3d e1 = ( v2 - v1 );
    osg::Vec3d e2 = ( v3 - v1 );

    osg::Vec3d normal1 = e1 ^ e2;

    osg::Vec3d v4( 
        _cornerTable->getAttributes()[ 3 * vl ],
        _cornerTable->getAttributes()[ 3 * vl + 1 ],
        _cornerTable->getAttributes()[ 3 * vl + 2 ] );

    osg::Vec3d v5( 
        _cornerTable->getAttributes()[ 3 * vm ],
        _cornerTable->getAttributes()[ 3 * vm + 1 ],
        _cornerTable->getAttributes()[ 3 * vm + 2 ] );

    osg::Vec3d v6( 
        _cornerTable->getAttributes()[ 3 * vn ],
        _cornerTable->getAttributes()[ 3 * vn + 1 ],
        _cornerTable->getAttributes()[ 3 * vn + 2 ] );  

    osg::Vec3d e3 = ( v5 - v4 );
    osg::Vec3d e4 = ( v6 - v4 );

    osg::Vec3d normal2 = e3 ^ e4;
    auto cross = normal1 * normal2;
    
    /*if( cross < 0 )
        return DBL_MAX;*/
    
    return std::acos( cross );
}
        
HoleBoundary MeshCompletionApplication::calculateMinimumPatchMesh( HoleBoundary boundary )
{          
    std::map< std::tuple< CornerType, CornerType >, std::tuple< CornerType, DihedralAngleWeight > > weightSet;
    
    CornerType n = ( CornerType )boundary.size();
    
    auto weightFunction = [ & ]( CornerType vi, CornerType vj, CornerType vk )
    {        
        osg::Vec3d v1( 
            _cornerTable->getAttributes()[ 3 * boundary[ vi ] ], 
            _cornerTable->getAttributes()[ 3 * boundary[ vi ] + 1 ], 
            _cornerTable->getAttributes()[ 3 * boundary[ vi ] + 2 ] );
        
        osg::Vec3d v2( 
            _cornerTable->getAttributes()[ 3 * boundary[ vj ] ], 
            _cornerTable->getAttributes()[ 3 * boundary[ vj ] + 1 ], 
            _cornerTable->getAttributes()[ 3 * boundary[ vj ] + 2 ] );
        
        osg::Vec3d v3( 
            _cornerTable->getAttributes()[ 3 * boundary[ vk ] ], 
            _cornerTable->getAttributes()[ 3 * boundary[ vk ] + 1 ], 
            _cornerTable->getAttributes()[ 3 * boundary[ vk ] + 2 ] );
        
        double a = ( v2 - v1 ).length(); 
        double b = ( v2 - v3 ).length();
        double c = ( v3 - v1 ).length();
        
        double area = 0.5 * a * b * sin( c );       
        double angle = 0;         
        
        auto findCommonTriangle = [ & ]( std::vector< CornerType > n1, std::vector< CornerType > n2 )
        {
            for( auto c1 : n1 )
            {
                for( auto c2 : n2 )
                {
                    CornerType t1 = _cornerTable->cornerTriangle( c1 );
                    CornerType t2 = _cornerTable->cornerTriangle( c2 );
                    
                    if( t1 == t2 )
                        return t1;
                }
            }
            
            return CornerTable::BORDER_CORNER;
        };
        
        if( vj == vi + 1 && vk == vj + 1 )
        {
            CornerType c1 = _cornerTable->vertexToCornerIndex( boundary[ vi ] );
            CornerType c2 = _cornerTable->vertexToCornerIndex( boundary[ vj ] );
            CornerType c3 = _cornerTable->vertexToCornerIndex( boundary[ vk ] );        
            
            auto c1Neighbours = _cornerTable->getCornerNeighbours( c1 );
            auto c2Neighbours = _cornerTable->getCornerNeighbours( c2 );
            auto c3Neighbours = _cornerTable->getCornerNeighbours( c3 );
            
            CornerType t1 = findCommonTriangle( c1Neighbours, c2Neighbours );
            CornerType t2 = findCommonTriangle( c2Neighbours, c3Neighbours );
            
            assert( t1 != CornerTable::BORDER_CORNER && t2 != CornerTable::BORDER_CORNER );      
            
            CornerType t1v1 = 3 * t1;
            CornerType t1v2 = 3 * t1 + 1;
            CornerType t1v3 = 3 * t1 + 2;
            
            CornerType t2v1 = 3 * t2;
            CornerType t2v2 = 3 * t2 + 1;
            CornerType t2v3 = 3 * t2 + 2;
            
            angle = MAX(
                    calculateDihedralAngle( boundary[ vi ], boundary[ vj ], boundary[ vk ], t1v1, t1v2, t1v3 ), 
                    calculateDihedralAngle( boundary[ vi ], boundary[ vj ], boundary[ vk ], t2v1, t2v2, t2v3 ) );
            
            /*if( t1 == t2 )
                angle = DBL_MAX;*/
        }
        else
        {
            CornerType vij = std::get< 0 >( weightSet[ std::make_tuple( vi, vj ) ] );
            CornerType vjk = std::get< 0 >( weightSet[ std::make_tuple( vj, vk ) ] );
            
            angle = MAX( 
                    calculateDihedralAngle( boundary[ vi ], boundary[ vj ], boundary[ vk ], boundary[ vi ], boundary[ vij ], boundary[ vj ] ), 
                    calculateDihedralAngle( boundary[ vi ], boundary[ vj ], boundary[ vk ], boundary[ vj ], boundary[ vjk ], boundary[ vk ] ) );
            
            if( vi == 0 && vk == n - 1 )
            {
                CornerType c1 = _cornerTable->vertexToCornerIndex( boundary[ 0 ] );
                CornerType c2 = _cornerTable->vertexToCornerIndex( boundary[ n - 1 ] );
            
                auto c1Neighbours = _cornerTable->getCornerNeighbours( c1 );
                auto c2Neighbours = _cornerTable->getCornerNeighbours( c2 );

                CornerType t = findCommonTriangle( c1Neighbours, c2Neighbours );
                
                CornerType v1 = 3 * t;
                CornerType v2 = 3 * t + 1;
                CornerType v3 = 3 * t + 2;
                
                angle = MAX(
                    angle, 
                    calculateDihedralAngle( boundary[ vi ], boundary[ vj ], boundary[ vk ], v1, v2, v3 ) );
            }
        }
        
        return DihedralAngleWeight( angle, area );
    };
    
    for( CornerType i = 0; i <= n - 2; i++ )
    {
        weightSet[ std::make_tuple( i, i + 1 ) ] = std::make_tuple( -1, DihedralAngleWeight() );
    }
    
    for( CornerType i = 0; i <= n - 3; i++ )
    {
        weightSet[ std::make_tuple( i, i + 2 ) ] = std::make_tuple( i + 1, weightFunction( i, i + 1, i + 2 ) );
    }
    
    CornerType j = 2;    
    
    while( j < n - 1 )
    {                
        j++;
        
        for( CornerType i = 0; i <= n - j - 1; i++ )
        {
            CornerType k = i + j;
            int minIndex = -1;
            DihedralAngleWeight minWeight( M_PI, DBL_MAX );
            
            for( CornerType m = i + 1; m <= k - 1; m++ )
            {
                DihedralAngleWeight wim = std::get< 1 >( weightSet[ std::make_tuple( i, m ) ] );
                DihedralAngleWeight wmk = std::get< 1 >( weightSet[ std::make_tuple( m, k ) ] );
                DihedralAngleWeight f = weightFunction( i, m, k );
                DihedralAngleWeight total = wim + wmk + f;
                
                if( total < minWeight )
                {
                    minWeight = total;
                    minIndex = m;
                }
            }
            
            weightSet[ std::make_tuple( i, k ) ] = std::make_tuple( minIndex, minWeight );
        }
    }        
    
    std::vector< CornerType > indexes; 
    
    std::function< void ( CornerType, CornerType ) > trace = [ & ]( CornerType i, CornerType k )
    {
        if( i + 2 == k )
        {
            indexes.push_back( i );
            indexes.push_back( i + 1 );
            indexes.push_back( k );
        }
        else
        {            
            CornerType o = std::get< 0 >( weightSet[ std::make_tuple( i, k ) ] );
            
            if( o != i + 1 )
                trace( i, o );
            
            indexes.push_back( i );
            indexes.push_back( o );
            indexes.push_back( k );
            
            if( o != k - 1 )
                trace( o, k );
        }
    };
    
    trace( 0, n - 1 );
    
    return indexes;
}

TriMesh createMesh( std::vector< double >& vertexArray, std::vector< CornerType >& indexArray )
{
    TriMesh mesh;        
    TriMesh::VertexHandle vhandle[ vertexArray.size() ];    
    std::vector< TriMesh::VertexHandle > face_vhandles;
    int nPoints = 0;
    
    for( unsigned int i = 0; i < vertexArray.size(); i += 3 )
    {
        vhandle[ nPoints++ ] = mesh.add_vertex( TriMesh::Point( vertexArray[ i ], vertexArray[ i + 1 ], vertexArray[ i + 2 ] ) );
    }
    
    for( unsigned int i = 0; i < indexArray.size(); i += 3 )
    {
        face_vhandles.clear();
        face_vhandles.push_back( vhandle[ indexArray[ i ] ] );
        face_vhandles.push_back( vhandle[ indexArray[ i + 1 ] ] );
        face_vhandles.push_back( vhandle[ indexArray[ i + 2 ] ] );
        mesh.add_face( face_vhandles );
    }
    
    return mesh;
}


osg::Vec3 MeshCompletionApplication::calculateCentroid( std::shared_ptr< CornerTable > patch, CornerType vi, CornerType vj, CornerType vk )
{
    osg::Vec3d v1( 
        patch->getAttributes()[ 3 * vi ], 
        patch->getAttributes()[ 3 * vi + 1 ], 
        patch->getAttributes()[ 3 * vi + 2 ] );
       
    osg::Vec3d v2( 
        patch->getAttributes()[ 3 * vj ], 
        patch->getAttributes()[ 3 * vj + 1 ], 
        patch->getAttributes()[ 3 * vj + 2 ] );
        
    osg::Vec3d v3( 
        patch->getAttributes()[ 3 * vk ], 
        patch->getAttributes()[ 3 * vk + 1 ], 
        patch->getAttributes()[ 3 * vk + 2 ] );
    
    return (v1 + v2 + v3) / 3;
}
    
bool MeshCompletionApplication::isInCircumsphere( TriMesh& mesh, TriMesh::EdgeHandle edge )
{        
    TriMesh::HalfedgeHandle he1 = mesh.halfedge_handle( edge, 0 );
    TriMesh::HalfedgeHandle he2 = mesh.halfedge_handle( edge, 1 );
    
    TriMesh::VertexHandle vh1 = mesh.to_vertex_handle( he1 );
    TriMesh::VertexHandle vh2 = mesh.from_vertex_handle( he1 );

    TriMesh::FaceHandle fh1 = mesh.face_handle( he1 );
    TriMesh::FaceHandle fh2 = mesh.face_handle( he2 );
    
    TriMesh::Point p1 = mesh.point( vh1 );
    TriMesh::Point p2 = mesh.point( vh2 );
    TriMesh::Point p3, p4;
    
    TriMesh::FaceHalfedgeIter fh1It = mesh.fh_iter( fh1 );
    for( ; fh1It.is_valid(); ++fh1It )
    {
        TriMesh::VertexHandle newvh1 = mesh.to_vertex_handle( *fh1It );
        TriMesh::VertexHandle newvh2 = mesh.from_vertex_handle( *fh1It );
        
        TriMesh::Point newp1 = mesh.point( newvh1 );
        TriMesh::Point newp2 = mesh.point( newvh2 );
        
        if( newp1 != p1 && newp1 != p2 )
        {
            p3 = newp1;
            break;
        }
        
        if( newp2 != p1 && newp2 != p2 )
        {
            p3 = newp2;
            break;
        }
    }
   
    TriMesh::FaceHalfedgeIter fh2It = mesh.fh_iter( fh2 );
    for( ; fh2It.is_valid(); ++fh2It )
    {
        TriMesh::VertexHandle newvh1 = mesh.to_vertex_handle( *fh2It );
        TriMesh::VertexHandle newvh2 = mesh.from_vertex_handle( *fh2It );
        
        TriMesh::Point newp1 = mesh.point( newvh1 );
        TriMesh::Point newp2 = mesh.point( newvh2 );
        
        if( newp1 != p1 && newp1 != p2 )
        {
            p4 = newp1;
            break;
        }
        
        if( newp2 != p1 && newp2 != p2 )
        {
            p4 = newp2;
            break;
        }
    }
    
    double radius = mesh.calc_edge_length( edge ) / 2;
    
    TriMesh::Point midpoint( mesh.point( vh1 ) );

    midpoint +=  mesh.point( mesh.to_vertex_handle( he2 ) );    
    midpoint *= 0.5;
    
    double l1 = (p3 - midpoint).length(); 
    double l2 = (p4 - midpoint).length(); 
          
    return ( l1 <= radius && l2 <= radius );
}

bool MeshCompletionApplication::relaxEdge( TriMesh& mesh, TriMesh::EdgeHandle edge )
{
    if( !mesh.is_boundary( edge ) ) 
    {             
        // Flip edge
        if( isInCircumsphere( mesh, edge ) )
        {
            mesh.flip( edge );
            return true;
        }
    }
    
    return false;
}

bool MeshCompletionApplication::relaxAllEdges( TriMesh& mesh )
{
    bool hasRelaxed = false;    
        
    for( TriMesh::EdgeIter it = mesh.edges_begin(); it != mesh.edges_end(); ++it ) 
    {
        if( relaxEdge( mesh, it.handle() ) )
            hasRelaxed = true;
    }
    
    return hasRelaxed;
}
    
std::shared_ptr< CornerTable > MeshCompletionApplication::calculateRefinedPatchMesh( std::shared_ptr< CornerTable > patchMesh, HoleBoundary boundary )
{        
    auto densityControl = M_SQRT2;
    
    std::vector< double > scaleAttributes;
    std::vector< double > newVertexArray( 
        patchMesh->getAttributes(), 
        patchMesh->getAttributes() + patchMesh->getNumberVertices() * patchMesh->getNumberAttributesByVertex() );
    HoleBoundary newIndexArray;
    HoleBoundary indexArray( 
        patchMesh->getTriangleList(),
        patchMesh->getTriangleList() + patchMesh->getNumTriangles() * 3 );
    
    // Calcula averages
    for( auto iVertex : boundary )
    {
        double average = _cornerTable->getVertexAverageEdgeLength( iVertex );
        scaleAttributes.push_back( average );
    }
    
    bool hadCreatedTriangles = false;
    bool hasDoneSwaps = false;
    
    auto makeTriangle = [ & ]( CornerType i, CornerType j, CornerType k )
    {
        newIndexArray.push_back( i );
        newIndexArray.push_back( j );
        newIndexArray.push_back( k );
    };
    
    while( !hasDoneSwaps )
    {
        std::vector< CornerType > verticesToFlip;
        newIndexArray.clear();
        
        for( unsigned int iTriangle = 0; iTriangle < indexArray.size(); iTriangle += 3 )
        {
            CornerType vi = indexArray[ iTriangle ];
            CornerType vj = indexArray[ iTriangle + 1 ];
            CornerType vk = indexArray[ iTriangle + 2 ];

            osg::Vec3 v1( patchMesh->getAttributes()[ 3 * vi ],
                          patchMesh->getAttributes()[ 3 * vi + 1 ],
                          patchMesh->getAttributes()[ 3 * vi + 2 ] );
            
            osg::Vec3 v2( patchMesh->getAttributes()[ 3 * vj ],
                          patchMesh->getAttributes()[ 3 * vj + 1 ],
                          patchMesh->getAttributes()[ 3 * vj + 2 ] );
            
            osg::Vec3 v3( patchMesh->getAttributes()[ 3 * vk ],
                          patchMesh->getAttributes()[ 3 * vk + 1 ],
                          patchMesh->getAttributes()[ 3 * vk + 2 ] );

            auto centroid = calculateCentroid( patchMesh, vi, vj, vk );
            double centroidScaleAttribute = 
                (scaleAttributes[ vi ] + scaleAttributes[ vj ] + scaleAttributes[ vk ]) / 3;

            if( densityControl * (centroid - v1).length() > centroidScaleAttribute && densityControl * (centroid - v1).length() > scaleAttributes[ vi ] &&
                densityControl * (centroid - v2).length() > centroidScaleAttribute && densityControl * (centroid - v2).length() > scaleAttributes[ vj ] &&
                densityControl * (centroid - v3).length() > centroidScaleAttribute && densityControl * (centroid - v3).length() > scaleAttributes[ vk ] )
            {
                hadCreatedTriangles = true;                
                //trianglesToSplit.push_back( std::make_pair( iTriangle, centroid ) );
                                
                newVertexArray.push_back( centroid.x() );
                newVertexArray.push_back( centroid.y() );
                newVertexArray.push_back( centroid.z() );
                
                unsigned int c = newVertexArray.size() / 3 - 1;
                makeTriangle( c, vj, vk );
                makeTriangle( vi, c, vk );
                makeTriangle( vi, vj, c );
                                
                verticesToFlip.push_back( c );                
                scaleAttributes.push_back( centroidScaleAttribute );
            }
            else
            {
                makeTriangle( vi, vj, vk );
            }
        }
        
        /*CornerTable* relaxationTable = new CornerTable( newIndexArray.data(), newVertexArray.data(),
            newIndexArray.size() / 3, newVertexArray.size() / 3, 3 );*/
        
        TriMesh mesh = createMesh( newVertexArray, newIndexArray );
            
        for( auto vertex : verticesToFlip )
        {
            TriMesh::VertexHandle v = mesh.vertex_handle( vertex );            
            TriMesh::VertexFaceIter faceIt = mesh.vf_iter( v );
            
            for( TriMesh::VertexFaceIter fit = faceIt++; fit != faceIt; fit++ )
            {
                TriMesh::FaceHandle fh = *fit;                
                TriMesh::FaceEdgeIter edgeIt = mesh.fe_iter( fh );
                
                for( TriMesh::FaceEdgeIter eit = edgeIt++; eit != edgeIt; eit++ )
                {
                    TriMesh::HalfedgeHandle h0 = mesh.halfedge_handle( *eit, 0 );
                    TriMesh::HalfedgeHandle h1 = mesh.halfedge_handle( *eit, 1 );

                    TriMesh::VertexHandle v0 = mesh.to_vertex_handle( h0 );
                    TriMesh::VertexHandle v1 = mesh.to_vertex_handle( h1 );
                    
                    if( v0 != v && v1 != v )
                    {
                        relaxEdge( mesh, *edgeIt );
                    }
                }
            }
            
            /*auto currentCorner = relaxationTable->vertexToCornerIndex( vertex );
            auto neighbourCorners = relaxationTable->getCornerNeighbours( currentCorner );
            neighbourCorners.push_back( currentCorner );
            
            for( auto neighbour : neighbourCorners ) 
            {
                if( relaxationTable->areEdgeTrianglesInCircumsphere( neighbour ) )
                {
                    relaxationTable->edgeFlip( neighbour );
                }
            }*/
        }              
            
        /*newVertexArray = std::vector< double >( relaxationTable->getAttributes(),
            relaxationTable->getAttributes() + relaxationTable->getNumberVertices() * 3 );

        newIndexArray = std::vector< CornerType >( relaxationTable->getTriangleList(),
            relaxationTable->getTriangleList() + relaxationTable->getNumTriangles() * 3 );*/
        
        //delete relaxationTable;                
        
        if( !hadCreatedTriangles )
            break;       
                    
        do
        {
            hasDoneSwaps = relaxAllEdges( mesh );
        } 
        while( hasDoneSwaps );
        
        indexArray = newIndexArray;
    }
    
    //DONE
    return std::make_shared< CornerTable >( newIndexArray.data(), newVertexArray.data(),
                        newIndexArray.size() / 3, newVertexArray.size() / 3, 3 );
}