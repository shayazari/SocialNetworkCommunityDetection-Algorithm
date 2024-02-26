/* A simplified community detection algorithm:
 *
 * Skeleton code written by Jianzhong Qi, May 2023
 * Edited by: Shayan Azari Pour 1317598
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define STAGE_NUM_ONE 1						  /* stage numbers */
#define STAGE_NUM_TWO 2
#define STAGE_NUM_THREE 3
#define STAGE_NUM_FOUR 4
#define STAGE_HEADER "Stage %d\n==========\n" /* stage header format string */

#define MAX_USERS 50
#define MAX_HASHTAGS 10
#define MAX_HASH_LENGTH 20

/* input into s_o_c_calc func in stage 2 */
#define USER_0 0
#define USER_1 1

#define OUT_OF_BOUNDS 2		/* out of bounds of friendship matrix */

/* struct to hold user information */
typedef struct {
    int year_joined;
    char hashtags[MAX_HASHTAGS+1][MAX_HASH_LENGTH+1];
    int num_hashtags;
} user_t;

typedef char data_t[MAX_HASH_LENGTH+1];							  /* to be modified for Stage 4 */

/* linked list type definitions below, from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c 
*/
typedef struct node node_t;

struct node {
	data_t data;
	node_t *next;
};

typedef struct {
	node_t *head;
	node_t *foot;
} list_t;

/* struct to hold community information */
typedef struct {
	int core_user;
	int close_friends[MAX_USERS+1];
} community_t;

/****************************************************************/

/* function prototypes */
list_t *make_empty_list(void);
void free_list(list_t *list);
void print_list(list_t *list);
list_t *insert_unique_in_order(list_t *list, data_t value);

void print_stage_header(int stage_num);

void stage_one(user_t users[MAX_USERS+1], int matrix[][MAX_USERS+1], double *ths, int *thc);
void stage_two(int matrix[][MAX_USERS+1]);
void stage_three(int matrix[][MAX_USERS+1], double s_o_c[][MAX_USERS+1]);
void stage_four(double s_o_c[][MAX_USERS+1], community_t community[MAX_USERS+1], 
	double ths, int thc, user_t users[MAX_USERS+1]);

/* add your own function prototypes here */
double s_o_c_calc(int matrix[][MAX_USERS+1], int index_1, int index_2);
int core_user_check(double s_o_c[][MAX_USERS+1], double ths, int thc, int i);
void close_friends_output(double s_o_c[][MAX_USERS+1], double ths, int i, 
	community_t community[MAX_USERS+1]);
void read_users(user_t users[MAX_USERS+1], char c, int i);
int read_matrix(user_t users[MAX_USERS+1], int matrix[MAX_USERS+1][MAX_USERS+1], char c, 
	int i, int num_users);
void read_ths_thc(double *ths, int *thc, char c);
void stage_one_output(user_t users[MAX_USERS+1], int num_users, int max_hashtag_count);

/****************************************************************/

/* main function controls all the action; modify if needed */
int
main(int argc, char *argv[]) {

	user_t users[MAX_USERS+1];					/* user info struct */
	int matrix[MAX_USERS+1][MAX_USERS+1];		/* friendship matrix 2D array */
	double s_o_c[MAX_USERS+1][MAX_USERS+1];		/* strength of connection 2D array */
	community_t community[MAX_USERS+1];			/* community info struct */
	double ths;
	int thc;

	/* stage 1: read user profiles */
	stage_one(users, matrix, &ths, &thc);

	/* stage 2: compute the strength of connection between u0 and u1 */
	stage_two(matrix);
	
	/* stage 3: compute the strength of connection for all user pairs */
	stage_three(matrix, s_o_c);
	
	/* stage 4: detect communities and topics of interest */
	stage_four(s_o_c, community, ths, thc, users);
	
	/* all done; take some rest */
	return 0;
}

/****************************************************************/

/* add your code below; you can also modify the function return type 
   and parameter list 
*/

/* algorithms are fun */

