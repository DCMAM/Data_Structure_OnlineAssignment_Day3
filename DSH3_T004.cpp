#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/*
	belum selesai:
	1. remove all di exit
*/

unsigned long hash(char *str);

struct song{
	int value;
	char name[50];
	song *next;
}*head[100];

struct song *createNewNode(int value, char *str){
	song *newNode = (song*)malloc(sizeof(song));
	strcpy(newNode->name, str);
	newNode->value = value;
	newNode->next = NULL;
	return newNode;
}

char *set_path(char title[], long hash){
	char *path = (char *)malloc(sizeof(100));
	char snum[4];
	itoa(hash, snum, 10);
	
	int i=0;
	for(i=0; i<strlen(snum); ){
		path[i] = snum[i];
		i++;
	}
	path[i] = '/';
	int j=0;
	int k=0;
	for(j=i+1; j<strlen(title)+i+1; ){
		path[j] = title[k];
		k++;
		j++;
	}
	path[j] = '.';
	path[j+1] = 't';
	path[j+2] = 'x';
	path[j+3] = 't';
	path[j+4] = '\0';
	
	return path;
}

void create_and_input_file(char *path, int idx){
	char line[100][200];
	int x=0;
	do{
		do{
			printf("Input song lyrics[0 to exit |  1 to finnish]: ");
			scanf("%[^\n]", line[x]); getchar();
			if(strlen(line[x]) > 0) break;
			printf("An error occured while inserting song\n");
			system("pause");
		}while(1);
		if(strcmp(line[x], "0") == 0) return;
		else if(strcmp(line[x], "1") == 0) break;
		x++;
	}while(x<100);
	
	char snum[4];
	itoa(idx, snum, 10);
	int check = mkdir(snum);
	FILE *fp = fopen(path, "w");
	for(int i=0; i<x; i++){
		fprintf(fp, "%s\n", line[i]);
	}
	
	fclose(fp);
}

int push(int value, char *str){
	int idx = hash(str);
	song *newNode = createNewNode(value, str);
	if(head[idx] == NULL) head[idx] = newNode;
	else{
		song *temp = head[idx];
		while(temp->next != NULL ) temp = temp->next;
		temp->next = newNode;
	}
	create_and_input_file(set_path(str, idx), idx);
}

unsigned long hash(char *str){
	unsigned long hash = 5381;
    int h;
	int i=0;
    while ( h = str[i++] ) hash = ((hash << 5) + hash) + h; 	
    return hash % 100;
}

void error_1(){
	printf("An error occured while searching song\n");
	printf("ERR: SONG NOT FOUND\n");
}

bool pop(int value, char *str){
	int idx = hash(str);
	if(head[idx] == NULL){
		error_1();	
		return false;
	} 
	else{
		song *temp = head[idx];
		if(strcmp(head[idx]->name, str)==0){
			head[idx] = head[idx]->next;
			free(temp);
			temp = NULL;
		}
		else{
			while(temp->next != NULL && strcmp(temp->next->name, str) != 0) 
				temp = temp->next;
			if(temp->next == NULL){
				error_1();
				return false;
			} 
			song *toDelete = temp->next;
			temp->next = toDelete->next;
			free(toDelete);
			toDelete = NULL;
		}
		remove(set_path(str, idx));
		char snum[4];
		itoa(idx, snum, 10);
		rmdir(snum);
		printf("removed\n");
		return true;
	}
}

void displayTable(){
	printf("+====================================================+\n");
	printf("| %-22s%-28s |\n", " ", "My Album");
	printf("+====================================================+\n");
	int flag =0;
	for(int i=0; i<100; i++){
		if(head[i] != NULL){
			song *temp = head[i];
			while(temp != NULL){
				flag=1;
				printf("| %-50s |\n", temp->name);
				temp = temp->next;
			}
		}
	}
	if(flag == 0) printf("| %-50s |\n", "No Data");
	printf("+====================================================+\n");
}

void view_lyric(char *str){
	FILE *fp = fopen(set_path(str, hash(str)), "r");
	
	char line[200];
	
	if(!fp) return;
	
	int x=0;
    while (1) {
		if(feof(fp)) break;
		fscanf(fp, "%[^\n]", line); 
		fgetc(fp);
		printf("%s\n", line);
		if(x%4 == 3){
			printf("\n");
			Sleep(500);
		}
		x++;
		Sleep(100);
	}
	fclose(fp);
}

void cls(){
	for(int i=0; i<45; i++){
		printf("\n");
	}
}

int validate_song(char name[]){
	int idx = hash(name);
	if(head[idx] == NULL){
		return 1;
	} 
	else{
		song *temp = head[idx];
		while(temp->next != NULL && strcmp(temp->next->name, name) != 0) 
			temp = temp->next;
		if(temp->next == NULL) return 0; 
	}
	return 1;
}

void insert_song(){
	char title[32];
	do{
		printf("Song title: ");
		scanf("%[^\n]", title); getchar();
		if(strlen(title) > 0 && validate_song(title) == 1) break;
	}while(1);
	push(hash(title), title);
}

void remove_song(){
	char song[50];
	printf("Input song title to delete [Case Sensitive]: ");
	scanf("%[^\n]", song); getchar();
	pop(hash(song), song);
}

void view_song(){
	displayTable();
	char song[50];
	printf("What song would you like to listen? [Case Sensitive]\n");
	printf(">> ");
	scanf("%[^\n]", song); getchar();
	view_lyric(song);
}

void print_menu(){
	printf("SPOJIFY\n");
	printf("=======\n");
	printf("1. Insert Song\n");
	printf("2. Remove Song\n");
	printf("3. View Song\n");
	printf("4. Exit\n");
	printf(">> ");
}

void exit(){
	for(int i=0; i<100; i++){
		if(head[i] != NULL){
			song *temp = head[i];
			while(temp != NULL){
				pop(hash(temp->name), temp->name);
				temp = temp->next;
			}
		}
	}
	for(int i=0; i<100; i++)
		free(head[i]);
}

int main(){
	int menu=0;
	do{
		cls();
//		printf("%s\n\n", set_path("david", hash("david")));
		print_menu();
		scanf("%d", &menu); getchar();
		if(menu==1) insert_song();
		else if(menu==2) remove_song();
		else if(menu==3) view_song();
		else if(menu==4) {
			exit(); 
			break;
		}
		system("pause");
	}while(1);
	return 0;
}
