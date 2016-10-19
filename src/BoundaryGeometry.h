/* 
 * File:   BoundaryGeometry.h
 * Author: allanws
 *
 * Created on October 18, 2016, 9:22 PM
 */

#ifndef BOUNDARYGEOMETRY_H
#define	BOUNDARYGEOMETRY_H

#include <osg/Geometry>
#include <memory>

#include "CornerTable.h"

class BoundaryGeometry : public osg::Geometry
{
public:
    
    BoundaryGeometry( std::shared_ptr< CornerTable > cornerTable );
    
    virtual ~BoundaryGeometry();
    
private:

    void buildGeometry( std::shared_ptr< CornerTable > cornerTable );
};

#endif	/* BOUNDARYGEOMETRY_H */