/* stage 1: read user profiles */
void 
stage_one(user_t users[MAX_USERS+1], int matrix[][MAX_USERS+1], double *ths, int *thc){

	/* print stage header */
	print_stage_header(STAGE_NUM_ONE);

	char c;
	int i = 0, num_users = 0, max_hashtag_count = 0;

	/* read in and store user info */
	while ((c = getchar()) == 'u'){
		read_users(users, c, i);
		i++;
		num_users++;
	}

	users[i].hashtags[0][0] = '\0'; /* null byte indicator */ 

	/* read in and store friendship matrix info */
	i = 0;
	while (i<num_users){
		i = read_matrix(users, matrix, c, i, num_users);
	}
	matrix[num_users][0] = OUT_OF_BOUNDS;
	
	/* read in and store ths and thc */
	read_ths_thc(ths, thc, c);

	stage_one_output(users, num_users, max_hashtag_count);

	printf("\n");
}

/* stage 2: compute the strength of connection between u0 and u1 */
void 
stage_two(int matrix[][MAX_USERS+1]) {

	/* print stage header */
	print_stage_header(STAGE_NUM_TWO);

	printf("Strength of connection between u0 and u1: %4.2f", s_o_c_calc(matrix, USER_0, USER_1));

	printf("\n\n");
}

/* stage 3: compute the strength of connection for all user pairs */
void 
stage_three(int matrix[][MAX_USERS+1], double s_o_c[][MAX_USERS+1]) {

	/* print stage header */
	print_stage_header(STAGE_NUM_THREE);
	
	int i = 0, j = 0;


	while(matrix[i][0] != OUT_OF_BOUNDS){		/* for every user */
		while(matrix[i][j] != OUT_OF_BOUNDS){	/* for every friend of the user */

			/* calculates strength of connection between the two users */
			s_o_c[i][j] = s_o_c_calc(matrix, i, j);		

			/* no whitespace for final value in line */
			if(matrix[i][j+1] == OUT_OF_BOUNDS){
				s_o_c[i][j+1] = OUT_OF_BOUNDS;
				printf("%4.2f", s_o_c[i][j]);
			}
			else{
				printf("%4.2f ", s_o_c[i][j]);			
			}
			j++;
		}

		printf("\n");
		j = 0;
		i++;
	}

	s_o_c[i][0] = OUT_OF_BOUNDS;

	printf("\n");
}

/* stage 4: detect communities and topics of interest */
void stage_four(double s_o_c[][MAX_USERS+1], community_t community[MAX_USERS+1], 
	double ths, int thc, user_t users[MAX_USERS+1]) {
	
	/* print stage header */
	print_stage_header(STAGE_NUM_FOUR);
	int i = 0, k = 0, j = 0, close_friend_i;

	/* will store all unique hashtags in alphabetical order for each community */
	list_t *hashlist = make_empty_list();
	
	while(s_o_c[i][0] != OUT_OF_BOUNDS){
		/* if it is a core user */
		if(core_user_check(s_o_c, ths, thc, i) == 1){
			/* core user's hashtags */
			while(j < (users[i].num_hashtags)){
				insert_unique_in_order(hashlist, users[i].hashtags[j]);
				j++;
			}
			j = 0;

			/* Stage 4.1 output */
			printf("Stage 4.1. Core user: u%d; ", i);
			close_friends_output(s_o_c, ths, i, community);

			/* Stage 4.2 output */
			printf("Stage 4.2. Hashtags:\n");
			/* for every close friend of core user */
			while(community[i].close_friends[k] != MAX_USERS){
				close_friend_i = community[i].close_friends[k];
				/* close friend's hashtags */
				while(j < (users[close_friend_i].num_hashtags)){
					insert_unique_in_order(hashlist, users[close_friend_i].hashtags[j]);
					j++;
				}
				j = 0;
				k++;
			}
			k = 0;
	
			print_list(hashlist);
			free_list(hashlist);
			hashlist = make_empty_list();
			printf("\n");
		}
		i++;
	}
}

/****************************************************************/

