/* 
 * File:   MeshLoader.cpp
 * Author: allan
 * 
 * Created on April 11, 2016, 10:36 PM
 */

#include "OFFMeshLoader.h"
#include <gtk/gtk.h>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <clocale>
#include <algorithm>

using namespace std;

OFFMeshLoader::OFFMeshLoader() :
    _currentPath( g_get_current_dir() ) 
{
}

static size_t countWords( std::string s ) 
{ 
    return ((size_t)std::count_if( s.begin(), s.end(), std::ptr_fun <int, int> ( std::isspace ) ) == s.length()) ? 
        0  : std::count_if( std::find_if( s.begin(), s.end(), 
            std::not1( std::ptr_fun <int, int> ( std::isspace ) ) ), std::unique( s.begin(), s.end() ), 
            std::ptr_fun <int, int> ( std::isspace ) ) + !std::isspace( *s.rbegin() ); 
}

std::shared_ptr< CornerTable > OFFMeshLoader::parse( string filename ) 
{
    setlocale(LC_ALL, "C");
            
    //std::string filePath(_currentPath + "/data/" + filename);
    std::string filePath( filename );
    
    int nv, nf;
    
    // Container holding last line read
    string readLine;
    // Containers for delimiter positions
    int delimiterPos_0, delimiterPos_1, delimiterPos_2, delimiterPos_3, delimiterPos_4, delimiterPos_5;

    // Open file for reading
    ifstream in(filePath.c_str());

    // Check if file is in OFF format
    getline(in, readLine);
    readLine.erase(remove_if(readLine.begin(), readLine.end(), ::isspace), readLine.end());
    
    if (readLine != "OFF") {
        cout << "The file to read is not in OFF format. " << readLine << endl;
        return 0;
    }

    // Read values for Nv and Nf
    getline(in, readLine);
    delimiterPos_1 = readLine.find(" ", 0);
    nv = atoi(readLine.substr(0, delimiterPos_1 + 1).c_str());
    delimiterPos_2 = readLine.find(" ", delimiterPos_1);
    nf = atoi(readLine.substr(delimiterPos_1, delimiterPos_2 + 1).c_str());

    //Skip linebreak
    getline(in, readLine);
    
    // Read the vertices
    std::vector< double > vertexes;
    vertexes.reserve(nv*3);
    int index = 0;
    
    for (int n = 0; n < nv; n++) {        
        getline(in, readLine);
        
        int nNumbers = countWords( readLine );
        
        delimiterPos_0 = 0;
        delimiterPos_1 = readLine.find(" ", 0);
        
        if( nNumbers > 3 )
        {
            delimiterPos_0 = delimiterPos_1;
            delimiterPos_1 = readLine.find(" ", delimiterPos_1 + 1 );
        }
        
        vertexes[index++] = ( stod(readLine.substr(delimiterPos_0, delimiterPos_1).c_str()) );
        //vertexes.push_back( atof(readLine.substr(0, delimiterPos_1).c_str()) );
        //vertexes.push_back( strtod( buffer, &buffer ) );
        delimiterPos_2 = readLine.find(" ", delimiterPos_1 + 1);
        vertexes[index++] = ( stod(readLine.substr(delimiterPos_1, delimiterPos_2).c_str()) );
        //vertexes.push_back( atof(readLine.substr(delimiterPos_1, delimiterPos_2).c_str()));
        //vertexes.push_back( strtod( buffer, &buffer ) );
        delimiterPos_3 = readLine.find(" ", delimiterPos_2 + 1);
        vertexes[index++] = ( stod(readLine.substr(delimiterPos_2, delimiterPos_3).c_str()) );
        //vertexes.push_back( atof(readLine.substr(delimiterPos_2, delimiterPos_3).c_str()) );
        //vertexes.push_back( strtod( buffer, &buffer ) );

        /*cout << vertexes[n] << "\t" << vertexes[n+1] << "\t" <<
                vertexes[n+2] << "\t" << endl;*/
    }

    // Read the facades
    std::vector< int > indices;
    indices.reserve(nf*3);
    
    for (int n = 0; n < nf; n++) {
        getline(in, readLine);
        delimiterPos_1 = readLine.find(" ", 0);
        //delimiterPos_2 = readLine.find(" ", delimiterPos_1 + 1);
        delimiterPos_2 = readLine.find(" ", 0);
//        indices.push_back(
//                atoi(readLine.substr(delimiterPos_1, delimiterPos_2).c_str()));
        delimiterPos_3 = readLine.find(" ", delimiterPos_2 + 1);
        indices.push_back(
                atoi(readLine.substr(delimiterPos_2, delimiterPos_3).c_str()));
        delimiterPos_4 = readLine.find(" ", delimiterPos_3 + 1);
        indices.push_back(
                atoi(readLine.substr(delimiterPos_3, delimiterPos_4).c_str()));
        delimiterPos_5 = readLine.find(" ", delimiterPos_4 + 1);
        indices.push_back(
                atoi(readLine.substr(delimiterPos_4, delimiterPos_5).c_str()));

        /*cout << indices[n] << "\t" << indices[n+1] << "\t" <<
                indices[n+2] << "\t" << endl;*/
    }    
    
    return std::make_shared< CornerTable >( &indices[ 0 ], &vertexes[ 0 ], nf, nv, 3 );
}

