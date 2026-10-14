#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes

int main ( void )
{
    
    SYS_Initialize ( NULL );

    while ( true )
    {
        
        SYS_Tasks ( );
    }

    

    return ( EXIT_FAILURE );
}