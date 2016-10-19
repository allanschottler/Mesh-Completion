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
            return ( this->angle < rdaw.angle ) || ( ( this->angle == rdaw.angle ) && ( this->area < rdaw.area ) );
        }
    };
    
    virtual ~MeshCompletionApplication();
    
    static MeshCompletionApplication* getInstance();
    
    bool openFile( std::string file );
    
    void setLightingEnabled( bool isLightingEnabled );
    
    void setWireframeEnabled( bool isLightingEnabled );        
    
    void calculateHoleBoundaries();
    
    std::vector< CornerType > calculateMinimumPatchMesh( HoleBoundary boundary );
    
private:
    
    MeshCompletionApplication();    
    
    static MeshCompletionApplication* _instance;
    
    void buildGeometries();
        
    MainWindow* _window;
    
    std::shared_ptr< CornerTable > _cornerTable;
    
    osg::ref_ptr< osg::Group > _scene;
    
    osg::ref_ptr< osg::Geode > _rootGeode;
    
    osg::ref_ptr< osg::Geode > _boundariesGeode;
    
    osg::ref_ptr< MeshGeometry > _meshGeometry;
    
    osg::ref_ptr< WireframeGeometry > _wireframeGeometry;   
    
    
    bool _isWireframeEnabled;
    
    std::vector< HoleBoundary > _boundaries;
};

#endif /* MESHCOMPLETIONAPPLICATION_H */

