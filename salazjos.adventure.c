/*
Name: Joseph Salazar
email: salazjos@oregonstate.edu
class: CS 344 Program 2
File: salazjos.adventure.c
Description: Program reads the seven files generated
from salazjos.buildrooms.c. and populates a graph
from all the connected rooms. The game is played by
allowing the player to select a room from a connected
room list. The game ends when the player has reached
the ending room.
*/

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_ROOM_CONNECTIONS 6
#define MAX_ROOM_AMOUNT 7
#define TRUE 1
#define FALSE 0

//mutex for the time thread
pthread_mutex_t timeLock = PTHREAD_MUTEX_INITIALIZER;

//enum to hold room type
enum RoomType{ START_ROOM = 1, MID_ROOM, END_ROOM};

//struct to represent a room
struct Room{
    enum RoomType roomType;
    int numConnections;
    char roomName[15];
    struct Room *roomConnection[MAX_ROOM_CONNECTIONS];
    char connectionNameList[MAX_ROOM_CONNECTIONS][16];
};

/*
Function name: createRooms
Function type: void
Parameters: array of pointers of struct Room (rooms)
Description: Allocaate memory for each pointer in rooms,
set their connection amount to zero, and null each pointer
in its roomConnection array.
*/
void createRooms(struct Room *rooms[])
{
    int i, j;
    for(i = 0; i < MAX_ROOM_AMOUNT; i++)
    {
        rooms[i] = malloc(sizeof(struct Room));
        rooms[i]->numConnections = 0;
        memset(rooms[i]->roomName, '\0', sizeof(rooms[i]->roomName));
        //NUll all the connections
        for(j = 0; j < MAX_ROOM_CONNECTIONS; j++)
            rooms[i]->roomConnection[j] = NULL;
    }
}

/*
Function name: printCurrentLocation
Function type: void
Parameters: pointer to a struct Room (room)
Description: Print current location header and the
room name of room
*/
void printCurrentLocation(struct Room *room)
{
    char *currLocHeader = "CURRENT LOCATION: ";
    printf("%s",currLocHeader);
    printf("%s\n", room->roomName);
}

/*
Function name: printInputErrMsg
Function type: void
Parameters: none
Description: Print error mesage for incorrect room spelling
*/
void printInputErrMsg()
{
    char *errorMsg = "HUH? I DONT UNDERSTAND THAT ROOM, TRY AGAIN.";
    printf("\n%s\n\n", errorMsg);
}

/*
Function name: printCongradMsg
Function type: void
Parameters: none
Description: Print reaching end room congradulation message.
*/
void printCongradMsg()
{
    char *congradMsg = "YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n";
    printf("\n%s", congradMsg);
}

/*
Function name: printConnectionList
Function type: void
Parameters: pointer to a struct Room (room)
Description: Loop through room's roomConnection array
and print all the connectd room's room names.
*/
void printConnectionList(struct Room *room)
{
    int i;
    char *connectionHeader = "POSSIBLE CONNECTIONS: ";
    char allConnectionsStr[128];
    memset(allConnectionsStr, '\0', sizeof(allConnectionsStr));

    printf("%s", connectionHeader);
    for(i = 0; i < room->numConnections; i++)
    {
        if(i == room->numConnections - 1) //last room in the array to followed by a period
        {
            strcat(allConnectionsStr, room->roomConnection[i]->roomName);
            strcat(allConnectionsStr, ".");
        }
        else //all other rooms in the array to be followed by a coma
        {
            strcat(allConnectionsStr, room->roomConnection[i]->roomName);
            strcat(allConnectionsStr, ", ");
        }
    }
    printf("%s\n", allConnectionsStr);
}

/*
Function name: getNewestDir
Function type: pointer of struct dirent
Parameters: ponter of DIR tpye (dir)
Description: Loops through directory where executable is,
looking for directory with newest time modication. Newest
directory will have highest process id appended after "room.".
returns pointer to the newest directory found.
*/
struct dirent *getNewestDir(DIR *dir)
{
    int  newestDirTime = -1; // Modified timestamp of newest subdir examined
    char targetDirPrefix[32] = "salazjos.room."; // Prefix we're looking for
    struct dirent *fileInDir;
    struct dirent *newest_dir = NULL;
    struct stat dirAttributes;

