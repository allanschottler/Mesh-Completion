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
#include <memory>

class MeshCompletionApplication 
{
public:
    
    virtual ~MeshCompletionApplication();
    
    static MeshCompletionApplication* getInstance();
    
    bool openFile( std::string file );
    
    void setLightingEnabled( bool isLightingEnabled );
    
    void setWireframeEnabled( bool isLightingEnabled );
        
private:
    
    MeshCompletionApplication();
    
    
    static MeshCompletionApplication* _instance;
    
    MainWindow* _window;
    
    std::shared_ptr< CornerTable > _cornerTable;
    
    osg::ref_ptr< osg::Geode > _scene;
    
    osg::ref_ptr< MeshGeometry > _meshGeometry;
    
    osg::ref_ptr< WireframeGeometry > _wireframeGeometry;    
    
    bool _isWireframeEnabled;
};

#endif /* MESHCOMPLETIONAPPLICATION_H */

