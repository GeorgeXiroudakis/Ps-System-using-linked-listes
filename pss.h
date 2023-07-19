/***************************************************************
 *
 * file: pss.h
 *
 * @Author  Nikolaos Vasilikopoulos (nvasilik@csd.uoc.gr)
 * @Version 20-10-2020
 *
 * @e-mail       hy240-list@csd.uoc.gr
 *
 * @brief   Header file for the Public Subscribe System,
 * with the structures and function prototypes
 *
 ***************************************************************
 */

#ifndef pss_h
#define pss_h
#define MG 64

struct Group *G;
struct SubInfo *S;

struct Info {
    int iId;
    int itm;
    int igp[MG];
    struct Info *iprev;
    struct Info *inext;
};
struct Subscription {
    int sId;
    struct Subscription *snext;
};
struct Group {
    int gId;
    struct Subscription *ggsub;
    struct Info *gfirst;
    struct Info *glast;
};
struct SubInfo {
    int sId;
    int stm;
    struct Info *sgp[MG];
    struct SubInfo *snext;
};

/**
 * @brief Optional function to initialize data structures that
 *        need initialization
 *
 * @return 0 on success
 *         1 on failure
 */
int initialize(void);


struct Group* InitializeGroup(struct Group* head);

/**
 * @brief Free resources
 *
 * @return 0 on success
 *         1 on failure
 */
int free_all(void);

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
int Insert_Info(int iTM,int iId,int* gids_arr,int size_of_gids_arr);

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
int Subscriber_Registration(int sTM,int sId,int* gids_arr,int size_of_gids_arr);

/**
 * @brief Consume Information for subscriber
 *
 * @param sId Subscriber identifier
 * @return 0 on success
 *          1 on failure
 */
int Consume(int sId);

/**
 * @brief Delete subscriber
 *
 * @param sId Subscriber identifier
 * @return 0 on success
 *          1 on failure
 */
int Delete_Subscriber(int sId);

/**
 * @brief Print Data Structures of the system
 *
 * @return 0 on success
 *          1 on failure
 */
int Print_all(void);

struct SubInfo* SL_Insert(struct SubInfo* head, int sTm, int sId);

struct SubInfo* SL_Delete(struct SubInfo* head, int idKey);

struct SubInfo* SL_LookUp(struct SubInfo* head, int key);

void  SL_Print(struct SubInfo* head);


struct Info* DL_Insert(struct Info* head, int iId, int itm);

struct Info* DL_Delete(struct Info* head, int itm);

struct Info* DL_LookUp(struct Info* head, int idkey);

struct Info* DL_findLast(struct Info* head);

void DL_Print(struct Info* head);

void InsertPirnt(struct Group group);

void Printids(struct Info* list);


struct Subscription* L_Insert(struct Subscription* head, int id);

struct Subscription* L_Delete(struct Subscription* head, int id);

struct Subscription* L_LookUp(struct Subscription* head, int id);

void L_print(struct Subscription* head);


void RegPrint(int* gids_arr, int size_of_gids_arr);

void PrintSubListIDs(struct SubInfo* head);

void PrintGroupSubLists(int* gids_arr, int size_of_gids_arr);


void PrintSubIDsWithGLists(struct SubInfo* head);

int CountOfSubs(struct SubInfo* head);

#endif /* pss_h */

