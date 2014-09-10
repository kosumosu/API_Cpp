/*
Copyright (c) 2013-2014, UMR STMS 9912 - Ircam-Centre Pompidou / CNRS / UPMC
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**

Spatial acoustic data file format - AES X212 standard
http://www.aes.org

SOFA (Spatially Oriented Format for Acoustics)
http://www.sofaconventions.org

*/


/************************************************************************************/
/*  FILE DESCRIPTION                                                                */
/*----------------------------------------------------------------------------------*/
/*!
 *   @file       SOFANcFile.cpp
 *   @brief      Class for NetCDF Files 
 *   @author     Thibaut Carpentier, UMR STMS 9912 - Ircam-Centre Pompidou / CNRS / UPMC
 *
 *   @date       10/05/2013
 * 
 */
/************************************************************************************/
#include "../src/SOFANcFile.h"
#include "../src/SOFANcUtils.h"
#include "../src/SOFAUtils.h"

using namespace sofa;

/************************************************************************************/
/*!
 *  @brief          Class constructor
 *  @param[in]      path : the file path
 *  @param[in]      mode : opening mode
 *
 */
/************************************************************************************/
NetCDFFile::NetCDFFile(const std::string & path,
                       const netCDF::NcFile::FileMode &mode)
: file( path, mode )
, filename( path )
{
}

/************************************************************************************/
/*!
 *  @brief          Class destructor
 *
 */
/************************************************************************************/
NetCDFFile::~NetCDFFile()
{
}

/************************************************************************************/
/*!
 *  @brief          Returns the filename
 *
 */
/************************************************************************************/
const std::string NetCDFFile::GetFilename() const
{
    return filename;
}


void NetCDFFile::PrintAllAttributes(std::ostream & output) const
{
    const std::multimap< std::string, netCDF::NcGroupAtt > attributes = file.getAtts();
    
    for( std::multimap< std::string, netCDF::NcGroupAtt >::const_iterator it = attributes.begin();
        it != attributes.end();
        ++it )
    {
        const std::string attributeName = (*it).first;
        const netCDF::NcGroupAtt att    = (*it).second;
                
        if( sofa::NcUtils::IsChar( att ) == true )
        {
            const std::string value = sofa::NcUtils::GetAttributeValueAsString( att );
            output << attributeName << " = " << value << std::endl;
        }
    }    
}

void NetCDFFile::PrintAllDimensions(std::ostream & output) const
{
    const std::multimap< std::string, netCDF::NcDim > dims = file.getDims();
    
    for( std::multimap< std::string, netCDF::NcDim >::const_iterator it = dims.begin();
        it != dims.end();
        ++it )
    {
        const std::string dimName = (*it).first;
        const netCDF::NcDim dim   = (*it).second;
        
        if( sofa::NcUtils::IsValid( dim ) == true )
        {
            const std::size_t size = dim.getSize();
            output << dimName << " = " << size << std::endl;
        }
    }

}

void NetCDFFile::PrintAllVariables(std::ostream & output) const
{
    /// retrieves all the variables
    const std::multimap< std::string, netCDF::NcVar > vars = file.getVars();
    
    for( std::multimap< std::string, netCDF::NcVar >::const_iterator it = vars.begin();
        it != vars.end();
        ++it )
    {
        const std::string varName       = (*it).first;
        
        std::vector< std::size_t > dimensions;
        GetVariableDimensions( dimensions, varName ),
        
        output << varName << " = " << "(";
        
        for( std::size_t k = 0; k < dimensions.size(); k++ )
        {
            output << dimensions[k];
            
            if( k < dimensions.size() - 1 )
            {
                output << ",";
            }
        }
        output << ")" << std::endl;
        
    }
    
}

/************************************************************************************/
/*!
 *  @brief          Returns the number of (global) attributes
 *
 */
/************************************************************************************/
const unsigned int NetCDFFile::GetNumGlobalAttributes() const
{
    const int nattr = file.getAttCount();
    
    return (unsigned int) sofa::smax( (int) 0 , nattr );
}

