/***************************************************************
 *
 * file: pss.h
 *
 * @Author  Nikolaos Vasilikopoulos (nvasilik@csd.uoc.gr)
 * @Version 20-10-2020
 *
 * @e-mail       hy240-list@csd.uoc.gr
 *
 * @brief   Implementation of the "pss.h" header file for the Public Subscribe System,
 * function definitions
 *
 *
 ***************************************************************
 */
#include <stdio.h>
#include <stdlib.h>

#include "pss.h"

/**
 * @brief Optional function to initialize data structures that
 *        need initialization
 *
 * @return 0 on success
 *         1 on failure
 */
int initialize(void){
    G = malloc(MG * sizeof(struct Group));
    if(G == NULL)return EXIT_FAILURE;

    int i;
    for(i = 0; i < MG; i++){
        G[i].gId = i;
        G[i].ggsub = NULL;
        G[i].gfirst = NULL;
        G[i].glast = NULL;
    }

    S = NULL; 

    return EXIT_SUCCESS;
}


/**
 * @brief Free resources
 *
 * @return 0 on success
 *         1 on failure
 */
int free_all(void){
    int i;
    struct Info* temp;
    struct Subscription* temp2;
    struct SubInfo* temp3;
    for(i = 0; i < MG; i++){
        while(G[i].glast != NULL){
            temp = G[i].glast->iprev;
            free(G[i].glast);
            G[i].glast = temp;
        }
        while(G[i].ggsub != NULL){
            temp2 = G[i].ggsub->snext;
            free(G[i].ggsub);
            G[i].ggsub = temp2;
        }
    }
    free(G);

    while(S != NULL){
        temp3 = S->snext;
        free(S);
        S = temp3;
    }
    free(S);
    
    return EXIT_SUCCESS;
}

/**
 * @brief Insert info
 *
 * @param iTM Timestamp of arrival
 * @param iId Identifier of information
 * @param gids_arr Pointer to array containing the gids of the Event.
 * @param size_of_gids_arr Size of gids_arr including -1
 * @return 0 on success
 *          1 on failure
 */
int Insert_Info(int iTM,int iId,int* gids_arr,int size_of_gids_arr){
    int i;
    int j;
    for(i = 0; i < size_of_gids_arr - 1; i++){
        if(DL_LookUp(G[gids_arr[i]].gfirst, iId) != NULL)return EXIT_FAILURE;
        G[gids_arr[i]].gfirst = DL_Insert(G[gids_arr[i]].gfirst, iId, iTM);
        G[gids_arr[i]].glast = DL_findLast(G[gids_arr[i]].gfirst);
        
        struct Info* info = DL_LookUp(G[gids_arr[i]].gfirst, iId);
        if(info == NULL)return EXIT_FAILURE;
        for(j = 0; j < size_of_gids_arr - 1; j++){
            info->igp[gids_arr[j]] = 1;           /*pa*/
        }

        //ananeosh
        struct Subscription* subs = G[gids_arr[i]].ggsub;
        while(subs != NULL){
            struct SubInfo* subsInfo = SL_LookUp(S, subs->sId);
            if(subsInfo == NULL)return EXIT_FAILURE;
            subsInfo->sgp[gids_arr[i]] = G[gids_arr[i]].glast;
            subs = subs->snext;
        }

        InsertPirnt(G[gids_arr[i]]);
        printf("\n");
    }
    
    return EXIT_SUCCESS;
}
/**
 * @brief Subsriber Registration
 *
 * @param sTM Timestamp of arrival
 * @param sId Identifier of subscriber
 * @param gids_arr Pointer to array containing the gids of the Event.
 * @param size_of_gids_arr Size of gids_arr including -1
 * @return 0 on success
 *          1 on failure
 */
int Subscriber_Registration(int sTM,int sId,int* gids_arr,int size_of_gids_arr){
    S = SL_Insert(S, sTM, sId);
    int i;
    struct SubInfo* temp = SL_LookUp(S, sId);
    if(temp == NULL)return EXIT_FAILURE;
    for(i = 0; i < size_of_gids_arr -  1; i++){
        if(L_LookUp(G[gids_arr[i]].ggsub, sId) != NULL)return EXIT_FAILURE;
        G[gids_arr[i]].ggsub = L_Insert(G[gids_arr[i]].ggsub, sId);

        temp->sgp[gids_arr[i]] = G[gids_arr[i]].glast;
    }

    RegPrint(gids_arr, size_of_gids_arr);
    printf("\n");

    return EXIT_SUCCESS;
}
/**
 * @brief Consume Information for subscriber
 *
 * @param sId Subscriber identifier
 * @return 0 on success
 *          1 on failure
 */
