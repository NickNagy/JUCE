/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   README_md;
    const int            README_mdSize = 268;

    extern const char*   wp2418964_jpg;
    const int            wp2418964_jpgSize = 459655;

    extern const char*   _3806905090_ce4e1f6c7e_o_jpg;
    const int            _3806905090_ce4e1f6c7e_o_jpgSize = 3173715;

    extern const char*   BAUHS93_TTF;
    const int            BAUHS93_TTFSize = 47644;

    extern const char*   BERNHC_TTF;
    const int            BERNHC_TTFSize = 70656;

    extern const char*   BROADW_TTF;
    const int            BROADW_TTFSize = 60372;

    extern const char*   FORTE_TTF;
    const int            FORTE_TTFSize = 61776;

    extern const char*   OCRAEXT_TTF;
    const int            OCRAEXT_TTFSize = 55400;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 8;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