/************************************************************************************/
/*!
 *  @brief          Returns the number of dimensions
 *
 */
/************************************************************************************/
const unsigned int NetCDFFile::GetNumDimensions() const
{
    const int ndims = file.getDimCount();
    
    return (unsigned int) sofa::smax( (int) 0 , ndims );
}

/************************************************************************************/
/*!
 *  @brief          Returns the number of variables
 *
 */
/************************************************************************************/
const unsigned int NetCDFFile::GetNumVariables() const
{
    const int nvars = file.getVarCount();
    
    return (unsigned int) sofa::smax( (int) 0 , nvars );
}

/************************************************************************************/
/*!
 *  @brief          Returns the value of a dimension, given its name.
 *                  Returns 0 if an error occured or if the dimension does not exist
 *
 */
/************************************************************************************/
const std::size_t NetCDFFile::GetDimension(const std::string &dimensionName) const
{
    if( HasDimension( dimensionName ) == false )
    {
        return 0;
    }
    else
    {
        const netCDF::NcDim dim = file.getDim( dimensionName );
        return dim.getSize();
    }
}

/************************************************************************************/
/*!
 *  @brief          Returns true if the file contains the named dimension
 *  @param[in]      dimensionName : the named dimension to query
 *
 */
/************************************************************************************/
const bool NetCDFFile::HasDimension(const std::string &dimensionName) const
{
    const netCDF::NcDim dim = getDimension( dimensionName );
    
    return sofa::NcUtils::IsValid( dim );
}

/************************************************************************************/
/*!
 *  @brief          Returns true if the file contains the named variable
 *  @param[in]      variableName : the named variable to query
 *
 */
/************************************************************************************/
const bool NetCDFFile::HasVariable(const std::string &variableName) const
{
    const netCDF::NcVar var = getVariable( variableName );
    
    return sofa::NcUtils::IsValid( var );
}

/************************************************************************************/
/*!
 *  @brief          Returns the type of a named attribute
 *                  Returns a null object if an error occured or if the attribute does not exist
 *  @param[in]      attributeName : name of the attribute to query
 *
 */
/************************************************************************************/
const netCDF::NcType NetCDFFile::GetAttributeType(const std::string &attributeName) const
{
    const netCDF::NcGroupAtt att = getAttribute( attributeName );
    
    return sofa::NcUtils::GetType( att );
}

/************************************************************************************/
/*!
 *  @brief          Returns the dimensionality of a named variable
 *                  Returns -1 if an error occured or if the variable does not exist
 *  @param[in]      variableName : the named variable to query
 *
 */
/************************************************************************************/
const int NetCDFFile::GetVariableDimensionality(const std::string &variableName) const
{
    const netCDF::NcVar var = getVariable( variableName );
    return sofa::NcUtils::GetDimensionality( var );
}

/************************************************************************************/
/*!
 *  @brief          Returns the dimensions of a named variable
 *                  Returns an empty vector if an error occured or if the variable does not exist
 *
 *  @details        the vector dims is resized accordingly to the dimensionality of the variable
 */
/************************************************************************************/
void NetCDFFile::GetVariableDimensions(std::vector< std::size_t > &dims, const std::string &variableName) const
{
    const netCDF::NcVar var = getVariable( variableName );
    sofa::NcUtils::GetDimensions( dims, var );    
}

/************************************************************************************/
/*!
 *  @brief          Returns true if a given variable is a scalar (i.e. dimensionality = 1 and dimension = 1)
 *  @param[in]      variableName : the named variable to query
 *
 */
/************************************************************************************/
const bool NetCDFFile::VariableIsScalar(const std::string &variableName) const
{
    const netCDF::NcVar var = getVariable( variableName );
    return sofa::NcUtils::IsScalar( var );
}

/************************************************************************************/
/*!
 *  @brief          Returns the type of a named variable
 *                  Returns ncNoType if an error occured or if the variable does not exist
 *  @param[in]      variableName : the named variable to query
 *
 */