/* reads all lines containing user information and 
stores them in the users struct */
void read_users(user_t users[MAX_USERS+1], char c, int i){

	int j = 0, k = 0;
	users[i].year_joined = 0;
	users[i].num_hashtags = 0;

	/* skip past 'u0' */
	while(c != ' '){
		c = getchar();
	}

	/* reading user's year joined */
	while((c = getchar()) != '#' && c != ' '){
		users[i].year_joined = users[i].year_joined * 10 + (c - '0');					
	}

	/* reading user's hashtags */
	while ((c = getchar()) == '#'){	/* for each hashtag */
		while((c = getchar()) != ' ' && c != '\n'){	/* for each letter in hashtag */
			users[i].hashtags[k][j] = c;
			j++;
		}
		users[i].hashtags[k][j] = '\0'; /* null byte at the end of each hashtag */
		users[i].num_hashtags++;	/* number of hashtags of each user */
		j = 0;
		k++;

		/* finished reading user's line */
		if(c == '\n'){
			break;
		}
	}
}

/****************************************************************/

/* reads the friendship matrix and stores it in a 2D array, matrix */
int read_matrix(user_t users[MAX_USERS+1], int matrix[MAX_USERS+1][MAX_USERS+1], char c, 
	int i, int num_users){

	int j = 0;

	while (i<num_users){
		if(c == '0' || c == '1'){
			matrix[i][j] = (c - '0');	/* convert char to int */
			j++;	
		}
		else if(c == '\n'){
			matrix[i][j] = OUT_OF_BOUNDS;
			i++;
			j = 0;
		}
		c = getchar();
	}

	return i;

}

/****************************************************************/

/* reads the final line of the input which are ths & thc values;
	converts ths into decimal */
void read_ths_thc(double *ths, int *thc, char c){

	/* for ths value; after '0.'*/
	double decimal = 0;

	getchar(); /* discard the '.' after 0 */
	c = getchar();

	/* read the fractional part of ths */
	while(c != ' '){
		decimal = decimal * 10 + (c - '0');
		c = getchar();
	}

	/* input whole number and fractional part into ths */
	while(decimal >= 1){
		decimal = decimal/10;
	}
	*ths = decimal;
	c = getchar();

	/* thc is final non-EOF value */
	while(c != EOF){
		*thc = *thc * 10 + (c - '0');
		c = getchar();
	}
}

/****************************************************************/

/* prints the relevant information for stage 1 after being stored */
void stage_one_output(user_t users[MAX_USERS+1], int num_users, int max_hashtag_count){

	int i = 0, most_hashtags_user;

	printf("Number of users: %d\n", num_users);
	
	/* finding user with most hashtags and number of */
	while(users[i].hashtags[0][0] != '\0'){
		if(users[i].num_hashtags > max_hashtag_count){
			max_hashtag_count = users[i].num_hashtags;
			most_hashtags_user = i;
		}
		i++;
	}
	
	printf("u%d has the largest number of hashtags:\n", most_hashtags_user);
	
	/* first hashtag without whitespace before */
	printf("#%s", users[most_hashtags_user].hashtags[0]);

	/* rest of the hashtags with whitespace seperation */
	for (int l = 1; l < users[most_hashtags_user].num_hashtags; l++){
		printf(" #%s", users[most_hashtags_user].hashtags[l]);
	}

	printf("\n");

}

/****************************************************************/

/* given two users, calculates their strength of connection */
double s_o_c_calc(int matrix[][MAX_USERS+1], int index_1, int index_2){

	int j = 0, numerator = 0, denominator = 0;
	double strength_of_connection = 0;
	
	/* if the two users are friends */
	if(matrix[index_1][index_2] == 1){
		while(matrix[index_1][j] != OUT_OF_BOUNDS){
			if(matrix[index_1][j] == 1 || matrix[index_2][j] == 1){ /* part of union */
				if(matrix[index_1][j] == matrix[index_2][j]){	/* part of intersection */
					numerator++;
				}
				denominator++;
			}
			j++;
		}
		strength_of_connection = (double)numerator/denominator;
	}

	return strength_of_connection;

}

/****************************************************************/

/* given a user, finds if they are a core user based on ths and thc */
int core_user_check(double s_o_c[][MAX_USERS+1], double ths, int thc, int i){
	int j = 0, close_friend_count = 0;
	
	while(s_o_c[i][j] != OUT_OF_BOUNDS){
		if(s_o_c[i][j] > ths){
			close_friend_count++;
		}
		/* a core user */
		if(close_friend_count > thc){
			return 1;
		}
		j++;
	}

	/* not a core user */
	return 0;
}

/****************************************************************/

