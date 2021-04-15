/*
Name: Joseph Salazar
Email: salazjos@oregonstate.edu
Class: CS 344, Program 2
File: salazjos.buildrooms.c
Description: Build a graph of seven struct Room types
connected together. Print the data of each room into its own
seperate file.
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define SAMPLE_ROOM_AMT 10
#define MAX_ROOM_CONNECTIONS 6
#define MIN_ROOM_CONNECTIONS 3
#define MAX_ROOM_AMOUNT 7
#define TRUE 1
#define FALSE 0

/*Enum for room type
  START_ROOM = 1, MID_ROOM = 2, END_ROOM = 3
 */
enum RoomType{START_ROOM = 1, MID_ROOM, END_ROOM};

struct Room{
    enum RoomType roomType;
    int  numConnections;
    int  targetNumConnections;
    char roomName[15];
    struct Room *roomConnection[MAX_ROOM_CONNECTIONS];
};

/*
Function name: generateRandomNumber
Function type: int
Parameters: int for low range (low),
int for high range (high).
Description: return random number between, and
possibly including, low and high.
*/
int generateRandomNumber(int low, int high)
{
    return  (rand() % (high - low + 1)) + low;
}

/*
Function name: getFreeConnectionIndex
Function type: int
Parameters: pointer to struct room (roomA)
Description: Loop through roomA's
roomConnection array looking for first available
index. return -1 if array is already full,
return 0 (first index) if there is nothing in the
array, return index location >=1 of first available
location.
*/
int getFreeConnectionIndex(struct Room *roomA)
{
    int i, freeIndex = 0;
    int targetConnect = roomA->targetNumConnections;

    if(roomA->numConnections == roomA->targetNumConnections)
        return -1; //no free space
    else if(roomA->numConnections == 0) //no connections
        return freeIndex; //return 0
    else{
        //find the first available location
        for(i = 0; i < targetConnect; i++)
        {
            if(roomA->roomConnection[i] == NULL)
            {
                freeIndex = i;
                break;
            }
        }
    }
    return freeIndex;
}

/*
Function name: fillSampleStringArray
Function type: void
Parameters: char array of pointers (array)
Description: Hard code/assign  array with ten room names
 */
void fillSampleStringArray(char *array[])
{
    array[0] = "BirdRoom";
    array[1] = "SnakeRoom";
    array[2] = "LizardRoom";
    array[3] = "BearRoom";
    array[4] = "CatRoom";
    array[5] = "RodentRoom";
    array[6] = "ZebraRoom";
    array[7] = "ApeRoom";
    array[8] = "WolfRoom";
    array[9] = "InsectRoom";
}

/*
Function name: swapNumbers
Function type: void
Parameters: pointer to int (a),
pointer to int (b).
Description: Swap the two numbers.
Citation:
1) "Suffle a given Array Using Fisher-Yates Shuffle Algorithm."
    GeeksforGeeks, 23 July 2019
    https://www.geeksforgeeks.org/shuffle-a-given-array-using-fisher-yates-shuffle-algorithm/.

*/
void swapNumbers(int *a, int * b)
{
    int temp_number = *a; //deferrence a
    *a = *b;
    *b = temp_number;
}

/*
Function name: fisher-yatesShuffle
Function type: void
Parameters: pointer to int array (array), int for
size of the array (size).
Description: Use Fisher-Yates shuffle agorithm to
shuffle an array of integers.
Citation:
1) "Suffle a given Array Using Fisher-Yates Shuffle Algorithm."
   GeeksforGeeks, 23 July 2019,
   https://www.geeksforgeeks.org/shuffle-a-given-array-using-fisher-yates-shuffle-algorithm/.
2) "Fisher-Yates Shuffle."
   Wikipedia, Wikimedia Foundation, 14 Sept 2019
   https://en.wikipedia.org/wiki/Fisher-Yates_shuffle.

*/
void fisher_yatesShuffle(int *array, int size)
{
    int i = 0, j = 0;
    for(i = size - 1; i > 0; i--)
    {
        j = rand() % (i + 1);
        swapNumbers(&array[i], &array[j]);
    }
}

/*
Function name: assignRandomRoomType
Function type: void
Parameters: array of pointers to room structs (rooms_arr)
Description: Randomly assign each room struct with a
room type. Only type 1 (START_ROOM) and 3 (END_ROOM)
are each assigned once.
*/
void assignRandomRoomtype(struct Room *rooms_arr[])
{
    int i;
    int tempArray[MAX_ROOM_AMOUNT] = {1,2,2,3,2,2,2};
    //shuffle the array randomly
    fisher_yatesShuffle(tempArray, MAX_ROOM_AMOUNT);
    //fill the room types
    for(i = 0; i < MAX_ROOM_AMOUNT; i++)
        rooms_arr[i]->roomType = tempArray[i];
}