    dir = opendir("."); //open the current directory
    if(dir > 0) //not null
    {
        while((fileInDir = readdir(dir)) != NULL)
        {
            if(strstr(fileInDir->d_name, targetDirPrefix) != NULL)
            {
                stat(fileInDir->d_name, &dirAttributes);
                if((int)dirAttributes.st_mtime > newestDirTime)
                {
                    newestDirTime = (int)dirAttributes.st_mtime;
                    newest_dir = fileInDir;
                }
            }
        }
    }
    closedir(dir);
    return newest_dir;
}

/*
Function name: getRoom
Function type: pointer to a struct Room
Parameters: array of pointers of struct Room (rooms),
pointer to char array (nameToFind).
Description: Loop through the array looking for a
room name that matches nameToFind. Return pointer to
struct Room with matching name. Should never return null
when this function is called.
*/
struct Room *getRoom(struct Room *rooms[], char *nameToFind)
{
    int i;
    struct Room *foundRoom = NULL;
    for(i = 0; i < MAX_ROOM_AMOUNT; i++)
    {
        if(strcmp(nameToFind, rooms[i]->roomName) == 0)
        {
            foundRoom = rooms[i];
            break;
        }
    }
    return foundRoom;
}

/*
Function name: assignRoomConnections
Function type: void
Parameters: array of pointers of Struct Room (rooms)
Description: Loop through each pointer in rooms, getting
room name in its connectionNameList and assign pionter to
struct with matching name.
*/
void assignRoomConnections(struct Room *rooms[])
{
    int i, j;
    char *tempChar = NULL;
    for(i = 0; i < MAX_ROOM_AMOUNT; i++)
    {
        for(j = 0; j < rooms[i]->numConnections; j++)
        {
            tempChar = &(rooms[i]->connectionNameList[j][0]);
            //find room with matching name and assign pointer.
            rooms[i]->roomConnection[j] = getRoom(rooms, tempChar);
        }
    }
}

/*
Function name: populateRoomsArrFromFiles
Function type: void
Parameters: pointer of struct dirent type (newestDir),
pointer of DIR type (dir), array of pointers of struct Room (rooms).
Description: Loop through each of the seven files in the newest directory.
Populate each struct in rooms with data from the files. Data will be room
name, room tpye, and names of connecting rooms.
*/
void populateRoomsArrFromFiles(struct dirent *newestDir, DIR *dir, struct Room *rooms[])
{
    dir = NULL;
    struct dirent *direntTemp = NULL;
    FILE *fp;
    char fileName[256];
    char textLine[256];
    char roomName[15];
    char roomType[15];
    char tempArray1[15];
    char tempArray2[15];
    char roomLook[5]        = "NAME";
    char connectionLook[11] = "CONNECTION";
    char typeLook[5]        = "TYPE";
    char newestDirName[50];// = NULL;

    int i, j;
    //newestDirName = newestDir->d_name;
    dir = opendir(newestDir->d_name);
    if(dir > 0)
    {
        memset(newestDirName, '\0', sizeof(newestDirName));
        strcpy(newestDirName, newestDir->d_name);
        i = j = 0;
        while((direntTemp = readdir(dir)) != NULL )
        {
            //continue if file is a directory file named "."
            if(!strcmp (direntTemp->d_name, "." ))
                continue;

            //continue if file is a directory file named ".."
            if(!strcmp (direntTemp->d_name, ".."))
                continue;

            memset(fileName, '\0', sizeof(fileName));
            strcpy(fileName, newestDirName);
            strcat(fileName, "/");
            strcat(fileName, direntTemp->d_name);
            fp = fopen(fileName, "r");
            if(fp == NULL)
            {
                printf("Failed to open: %s\n", direntTemp->d_name );
                fclose(fp);
                break;
            }

            memset(textLine, '\0', sizeof(textLine));
            while(fgets(textLine, 256, fp))
            {
                memset(tempArray1, '\0', sizeof(tempArray1));
                memset(tempArray2, '\0', sizeof(tempArray2));

                if(strstr(textLine,roomLook) != NULL)
                {
                    //fill struct array pointer location with its room name
                    sscanf(textLine, "%s %s %s", tempArray1, tempArray2, roomName);
                    strcpy(rooms[i]->roomName,roomName);
                    memset(roomName, '\0', sizeof(roomName));
                }
                else if(strstr(textLine,typeLook) != NULL)
                {
                    //fill struct array pointer location with its room type
                    sscanf(textLine, "%s %s %s", tempArray1, tempArray2, roomType);
                    if(strcmp(roomType, "START_ROOM") == 0)
                        rooms[i]->roomType = 1;
                    else if(strcmp(roomType, "MID_ROOM") == 0)
                        rooms[i]->roomType = 2;
                    else
                        rooms[i]->roomType = 3; //END_ROOM

                    memset(roomType, '\0', sizeof(roomType));
                }
                else //if(strstr(textLine,connectionLook) == 0)
                {
                    j = rooms[i]->numConnections;
                    sscanf(textLine, "%s %s %s", tempArray1, tempArray2, &(rooms[i]->connectionNameList[j][0]));
                    rooms[i]->numConnections++;
                }
            }

            fclose(fp);
            i++;
        }
    }
    closedir(dir);
}