/* given a corse user, finds all of their close friends based on ths */
void close_friends_output(double s_o_c[][MAX_USERS+1], double ths, int i, 
	community_t community[MAX_USERS+1]){
		
	int j = 0, k = 0;

	/* add core user into community struct */
	community[i].core_user = i;

	printf("close friends:");

	while(s_o_c[i][j] != OUT_OF_BOUNDS){
		/* is a close friend; add to struct */
		if(s_o_c[i][j] > ths){
			community[i].close_friends[k] = j;
			printf(" u%d", j);
			k++;
		}	
		j++;
	}

	/* end of array indicator */
	community[i].close_friends[k] = MAX_USERS;

	printf("\n");
}

/****************************************************************/

/* print stage header given stage number */
void 
print_stage_header(int stage_num) {
	printf(STAGE_HEADER, stage_num);
}

/****************************************************************/
/* linked list implementation below, adapted from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c 
*/

/* create an empty list */
list_t
*make_empty_list(void) {
	list_t *list;

	list = (list_t*)malloc(sizeof(*list));
	assert(list!=NULL);
	list->head = list->foot = NULL;

	return list;
}

/****************************************************************/

/* free the memory allocated for a list (and its nodes) */
void
free_list(list_t *list) {
	node_t *curr, *prev;

	assert(list!=NULL);
	curr = list->head;
	while (curr) {
		prev = curr;
		curr = curr->next;
		free(prev);
	}

	free(list);
}

/****************************************************************/

/* insert a new data element into a linked list, keeping the
	data elements in the list unique and in alphabetical order
*/
list_t
*insert_unique_in_order(list_t *list, data_t value){

	node_t *new, *current, *prev;

	new = (node_t*)malloc(sizeof(*new));
	assert(list!=NULL && new!=NULL);
	strcpy(new->data, value);
	new->next = NULL;
	
	if (list->foot==NULL) {
		/* this is the first insertion into the list */
		list->head = list->foot = new;
	} else {
		prev = NULL;
		current = list->head;

		/* go through list until hashtag is lower 
		in alphabetical order than 'current'*/
		while (current != NULL && strcmp(current->data, value) < 0){
			prev = current;
			current = current->next;
		}

		/* hashtag is duplicate if both hashtags are same ASCII and length */
		if (current != NULL && strcmp(current->data, value) == 0){
			if((strlen(current->data) == strlen(value))){
				return list;
			}
		}

		/* hashtag is lowest in alphabetical order; change head */
		if (prev == NULL){
			new->next = current;
			list->head = new;
		}

		/* hashtag is after the head; insert before 'current' */
		else{
			prev->next = new;
			new->next = current;
		}

		/* hashtag is highest in alphabetical order; change foot */
		if (current == NULL){
			list->foot = new;
		}
	}
	return list;	
}

/****************************************************************/

/* print the data contents of a list */
void
print_list(list_t *list) {
	/* add code to print list */
	int hashtag_count = 0;
	node_t *current = list->head;
	
	/* printing every unqiue hashtag of community in 
	alhpabetical order; new line after every 5 hashtags */
	while(current->next != NULL){
		printf("#%s", current->data);
		hashtag_count++;
		if(hashtag_count==5){
			printf("\n");
			hashtag_count = 0;
		}
		else{
			printf(" ");
		}
		current = current->next;
	}

	/* current->next of foot is NULL; 
	not executed in above loop */
	printf("#%s", list->foot->data);
}

/****************************************************************/
/*
	Write your time complexity analysis below for Stage 4.2, 
	assuming U users, C core users, H hashtags per user, 
	and a maximum length of T characters per hashtag:

	The outer loop iterates through each core user in s_o_c[i][0], which has time complexity O(C).
	Within this loop, the program iterates over community[i].close_friends, which in worst case 
	has time complexity O(U). Within the outer loop, the inner loop iterates over 
	users[close_friend_i].num_hashtags, which has time complexity O(H). Inside this loop, the 
	insert_unique_in_order function is called, which has a time complexity of O(H*T).
	Once this is done, we simply print the linked list going node from node, which will take O(H) 
	complexity. Putting everything together, we get a time complexity of O(C * U * (H+H) * (H*T)).

	Overall, the time complexity of stage 4.2 will be O(C * U * H^2 * T).
*/