/************************************************************************************/
const netCDF::NcType NetCDFFile::GetVariableType(const std::string &variableName) const
{
    const netCDF::NcVar var = getVariable( variableName );
    return sofa::NcUtils::GetType( var );
}


const bool NetCDFFile::VariableHasDimension(const std::size_t dim,
                                            const std::string &variableName) const
{
    const netCDF::NcVar var = getVariable( variableName );
    return sofa::NcUtils::HasDimension( dim, var );    
}

const bool NetCDFFile::VariableHasDimensions(const std::size_t dim1,
                                             const std::size_t dim2,
                                             const std::string &variableName) const
{
    const netCDF::NcVar var = getVariable( variableName );
    return sofa::NcUtils::HasDimensions( dim1, dim2, var );    
}

const bool NetCDFFile::VariableHasDimensions(const std::size_t dim1,
                                             const std::size_t dim2,
                                             const std::size_t dim3,
                                             const std::string &variableName) const
{
    const netCDF::NcVar var = getVariable( variableName );
    return sofa::NcUtils::HasDimensions( dim1, dim2, dim3, var );
}

/************************************************************************************/
/*!
 *  @brief          Checks if a named variable has a given attribute
 *  @param[in]      attributeName : name of the attribute to query
 *  @param[out]     -
 *  @param[in, out] -
 *  @return         -
 *
 *  @details
 *  @n  
 */
/************************************************************************************/
const bool NetCDFFile::VariableHasAttribute(const std::string &attributeName, const std::string &variableName) const
{
    const netCDF::NcVar var = getVariable( variableName );
    return sofa::NcUtils::HasAttribute( var, attributeName );
}

/************************************************************************************/
/*!
 *  @brief          Checks if the i-th variable has a given NcType
 *                    Returns false is the type does not match or if the index is out of range
 *                    or if any error occured
 *  @param[in]      -
 *  @param[out]     -
 *  @param[in, out] -
 *  @return         -
 *
 *  @details
 *  @n  
 */
/************************************************************************************/
const bool NetCDFFile::HasVariableType(const netCDF::NcType &type_, const std::string &variableName) const
{
    const netCDF::NcVar var = getVariable( variableName );
    return sofa::NcUtils::CheckType( var, type_ );    
}

/************************************************************************************/
/*!
 *  @brief          Returns true if the file contains the named attribute
 *  @param[in]      attributeName : name of the attribute to query
 *
 */
/************************************************************************************/
const bool NetCDFFile::HasAttribute(const std::string & attributeName) const
{
    const netCDF::NcGroupAtt att = getAttribute( attributeName );
    
    return sofa::NcUtils::IsValid( att );
}



/************************************************************************************/
/*!
 *  @brief          Returns the value of an attribute, as a string; if the attribute exists.
 *                  Returns an empty string otherwise
 *  @param[in]      attributeName : name of the attribute to query
 *
 */
/************************************************************************************/
const std::string NetCDFFile::GetAttributeValueAsString(const std::string &attributeName) const
{
    if( HasAttribute( attributeName ) == false )
    {
        return std::string();
    }
    
    const netCDF::NcGroupAtt att = file.getAtt( attributeName );
    
    return sofa::NcUtils::GetAttributeValueAsString( att );
}

/************************************************************************************/
/*!
 *  @brief          Retrieves an attribute given its name;
 *                  Returns a null object in case the attribute is not found or any error occured.
 *  @param[in]      attributeName : name of the attribute to query
 *
 */
/************************************************************************************/
const netCDF::NcGroupAtt NetCDFFile::getAttribute(const std::string &attributeName) const
{    
    if( attributeName.empty() == true )
    {
        /// returns a null object
        return netCDF::NcGroupAtt();
    }
    
    const std::multimap< std::string, netCDF::NcGroupAtt > attributes = file.getAtts();
    
    for( std::multimap< std::string, netCDF::NcGroupAtt >::const_iterator it = attributes.begin();
        it != attributes.end();
        ++it )
    {
        const std::string attrName      = (*it).first;
        
        if( attrName == attributeName )
        {
            const netCDF::NcGroupAtt att    = (*it).second;
            
            return att;
        }
    }
    
    return netCDF::NcGroupAtt();
}