int Consume(int sId){
    struct SubInfo* sub = SL_LookUp(S, sId);
    if(sub == NULL)return EXIT_FAILURE;
    int i;
    for(i = 0; i < MG; i++){
        if(sub->sgp[i] != (struct Info*) 1 && sub->sgp[i] != NULL){
            printf("GROUPID = %d, INFOLIST = ", i);
            while(1){
            printf(" %d", sub->sgp[i]->iId);
            if(sub->sgp[i]->iprev == NULL)break;
            sub->sgp[i] = sub->sgp[i]->iprev;
            }
            printf(" NEWSGP = %d\n", sub->sgp[i]->iId);
        }
    }
    printf("\n");
    
    return EXIT_SUCCESS;
}
/**
 * @brief Delete subscriber
 *
 * @param sId Subscriber identifier
 * @return 0 on success
 *          1 on failure
 */
int Delete_Subscriber(int sId){
    struct SubInfo* ToDelete = SL_LookUp(S, sId);
    if(ToDelete == NULL)return EXIT_FAILURE;
    int i;
    int k = 0;
    int num[MG];
    for(i = 0; i < MG; i++){
        if(ToDelete->sgp[i] != (struct Info*) 1){
            G[i].ggsub = L_Delete(G[i].ggsub, sId);
            num[k++] = i;
        }
    }
    S = SL_Delete(S, sId);

    RegPrint(num, k + 1);
    printf("\n");

    return EXIT_SUCCESS;
}
/**
 * @brief Print Data Structures of the system
 *
 * @return 0 on success
 *          1 on failure
 */
int Print_all(void){
    int i;
    for(i = 0; i < MG; i++){
        InsertPirnt(G[i]);
        printf("\n");
    }
    printf("SUBSCRIBERLIST =");
    PrintSubListIDs(S);
    printf("\n\n");
    PrintSubIDsWithGLists(S);
    printf("\n");
    printf("NO_GROUPS = %d, NO_SUBSCRIBERS = %d\n", MG, CountOfSubs(S));
    printf("\n");

    return EXIT_SUCCESS;
}


struct SubInfo* SL_Insert(struct SubInfo* head, int sTm, int sId){
    struct SubInfo* temp;
    struct SubInfo* new = (struct SubInfo*) malloc(sizeof(struct SubInfo));
    new->sId = sId;
    new->stm = sTm;
    new->snext = NULL;
    int i;
    for(i = 0; i < MG; i++)new->sgp[i] = (struct Info*) 1;
    
    if(head == NULL || sTm < head->stm){
        new->snext = head;
        head = new;
    }else{
        temp = head;
        while(temp->snext != NULL && temp->snext->stm < sTm)temp = temp->snext;
        new->snext = temp->snext;
        temp->snext = new;
    }
    return head;
}

struct SubInfo* SL_Delete(struct SubInfo* head, int idKey){
    struct SubInfo* cur = head;
    struct SubInfo* prev = head;

    if(head == NULL)return head;

    if(head->sId == idKey){
        head = cur->snext;
        free(cur);
        cur = NULL;
        return head;
    }
    while(cur->sId != idKey){
        if(cur->snext == NULL)return head;
        prev = cur;
        cur = cur->snext;
    }
    prev->snext = cur->snext;
    free(cur);
    cur = NULL;
return head;
}

struct SubInfo* SL_LookUp(struct SubInfo* head, int key){
    struct SubInfo* temp = head;
    while (temp != NULL && temp->sId != key)temp = temp->snext;
    return temp;
}

void  SL_Print(struct SubInfo* head){
    struct SubInfo* temp = head;
    while (temp != NULL){
        printf("Timestamp: %d id: %d\n", temp->stm, temp->sId);
        temp = temp->snext;
    }
}

struct Info* DL_Insert(struct Info* head, int iId, int itm){
    struct Info* new = (struct Info*) malloc(sizeof(struct Info));  
    new->iId = iId;
    new->itm = itm;
    new->inext = NULL;
    new->iprev = NULL;
    int i;
    for(i = 0; i < MG; i++)new->igp[i] = 0;

    struct Info* temp;
    struct Info* temp2;

    if(head == NULL)return new;

    if(head->itm > itm){
        new->inext = head;
        new->inext->iprev = new;
        head = new;
        return head;
    }