/*
Function name: assignRandomRoomName
Function type: void
Parameters: array to points of struct Room (rooms_arr),
array of pointers to char arrays (strArray).
Description: Randomly assign a room name, from strArray, to each
Room struct in rooms_arr.
*/
void assignRandomRoomName(struct Room *rooms_arr[], char *strArray[])
{
    int i, tempIndex = 0;
    int tempArr[10] = {0,1,2,3,4,5,6,7,8,9};
    // randomly shuffle the tempArr array
    fisher_yatesShuffle(tempArr, 10);
    //fill the room names
    for(i = 0; i < MAX_ROOM_AMOUNT; i++)
    {
        tempIndex = tempArr[i];
        memset(rooms_arr[i]->roomName, '\0', sizeof(rooms_arr[i]->roomName));
        strcpy(rooms_arr[i]->roomName, strArray[tempIndex]);
    }
}

/*
Function name: assignRandomTargetConnectionAmount
Function type: void
Parameters: array of pointers to struct Room (rooms_arr).
Description: Assign each room struct in rooms_arr
 a random room connection amount
between 3 and 6.
*/
void assignRandomTargetConnectionAmount(struct Room *rooms_arr[])
{
    int i, rand = 0;
    for(i = 0; i < MAX_ROOM_AMOUNT;i++)
    {
        rand = generateRandomNumber(MIN_ROOM_CONNECTIONS, MAX_ROOM_CONNECTIONS);
        rooms_arr[i]->targetNumConnections = rand;
    }
}

/*
Function name: assignConnectionsNull
Function type: void
Parameters: array of pointers of struct Roomi (rooms_arr)
Description: Assign Null to every pointer in
each Room's roomConnection array.
*/
void assignConnectionsNull(struct Room *rooms_arr[])
{
    int i, j;
    for(i = 0; i < MAX_ROOM_AMOUNT; i++)
    {
        for(j = 0; j < MAX_ROOM_CONNECTIONS; j++)
            rooms_arr[i]->roomConnection[j] = NULL;
    }
}

/*
Function name: createRooms
Function type: void
Parameters: array of pointers of struct Room (rooms)
Description: Malloc each room in the rooms array.
*/
void createRooms(struct Room *rooms[])
{
    int i;
    for(i = 0; i < MAX_ROOM_AMOUNT; i++)
    {
        rooms[i] = malloc(sizeof(struct Room));
        //numConnection and targetNumConnections starting value of 0
        rooms[i]->numConnections = 0;
        rooms[i]->targetNumConnections = 0;
    }
}

/*
Function name: initializeStructRooms
Function type: void
Parameters: array of pointers of struct Room (rooms),
 array of pointers to char arrays (strArray)
*/
void initializeStructRooms(struct Room *rooms[], char *strArray[])
{
    //malloc the rooms
    createRooms(rooms);
    //give rooms a random type
    assignRandomRoomtype(rooms);
    //give rooms a random name
    assignRandomRoomName(rooms, strArray);
    //give rooms a random target connection amount
    assignRandomTargetConnectionAmount(rooms);
    //assign connections to null
    assignConnectionsNull(rooms);
}

/*
Function name: createDirectoryName
Function type: char pointer
Parameters: int representing process id (pID)
Description: Build and return string that holds the name
of the directory that the room files will be stored in.
format is "salazjos.room.pID"
*/
char *createDiretoryName(int pID)
{
    char myname[] = "salazjos";
    char room[]   = "room";
    char dot[]    = ".";
    char pIDtoStr[10];
    char directoryName[25];
    char *dirName = malloc( 25 * sizeof(char));

    memset(pIDtoStr, '\0', sizeof(pIDtoStr));
    memset(directoryName, '\0', sizeof(directoryName));
    strcpy(directoryName, myname);
    strcat(directoryName, dot);
    strcat(directoryName, room);
    strcat(directoryName, dot);
    sprintf(pIDtoStr,"%d",pID);
    strcat(directoryName, pIDtoStr);
    strcpy(dirName, directoryName);
    return dirName;
}

/*
Function name: isGraphFull
Function type: int
Parameters: array of pointers of struct Room (rooms)
Description: Determine if every room has between 3 and 6
connections to other rooms. return False (0) if not and
TRUE (1) if yes.
*/
int isGraphFull(struct Room *rooms[])
{
    int i;
    int fullRoomAmt    = 0;
    int tempConnectAmt = 0;
    struct Room *tempR = NULL;

    for(i = 0; i < MAX_ROOM_AMOUNT; i++)
    {
        tempR = rooms[i];
        tempConnectAmt = tempR->numConnections;
        //test if connect amount is within 3 and 6 connections
        if(tempConnectAmt >= MIN_ROOM_CONNECTIONS && tempConnectAmt <= MAX_ROOM_CONNECTIONS)
            fullRoomAmt++;
    }
    //test if the amount of full rooms is less than the maximum amount of rooms
    return (fullRoomAmt < MAX_ROOM_AMOUNT) ? FALSE:TRUE;
}