/*
Function name: getStartRoom
Function type: pointer struct Room
Parameters: array of pointers of struct Room (rooms),
int for room type (roomType).
Description: Loop through rooms array and return pointer
of struct Room that has a room type of 1 (START_ROOM)
*/
struct Room *getStartRoom(struct Room *rooms[], int roomType)
{
    int i;
    struct Room *startRoom = NULL;
    for(i = 0; i < MAX_ROOM_AMOUNT; i++)
    {
        if(rooms[i]->roomType == 1)
        {
            startRoom = rooms[i];
            break;
        }
    }
    return startRoom;
}

/*
Function name freeRooms
Function type: void
Parameters: array of pointers of struct Room (rooms)
Description: Free every allocated room in the rooms array
*/
void freeRooms(struct Room *rooms[])
{
    int i;
    for(i = 0; i < MAX_ROOM_AMOUNT; i++)
    {
        if(rooms[i] != NULL)
            free(rooms[i]);
    }
}

/*
Function name: isRoomNameValid
Function type: int
Parameters: pointer to a struct Room (currRoom),
pointer to char array (roomName)
Description: determine if roomName is a valid room name
of a connection room to currRoom. Return TRUE(1) for yes,
FALSE(0) for no.
*/
int isRoomNameValid(struct Room *currRoom, char *roomName)
{
    int i;
    for(i = 0; i < currRoom->numConnections; i++)
    {
        if(strcmp(roomName, currRoom->roomConnection[i]->roomName) == 0)
            return TRUE;
    }
    return FALSE;
}

/*
Function name: isTimeEntryValid
Function type: int
Parameters: pointer to char array (timeWord)
Description: Determine if timeWord is the word "time".
Return TRUE(1) for yes, FALSE(0) for no.
*/
int isTimeEntryValid(char *timeWord)
{
    return ( strcmp(timeWord, "time") == 0 ) ? TRUE : FALSE;
}


/*
Function name: isEndRoom
Function type: int
Parameters: pointer to a struct Room (currRoom)
Description: Determine if currRoom has room type of END_ROOM.
return TRUE (1) for yes, FALSE (0) for no.
*/
int isEndRoom(struct Room *currRoom)
{
    return (currRoom->roomType == 3) ? TRUE : FALSE;
}

/*
Function name: isValidInput
Function type: int
Parameters: pointer to struct Room (room),
pointer to char array (userinput)
Description: return 1 if userInput is a valid connecting room
in room, return 2 if userInput is the word "time", return 3 if
neither were true.
*/
int isValidInput(struct Room *room, char *userInput)
{
    if(isRoomNameValid(room, userInput) == TRUE )
        return 1;
    else if(isTimeEntryValid(userInput) == TRUE)
        return 2;
    else
        return 3;
}

/*
Function name: writeTime
Function type: void, function pointer
Description: Called by tiem thread, writes the
current system time/date to a file called
currentTime.txt
*/
void* writeTime(void* argument)
{
    pthread_mutex_lock(&timeLock);
    FILE *f_out;
    char *timeFileName = "currentTime.txt";
    char timeInfoBuffer[128];
    memset(timeInfoBuffer, '\0', sizeof(timeInfoBuffer));
    time_t rawtime;
    struct tm *info;
    time(&rawtime);
    info  = localtime(&rawtime);
    f_out = fopen(timeFileName, "w");
    if(!f_out)
        printf("Cannot open %s\n", timeFileName);
    strftime(timeInfoBuffer, 128, "%I:%M%p, %A, %B %d, %Y", info);
    fprintf(f_out, "%s\n", timeInfoBuffer);
    fclose(f_out);
    pthread_mutex_unlock(&timeLock);
    return NULL;
}