    temp = head;
    while(temp->inext != NULL && temp->inext->itm < itm)temp = temp->inext;
    if(temp->inext == NULL){
        temp->inext = new;
        new->iprev = temp;
    }else{
        temp2 = temp->inext;
        temp->inext = new;
        temp2->iprev = new;
        new->iprev = temp;
        new->inext = temp2;
    }
return head;
}

struct Info* DL_Delete(struct Info* head, int itm){
    if (head == NULL)return head;

    
    if(head->itm == itm){
        head = head->inext;
        free(head->iprev);
        head->iprev = NULL;
        return head;
    }

    struct Info* temp = head;
    while(temp != NULL && temp->itm != itm)temp = temp->inext;
    if(temp == NULL)return head;

    if(temp->inext != NULL)temp->inext->iprev = temp->iprev;

    if(temp->iprev != NULL)temp->iprev->inext = temp->inext;

    free(temp);
    return head;
}

struct Info* DL_LookUp(struct Info* head, int idkey){
    struct Info* temp = head;
    while(temp != NULL && temp->iId != idkey)temp = temp->inext;
    return temp;
}

struct Info* DL_findLast(struct Info* head){
    if(head == NULL)return head;
    struct Info* temp  = head;
    while(temp->inext != NULL)temp = temp->inext;
    return temp; 
}

void DL_Print(struct Info* head){
    struct Info* temp = head;
    struct Info* last;
    while(temp != NULL){
        printf("id :%d   tm: %d\n", temp->iId, temp->itm);
        if(temp->inext == NULL)last = temp;
        temp = temp->inext;
    }
    printf("back\n");
     while(last != NULL){
        printf("id :%d   tm: %d\n", last->iId, last->itm);
        last = last->iprev;
    }
}

void InsertPirnt(struct Group group){
    printf("GROUPID = %d, INFOLIST =", group.gId);
    Printids(group.gfirst);
    printf("\n");
}

void Printids(struct Info* list){
    struct Info* temp = list;
    if(temp == NULL)printf(" -EMPTY-");
    while(temp != NULL){
        printf(" %d", temp->iId);
        temp = temp->inext;
    }
}


struct Subscription* L_Insert(struct Subscription* head, int id){
    struct Subscription* temp = head;
    struct Subscription* new = (struct Subscription*) malloc(sizeof (struct Subscription));
    new->sId = id;
    new->snext = NULL;
    if(head == NULL)return new;
    
    new->snext = head;
    head = new;
}

struct Subscription* L_Delete(struct Subscription* head, int id){
    if(head == NULL)return head;

    if(head->sId == id){
        head = head->snext;
        free(head);
        return head;
    }
    struct Subscription* cur = head;
    struct Subscription* prev = head;
    while(cur != NULL && cur->sId != id){prev = cur;cur = cur->snext;}
    if(cur == NULL)return head;
    prev->snext = cur->snext;
    free(cur);

    return head;
}

struct Subscription* L_LookUp(struct Subscription* head, int id){
    struct Subscription* temp = head;
    while(temp != NULL && temp->sId != id)temp = temp->snext;
    return temp;
}

void L_print(struct Subscription* head){
    struct Subscription* temp = head;
    while (temp != NULL){
        printf(" %d", temp->sId);
        temp = temp->snext;
    }   
}

void RegPrint(int* gids_arr, int size_of_gids_arr){
    printf("SUBSCRIBERLIST =");
    PrintSubListIDs(S);
    printf("\n\n");

    PrintGroupSubLists(gids_arr, size_of_gids_arr);
}

void PrintSubListIDs(struct SubInfo* head){
    struct SubInfo* temp = head;
    while(temp != NULL){
        printf(" %d", temp->sId);
        temp = temp->snext;
    }
}

void PrintGroupSubLists(int* gids_arr, int size_of_gids_arr){
    int i;
    for(i = 0; i < size_of_gids_arr - 1; i++){
        printf("\tGROUPID = %d, SUBLIST =", G[gids_arr[i]].gId);
        L_print(G[gids_arr[i]].ggsub);
        printf("\n");
    }
}

void PrintSubIDsWithGLists(struct SubInfo* head){
    struct SubInfo* temp = head;

    while(temp != NULL){
        printf("SUBSCRIBERID = %d, GROUPLIST =", temp->sId);
        int i;
        for(i = 0; i < MG; i++){
            if(temp->sgp[i] != (struct Info*) 1 )printf(" %d", G[i].gId);
        }
        printf("\n");
        temp = temp->snext;
    }
}

int CountOfSubs(struct SubInfo* head){
    struct SubInfo* temp = head;
    int i = 0;
    while(temp != NULL){i++; temp = temp->snext;}

    return i;
}