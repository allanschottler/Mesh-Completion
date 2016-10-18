/* 
 * File:   WireframeGeometry.h
 * Author: allan
 *
 * Created on April 15, 2016, 5:06 PM
 */

#ifndef WIREFRAMEGEOMETRY_H
#define WIREFRAMEGEOMETRY_H

#include <osg/Geometry>
#include <memory>

class CornerTable;

class WireframeGeometry : public osg::Geometry
{
public:
    
    WireframeGeometry( std::shared_ptr< CornerTable > cornerTable );
    
    virtual ~WireframeGeometry();
    
private:
    
    void buildGeometry( std::shared_ptr< CornerTable > cornerTable );
};

#endif /* WIREFRAMEGEOMETRY_H */

