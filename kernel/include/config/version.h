/*
 version.h
 Product configuration.
 Here is where we update the version and the build number.
 Created by Fred Nora.
*/

#ifndef __PRODUCT_VERSION_H
#define __PRODUCT_VERSION_H  1

// Current build number
// Updated by the builder
#define __CURRENT_BUILD_NUMBER    357

// Version
#define VERSION_MAJOR    0
#define VERSION_MINOR    8
#define VERSION_BUILD    __CURRENT_BUILD_NUMBER
// Version word
// Two bytes
#define VERSION_WORD    0x0008
// Strings
#define VERSION_STRING    "0.8"
#define BUILD_STRING      "357"
#define BUILD_CODENAME_STRING  "Panda"
#define RELEASE_STRING    PRODUCT_TYPE_STRING

#endif  
