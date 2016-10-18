/* 
 * File:   OFFMeshLoader.h
 * Author: allan
 *
 * Created on April 11, 2016, 10:36 PM
 */

#ifndef OFFMESHLOADER_H
#define OFFMESHLOADER_H

#include "CornerTable.h"
#include <string>
#include <memory>

class OFFMeshLoader 
{    
public:
    
    OFFMeshLoader();
    
    virtual ~OFFMeshLoader() {};
        
    std::shared_ptr< CornerTable > parse( std::string filename );
    
private:
    
    std::string _currentPath;

};

#endif /* MESHLOADER_H */

