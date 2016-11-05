#include <iostream>
#include <pthread.h>

void * thread(void *)
{
	int i = 3;
	while(i--)
	{
		std::cout << "i = " << i << std::endl;
	}

	return NULL;

}

void printA()
{
	std::cout << "Hello, this is A" << std::endl;

	pthread_t tid;
	pthread_create(&tid, NULL, thread, NULL);
	pthread_join(tid, NULL);

}