/*
Function name: getRandomRoom
Function type: struct Room pointer
Parameters: array of pointers of struct Room (rooms)
Description: Pick a random room struct from rooms
and return a pointer to it.
*/
struct Room *getRandomRoom(struct Room *rooms[])
{
    int randomNumber = generateRandomNumber(0, 6);
    struct Room *randRoom = rooms[randomNumber];
    return randRoom;
}

/*
Function name: isSameRoom
Function type: int
Parameters: pointer to struct Room (roomA),
pointer to struct Room (roomB).
description: Determine if roomA and roomB are the same
Room struct. return TRUE (1) if yes, FALSE (0) if they are not
the same room.
*/
int isSameRoom(struct Room *roomA, struct Room *roomB)
{
    return (roomA == roomB) ? TRUE:FALSE;
}

/*
Function name: isConnectionPresent
Function type: int
Parameters: pointer to struct Room (roomA),
 pointer to struct Room (roomB),
Description: Determine if there is a present connection
between roomA and roomB. return TRUE (1) if there is a
connection, FALSE (0) if there is not.
*/
int isConnectionPresent(struct Room *roomA, struct Room *roomB)
{
    int i;
    int targetAmt_RoomA  = roomA->targetNumConnections;
    int targetAmt_RoomB  = roomB->targetNumConnections;
    struct Room *tempRoomA = NULL;
    struct Room *tempRoomB = NULL;

    //test if roomA or roomB have any present connections.
    if(roomA->numConnections == 0 || roomB->numConnections == 0)
        return FALSE; //no connections to each other since roomA or roomB has no connections
    else
    {
        for(i = 0; i < roomB->numConnections; i++)
        {
            tempRoomB = roomB->roomConnection[i];
            if(tempRoomB == roomA)
                return TRUE; //a connection between roomA and roomB was found
        }
    }
    return FALSE; //conection not found, 0
}

/*
Function name: isCanAddConnectionFrom
function type: int
Parameters: pointer to struct room (roomA)
Description: call getFreeConnectionIndex function
to determine if roomA has a free index location in
its roomConnection array. return -1 for no free index
location, else return number >= 0 representing the
available index.
*/
int isCanAddConnectionFrom(struct Room *roomA)
{
    //will return index or -1 for no available index
    return getFreeConnectionIndex(roomA);
}


/*
Function name: isConnectionPossible
Function type: int
Parameters: pointer to struct room (roomA),
pointer to struct room (roomB).
Description: Determine if connection can be made between
roomA and roomB. Return FALSE (0) if the are the same room,
return FALSE (0) if they presently share a connection, else
return TRUE (1).
*/
int isConnectionPossible(struct Room *roomA, struct Room *roomB)
{
    //test if they are the same room
    if(isSameRoom(roomA, roomB) == TRUE)
        return FALSE;

    //test if they already share a connection
    if(isConnectionPresent(roomA, roomB) == TRUE)
        return FALSE;

    return TRUE; //ok to connect both
}


/*
Function name: makeConnection
Function type: void
Parameters: pointer to a struct room (roomA), pointer to a
struct room (roomB).
Description: Find first available index in each
structs roomConnection array. Assign roomA and roomB
as connected rooms.
*/
void makeConnection(struct Room *roomA, struct Room *roomB)
{
    //find first free index in room A and room B
    //freeIndexA and freeIndexB should not be -1 at this point
    int freeIndexA = getFreeConnectionIndex(roomA);
    int freeIndexB = getFreeConnectionIndex(roomB);

    //connect room A and B together at first available index
    //increase connection amount for each room
    roomA->roomConnection[freeIndexA] = roomB;
    roomA->numConnections++;
    roomB->roomConnection[freeIndexB] = roomA;
    roomB->numConnections++;
}

