#pragma once

#ifdef __cplusplus
extern "C" { 
#endif

    void* OpenLibrary( const char* name );

    void* LoadFunction( void* library, const char* name );

    void CloseLibrary( void* library );

#ifdef __cplusplus
}
#endif
