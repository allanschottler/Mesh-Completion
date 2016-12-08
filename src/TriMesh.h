/* 
 * File:   TriMesh.h
 * Author: allanws
 *
 * Created on December 8, 2016, 1:07 PM
 */

#ifndef TRIMESH_H
#define	TRIMESH_H

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

struct MyTraits : public OpenMesh::DefaultTraits
{
    VertexAttributes( OpenMesh::Attributes::Status );
    FaceAttributes( OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal );
    EdgeAttributes( OpenMesh::Attributes::Status );
    HalfedgeAttributes( OpenMesh::Attributes::PrevHalfedge );
};

typedef OpenMesh::TriMesh_ArrayKernelT< MyTraits > TriMesh;

#endif	/* TRIMESH_H */

