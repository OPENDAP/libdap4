
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implmentation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: Rob Morris <Robert.O.Morris@jpl.nasa.gov>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
#include <pthread.h>
#include <stdio.h>
#include <stdio.h>
#include <assert.h>
#include <io.h>
#include <fcntl.h>
#include <string>

#include "Connect.h"

#define NUM2SPAWN 20

bool
read_data(FILE *fp)
{
    char c;
    while (fp && !feof(fp) && fread(&c, 1, 1, fp))
	printf("%c", c);	// stick with stdio 

    return true;
}

void *threads_work(void *a)
{
	printf("hello world - %d\n", *((int *) a));

	HTTPConnect http(RCReader::instance());
	string url_string = "http://dods.gso.uri.edu/cgi-bin/nph-nc/data/fnoc1.nc.das";

	try
	{
           FILE *fp = http.fetch_url(url_string);
           cerr << "Server version: " << http.server_version() << endl;
           fclose(fp);
	}
	catch (Error &e)
	{
           e.display_message();
	}

	printf("\nprocess %d exiting\n",*((int *) a));

	return((void *)NULL);

}

void main(void)
{
	pthread_t threads[NUM2SPAWN];
	int status;
	int i = 0;

	threads_work(&i);
	i++;

	while(i < NUM2SPAWN)
		{
			status = pthread_create(&threads[i],0,threads_work,&i);
			status = pthread_join(threads[i],NULL);
			i++;
		}

	pthread_exit(NULL);
}


