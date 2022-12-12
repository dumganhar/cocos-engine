// Define module
// target_namespace means the name exported to JS, could be same as which in other modules
// 'your_module' at the last means the suffix of binding function name, different modules should use unique name
// Note: doesn't support number prefix
%module(target_namespace="xr") ar

// Insert code at the beginning of generated header file (.h)
%insert(header_file) %{
#pragma once
#include "bindings/jswrapper/SeApi.h"
#include "bindings/manual/jsb_conversions.h"
#include "xr/ar/IARAPI.h"
#include "xr/ar/ARModule.h"
%}

// Insert code at the beginning of generated source file (.cpp)
%{
#include "bindings/auto/jsb_ar_auto.h"

using namespace cc::ar;
%}

// ----- Ignore Section ------
// Brief: Classes, methods or attributes need to be ignored
//
// Usage:
//
//  %ignore your_namespace::your_class_name;
//  %ignore your_namespace::your_class_name::your_method_name;
//  %ignore your_namespace::your_class_name::your_attribute_name;
//
// Note: 
//  1. 'Ignore Section' should be placed before attribute definition and %import/%include
//  2. namespace is needed
//
%ignore cc::ar::ARModule::getInfoLength;
%ignore cc::ar::ARModule::getAddedPlanesInfo;
%ignore cc::ar::ARModule::getUpdatedPlanesInfo;
%ignore cc::ar::ARModule::getRemovedPlanesInfo;
%ignore cc::ar::ARModule::getPlanePolygon;
%ignore cc::ar::ARModule::getAddedSceneMesh;
%ignore cc::ar::ARModule::getUpdatedSceneMesh;
%ignore cc::ar::ARModule::getRemovedSceneMesh;
%ignore cc::ar::ARModule::requireSceneMesh;
%ignore cc::ar::ARModule::getSceneMeshVertices;
%ignore cc::ar::ARModule::getSceneMeshTriangleIndices;
%ignore cc::ar::ARModule::getAddedImagesInfo;
%ignore cc::ar::ARModule::getUpdatedImagesInfo;
%ignore cc::ar::ARModule::getRemovedImagesInfo;
%ignore cc::ar::ARModule::getAddedObjectsInfo;
%ignore cc::ar::ARModule::getUpdatedObjectsInfo;
%ignore cc::ar::ARModule::getRemovedObjectsInfo;
%ignore cc::ar::ARModule::getAddedFacesInfo;
%ignore cc::ar::ARModule::getUpdatedFacesInfo;
%ignore cc::ar::ARModule::getRemovedFacesInfo;
%ignore cc::ar::ARModule::getFaceBlendShapes;

// ----- Rename Section ------
// Brief: Classes, methods or attributes needs to be renamed
//
// Usage:
//
//  %rename(rename_to_name) your_namespace::original_class_name;
//  %rename(rename_to_name) your_namespace::original_class_name::method_name;
//  %rename(rename_to_name) your_namespace::original_class_name::attribute_name;
// 
// Note:
//  1. 'Rename Section' should be placed before attribute definition and %import/%include
//  2. namespace is needed


// ----- Module Macro Section ------
// Brief: Generated code should be wrapped inside a macro
// Usage:
//  1. Configure for class
//    %module_macro(CC_USE_GEOMETRY_RENDERER) cc::pipeline::GeometryRenderer;
//  2. Configure for member function or attribute
//    %module_macro(CC_USE_GEOMETRY_RENDERER) cc::pipeline::RenderPipeline::geometryRenderer;
// Note: Should be placed before 'Attribute Section'

// Write your code bellow


// ----- Attribute Section ------
// Brief: Define attributes ( JS properties with getter and setter )
// Usage:
//  1. Define an attribute without setter
//    %attribute(your_namespace::your_class_name, cpp_member_variable_type, js_property_name, cpp_getter_name)
//  2. Define an attribute with getter and setter
//    %attribute(your_namespace::your_class_name, cpp_member_variable_type, js_property_name, cpp_getter_name, cpp_setter_name)
//  3. Define an attribute without getter
//    %attribute_writeonly(your_namespace::your_class_name, cpp_member_variable_type, js_property_name, cpp_setter_name)
//
// Note:
//  1. Don't need to add 'const' prefix for cpp_member_variable_type 
//  2. The return type of getter should keep the same as the type of setter's parameter
//  3. If using reference, add '&' suffix for cpp_member_variable_type to avoid generated code using value assignment
//  4. 'Attribute Section' should be placed before 'Import Section' and 'Include Section'
//

// ----- Import Section ------
// Brief: Import header files which are depended by 'Include Section'
// Note: 
//   %import "your_header_file.h" will not generate code for that header file
//
%import "serialization/ISerializable.h"
%import "base/Macros.h"

// ----- Include Section ------
// Brief: Include header files in which classes and methods will be bound
%include "xr/ar/ARModule.h"
