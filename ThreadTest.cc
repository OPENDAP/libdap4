
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


