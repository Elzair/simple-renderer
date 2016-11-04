#include <stdlib.h>
#include <assert.h>
#include <dynlink.h>

#if defined( __WIN32 ) || defined( __WIN64 )
#include <windows.h>
#elif defined( __unix__ ) || ( defined( __APPLE__ ) && defined( __MACH__ ) )
#include <dlfcn.h>
#endif

void* OpenLibrary( const char* name )
{
    void* ret;

#if defined( __WIN32 ) || defined( __WIN64 )
    ret = LoadLibrary( name );
    assert( ret );
#elif defined( __unix__ ) || ( defined( __APPLE__ ) && defined( __MACH__ ) )
    ret = dlopen( name, RTLD_NOW );
    assert( ret );
    dlerror();
#endif

    return ret;
}

void* LoadFunction( void* library, const char* name )
{
    void* ret;

#if defined( __WIN32 ) || defined( __WIN64 )
    ret = LoadProcAddress( library, name );
    assert( ret );
#elif defined( __unix__ ) || ( defined( __APPLE__ ) && defined( __MACH__ ) )
    ret = dlsym( library, name );
    assert( ret );
    dlerror();
#endif

    return ret;
}

void CloseLibrary( void* library )
{
#if defined( __WIN32 ) || defined( __WIN64 )
    BOOL ret = FreeLibrary( library );
    assert( ret != 0 );
#elif defined( __unix__ ) || ( defined( __APPLE__ ) && defined( __MACH__ ) )
    int ret = dlclose( library );
    assert( ret != 0 );
    dlerror();
#endif
}
