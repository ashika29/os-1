// A Multithreaded Program that implements the banker's algorithm and prints safe sequence if any:

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
//variable declarations ahead
pthread_mutex_t r_lock;
pthread_cond_t limit;
int num_res,num_process;
int *resources;
int **num_allocated;
int **max;
int **need;
int *s_state;
int pro_ready = 0;
char choice;
//function declarations ahead
bool get_safe_sequence();
void* final_p_execution(void* );

void thankyou(){
	printf("\n*****************************************************************************************\n");
	printf("\n\n\t\tTHANKS FOR USING BANKER'S SOLUTION DESIGNED BY ASHIKA!\n\n");
	printf("\n*****************************************************************************************\n");
	exit(1);
}

int main() {
	A:
	srand(time(NULL));
	printf("\n*****************************************************************************************\n");
	printf("\n\n\t\t\t INDEX:\n\n");
	printf("\n*****************************************************************************************\n");
        printf("\n 1. Enter number of PROCESSES\t: ");
        scanf("%d", &num_process);
        printf("\n 2. Enter number of RESOURCES\t: ");
        scanf("%d", &num_res);

        num_allocated = (int **)malloc(num_process * sizeof(*num_allocated));
        for(int i=0; i<num_process; i++)
                num_allocated[i] = (int *)malloc(num_res * sizeof(**num_allocated));

        max = (int **)malloc(num_process * sizeof(*max));
        for(int i=0; i<num_process; i++)
                max[i] = (int *)malloc(num_res * sizeof(**max));

        // allocated resources
        for(int i=0; i<num_process; i++) {
                printf("\n 3.%d. Enter number of resources allocated to P%d of each Rj:\n ", i+1,i+1);
                for(int j=0; j<num_res; j++){
			printf("\n\t R%d :\t",j+1);
                        scanf("%d", &num_allocated[i][j]);
		}
        }
        printf("\n");

	// maximum required resources
        for(int i=0; i<num_process; i++) {
                printf("\n 4.%d. Enter max number of resources required by P%d of each Rj:\n ", i+1, i+1);
                for(int j=0; j<num_res; j++){
			printf("\n\t R%d :\t",j+1);
                        scanf("%d", &max[i][j]);
		}
        }
        printf("\n");
        resources = (int *)malloc(num_res * sizeof(*resources));
        printf("\n 5. Enter total number of R(available) of each Rj:\n ");
        for(int i=0 ; i<num_res; i++){
		printf("\n\t R%d :\t",i+1);
                scanf("%d", &resources[i]);
	}
        printf("\n");

	// calculate need matrix
	printf("\n Calculating need matrix from the formula:\n\tN E E D\t=\tA L L O C A T E D\t+\tA V A I L A B L E....\n");
        need = (int **)malloc(num_process * sizeof(*need));
        for(int i=0; i<num_process; i++)
                need[i] = (int *)malloc(num_res * sizeof(**need));

        for(int i=0; i<num_process; i++)
                for(int j=0; j<num_res; j++)
                        need[i][j] = max[i][j] - num_allocated[i][j];

	// get safe sequence
	printf("\n Checking for the possibility of a s-state....\n");
	s_state = (int *)malloc(num_process * sizeof(*s_state));
        for(int i=0; i<num_process; i++) s_state[i] = -1;

        if(!get_safe_sequence()) {
                printf("\n\tUNSAFE STATE!!!\n\n These processes lead to a deadlock state! Kindly check. \n\n");
		C:
			printf("\n Would you like to reuse this Banker's program? (y/n)\n");
			scanf("%c",&choice);
			if(choice != 'y' && choice != 'n' && choice != 'Y' && choice != 'N'){
				printf("\nSorry, invalid choice entered. Please enter again.. \n");
				goto C;
			}
			else if(choice == 'n' || choice == 'N')		thankyou();
			else	goto A;
        }

        printf("\n\n\tSAFE STATE! Safe Sequence Found : ");
        for(int i=0; i<num_process; i++) {
                printf("%5d  -->  ", s_state[i]+1);
        }

        printf("\nExecuting Processes...\n\n");
        sleep(1);
	
	// run threads
	pthread_t processes[num_process];
        pthread_attr_t attribute;
        pthread_attr_init(&attribute);

	int processNumber[num_process];
	for(int i=0; i<num_process; i++) processNumber[i] = i;

        for(int i=0; i<num_process; i++)
                pthread_create(&processes[i], &attribute, final_p_execution, (void *)(&processNumber[i]));

        for(int i=0; i<num_process; i++)
                pthread_join(processes[i], NULL);

        printf("\nAll Processes Executed Succesfully\n");
	
	B:
	printf("\n Would you like to reuse this Banker's program? (y/n)\n");
	scanf("%c",&choice);
	if(choice != 'y' && choice != 'n' && choice != 'Y' && choice != 'N'){
		printf("\nSorry, invalid choice entered. Please enter again.. \n");
		goto B;
	}
	else if(choice == 'n' || choice == 'N')		thankyou();
	else	goto A;
	// free resources
        free(resources);
        for(int i=0; i<num_process; i++) {
                free(num_allocated[i]);
                free(max[i]);
		free(need[i]);
        }
        free(num_allocated);
        free(max);
	free(need);
        free(s_state);
	
	thankyou();
	return 0;
}