/*
Function name: printTimeFromFile
Function type: void
Parameters: none
Description: Read the file currentTime.txt and
print the time/date written in that file.
*/
void printTimeFromFile()
{
    FILE *f_open;
    char *file_name = "currentTime.txt";
    char timeLine[128];
    memset(timeLine, '\0', sizeof(timeLine));
    f_open = fopen(file_name, "r");

    while(fgets(timeLine, 128, f_open))
        printf("\n\n%s\n\n", timeLine);

    fclose(f_open);
}

int main(int argc, const char * argv[]) {

    int i, j, index = 0, len = 0;
    int numCharsEntered = 0, inputValidation = 0,
            stepCount = 0, resultID = 0;
    struct dirent *newest_dir = NULL;
    struct Room *currentRoom  = NULL;
    struct Room *roomsArray[MAX_ROOM_AMOUNT];
    struct Room *roomsVisited[50];
    struct Connections *connArray[MAX_ROOM_AMOUNT];
    DIR  *dir = NULL; // starting directory
    char *userInput = NULL;
    size_t buffersize = 0;
    pthread_t timeThreadID;

    //create all the rooms in roomsArray
    createRooms(roomsArray);

    //find the most recent directory name
    newest_dir = getNewestDir(dir);

    //fill the roomsArray with data from the files
    populateRoomsArrFromFiles(newest_dir, dir, roomsArray);

    //assign all the room connections between all room in roomsArray
    assignRoomConnections(roomsArray);

    // lock the mutex
    pthread_mutex_lock( &timeLock );

    //create the time thread
    resultID = pthread_create(&timeThreadID, NULL, writeTime, NULL);

    //play the game
    currentRoom = getStartRoom(roomsArray, 1); //current room is the starting room
    printCurrentLocation(currentRoom); //print current room location
    printConnectionList(currentRoom); //print connections from this room
    while(currentRoom->roomType != END_ROOM)
    {
        printf("WHERE TO? >");
        //get user input
        numCharsEntered = getline(&userInput, &buffersize, stdin);
        len = strlen(userInput);
        userInput[len -1 ] = '\0';
        //determine if input was a valid room name or valid "time" word
        inputValidation = isValidInput(currentRoom, userInput);
        switch (inputValidation)
        {
            case 1: //user entered a valid room name
                //reassign the current room
                currentRoom = getRoom(roomsArray, userInput);
                printf("\n");
                //assign the current room into rooms visited array
                roomsVisited[index] = currentRoom;
                index++;
                stepCount++;
                buffersize = 0;
                free(userInput);
                userInput = NULL;
                printCurrentLocation(currentRoom); //print current room location
                printConnectionList(currentRoom); //print connections from this room
                break;
            case 2: //user entered "time"
                //unlock mutex
                pthread_mutex_unlock( &timeLock );
                //join the time thread
                resultID = pthread_join(timeThreadID, NULL);
                //print the time from the file
                printTimeFromFile();
                //lock mutex
                pthread_mutex_lock( &timeLock);
                //create time thread
                resultID = pthread_create(&timeThreadID, NULL, writeTime, NULL);
                buffersize = 0;
                free(userInput);
                userInput = NULL;
                break;
            case 3: //invalid input
                printInputErrMsg();
                buffersize = 0;
                free(userInput);
                userInput = NULL;
                printCurrentLocation(currentRoom); //print current room location
                printConnectionList(currentRoom); //print connections from this room
                break;
            default:
                break;
        }
    }
    //print end room reach congrad message
    printCongradMsg();
    //print amount of steps taken
    printf("YOU TOOK %d steps. ", stepCount);
    printf("YOUR PATH TO VICTORY WAS: \n");
    //print rooms visited
    for(i = 0; i < stepCount; i++)
        printf("%s\n", roomsVisited[i]->roomName);

    //free all the memory allocated for each room in roomsArray
    freeRooms(roomsArray);

    return 0;
}

