#include "stable.h"

// Constructor
STable::STable()
{
	this->bm = new Bitmap(MAX_SEMAPHORE);

	for (int i = 0; i < MAX_SEMAPHORE; i++)
	{
		this->semTab[i] = NULL;
	}
}

// Destructor
STable::~STable()
{
	if (this->bm)
	{
		delete this->bm;
		this->bm = NULL;
	}
	for (int i = 0; i < MAX_SEMAPHORE; i++)
	{
		if (this->semTab[i])
		{
			delete this->semTab[i];
			this->semTab[i] = NULL;
		}
	}
}

int STable::Create(char *name, int init)
{

	// Check da ton tai semaphore nay chua?
	for (int i = 0; i < MAX_SEMAPHORE; i++)
	{
		if (bm->Test(i))
		{
			if (strcmp(name, semTab[i]->GetName()) == 0)
			{
				return -1;
			}
		}
	}
	// Tim slot tren bang semTab trong
	int id = this->FindFreeSlot();

	// Neu k tim thay thi tra ve -1
	if (id < 0)
	{
		return -1;
	}

	// Neu tim thay slot trong thi nap Semaphore vao semTab[id]
	this->semTab[id] = new Sem(name, init);
	return 0;
}

int STable::Wait(char *name)
{
	for (int i = 0; i < MAX_SEMAPHORE; i++)
	{
		// Kiem tra o thu i da duoc nap semaphore chua
		if (bm->Test(i))
		{
			// Neu co thi tien hanh so sanh name voi name cua semaphore trong semTab
			// 19120447 here
			/*
			printf("Vao duoc ham STable::Wait\n");
			printf("name = ");
			printf(name);
			printf("\nAll semaphores:\n");
			for (int i = 0; i < 10; ++i)
			{
				if (semTab[i] != NULL)
				{
					printf(semTab[i]->GetName());
					printf("  value = ");
					printf("%d", semTab[i]->sem->value);
					printf("   strcmp = ");
					printf("%d", strcmp(name, semTab[i]->GetName()));
					printf("\n");
				}
			}
			*/
			// 19120447 end here

			if (strcmp(name, semTab[i]->GetName()) == 0)
			{
				// Neu ton tai thi cho semaphore down();
				// 19120447 start
				//printf("Tim duoc name trong semTab:\n");
				//printf(semTab[i]->GetName());
				//printf("\n\n");
				// 19120447 end
				semTab[i]->wait();
				return 0;
			}
			// 19120447
			if(i == MAX_SEMAPHORE - 1)
			printf("Khong tim duoc name trong semTab:\n\n");
		}
	}
	return -1;
}

int STable::Signal(char *name)
{
	// 19120447 start
	/*
	printf("Vao duoc ham STable::Signal\n");
	printf("name = ");
	printf(name);
	printf("\nAll semaphores:\n");
	for (int i = 0; i < MAX_SEMAPHORE; ++i)
	{
		if (semTab[i] != NULL)
		{
			printf(semTab[i]->GetName());
			printf("  value = ");
			printf("%d", semTab[i]->sem->value);
			printf("   strcmp = ");
			printf("%d", strcmp(name, semTab[i]->GetName()));
			printf("\n");
		}
	}
	*/
	// 19120447 end

	for (int i = 0; i < MAX_SEMAPHORE; i++)
	{
		// Kiem tra o thu i da duoc nap semaphore chua
		if (bm->Test(i))
		{

			// Neu co thi tien hanh so sanh name voi name cua semaphore trong semTab
			if (strcmp(name, semTab[i]->GetName()) == 0)
			{
				// Neu ton tai thi cho semaphore up();
				// 19120447 start
				//printf("Tim duoc name trong semTab:\n");
				//printf(semTab[i]->GetName());
				//printf("\n\n");
				// 19120735 end
				semTab[i]->signal();
				return 0;
			}
			if(i == MAX_SEMAPHORE - 1)
				printf("Khong tim duoc name trong semTab\n\n");
		}
	}
	return -1;
}

int STable::FindFreeSlot()
{
	return this->bm->FindAndSet();
}