
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
#include <pthread.h>
#include <cstdio>
#ifdef WIN32
#include <io.h>
#endif
#include <fcntl.h>
#include <string>

#define NUM2SPAWN 10

#include "Connect.h"

void *threads_work(void *);
bool read_data(FILE *);

//  Summarily test Threading functionality as deployed by Dods.  This
//  would be better turned into a unit test perhaps.
int main(int argc, char **argv)
{
	int k;
	//  Each thread
	pthread_t threads[NUM2SPAWN];
	//  Individually valued args to each thread
	int i[NUM2SPAWN];

	//  Initialize args to each thread
	for(k=0;k < NUM2SPAWN;k++)
		i[k] = k;

	//  Run each thread
	for(k=0;k < NUM2SPAWN;k++)
		{
			pthread_create(&threads[k],0,threads_work,&i[k]);
		}

	//  Give a little time for all to finish up
	sleep(5);

	//  Have the main thread wait for the last to finish.
	//  If they finish out of order, all won't finish.
	pthread_join(threads[k-1],NULL);

	return (0);
}

bool read_data(FILE *fp)
{
    char c;
    while (fp && !feof(fp) && fread(&c, 1, 1, fp))
		;

    return true;
}

void *threads_work(void *a)
{
	printf("Thread # %d (thread ID = %ld) starting\n", *((int *) a), pthread_self());

	HTTPConnect http(RCReader::instance());
	string url_string = "http://dods.gso.uri.edu/cgi-bin/nph-nc/data/fnoc1.nc.das";

	try
	{
		Response *r = http.fetch_url(url_string);		
		if(read_data(r->get_stream()))
			printf("thread %ld pulled data from the url\n",pthread_self());
		else
			printf("thread %ld did NOT pull data from the url\n",pthread_self());
		delete r;  r = 0;
	}
	catch (Error &e)
	{
		printf("Thread # %d (thread ID = %ld) received a DODS-relevant critical error\n",
			*((int *) a), pthread_self());
		//e.display_message();
		printf("Thread # %d (thread ID %ld) terminating unsuccessfully\n",*((int *) a),pthread_self());
		pthread_exit(NULL);
	}

	printf("Thread # %d (thread ID %ld) terminating successfully\n",*((int *) a),pthread_self());

	pthread_exit(NULL);

	return((void *)NULL);

}