/*
Function name: addRandomConnection
Function type: void
Parameters: array of pointers of  struct Room (rooms)
Description: Loop through all rooms in rooms and add a
random connection between each one.
*/
void addRandomConnection(struct Room *rooms[])
{
    struct Room *firstRoom  = NULL;
    struct Room *secondRoom = NULL;

    int possibleConnection = FALSE;
    int spaceA = FALSE;
    int spaceB = FALSE;
    int firstRoomGood  = FALSE;
    int secondRoomGood = FALSE;

    //continue loop while graph is not full
    while(isGraphFull(rooms)==FALSE)
    {
        while(possibleConnection == FALSE)
        {
            //test if room A has space
            while(firstRoomGood == FALSE)
            {
                firstRoom  = getRandomRoom(rooms); //get random room
                spaceA     = isCanAddConnectionFrom(firstRoom);
                if(spaceA > -1) //there is space between index 0 to n
                    firstRoomGood = TRUE;
            }
            firstRoomGood = FALSE;

            //test if room B has space
            while(secondRoomGood == FALSE)
            {
                secondRoom = getRandomRoom(rooms); //get random room
                spaceB     = isCanAddConnectionFrom(secondRoom);
                if(spaceB > -1) //there is space between index 0 to n
                    secondRoomGood = TRUE;
            }
            secondRoomGood = FALSE;

            //test if connection between both is possible
            possibleConnection = isConnectionPossible(firstRoom, secondRoom);
            //test if possible to connect both rooms, if true then connect.
            if(possibleConnection == TRUE)
                makeConnection(firstRoom, secondRoom);
        }
        possibleConnection = FALSE;
    }
}

/*
Function name: writeToFiles
Function type: void
Parameters: Array of pointers to room struct (rooms),
pointer of FILE type (fPtr), pointer to char array for directory name (dirName).
Description: Creates MAX_ROOM_AMOUNT of files. Each struct in
rooms is written to its own file. Files contain room name,
room type, names of connected rooms.
*/
void writeToFiles(struct Room *rooms[], FILE *fPtr, char *dirName)
{
    int i, j;
    int room_type = 0;
    int connectionAmt = 0;
    char dirPath_filename[50];
    char roomHeader[]         = "ROOM NAME: ";
    char connectionHeader[15] = "CONNECTION ";
    char roomTypeHeader[]     = "ROOM TYPE: ";
    char numToStr[4];
    char *roomTypes[3];
    roomTypes[0] = "START_ROOM";
    roomTypes[1] = "MID_ROOM";
    roomTypes[2] = "END_ROOM";

    //Write data from each struct Room into its own file
    for(i = 0; i < MAX_ROOM_AMOUNT; i++)
    {
        memset(dirPath_filename, '\0', sizeof(dirPath_filename));
        strcpy(dirPath_filename, dirName);
        strcat(dirPath_filename, "/");
        strcat(dirPath_filename, rooms[i]->roomName);

        room_type = rooms[i]->roomType;
        fPtr = fopen(dirPath_filename, "w+");
        fprintf(fPtr, "%s",roomHeader); //room name header
        fprintf(fPtr, "%s",rooms[i]->roomName); //room name
        fprintf(fPtr, "\n");
        //print all the room connections
        connectionAmt = rooms[i]->numConnections;
        for(j = 0; j < connectionAmt; j++)
        {
            memset(numToStr, '\0', sizeof(numToStr));
            fprintf(fPtr, "%s",connectionHeader); //connection header
            sprintf(numToStr,"%d", j+1);
            strcat(numToStr, ": ");
            fprintf(fPtr, "%s",numToStr);
            fprintf(fPtr, "%s",rooms[i]->roomConnection[j]->roomName);
            fprintf(fPtr, "\n");
        }

        fprintf(fPtr, "%s",roomTypeHeader);
        fprintf(fPtr, "%s",roomTypes[room_type - 1]);
        fprintf(fPtr, "\n");
        fclose(fPtr);
    }

}

/*
Function name: freeRooms
Function type: void
Parameters: array of pointers of struct Room (roomsArr)
Description: Free all the allocated memory
for each Room struct in roomsArr.
*/
void freeRooms(struct Room *roomsArr[])
{
    int i;
    for(i = 0; i < MAX_ROOM_AMOUNT; i++)
        free(roomsArr[i]);

}

int main(int argc, const char * argv[]) {
    //for random function
    srand(time(NULL));

    int i = 0, j = 0;
    int createDir = 0;
    int pid = 0;//getpid(); //get the process id
    FILE *fp  = NULL;

    char *dir_name;
    char *stringsArray[SAMPLE_ROOM_AMT];
    struct Room *roomArray[MAX_ROOM_AMOUNT]; //7

    //create the directory
    pid = getpid();
    dir_name = createDiretoryName(pid);
    createDir = mkdir(dir_name, 0755);

    //fill sample array with 10 strings
    fillSampleStringArray(stringsArray);

    //initialize the
    initializeStructRooms(roomArray, stringsArray);

    //randomly connect the rooms
    addRandomConnection(roomArray);

    //write the struct array data to files
    writeToFiles(roomArray, fp, dir_name);

    //free allocated rooms
    freeRooms(roomArray);

    //free directory name
    free(dir_name);

    return 0;
}
