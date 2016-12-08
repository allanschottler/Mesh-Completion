/* 
 * File:   MeshCompletionApplication.h
 * Author: allan
 *
 * Created on April 12, 2016, 11:48 PM
 */

#ifndef MESHCOMPLETIONAPPLICATION_H
#define MESHCOMPLETIONAPPLICATION_H

#include "MainWindow.h"
#include "CornerTable.h"
#include "MeshGeometry.h"
#include "WireframeGeometry.h"
#include "BoundaryGeometry.h"
#include <memory>
#include "TriMesh.h"
    
typedef std::vector< CornerType > HoleBoundary;

class MeshCompletionApplication 
{
public:
        
    class DihedralAngleWeight
    {
    public:
        
        DihedralAngleWeight() { area = 0; angle = 0; };
        
        DihedralAngleWeight( double an, double ar ) { area = ar; angle = an; };
        
        double angle, area;
        
        inline DihedralAngleWeight& operator+( const DihedralAngleWeight& rdaw )
        {
            this->area += rdaw.area;
            this->angle = MAX( this->angle, rdaw.angle );
            return *this;
        }
        
        inline bool operator<( const DihedralAngleWeight& rdaw )
        {
            //return ( this->angle < rdaw.angle ) || ( ( this->angle == rdaw.angle ) && ( this->area < rdaw.area ) );
            return ( this->area < rdaw.area );
        }
    };
    
    enum FairingMode
    {
        NONE = 0,
        SCALAR,
        HARMONIC,
        SECOND_ORDER
    };
    
    virtual ~MeshCompletionApplication();
    
    static MeshCompletionApplication* getInstance();
    
    bool openFile( std::string file );
    
    void setLightingEnabled( bool isLightingEnabled );
    
    void setWireframeEnabled( bool isWireframeEnabled );        
    
    void setBoundariesEnabled( bool isBoundariesEnabled );        
    
    void setFairingMode( FairingMode mode );
    
    void calculateHoleBoundaries();
    
    HoleBoundary calculateMinimumPatchMesh( HoleBoundary boundary );
        
    TriMesh calculateRefinedPatchMesh( std::shared_ptr< CornerTable > patchMesh, HoleBoundary boundary );    
    
    std::shared_ptr< CornerTable > calculateFairedPatchMesh( TriMesh& mesh );    
        
private:
    
    MeshCompletionApplication();    
    
    double calculateDihedralAngle( CornerType vi, CornerType vj, CornerType vk,
                               CornerType vl, CornerType vm, CornerType vn );
    
    osg::Vec3 calculateCentroid( std::shared_ptr< CornerTable > patch, CornerType vi, CornerType vj, CornerType vk );
    
    //void relaxEdge( std::vector< double >& vertexArray, std::vector< CornerType >& indexArray, CornerType corner );
    
    bool isInCircumsphere( TriMesh& mesh, TriMesh::EdgeHandle edge );
    
    bool relaxEdge( TriMesh& mesh, TriMesh::EdgeHandle edge );
    
    bool relaxAllEdges( TriMesh& mesh );
    
    static MeshCompletionApplication* _instance;
    
    void buildMesh();
    
    void buildGeometries();
    
    void clearMesh();
    
    void clearGeometries();
        
    MainWindow* _window;
    
    std::shared_ptr< CornerTable > _cornerTable;
    
    osg::ref_ptr< osg::Group > _scene;
    
    osg::ref_ptr< osg::Geode > _meshesGeode;    
    osg::ref_ptr< osg::Geode > _wireframesGeode;    
    osg::ref_ptr< osg::Geode > _boundariesGeode;
    
    osg::ref_ptr< MeshGeometry > _meshGeometry;    
    osg::ref_ptr< WireframeGeometry > _wireframeGeometry;
    std::vector< osg::ref_ptr< MeshGeometry > > _patchMeshesGeometry;
    std::vector< osg::ref_ptr< WireframeGeometry > > _patchWireframesGeometry;
    std::vector< osg::ref_ptr< BoundaryGeometry > > _boundariesGeometry;
    
    bool _isWireframeEnabled;
    bool _isBoundariesEnabled;
    
    std::vector< HoleBoundary > _boundaries;
    
    FairingMode _fairingMode;
};

#endif /* MESHCOMPLETIONAPPLICATION_H */

