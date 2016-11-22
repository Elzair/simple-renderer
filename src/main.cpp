#include "application.hpp"

int main( int argc, char** argv )
{
    VulkanApplication app;

    try
    {
        app.run( WIDTH, HEIGHT );
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

