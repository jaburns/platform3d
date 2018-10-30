#define _CRT_SECURE_NO_WARNINGS 1 // for fopen

#include "utils.h"

#include <string.h>
#include <stdlib.h>

char *utils_read_file_alloc( const char *path_prefix, const char *path, int *file_length )
{
    char path_str[1024];

    strcpy( path_str, path_prefix );
    strcat( path_str, path );

    size_t length;
    char *buffer = 0;
    FILE *f = fopen( path_str, "rb" );

    if( !f ) return NULL;

    fseek( f, 0, SEEK_END );
    length = (size_t)ftell( f );
    rewind( f );
    buffer = malloc( length + 1 );
    fread( buffer, 1, length, f );
    buffer[length] = 0;
    fclose( f );

    if( file_length ) *file_length = (int)length;

    return buffer;
}

void utils_write_string_file( const char *path, const char *contents )
{
    FILE *f = fopen( path, "wb" );

    if( !f ) PANIC( "Write file error: %s", path );

    fputs( contents, f );
    fclose( f );
}