const netCDF::NcDim NetCDFFile::getDimension(const std::string &dimensionName) const
{
    if( dimensionName.empty() == true )
    {
        /// returns a null object
        return netCDF::NcDim();
    }
    
    const std::multimap< std::string, netCDF::NcDim > dims = file.getDims();
    
    for( std::multimap< std::string, netCDF::NcDim >::const_iterator it = dims.begin();
        it != dims.end();
        ++it )
    {
        const std::string dimName = (*it).first;
        
        if( dimName == dimensionName )
        {
            const netCDF::NcDim dim   = (*it).second;
            return dim;
        }
        
    }
    
    /// returns a null object
    return netCDF::NcDim();
}

/************************************************************************************/
/*!
 *  @brief          Retrieves a variable given its name;
 *                  Returns a null object in case the variable is not found or any error occured.
 *  @param[in]      -
 *  @param[out]     -
 *  @param[in, out] -
 *  @return         -
 *
 */
/************************************************************************************/
const netCDF::NcVar NetCDFFile::getVariable(const std::string &variableName) const
{
    if( variableName.empty() == true )
    {
        /// returns a null object
        return netCDF::NcVar();
    }
    
    const std::multimap< std::string, netCDF::NcVar > vars = file.getVars();
    
    for( std::multimap< std::string, netCDF::NcVar >::const_iterator it = vars.begin();
        it != vars.end();
        ++it )
    {
        const std::string varName       = (*it).first;
        
        if( varName == variableName )
        {
            const netCDF::NcVar var         = (*it).second;
            return var;
        }
    }
    
    /// returns a null object
    return netCDF::NcVar();
}


/************************************************************************************/
/*!
 *  @brief          Checks if a given attribute if of type float, given its name.
 *                    Returns true if the attribute is float. False otherwise.
 *                    (thus false if the attribute does not exist).
 *  @param[in]      attributeName : name of the attribute to query
 *
 */
/************************************************************************************/
const bool NetCDFFile::IsAttributeFloat(const std::string &attributeName) const
{
    const netCDF::NcGroupAtt attr = getAttribute( attributeName );
    return sofa::NcUtils::IsFloat( attr );
}

/************************************************************************************/
/*!
 *  @brief          Checks if a given attribute if of type double, given its name.
 *                    Returns true if the attribute is double. False otherwise.
 *                    (thus false if the attribute does not exist).
 *  @param[in]      attributeName : name of the attribute to query
 *
 */
/************************************************************************************/
const bool NetCDFFile::IsAttributeDouble(const std::string &attributeName) const
{
    const netCDF::NcGroupAtt attr = getAttribute( attributeName );
    return sofa::NcUtils::IsDouble( attr );
}

/************************************************************************************/
/*!
 *  @brief          Checks if a given attribute if of type byte, given its name.
 *                    Returns true if the attribute is byte. False otherwise.
 *                    (thus false if the attribute does not exist).
 *  @param[in]      attributeName : name of the attribute to query
 *
 */
/************************************************************************************/
const bool NetCDFFile::IsAttributeByte(const std::string &attributeName) const
{
    const netCDF::NcGroupAtt attr = getAttribute( attributeName );
    return sofa::NcUtils::IsByte( attr );
}

/************************************************************************************/
/*!
 *  @brief          Checks if a given attribute if of type char, given its name.
 *                    Returns true if the attribute is char. False otherwise.
 *                    (thus false if the attribute does not exist).
 *  @param[in]      attributeName : name of the attribute to query
 *
 */
