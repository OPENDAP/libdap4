
#include <fcntl.h>
#include <stdio.h>

#include <iostream>
#include "fdiostream.h"

void build_test_file()
{
    int fd = open( "fdostream.txt", O_WRONLY|O_CREAT|O_TRUNC,  0777);
    fdostream out( fd );
    out << "Output from fdostream";
    // leaving the function calls the fdostream dtor which flushes the
    // stream
    out.flush();
    close( fd );
}

void read_test_file()
{
    int fd = open( "fdostream.txt", O_RDONLY );
    fdistream in ( fd );
    char c;
    while ( !in.eof() ) {
	in  >> c;
	std::cerr << "c: " << c << std::endl;
    }
    close( fd );
}

void read_test_file_ptr()
{
    FILE *fp = fopen( "fdostream.txt", "r" );
    fdistream in ( fileno( fp ) );
    char c;
    while ( !in.eof() ) {
	in  >> c;
	std::cerr << "c: " << c << std::endl;
    }
    fclose( fp );
}

// now first read from the FILE * and then read the remaining using fdistream
void read_test_file_ptr_2()
{
    FILE *fp = fopen( "fdostream.txt", "r" );
    char word[7];
    int num = fread( &word[0], 1, 6, fp );
    word[6] = '\0';
    std::cerr << "first word (" << num << "): " << word << std::endl;
    int pos = ftell( fp );
    int fd = fileno( fp );
    if ( lseek( fd, pos, SEEK_SET ) < 0 )
	std::cerr << "lseek error: Could not seek to " << pos << std::endl;
    fdistream in ( fd );
    char c;
    while ( !in.eof() ) {
	in  >> c;
	std::cerr << "c: " << c << std::endl;
    }
    fclose( fp );
}

// now test FILE*s using fpistream (not fd...)
void read_test_file_ptr_3()
{
    FILE *fp = fopen( "fdostream.txt", "r" );
    char word[7];
    int num = fread( &word[0], 1, 6, fp );
    word[6] = '\0';
    std::cerr << "first word (" << num << "): " << word << std::endl;
    fpistream in ( fp );
    char c;
    while ( !in.eof() ) {
	in  >> c;
	std::cerr << "c: " << c << std::endl;
    }
    fclose( fp );
}

int main(int, char *[])
{
    build_test_file();
    read_test_file();
    read_test_file_ptr();
    read_test_file_ptr_2();
    read_test_file_ptr_3();
}

#if 0
// This main() test the 'putback' feature and is interactive.
int main()
{
    fdostream out(1);

    out << "31 hex:" << std::hex << 31 << std::endl;

    fdistream in(0);

    char c;
    for (int i = 0; i < 20; ++i) {
	in.get(c);

	std::cout << c << std::flush;

	if (i == 8) {
	    in.unget();
	    in.unget();
	}
    }

    std::cout << std::endl;

    build_test_file();



    return 0;
}
#endif
