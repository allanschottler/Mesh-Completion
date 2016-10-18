/* 
 * File:   MeshHoleBuilder.h
 * Author: allanws
 *
 * Created on October 14, 2016, 7:55 PM
 */

#ifndef MESHHOLEBUILDER_H
#define	MESHHOLEBUILDER_H

#include "CornerTable.h"
#include <memory>

class MeshHoleBuilder 
{
public:
    
    MeshHoleBuilder( std::shared_ptr< CornerTable > mesh );
    
    virtual ~MeshHoleBuilder();
    
    std::shared_ptr< CornerTable > build( unsigned int holeSizeInTriangles );
    
private:

    std::shared_ptr< CornerTable > _originalMesh;
};

#endif	/* MESHHOLEBUILDER_H */

