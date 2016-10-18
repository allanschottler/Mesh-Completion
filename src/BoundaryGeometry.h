/* 
 * File:   BoundaryGeometry.h
 * Author: allanws
 *
 * Created on October 18, 2016, 9:22 PM
 */

#ifndef BOUNDARYGEOMETRY_H
#define	BOUNDARYGEOMETRY_H

#include <osg/Geometry>

class BoundaryGeometry : public osg::Geometry
{
public:
    
    BoundaryGeometry( std::vector< double > vertices );
    
    virtual ~BoundaryGeometry();
    
private:

    void buildGeometry( std::vector< double > vertices );
};

#endif	/* BOUNDARYGEOMETRY_H */

