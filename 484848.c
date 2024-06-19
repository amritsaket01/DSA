#include <stdio.h>
#include <stdlib.h>
static int BucketIDStart = 500;
typedef struct Time
{
    int hrs;
    int min;
} Time;

typedef struct AVLFlightTag
{
    int flightId;
    Time departureTime;
    Time ETA;
    struct AVLFlightTag *left;
    struct AVLFlightTag *right;
    int FlightHeight;
} AVLFlightSchedule;

typedef struct AVLBucketTag
{
    int bucketId;
    Time etaStart;
    Time etaEnd;
    AVLFlightSchedule *FlightSchedule;
    struct AVLBucketTag *left;
    struct AVLBucketTag *right;
    int BucketHeight;
} AVLBucket;

int timeDiff(Time a, Time b)
{
    int diff = 0;
    diff = (a.hrs * 60 + a.min) - (b.hrs * 60 + b.min);
    return diff;
}

int maxTime(Time a, Time b)
{
    int diff = timeDiff(a, b);
    if (diff == 0)
    {
        return 0;
    }
    else if (diff > 0)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

int max(int a, int b)
{
    int ret_val = a;
    if (a < b)
        ret_val = b;
    return ret_val;
}

AVLBucket *createBucket(int bucketId, Time etaStart, Time etaEnd);
AVLFlightSchedule *createFlightSchedule(int flightId, Time departureTime, Time eta);
void Print(AVLBucket *bucket);
int showStatus(AVLBucket *buketList, int flightId);
void insertFlightPlan(AVLBucket **bucketList, int flightId, Time departureTime, Time ETA);
AVLBucket *insertBucket(AVLBucket *bucketList, AVLBucket *newBucket);
AVLBucket* deleteFlightPlan(AVLBucket *bucketList, int flightId, Time DepartureTime);

AVLBucket *createBucket(int bucketId, Time etaStart, Time etaEnd)
{
    AVLBucket *newBucket = (AVLBucket *)malloc(sizeof(AVLBucket));
    newBucket->bucketId = bucketId;
    newBucket->etaStart = etaStart;
    newBucket->etaEnd = etaEnd;
    newBucket->FlightSchedule = NULL;
    newBucket->left = NULL;
    newBucket->right = NULL;
    newBucket->BucketHeight = 1;
    return newBucket;
}

AVLFlightSchedule *createFlightSchedule(int flightId, Time departureTime, Time eta)
{
    AVLFlightSchedule *newFlight = (AVLFlightSchedule *)malloc(sizeof(AVLFlightSchedule));
    newFlight->flightId = flightId;
    newFlight->departureTime = departureTime;
    newFlight->ETA = eta;
    newFlight->left = NULL;
    newFlight->right = NULL;
    newFlight->FlightHeight = 1;
    return newFlight;
}

void PrintFlightSchedule(AVLFlightSchedule *flight)
{
    if (flight != NULL)
    {
        PrintFlightSchedule(flight->left);
        printf("Flight Id: %d\n", flight->flightId);
        printf("Departure Time: %d:%d\n", flight->departureTime.hrs, flight->departureTime.min);
        printf("ETA: %d:%d\n", flight->ETA.hrs, flight->ETA.min);
        PrintFlightSchedule(flight->right);
    }
}

void Print(AVLBucket *bucket)
{
    if (bucket != NULL)
    {
        Print(bucket->left);
        printf("Bucket %d\n", bucket->bucketId);
        printf("ETA Start: %d:%d\n", bucket->etaStart.hrs, bucket->etaStart.min);
        printf("ETA End: %d:%d\n", bucket->etaEnd.hrs, bucket->etaEnd.min);
        PrintFlightSchedule(bucket->FlightSchedule);
        printf("\n");
        Print(bucket->right);
    }
}

int searchFlight(AVLFlightSchedule *flight, int flightID)
{   int ans = 0 ;
    int flag = 0 ;
    if (flight != NULL && flag == 0)
    {
        if (flight->flightId == flightID)
        {
            printf("Flight %d\n", flight->flightId);
            printf("Departure Time: %d:%d\n", flight->departureTime.hrs, flight->departureTime.min);
            printf("ETA: %d:%d\n", flight->ETA.hrs, flight->ETA.min);
            ans = 1;
            flag = 1;
        }
        searchFlight(flight->left, flightID);
        searchFlight(flight->right, flightID);
    }
    return ans;
}

int showStatus(AVLBucket *bucketList, int flightId)
{   int ans = 0 ;
    if (bucketList != NULL)
    {
        showStatus(bucketList->left, flightId);
        ans = searchFlight(bucketList->FlightSchedule, flightId);
        if(ans) return ans;
        showStatus(bucketList->right, flightId);
    }
    return ans;
}
int flightTreeHegiht(AVLFlightSchedule *Flight)
{
    int ans = 0;
    if (Flight != NULL)
    {
        ans = Flight->FlightHeight;
    }
    return ans;
}

int bucketTreeHeight(AVLBucket *bucket)
{
    int ans = 0;
    if (bucket != NULL)
    {
        ans = bucket->BucketHeight;
    }
    return ans;
}

AVLBucket *leftRotateBucket(AVLBucket *bucket)
{
    AVLBucket *bucketRight = bucket->right;
    AVLBucket *bucketRightLeft = bucketRight->left;

    bucketRight->left = bucket;
    bucket->right = bucketRightLeft;

    bucket->BucketHeight = max(bucketTreeHeight(bucket->left), bucketTreeHeight(bucket->right)) + 1;
    bucketRight->BucketHeight = max(bucketTreeHeight(bucketRight->left), bucketTreeHeight(bucketRight->right)) + 1;

    return bucketRight;
}

AVLBucket *rightRotateBucket(AVLBucket *bucket)
{
    AVLBucket *bucketLeft = bucket->left;
    AVLBucket *bucketLeftRight = bucketLeft->right;

    bucketLeft->right = bucket;
    bucket->left = bucketLeftRight;

    bucket->BucketHeight = max(bucketTreeHeight(bucket->left), bucketTreeHeight(bucket->right)) + 1;
    bucketLeft->BucketHeight = max(bucketTreeHeight(bucketLeft->left), bucketTreeHeight(bucketLeft->right)) + 1;

    return bucketLeft;
}

int balanceBucket(AVLBucket *bucket)
{
    int ans = 0;
    if (bucket != NULL)
    {
        ans = bucketTreeHeight(bucket->left) - bucketTreeHeight(bucket->right);
    }
    return ans;
}

AVLBucket *insertBucket(AVLBucket *bucketList, AVLBucket *newBucket)
{
    if (bucketList == NULL)
    {
        bucketList = newBucket;
        return bucketList;
    }

    if (maxTime(bucketList->etaStart, newBucket->etaEnd) == 1)
    {
        bucketList->left = insertBucket(bucketList->left, newBucket);
    }
    else if (maxTime(newBucket->etaStart, bucketList->etaEnd) == 1)
    {
        bucketList->right = insertBucket(bucketList->right, newBucket);
    }
    
    bucketList->BucketHeight = 1 + max(bucketTreeHeight(bucketList->left), bucketTreeHeight(bucketList->right));

    int balanceOfBucket = balanceBucket(bucketList);

    if (balanceOfBucket > 1 && maxTime(bucketList->left->etaEnd, newBucket->etaEnd) == 1)
    {
        return rightRotateBucket(bucketList);
    }
    else if (balanceOfBucket > 1 && maxTime(bucketList->left->etaEnd, newBucket->etaEnd) == -1)
    {
        bucketList->left = leftRotateBucket(bucketList->left);
        return rightRotateBucket(bucketList);
    }
    else if (balanceOfBucket < -1 && maxTime(bucketList->right->etaEnd, newBucket->etaEnd) == 1)
    {
        bucketList->right = rightRotateBucket(bucketList->right);
        return leftRotateBucket(bucketList);
    }
    else if (balanceOfBucket < -1 && maxTime(bucketList->right->etaEnd, newBucket->etaEnd) == -1)
    {
        return leftRotateBucket(bucketList);
    }

    return bucketList;
}

AVLFlightSchedule *leftRotateFlightSchedule(AVLFlightSchedule *flightSchedule)
{
    AVLFlightSchedule *flightScheduleRight = flightSchedule->right;
    AVLFlightSchedule *flightScheduleRightLeft = flightScheduleRight->left;

    flightScheduleRight->left = flightSchedule;
    flightSchedule->right = flightScheduleRightLeft;

    flightSchedule->FlightHeight = max(flightTreeHegiht(flightSchedule->left), flightTreeHegiht(flightSchedule->right)) + 1;
    flightScheduleRight->FlightHeight = max(flightTreeHegiht(flightScheduleRight->left), flightTreeHegiht(flightScheduleRight->right)) + 1;

    return flightScheduleRight;
}

AVLFlightSchedule *rightRotateFlightSchedule(AVLFlightSchedule *flightSchedule)
{
    AVLFlightSchedule *flightScheduleLeft = flightSchedule->left;
    AVLFlightSchedule *flightScheduleLeftRight = flightScheduleLeft->right;

    flightScheduleLeft->right = flightSchedule;
    flightSchedule->left = flightScheduleLeftRight;

    flightSchedule->FlightHeight = max(flightTreeHegiht(flightSchedule->left), flightTreeHegiht(flightSchedule->right)) + 1;
    flightScheduleLeft->FlightHeight = max(flightTreeHegiht(flightScheduleLeft->left), flightTreeHegiht(flightScheduleLeft->right)) + 1;

    return flightScheduleLeft;
}
int balanceFlight(AVLFlightSchedule *flightSchedule)
{
    int ans = 0;
    if (flightSchedule != NULL)
    {
        ans = flightTreeHegiht(flightSchedule->left) - flightTreeHegiht(flightSchedule->right);
    }
    return ans;
}

int liesBetween(Time etaStart, Time etaEnd, Time givenETA)
{
    int ret_val = 0;
    if (timeDiff(givenETA, etaStart) >= 0 && timeDiff(givenETA, etaStart) < 60 && timeDiff(etaEnd, givenETA) >= 0 && timeDiff(etaEnd, givenETA) < 60)
    {
        ret_val = 0;
    }
    else if (timeDiff(givenETA, etaStart) < 0)
    {
        ret_val = -1;
    }
    else if (timeDiff(givenETA, etaEnd) > 0)
    {
        ret_val = 1;
    }
    return ret_val;
}
AVLBucket *checkWhetherBucketExists(AVLBucket *bucketList, AVLFlightSchedule *newflight)
{
    AVLBucket *Bptr = NULL;
    if (bucketList != NULL)
    {
        if (liesBetween(bucketList->etaStart, bucketList->etaEnd, newflight->ETA) == -1)
        { // left
            Bptr = checkWhetherBucketExists(bucketList->left, newflight);
        }
        else if (liesBetween(bucketList->etaStart, bucketList->etaEnd, newflight->ETA) == 1)
        { // right
            Bptr = checkWhetherBucketExists(bucketList->right, newflight);
        }
        else if (liesBetween(bucketList->etaStart, bucketList->etaEnd, newflight->ETA) == 0)
        {// found
            Bptr = bucketList;
        }
    }
    return Bptr;
}

AVLFlightSchedule *insertFlightIntoBucket(AVLFlightSchedule *flightList, AVLFlightSchedule *newflight)
{
    if (flightList == NULL)
    {
        return newflight;
    }

    if (maxTime(flightList->departureTime, newflight->departureTime) == 1)
    {
        flightList->left = insertFlightIntoBucket(flightList->left, newflight);
    }
    else if (maxTime(flightList->departureTime, newflight->departureTime) == -1)
    {
        flightList->right = insertFlightIntoBucket(flightList->right, newflight);
    }

    flightList->FlightHeight = 1 + max(flightTreeHegiht(flightList->left), flightTreeHegiht(flightList->right));

    int balanceOfFlight = balanceFlight(flightList);

    if (balanceOfFlight > 1 && maxTime(flightList->left->departureTime, newflight->departureTime) == 1)
    {
        return rightRotateFlightSchedule(flightList);
    }
    if (balanceOfFlight > 1 && maxTime(flightList->left->departureTime, newflight->departureTime) == -1)
    {
        flightList->left = leftRotateFlightSchedule(flightList->left);
        return rightRotateFlightSchedule(flightList);
    }
    if (balanceOfFlight < -1 && maxTime(flightList->right->departureTime, newflight->departureTime) == 1)
    {
        flightList->right = rightRotateFlightSchedule(flightList->right);
        return leftRotateFlightSchedule(flightList);
    }
    if (balanceOfFlight < -1 && maxTime(flightList->right->departureTime, newflight->departureTime) == -1)
    {
        return leftRotateFlightSchedule(flightList);
    }
    return flightList;
}
void insertFlightPlan(AVLBucket **bucketList, int flightId, Time departureTime, Time ETA)
{
    AVLFlightSchedule *newFlight = createFlightSchedule(flightId, departureTime, ETA);

    if (*bucketList == NULL)
    {
        printf("New bucket added with bucketID %d\n", BucketIDStart);
        Time etaS, etaE;
        etaS.hrs = ETA.hrs;
        etaS.min = 0;
        etaE.hrs = ETA.hrs;
        etaE.min = 59;
        AVLBucket *newBucket = createBucket(BucketIDStart++, etaS, etaE);
        *bucketList = newBucket;
        (*bucketList)->FlightSchedule = newFlight;
    }

    else
    {
        AVLBucket *bucketExists = checkWhetherBucketExists(*bucketList, newFlight);

        if (bucketExists != NULL)
        {
            AVLFlightSchedule *flightList = bucketExists->FlightSchedule;
            bucketExists->FlightSchedule = insertFlightIntoBucket(flightList, newFlight);
            }
        else
        {
            printf("New bucket added with bucketID %d\n", BucketIDStart);
            Time etaS, etaE;
            etaS.hrs = ETA.hrs;
            etaS.min = 0;
            etaE.hrs = ETA.hrs;
            etaE.min = 59;
            AVLBucket *newBucket = createBucket(BucketIDStart++, etaS, etaE);
            newBucket->FlightSchedule = newFlight;
            *bucketList = insertBucket(*bucketList, newBucket);
        }
    }
}

AVLFlightSchedule *inorderSuccessor(AVLFlightSchedule *flightSchedule) {
    AVLFlightSchedule *temp = flightSchedule;
    while (temp->left != NULL) {
        temp = temp->left;
    }
    return temp;
}

AVLBucket* searchFlightToDelete(AVLBucket * bucket, AVLFlightSchedule* flightList, int flightId, Time departureTime) {
    AVLBucket *Bptr = NULL;
    if (flightList != NULL) {
        if (flightList->flightId == flightId) {
            Bptr = bucket;
        } 
        else if (maxTime(departureTime, flightList->departureTime) == -1) {
            Bptr = searchFlightToDelete(bucket, flightList->left, flightId, departureTime);
        } 
        else if (maxTime(departureTime, flightList->departureTime) == 1) {
            Bptr = searchFlightToDelete(bucket, flightList->right, flightId, departureTime);
        }
    }
    return Bptr;
}

AVLFlightSchedule *deleteHelper(AVLFlightSchedule *flightSchedule, int flightId, Time DepartureTime) {
    if (flightSchedule == NULL) return NULL;

    if (flightSchedule->flightId == flightId && maxTime(flightSchedule->departureTime, DepartureTime) == 0) {
        // 0 child
        if (flightSchedule->left == NULL && flightSchedule->right == NULL) {
            free(flightSchedule);
            return NULL;
        }
        // 1 child   
        if(flightSchedule->right == NULL && flightSchedule->left != NULL) {
            AVLFlightSchedule* temp = flightSchedule->left;
            free(flightSchedule);
            return temp;
        }
        if(flightSchedule->left == NULL && flightSchedule->right != NULL){
            AVLFlightSchedule* temp = flightSchedule->right;
            free(flightSchedule);
            return temp;
        }
        else {
            AVLFlightSchedule *successor = inorderSuccessor(flightSchedule->right);
            flightSchedule->flightId = successor->flightId;
            flightSchedule->departureTime = successor->departureTime;
            flightSchedule->ETA = successor->ETA;
            flightSchedule->right = deleteHelper(flightSchedule->right, successor->flightId, successor->departureTime);
        }
    } 
    else if (maxTime(flightSchedule->departureTime, DepartureTime) == 1) {
        flightSchedule->left = deleteHelper(flightSchedule->left, flightId, DepartureTime);
    }
    else if (maxTime(flightSchedule->departureTime, DepartureTime) == -1) {
        flightSchedule->right = deleteHelper(flightSchedule->right, flightId, DepartureTime);
    }

    if (flightSchedule != NULL) {
        flightSchedule->FlightHeight = 1 + max(flightTreeHegiht(flightSchedule->left), flightTreeHegiht(flightSchedule->right));
        int balanceOfFlight = balanceFlight(flightSchedule);

        if (balanceOfFlight > 1 && balanceFlight(flightSchedule->left) >= 0) {
            return rightRotateFlightSchedule(flightSchedule);
        }
        if (balanceOfFlight > 1 && balanceFlight(flightSchedule->left) < 0) {
            flightSchedule->left = leftRotateFlightSchedule(flightSchedule->left);
            return rightRotateFlightSchedule(flightSchedule);
        }
        if (balanceOfFlight < -1 && balanceFlight(flightSchedule->right) <= 0) {
            return leftRotateFlightSchedule(flightSchedule);
        }
        if (balanceOfFlight < -1 && balanceFlight(flightSchedule->right) > 0) {
            flightSchedule->right = rightRotateFlightSchedule(flightSchedule->right);
            return leftRotateFlightSchedule(flightSchedule);
        }
    }
    return flightSchedule;
}
AVLBucket* deleteFlightPlan(AVLBucket *bucketList, int flightId, Time DepartureTime) {
    if (bucketList == NULL) return NULL;

    bucketList->left = deleteFlightPlan(bucketList->left, flightId, DepartureTime);
    bucketList->right = deleteFlightPlan(bucketList->right, flightId, DepartureTime);

    AVLBucket* bucketContains = searchFlightToDelete(bucketList, bucketList->FlightSchedule, flightId, DepartureTime);
    if (bucketContains != NULL) {
        AVLFlightSchedule* flightList = deleteHelper(bucketContains->FlightSchedule, flightId, DepartureTime);
        bucketContains->FlightSchedule = flightList;
        return bucketList;
    }
    return bucketList;
}

void searchFlightInRange(AVLFlightSchedule *flight, Time startTime, Time endTime)
{
    if (flight != NULL)
    {
        if (maxTime(flight->departureTime, startTime) == 1 && maxTime(endTime, flight->departureTime) == 1)
        {
            printf("Flight %d\n", flight->flightId);
            printf("Departure Time: %d:%d\n", flight->departureTime.hrs, flight->departureTime.min);
            printf("ETA: %d:%d\n", flight->ETA.hrs, flight->ETA.min);
        }
        searchFlightInRange(flight->left, startTime, endTime);
        searchFlightInRange(flight->right, startTime, endTime);
    }
}

void showFlightInRange(AVLBucket *bucketList, Time startTime, Time endTime)
{
    if (bucketList != NULL)
    {
        showFlightInRange(bucketList->left, startTime, endTime);
        searchFlightInRange(bucketList->FlightSchedule, startTime, endTime);
        showFlightInRange(bucketList->right, startTime, endTime);
    }
}

void printMenu()
{
    printf("1. Add a new flight\n");
    printf("2. Delete a flight\n");
    printf("3. Show Status\n");
    printf("4. Show Flight in Range \n");
    printf("5. Exit\n");
}
int main()
{
    AVLBucket *bucketList = NULL;
    int flightId;
    Time departTime;
    Time ETA;
    Time currTime;
    FILE *fptr;
    fptr = fopen("data.txt", "r");
    if (fptr == NULL)
    {
        printf("File not found\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        while (!feof(fptr))
        {
            fscanf(fptr, "%d %d %d %d %d", &flightId, &departTime.hrs, &departTime.min, &ETA.hrs, &ETA.min);
            insertFlightPlan(&bucketList, flightId, departTime, ETA);
        }
    }
    fclose(fptr);
    Print(bucketList);
    printf("--------------------------------\n");
    int choice ;
    while (1)
    {
        printMenu();
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            printf("Enter the Flight Id\n");
            scanf("%d", &flightId);
            printf("Enter Departure Time (hrs min)\n");
            scanf("%d %d", &departTime.hrs, &departTime.min);
            printf("Enter Expected Time of Arrival (hrs min)\n");
            scanf("%d %d", &ETA.hrs, &ETA.min);
            insertFlightPlan(&bucketList, flightId, departTime, ETA);
            Print(bucketList);
            break;
        case 2:
            printf("Enter the Flight Id (100 to 340 already exists)\n");
            scanf("%d", &flightId);
            printf("Enter Departure Time (hrs min)\n");
            scanf("%d %d", &departTime.hrs, &departTime.min);
            bucketList = deleteFlightPlan(bucketList, flightId, departTime);
            if(bucketList == NULL)  printf("Failed to delete") ;
            else Print(bucketList);
            break;
        case 3:
            printf("Enter the Flight Id (100 to 340 only)\n");
            scanf("%d", &flightId);
            printf("\n");
            int a = showStatus(bucketList, flightId);
            if(a == 0) printf("Flight with flightId %d not found\n", flightId);
            printf("\n");
            break;
        case 4:
            printf("Enter starting time hours and minutes\n");
scanf("%d %d", &currTime.hrs, &currTime.min);
            printf("Enter ending time hours and minutes\n") ;
            scanf("%d %d", &departTime.hrs, &departTime.min);
            showFlightInRange(bucketList, currTime, departTime);
            break;
        case 5:
            printf("..Happy Hacking..\n");
            return 0;
        default:
            printf("Invalid choice. Please try again.\n");
            break;
        }
    }
    
    return 0;
}