bool get_safe_sequence() {
	// get safe sequence
        int temp[num_res];
        for(int i=0; i<num_res; i++) temp[i] = resources[i];

        bool execute[num_process];
        for(int i=0; i<num_process; i++) execute[i] = false;
        int executable=0;
        while(executable < num_process) {
                bool safe = false;

                for(int i=0; i<num_process; i++) {
                        if(!execute[i]) {
                                bool mumkin = true;

                                for(int j=0; j<num_res; j++)
                                        if(need[i][j] > temp[j]) {
                                                mumkin = false;
                                                break;
                                        }

                                if(mumkin) {
                                        for(int j=0; j<num_res; j++)
                                                temp[j] += num_allocated[i][j];
                                        s_state[executable] = i;
                                        execute[i] = true;
                                        ++executable;
                                        safe = true;
                                }
                        }
                }

                if(!safe) {
                        for(int k=0; k<num_process; k++) s_state[k] = -1;
                        return false; // no safe sequences were found
                }
        }
        return true; //a safe sequence was encountered
}

// process code
void* final_p_execution(void *kuchbhi) {
        int momo = *((int *) kuchbhi);

	// lock resources
        pthread_mutex_lock(&r_lock);

        // bound check
        while(momo != s_state[pro_ready])
                pthread_cond_wait(&limit, &r_lock);

	// process
        printf("\n--> Process %d", momo+1);
        printf("\n\tAllocated : ");
        for(int i=0; i<num_res; i++)
                printf("%3d", num_allocated[momo][i]);

        printf("\n\tNeeded    : ");
        for(int i=0; i<num_res; i++)
                printf("%3d", need[momo][i]);

        printf("\n\tAvailable : ");
        for(int i=0; i<num_res; i++)
                printf("%3d", resources[i]);

        printf("\n"); sleep(1);

        printf("\tResources allocated!");
        printf("\n"); sleep(1);
        printf("\tProcess is executing...");
        printf("\n"); sleep(rand()%3 + 2); // process code
        printf("\tProcess' execution completed...");
        printf("\n"); sleep(1);
        printf("\tProcess is now releasing resources...");
        printf("\n"); sleep(1);
        printf("\tResources Released!");

	for(int i=0; i<num_res; i++)
                resources[i] += num_allocated[momo][i];

        printf("\n\tFreshly available resources are: ");
        for(int i=0; i<num_res; i++)
                printf("%3d", resources[i]);
        printf("\n\n");

        sleep(1);

	// bound broadcast
        pro_ready++;
        pthread_cond_broadcast(&limit);
        pthread_mutex_unlock(&r_lock);
	pthread_exit(NULL);

}