/************************************************************************************/
const bool NetCDFFile::IsAttributeChar(const std::string &attributeName) const
{
    const netCDF::NcGroupAtt attr = getAttribute( attributeName );
    return sofa::NcUtils::IsChar( attr );
}

/************************************************************************************/
/*!
 *  @brief          Checks if a given attribute if of type short, given its name.
 *                    Returns true if the attribute is short. False otherwise.
 *                    (thus false if the attribute does not exist).
 *  @param[in]      attributeName : name of the attribute to query
 *
 */
/************************************************************************************/
const bool NetCDFFile::IsAttributeShort(const std::string &attributeName) const
{
    const netCDF::NcGroupAtt attr = getAttribute( attributeName );
    return sofa::NcUtils::IsShort( attr );
}

/************************************************************************************/
/*!
 *  @brief          Checks if a given attribute if of type int, given its name.
 *                    Returns true if the attribute is int. False otherwise.
 *                    (thus false if the attribute does not exist).
 *  @param[in]      attributeName : name of the attribute to query
 *
 */
/************************************************************************************/
const bool NetCDFFile::IsAttributeInt(const std::string &attributeName) const
{
    const netCDF::NcGroupAtt attr = getAttribute( attributeName );
    return sofa::NcUtils::IsInt( attr );
}

/************************************************************************************/
/*!
 *  @brief          Checks if a given attribute if of type long, given its name.
 *                    Returns true if the attribute is long. False otherwise.
 *                    (thus false if the attribute does not exist).
 *  @param[in]      attributeName : name of the attribute to query
 *
 */
/************************************************************************************/
const bool NetCDFFile::IsAttributeInt64(const std::string &attributeName) const
{
    const netCDF::NcGroupAtt attr = getAttribute( attributeName );
    return sofa::NcUtils::IsInt64( attr );
}

/************************************************************************************/
/*!
 *  @brief          Reads values of variable stored as a 2-dimensional array of double
 *                  Returns true if everything goes well, false otherwise (not a valid variable,
 *                  not a double variable, not the proper dimensions)
 *  @param[out]     values :
 *  @param[in]      variableName : the named variable to query
 *  @param[in]      dim1 : first dimension of the array
 *  @param[in]      dim2 : second dimension of the array
 *
 */
/************************************************************************************/
const bool NetCDFFile::GetValues(double *values,
                                 const std::size_t dim1,
                                 const std::size_t dim2,
                                 const std::string &variableName) const
{
    const netCDF::NcVar var = NetCDFFile::getVariable( variableName );
    
    if( sofa::NcUtils::IsValid( var ) == false )
    {
        return false;
    }
    
    if( sofa::NcUtils::IsDouble( var ) == false )
    {
        return false;
    }
    
    if( sofa::NcUtils::HasDimensions( dim1, dim2, var ) == false )
    {
        return false;
    }
    
    var.getVar( values );
    
    return true;
}


/************************************************************************************/
/*!
 *  @brief          Reads values of variable stored as a 3-dimensional array of double
 *                  Returns true if everything goes well, false otherwise (not a valid variable,
 *                  not a double variable, not the proper dimensions)
 *  @param[out]     values :
 *  @param[in]      variableName : the named variable to query
 *  @param[in]      dim1 : first dimension of the array
 *  @param[in]      dim2 : second dimension of the array
 *  @param[in]      third : second dimension of the array
 *
 */
/************************************************************************************/
const bool NetCDFFile::GetValues(double *values,
                                 const std::size_t dim1,
                                 const std::size_t dim2,
                                 const std::size_t dim3,
                                 const std::string &variableName) const
{
    const netCDF::NcVar var = NetCDFFile::getVariable( variableName );
    
    if( sofa::NcUtils::IsValid( var ) == false )
    {
        return false;
    }
    
    if( sofa::NcUtils::IsDouble( var ) == false )
    {
        return false;
    }
    
    if( sofa::NcUtils::HasDimensions( dim1, dim2, dim3, var ) == false )
    {
        return false;
    }
    
    var.getVar( values );
    
    return true;
}


