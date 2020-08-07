//
//  main.cpp
//  Hotel Management
//
//  Created by <Arif> on 4/2/17.
//  Copyright © 2017 <Arif>. All rights reserved.
//

#include <iostream>
#include <iomanip>
#define RTYPES 4
#define SPACE ' '
#define UNDERLINE '-'
#define NAMEW 20
#define ADJUST 11
#define NUMW 10
#define BOOLW  10
#define BSTATUS 2
#define SUCCESS 0
#define ERROR 1
enum RoomType{DELUXE,EXECUTIVE,FAMILY,COUPLES};


// Keeps a reference counting for every object allocated using malloc
class ReferenceCount
{
private:
    int mCount;

public:
    // Add reference to allocated object
    void addReference()
    {
        mCount++;
    }
    // Release reference to allocated object
    int releaseReference()
    {
        return --mCount;
    }
};

// Keeps track of objects allocated on heap through reference counting
template < typename T >
class SmartPointer
{
private:
    T*    mData;
    ReferenceCount* mReference;

public:
    //Constructor's
    SmartPointer() : mData(0), mReference(0)
    {
        mReference = new ReferenceCount();
        mReference->addReference();
    }
    SmartPointer(T* pValue) : mData(pValue), mReference(0)
    {
        mReference = new ReferenceCount();
        mReference->addReference();
    }

    SmartPointer(const SmartPointer<T>& sp) : mData(sp.mData), mReference(sp.mReference)
    {
        mReference->addReference();
    }
    //Destructor
    ~SmartPointer()
    {
        if(mReference->releaseReference() == 0)
        {
            delete mData;
            delete mReference;
        }
    }
    //Dereference object
    T& operator* ()
    {
        return *mData;
    }
    //Reference object
    T* operator-> ()
    {
        return mData;
    }
    //Overloaded equal to operator
    SmartPointer<T>& operator = (const SmartPointer<T>& sp)
    {
        if (this != &sp)
        {
            if(mReference->releaseReference() == 0)
            {
                delete mData;
                delete mReference;
            }
            mData = sp.mData;
            mReference = sp.mReference;
            mReference->addReference();
        }
        return *this;
    }
    //Increase reference count to object tracked by this smart pointer
    void addReference()
    {
        mReference->addReference();
    }
    //Decrease reference count to object tracked by this smart pointer
    void releaseReference()
    {
        mReference->releaseReference();
    }
    //Reference object
    T* getData(){
        return mData;
    }
    //Overloaded ostream for use with cout
    friend std::ostream &operator<<( std::ostream &output,const SmartPointer &sp ) {
        return output<<*(sp.mData);
    }
};

//Array that dynamically resizes it's capacity
template<class T>
class DynamicArray{
private:
    int mCount;
    int mCapacity;
    T **mArray;
    //To calclulate the new size of the array
    void resize(int capacity){
        T **mTempArray = new T*[capacity];
        mCapacity = capacity;
        for(int i=0;i<mCount;i++){
            mTempArray[i] = mArray[i];
        }
        mArray = mTempArray;
    }
public:
    //Constructor
    DynamicArray(){
        mCount = 0;
        mCapacity = 2;
        mArray = new T*[mCapacity];
    }
    //Destructor
    ~DynamicArray(){
    }
    //Get the number of items in this array
    int getLength() const{
        return mCount;
    }
    //Get item at index
    SmartPointer<T> getItem(int index){
        if(!(index>=0 && index<mCount)){
            //error
        }
        SmartPointer<T> item(mArray[index]);
        item.addReference();
        return item;
    }
    //Delete item at index
    void deleteItem(int index){
        SmartPointer<T> temp=mArray[index];
        for(int i = index+1;i<mCount;i++){
            mArray[i-1]=mArray[i];
        }
        mCount--;
        if(mCount == mCapacity/4){
            resize(mCapacity/2);
        }
    }
    //Append an item at the last
    void append(SmartPointer<T> item){
        item.addReference();
        if(mCount == mCapacity){
            resize(mCapacity*2);
        }
        mArray[mCount] = item.getData();
        mCount++;
    }
    //Overloaded ostream for use with cout
    friend std::ostream &operator<<( std::ostream &output,const DynamicArray &dArray ) {
        for(int i=0;i<dArray.mCount;i++){
            output<<std::left<<std::setw(NUMW)<<std::setfill(SPACE)<<i+1<<*(dArray.mArray[i]);
        }
        return output;
    }

};

//Forward declaration of classes
template<class T>
class PositionalListIteraor;

template<class T>
class PositionalList;

template<class T>
class Position;

class Hotel;
class CustomerData;

//Double Linked List
template<class T>
class DoubleLinkedBase{
public:
    //Datastructure for double linked list
    class Node{
    public:
        SmartPointer<T> mElement;
        Node *mPrevious;
        Node *mNext;
        Node(SmartPointer<T> element,Node *previous,Node *next){
            mElement = element;
            mPrevious = previous;
            mNext = next;
        }

        ~Node(){
        }
        friend class Position<T>;
    };
    Node *mHeader;
    Node *mTrailer;
    int mSize;
    //Constructor
    DoubleLinkedBase(){
        mHeader = new Node(NULL,NULL,NULL);
        mTrailer = new Node(NULL,NULL,NULL);
        mHeader->mNext = mTrailer;
        mTrailer->mPrevious = mHeader;
        mSize = 0;
    }
    //Get the size of the list
    int getLength(){
        return mSize;
    }
    //Check if list is empty
    bool isEmpty(){
        return mSize == 0;
    }
    //Insert data between a predecessor and successor
    Node* insertBetween(SmartPointer<T> element,Node *predecessor, Node *successor){
        Node *newest = new Node(element,predecessor,successor);
        predecessor->mNext = newest;
        successor->mPrevious = newest;
        mSize +=1;
        return newest;
    }
    //Delete the given node
    void deleteNode(Node *node){
        Node *predecessor = node->mPrevious;
        Node *successor = node->mNext;
        predecessor->mNext = successor;
        successor->mPrevious = predecessor;
        //delete node;
        mSize-=1;
    }
    //Destructor
    ~DoubleLinkedBase(){
        Node *header = mHeader->mNext;
        while(mHeader != mTrailer->mPrevious){
            Node *tmp = header->mNext;
            deleteNode(header);
            header = tmp;
        }
        delete mHeader;
        delete mTrailer;
    }

};

//Container for a linked list node
template<class T>
class Position{
private:
    PositionalList<T> *mContainer;
    typename DoubleLinkedBase<T>::Node *mNode;
public:
    friend class PositionalList<T>;
    //Constructor
    Position(){
        mContainer = NULL;
        mNode = NULL;
    }
    Position(const Position &p){
        mContainer = p.mContainer;
        mNode = p.mNode;
    }
    Position(PositionalList<T> *list,typename DoubleLinkedBase<T>::Node *node){
        mContainer = list;
        mNode = node;
    }
    //Overloaded equals operator
    void operator=(const Position &p) {
        mContainer = p.mContainer;
        mNode = p.mNode;
    }
    //Get the element corresponding to this Position
    SmartPointer<T> getElement(){
        return mNode->mElement;
    }
    //Overloaded ostream for use with cout
    friend std::ostream &operator<<( std::ostream &output,Position &pList ) {
        output<<pList.mNode->mElement;
        return output;
    }
    friend class PositionalListIteraor<T>;
};


template<class T>
class PositionalList : DoubleLinkedBase<T>{
private:
    //Get the header node
    typename DoubleLinkedBase<T>::Node * getHeader(){
        return DoubleLinkedBase<T>::mHeader;
    }
    //Get the trailer node
    typename DoubleLinkedBase<T>::Node * getTrailer(){
        return DoubleLinkedBase<T>::mTrailer;
    }
public:
    //Get the length of the positional list
    int getLength(){
        return DoubleLinkedBase<T>::getLength();
    }
    //Get the first element corresponding to the list
    Position<T> getFirst(){
        return Position<T>(this,DoubleLinkedBase<T>::mHeader->mNext);
    }
    //Get the last element corresponding to the list
    Position<T> getLast(){
        return Position<T>(this,DoubleLinkedBase<T>::mTrailer->mPrevious);
    }
    //Get the element after given position
    Position<T> getAfter(const Position<T> &p){
        return Position<T>(this,p.mNode->mNext);
    }
    //Add the element at first position
    Position<T> addFirst(SmartPointer<T> element){
        return Position<T>(this,DoubleLinkedBase<T>::insertBetween(element,DoubleLinkedBase<T>::mHeader,DoubleLinkedBase<T>::mHeader->mNext));
    }
    //Add the element at last position
    Position<T> addLast(SmartPointer<T> element){
        return Position<T>(this,DoubleLinkedBase<T>::insertBetween(element,DoubleLinkedBase<T>::mTrailer->mPrevious,DoubleLinkedBase<T>::mTrailer));
    }
    //Add the element before a given position
    Position<T> addBefore(Position<T> &p, SmartPointer<T> element){
        return Position<T>(this,DoubleLinkedBase<T>::insertBetween(element,p.mNode->mPrevious,p.mNode));
    }
    //Add the element after a given position
    Position<T> addAfter(Position<T> &p, SmartPointer<T> element){
        return Position<T>(this,DoubleLinkedBase<T>::insertBetween(element,p.mNode,p.mNode->mNext));
    }
    //Delete an item at the given position
    void deleteItem(Position<T> &p){
        DoubleLinkedBase<T>::deleteNode(p.mNode);
    }
    //Delete an item at the given index
    void deleteAtIndex(int index){
        SmartPointer<PositionalListIteraor<T> > list(new PositionalListIteraor<T>(this));
        int i = 1;
        do{
            Position<T> search = list->next();
            if(i==index){
                deleteItem(search);
                break;
            }
            i++;
        }while(!list->isLast());
    }
    //Get an item at the given index
    Position<T> getItemAtIndex(int index){
        Position<T> search;
        SmartPointer<PositionalListIteraor<T> > list(new PositionalListIteraor<T>(this));
        int i = 1;
        do{
             search = list->next();
            if(i==index)
                break;
            i++;
        }while(!list->isLast());
        return search;
    }
    //Overloaded ostream for use with cout
    friend std::ostream &operator<<( std::ostream &output,PositionalList &pList ) {
        if(!(pList.getLength() > 0) )
            return output;
        SmartPointer<PositionalListIteraor<T> > list(new PositionalListIteraor<T>(&pList));
        int index = 1;
        do{
            Position<T> search = list->next();
            output<<std::left<<std::setw(NUMW)<<std::setfill(SPACE)<<index<<search.getElement();
            index++;
        }while(!list->isLast());
        return output;
    }
    friend class PositionalListIteraor<T>;
};

//Stack used in this case for task handling
template<class T>
class Stack: public PositionalList<T>{
public:
    //Push an element to front of the stack
    void push(SmartPointer<T> element){
        PositionalList<T>::addFirst(element);
    }
    //Get the topmost element
    Position<T> top(){
        return PositionalList<T>::getFirst();
    }
    //Pop the recently inserted element
    void pop(){
        Position<T> topElement = top();
        PositionalList<T>::deleteItem(topElement);
    }
};

//Iteraor for looping through the list
template<class T>
class PositionalListIteraor{
    PositionalList<T> *mPlist;
    Position<T> mCurrentPosition;
public:
    //Constructor
    PositionalListIteraor(){

    }
    PositionalListIteraor(PositionalList<T> *list){
        mPlist=list;
        mCurrentPosition = (Position<T>(list,mPlist->getHeader()));
    }
    //Get the first element of the list
    Position<T> first(){
        return mPlist->getFirst();
    }
    //Get the last element of the list
    Position<T> last(){
        return mPlist->getLast();
    }
    //Get the next element
    Position<T> next(){
        mCurrentPosition = mPlist->getAfter(Position<T>(mCurrentPosition.mContainer,(mCurrentPosition.mNode)));
        return mCurrentPosition;
    }
    //Check if we have reached end of the list
    bool isLast(){
        return mPlist->getTrailer() == mCurrentPosition.mNode->mNext;
    }
    //Destructor
    ~PositionalListIteraor(){
    }
};


//Class for holding the date information
class Date{
private:
    int mDay;
    int mMonth;
    int mYear;
public:
    Date() {
        mDay=mMonth=mYear=0;
    }
    Date(int day, int month, int year){
        mDay=day;mMonth=month;mYear=year;
    }
    //Set the day
    void setDay(int day){
        mDay = day;
    }
    //Set the month
    void setMonth(int month){
        mMonth = month;
    }
    //Set the year
    void setYear(int year){
        mYear = year;
    }
    //Get the day
    int getDay(){
        return mDay;
    }
    //Get the month
    int getMonth(){
        return mMonth;
    }
    //Get the year
    int getYear(){
        return mYear;
    }
    //Check if it is a leap year
    bool isLeap(int year){
        return (year%4==0 && year%100 !=0)||year%400 == 0;
    }
    //Get the number of days passed in the given year
    int getDaysPassedInYear(){
        int days[] = {31,(isLeap(mYear))?29:28,31,30,31,30,31,31,30,31,30,31};
        int sum = mDay;
        for(int i=0;i<mMonth;i++){
            sum += days[i];
        }
        return sum;
    }
    //Compute max of two value
    int max(int a, int b){
        return (a>b)?a:b;
    }
    //Compute min of two value
    int min(int a,int b){
        return (a<b)?a:b;
    }
    //Compare two dates
    int operator>=(Date &other){
        if(mYear > other.mYear)
            return true;
        if(mYear < other.mYear)
            return false;
        if(mMonth > other.mMonth)
            return true;
        if(mMonth < other.mMonth)
            return false;
        return mDay >= other.mDay;
    }
    //Get the difference between two dates in days
    int operator-(Date &other)
    {
        int sum = 0;
        for(int i=min(mYear,other.mYear);i<max(mYear,other.mYear);i++){
            sum+=isLeap(i)?366:365;
        }
        int daysThis = getDaysPassedInYear();
        int daysThat = other.getDaysPassedInYear();
        sum+=max(daysThat,daysThis)-min(daysThis,daysThat)+1;
        return sum;
    }
    //Overloaded istream for used with cin
    friend std::istream& operator>>(std::istream &input,Date &date){
        std::cout<<"Enter Year:"<<std::endl;
        input>>date.mYear;
        int days[] = {31,(date.isLeap(date.mYear))?29:28,31,30,31,30,31,31,30,31,30,31};
        std::cout<<"Enter Month(1-12):"<<std::endl;
        input>>date.mMonth;
        if(date.mMonth<1 || date.mMonth > 12) {
            date.mMonth = 12;
            std::cout<<"Invalid Month - Resetting to 12"<<std::endl;
        }
        std::cout<<"Enter Day:"<<std::endl;
        input>>date.mDay;
        if(days[date.mMonth-1]<date.mDay || date.mDay < 0) {
            date.mDay = days[date.mMonth-1];
            std::cout<<"Invalid Day - Resetting to "<<date.mDay<<std::endl;
        }
        return input;
    }
    //Overloaded ostream for use with cout
    friend std::ostream &operator<<( std::ostream &output,const Date &date ) {
        std::string months[]={"JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"};
        output<<std::right<<std::setw(2)<<std::setfill('0')<<date.mDay<<"/"<<std::right<<std::setw(3)<<std::setfill(SPACE)<<months[date.mMonth-1]<<"/"<<std::right<<std::setw(4)<<std::setfill('0')<<date.mYear;
        return output;
    }
};

//Information pertaining to a room
class Room{
private:
    static int uid;
    int mTag;
    int mTotalCapacity;
    int mCost;
    RoomType mRoomType;
    bool mPrintBookings;
    //Information pertaining to booking status
    class BookingStatus{
    public:
        BookingStatus(Date checkin,Date checkout,int tag){
            mCheckinDate = checkin;
            mCheckoutDate = checkout;
            mCustomerTag = tag;
        }
        Date mCheckinDate;
        Date mCheckoutDate;
        int mCustomerTag;
    };
    //Check if dates are colliding while booking
    SmartPointer<DynamicArray<BookingStatus > > mBookingStatus;
    bool isColliding(Date checkin,Date checkout){
        for(int i=0;i<mBookingStatus->getLength();i++){
            SmartPointer<BookingStatus> status = mBookingStatus->getItem(i);
            if(checkin >= status->mCheckinDate && status->mCheckoutDate >= checkin)
                return true;
            if(checkout >= status->mCheckinDate && status->mCheckoutDate >= checkout)
                return true;
            if(status->mCheckinDate >= checkin && checkout >= status->mCheckoutDate)
                return true;
        }
        return false;
    }

    Position<CustomerData> getCustomer(int customerTag);
    SmartPointer<PositionalList<CustomerData> > mCustomers;
public:
    //Constructor
    Room():mTag(uid++){
        mBookingStatus = new DynamicArray<BookingStatus>();
        mPrintBookings = false;
    }
    bool isBooked(){
        return mBookingStatus->getLength() > 0 ;
    }
    Room(int capacity,int cost,RoomType roomType,SmartPointer<PositionalList<CustomerData> > customers):mTag(uid++){
        mTotalCapacity = capacity;
        mCost = cost;
        mRoomType = roomType;
        mBookingStatus = new DynamicArray<BookingStatus>();
        mCustomers = customers;
        mPrintBookings = false;
    }
    //Set the total capacity for the room
    void setTotalCapacity(int capacity){
        mTotalCapacity = capacity;
    }
    //Flag to alter what is printed
    void setPrint(bool status){
        mPrintBookings = status;
    }
    //Set the cost for the room
    void setCost(int cost){
        mCost = cost;
    }
    //Set the room type
    void setRoomType(RoomType roomType){
        mRoomType = roomType;
    }
    //Get the cost for the room
    int getCost(){
        return mCost;
    }
    //Get information on room type
    RoomType getRoomType(){
        return mRoomType;
    }
    //Get unique id for object allocated
    int getTag(){
        return mTag;
    }
    //Get total room capacity
    int getRoomCapacity(){
        return mTotalCapacity;
    }
    //Set the customers record
    void loadCustomerData(SmartPointer<PositionalList<CustomerData> > customers){
        mCustomers = customers;
    }
    //Add the booking information for this room
    bool addBooking(Date checkin,Date checkout,int customerTag,int hotelTag);
    //Cancel the booking for this room
    bool cancelBooking(int customerTag,int hotelTag);
    //Calculate the price from a target date
    int calculatePrice(Date targetDate){
        int sumPrice = 0;
        for(int i = 0;i<mBookingStatus->getLength();i++){
            SmartPointer<BookingStatus> status = mBookingStatus->getItem(i);
            if(!(targetDate>=status->mCheckoutDate)){
                Date startDate = status->mCheckinDate;
                if(targetDate >= status->mCheckinDate)
                    startDate = targetDate;
                sumPrice += (status->mCheckoutDate - startDate)*mCost;
            }
        }
        return sumPrice;
    }

    //Calculate the total number of checkins for the target date
    int calculateCheckins(Date targetDate){
        int checkins = 0;
        for(int i = 0;i<mBookingStatus->getLength();i++){
            SmartPointer<BookingStatus> status = mBookingStatus->getItem(i);
            if(!(targetDate>=status->mCheckoutDate)){
                checkins++;
            }
        }
        return checkins;
    }


    //Print the customer details
    void printCustomerDetails(std::ostream &output,int tag);
    //Print the booking details
    void printBookingDetails(std::ostream &output){
                output<<std::left<<std::setw(NAMEW)<<"Checkin Date"<<std::left<<std::setw(NAMEW)<<"Checkout Date"<<std::left<<std::setw(NAMEW)<<"First Name"<<std::left<<std::setw(NAMEW)<<"Last Name"<<std::left<<std::setw(NAMEW)<<"Tag"<<std::endl;
        for(int i = 0;i<mBookingStatus->getLength();i++){
            SmartPointer<BookingStatus> status = mBookingStatus->getItem(i);
            output<<status->mCheckinDate<<std::left<<std::setw(NAMEW-ADJUST)<<std::setfill(SPACE)<<" "<<std::left<<std::setw(NAMEW)<<std::setfill(SPACE)<<status->mCheckoutDate<<std::setw(NAMEW-ADJUST)<<std::setfill(SPACE)<<" ";
            printCustomerDetails(output, status->mCustomerTag);
        }
    }
    //Overloaded istream for use with cin
    friend std::istream& operator>>(std::istream &input,Room &room){
        int type;
        std::cout<<"Enter Room Capacity:"<<std::endl;
        input>>room.mTotalCapacity;
        std::cout<<"Enter Room Cost/Day($):"<<std::endl;
        input>>room.mCost;
        std::cout<<"Enter Room Type:"<<std::endl;
        std::cout<<"1) DELUXE"<<std::endl;
        std::cout<<"2) EXECUTIVE"<<std::endl;
        std::cout<<"3) FAMILY"<<std::endl;
        std::cout<<"4) COUPLES"<<std::endl;
        input>>type;
        switch (type) {
            case 1:
                room.mRoomType = DELUXE;
                break;
            case 2:
                room.mRoomType = EXECUTIVE;
                break;
            case 3:
                room.mRoomType = FAMILY;
                break;
            case 4:
                room.mRoomType = COUPLES;
                break;
            default:
                room.mRoomType = DELUXE;
                break;
        }
        return input;
    }
    //Overloaded ostream for use with cout
    friend std::ostream &operator<<( std::ostream &output,Room &room ) {
        std::string roomTypes[RTYPES] = {"DELUXE","EXECUTIVE","FAMILY","COUPLES"};

        if(room.isBooked() && room.mPrintBookings) {
            output<<std::left<<std::setw(NUMW)<<"Capacity"
            <<std::left<<std::setw(NUMW)<<"Cost/Day($)"<<std::left<<std::setw(NAMEW)<<"Room Type"<<std::left<<std::setw(NAMEW)<<"Bookings"<<std::endl;
            output<<std::left<<std::setw(NUMW)<<room.mTotalCapacity
            <<std::left<<std::setw(NUMW)<<room.mCost<<std::left<<std::setw(NAMEW)<<roomTypes[room.mRoomType]<<std::left<<std::setw(NAMEW)<<room.mBookingStatus->getLength()<<std::endl;
            room.printBookingDetails(output);
        } else {
            output<<std::left<<std::setw(NUMW)<<room.mTotalCapacity
            <<std::left<<std::setw(NUMW)<<room.mCost<<std::left<<std::setw(NAMEW)<<roomTypes[room.mRoomType]<<std::left<<std::setw(NAMEW)<<room.mBookingStatus->getLength()<<std::endl;
        }
        return output;
    }

    ~Room(){
    }

};
int Room::uid = 0;
//Hotel information
class Hotel{
private:
    static int uid;
    int mTag;
    std::string mHotelName;
    std::string mLocation;
    SmartPointer<DynamicArray<Room> > mRooms;
    bool mPrintRoom;
public:
    //Constructor
    Hotel():mTag(uid++){
        mPrintRoom = false;
        mRooms = new DynamicArray<Room>();
    }
    Hotel(std::string hotelName,std::string location):mTag(uid++){
        mHotelName=hotelName;
        mLocation=location;
        mRooms = new DynamicArray<Room>();
        mPrintRoom = false;
    }
    //Set hotel name
    void setHotelName(std::string hotelName){
        mHotelName = hotelName;
    }
    //Set location
    void setLocation(std::string location){
        mLocation = location;
    }
    //Check if the rooms in the hotel has bookings
    bool hasBookings(){
        bool flag = false;
        for(int i=0;i<mRooms->getLength();i++){
            if(mRooms->getItem(i)->isBooked()) {
                mRooms->getItem(i)->printBookingDetails(std::cout);
                flag = true;
            }
        }
        return flag;
    }
    //Get the hotel name
    std::string getHotelName(){
        return mHotelName;
    }
    //Get the hotel location
    std::string getLocation(){
        return mLocation;
    }
    //Add rooms to this hotel
    void addRoom(SmartPointer<Room> room){
        mRooms->append(room);
        room.addReference();
    }
    //Calculate sales from a given target date
    int calculatePrice(Date targetDate){
        int price = 0;
        for(int i=0;i<mRooms->getLength();i++){
            price += mRooms->getItem(i)->calculatePrice(targetDate);
        }
        return price;
    }

    //Calculate the total number of checkins for the target date
    int calculateCheckins(Date targetDate){
        int checkins = 0;
        for(int i=0;i<mRooms->getLength();i++){
            checkins += mRooms->getItem(i)->calculateCheckins(targetDate);
        }
        return checkins;
    }

    //Get a room for the given index
    SmartPointer<Room> getRoomAtIndex(int index){
        return mRooms->getItem(index);
    }
    //Get total number of rooms in this hotel
    int getNumberOfRooms(){
        return mRooms->getLength();
    }
    //Delete a room at a given index
    void deleteRoom(int index){
        mRooms->deleteItem(index);
    }
    //Alter what is printed
    void setPrintRooms(bool print){
        mPrintRoom = print;
    }
    //Get unique id corresponding to every object allocated
    int getTag(){
        return mTag;
    }
    //Overloaded ostream for use with cout
    friend std::ostream &operator<<( std::ostream &output,Hotel &hotel ) {
        output<<std::left<<std::setw(NAMEW)<<hotel.mHotelName<<std::left<<std::setw(NAMEW)<<hotel.mLocation<<std::endl;

        if(hotel.mPrintRoom && hotel.getNumberOfRooms() > 0){
            output<<std::left<<std::setw(NUMW)<<std::setfill(SPACE)<<"Index"<<std::left<<std::setw(NUMW)<<"Capacity"
            <<std::left<<std::setw(NUMW)<<"Cost/Day($)"<<std::left<<std::setw(NAMEW)<<"Room Type"<<std::left<<std::setw(NAMEW)<<"Bookings"<<std::endl;
            output<<hotel.mRooms;
        } else if(hotel.mPrintRoom && hotel.getNumberOfRooms() == 0) {
            std::cout<<"There are no rooms at this hotel"<<std::endl;
        }
        return output;
    }
    //Overloaded istream for use with cin
    friend std::istream& operator>>(std::istream &input,Hotel &hotel)
    {
        std::cout<<"Enter Hotel Name:"<<std::endl;
        input>>hotel.mHotelName;
        std::cout<<"Enter Hotel Location:"<<std::endl;
        input>>hotel.mLocation;
        return input;
    }
    //Destructor
    ~Hotel(){
        int size = mRooms->getLength();
        for(int i=0;i<size;i++){
            mRooms->deleteItem(0);
        }
    }
};
int Hotel::uid = 0;

class CustomerData{
private:
    static int uid;
    std::string mFirstName;
    std::string mLastName;
    int mBookedFor;
    int mTag;
    int mPrint;
    class Tag{
    public:
        int mHotelTag,mRoomTag;
        Tag(int hoteltag,int roomtag){
            mHotelTag = hoteltag;
            mRoomTag = roomtag;
        }
    };
    SmartPointer<DynamicArray<Tag > > mTags;
    //Search for a hotel with a specific tag
    SmartPointer<Hotel> searchHotel(int hotelTag,SmartPointer<DynamicArray<Hotel > > hotels){
        SmartPointer<Hotel> hotel,found;
        for(int i = 0;i<hotels->getLength();i++){
            hotel = hotels->getItem(i);
            if(hotel->getTag() == hotelTag){
                found = hotel;
            }
        }
        return found;
    }
    //Search for a room with a specific tag
    SmartPointer<Room> searchRoom(int roomTag,SmartPointer<Hotel> hotel){
        SmartPointer<Room> room,found;
        for(int i = 0;i<hotel->getNumberOfRooms();i++){
            room = hotel->getRoomAtIndex(i);
            if(room->getTag() == roomTag){
                found = room;
            }
        }
        return found;
    }
public:
    //Constructor
    CustomerData():mTag(uid++){
        mTags = new DynamicArray<Tag>();
        mPrint = false;
    }
    CustomerData(std::string firstName,std::string lastName):mTag(uid++){
        mFirstName = firstName;
        mLastName = lastName;
        mTags = new DynamicArray<Tag>();
        mPrint = false;
    }
    //Alter what is printed
    void setPrint(bool status){
        mPrint = status;
    }
    //Set the first name
    void setFirstName(std::string firstName){
        mFirstName = firstName;
    }
    //Set the last name
    void setLastName(std::string lastName){
        mLastName = lastName;
    }
    //Get the first name
    std::string getFirstName(){
        return mFirstName;
    }
    //Get the last name
    std::string getLastName(){
        return mLastName;
    }
    //Total people for which rooms are booked
    void setBookedFor(int no){
        mBookedFor = no;
    }
    //Get the booked information
    int getBookedFor(){
        return mBookedFor;
    }
    //Tag has a unique id for each object  allocated
    int getTag(){
        return mTag;
    }
    //Book a room
    void bookRoom(int hotelTag,int roomTag){
        mTags->append(new Tag(hotelTag,roomTag));
    }
    //Cancel booking
    void cancelBooking(int hotelTag,int roomTag){
        for (int i=0; i<mTags->getLength(); i++) {
            SmartPointer<Tag> tag = mTags->getItem(i);
            if(tag->mHotelTag == hotelTag && tag->mRoomTag == roomTag){
                mTags->deleteItem(i);
                break;
            }
        }
    }
    //Print rooms booked by this customer
    void printBookedRooms(SmartPointer<DynamicArray<Hotel > > hotels){
        std::cout<<"Booked:"<<std::endl;
        for (int i=0; i<mTags->getLength(); i++) {
            SmartPointer<Tag> tag = mTags->getItem(i);
            SmartPointer<Hotel> hotel = searchHotel(tag->mHotelTag, hotels);
            SmartPointer<Room> room = searchRoom(tag->mRoomTag, hotel);
            std::cout<<std::left<<std::setw(NAMEW)<<"Hotel Name"<<std::left<<std::setw(NAMEW)<<"Location"<<std::endl;
            std::cout<<hotel;
            std::cout<<std::left<<std::setw(NUMW)<<"Capacity"
            <<std::left<<std::setw(NUMW)<<"Cost/Day($)"<<std::left<<std::setw(NAMEW)<<"Room Type"<<std::left<<std::setw(NAMEW)<<"Bookings"<<std::endl;
            std::cout<<room;
        }
    }

        //Print rooms booked by this customer
    void printBookedRoomsOnHotel(SmartPointer<Hotel> hotel){
        std::cout<<"Booked:"<<std::endl;
        for (int i=0; i<mTags->getLength(); i++) {
            SmartPointer<Tag> tag = mTags->getItem(i);
            SmartPointer<Room> room = searchRoom(tag->mRoomTag, hotel);
            std::cout<<std::left<<std::setw(NAMEW)<<"Hotel Name"<<std::left<<std::setw(NAMEW)<<"Location"<<std::endl;
            std::cout<<hotel;
            std::cout<<std::left<<std::setw(NUMW)<<"Capacity"
            <<std::left<<std::setw(NUMW)<<"Cost/Day($)"<<std::left<<std::setw(NAMEW)<<"Room Type"<<std::left<<std::setw(NAMEW)<<"Bookings"<<std::endl;
            std::cout<<room;
        }
    }
    //Check if the customer has booked any rooms
    bool hasBooked(){
        return mTags->getLength()>0;
    }
    //Overloaded istream operator for use with cin
    friend std::istream& operator>>(std::istream &input,CustomerData &customer)
    {
        std::cout<<"Enter First Name:"<<std::endl;
        input>>customer.mFirstName;
        std::cout<<"Enter Last Name:"<<std::endl;
        input>>customer.mLastName;
        return input;
    }
    //Overloaded ostream operator for use with cout
    friend std::ostream &operator<<( std::ostream &output,const CustomerData &customerData ) {
        if(customerData.mPrint) {
        output<<std::left<<std::setw(NAMEW)<<std::setfill(SPACE)<<customerData.mFirstName<<std::left<<std::setw(NAMEW)<<std::setfill(SPACE)<<customerData.mLastName<<std::left<<std::setw(NAMEW)<<std::setfill(SPACE)<<customerData.mTag<<std::endl;
        }else {
        output<<std::left<<std::setw(NAMEW)<<std::setfill(SPACE)<<customerData.mFirstName<<std::left<<std::setw(NAMEW)<<std::setfill(SPACE)<<customerData.mLastName<<std::endl;
        }
        return output;
    }
    ~CustomerData(){
    }
};
int CustomerData::uid = 0;


void Room::printCustomerDetails(std::ostream &output,int tag){
    for(int i = 0;i<mCustomers->getLength();i++){
        Position<CustomerData> data = mCustomers->getItemAtIndex(i);
        if(data.getElement()->getTag() == tag){
            data.getElement()->setPrint(true);
            std::cout<<data;
            data.getElement()->setPrint(false);
        }
    }
}

Position<CustomerData> Room::getCustomer(int customerTag){
    Position<CustomerData> data;
    for (int i=0; i<mCustomers->getLength(); i++) {
        data = mCustomers->getItemAtIndex(i);
        if(data.getElement()->getTag() == customerTag)
            return data;
    }
    return data;
}

bool Room::addBooking(Date checkin,Date checkout,int customerTag,int hotelTag){
    if(!isColliding(checkin, checkout)) {
        mBookingStatus->append(new BookingStatus(checkin,checkout,customerTag));
        Position<CustomerData> customer = getCustomer(customerTag);
        int currentBookings = customer.getElement()->getBookedFor();
        currentBookings += getRoomCapacity();
        customer.getElement()->setBookedFor(currentBookings);
        customer.getElement()->bookRoom(hotelTag,getTag());
        return true;
    }
    return false;
}

bool Room::cancelBooking(int customerTag,int hotelTag){
    bool flag = false;
    for(int i=0;i<mBookingStatus->getLength();i++){
        SmartPointer<BookingStatus> status = mBookingStatus->getItem(i);
        if(status->mCustomerTag == customerTag){
            Position<CustomerData> customer = getCustomer(customerTag);
            int currentBookings = customer.getElement()->getBookedFor();
            currentBookings -= getRoomCapacity();
            customer.getElement()->cancelBooking(hotelTag, getTag());
            customer.getElement()->setBookedFor(currentBookings);
            mBookingStatus->deleteItem(i);
            flag = true;
        }
    }
    if(!flag)
        std::cout<<"Cancel Failed!"<<std::endl;
    return flag;
}

//Predefined Data set for hotels
void loadHotels( SmartPointer<DynamicArray<Hotel > > hotels, SmartPointer<PositionalList<CustomerData> > customers){
    SmartPointer<Hotel> hotel1(new Hotel("AAA","111"));
    Room *room11 = new Room(3,400,DELUXE,customers);
    room11->addBooking(Date(22,1,2014), Date(23,1,2014), customers->getItemAtIndex(0).getElement()->getTag(),hotel1->getTag());
    room11->addBooking(Date(24,1,2014), Date(25,1,2014), customers->getItemAtIndex(1).getElement()->getTag(),hotel1->getTag());
    room11->addBooking(Date(26,1,2014), Date(27,1,2014), customers->getItemAtIndex(0).getElement()->getTag(),hotel1->getTag());
    hotel1->addRoom(room11);
    hotel1->addRoom((new Room(3,400,DELUXE,customers)));
    hotel1->addRoom((new Room(1,500,EXECUTIVE,customers)));
    hotel1->addRoom((new Room(1,500,EXECUTIVE,customers)));
    hotel1->addRoom((new Room(4,600,FAMILY,customers)));
    hotel1->addRoom((new Room(4,600,FAMILY,customers)));
    hotel1->addRoom(SmartPointer<Room>(new Room(2,550,COUPLES,customers)));
    Room *room18 = new Room(2,550,COUPLES,customers);
    room18->addBooking(Date(22,1,2014), Date(23,1,2014), customers->getItemAtIndex(0).getElement()->getTag(),hotel1->getTag());
    room18->addBooking(Date(24,1,2014), Date(25,1,2014), customers->getItemAtIndex(1).getElement()->getTag(),hotel1->getTag());
    hotel1->addRoom(room18);
    hotels->append(hotel1);


    SmartPointer<Hotel> hotel2(new Hotel("BBB","222"));
    hotel2->addRoom(SmartPointer<Room>(new Room(3,400,DELUXE,customers)));
    hotel2->addRoom(SmartPointer<Room>(new Room(3,400,DELUXE,customers)));
    hotel2->addRoom(SmartPointer<Room>(new Room(1,500,EXECUTIVE,customers)));
    hotel2->addRoom(SmartPointer<Room>(new Room(1,500,EXECUTIVE,customers)));
    hotel2->addRoom(SmartPointer<Room>(new Room(4,600,FAMILY,customers)));
    hotel2->addRoom(SmartPointer<Room>(new Room(4,600,FAMILY,customers)));
    hotel2->addRoom(SmartPointer<Room>(new Room(2,550,COUPLES,customers)));
    hotel2->addRoom(SmartPointer<Room>(new Room(2,550,COUPLES,customers)));
    hotels->append(hotel2);

    SmartPointer<Hotel> hotel3(new Hotel("CCC","333"));
    hotel3->addRoom(SmartPointer<Room>(new Room(3,400,DELUXE,customers)));
    hotel3->addRoom(SmartPointer<Room>(new Room(3,400,DELUXE,customers)));
    hotel3->addRoom(SmartPointer<Room>(new Room(1,500,EXECUTIVE,customers)));
    hotel3->addRoom(SmartPointer<Room>(new Room(1,500,EXECUTIVE,customers)));
    hotel3->addRoom(SmartPointer<Room>(new Room(4,600,FAMILY,customers)));
    hotel3->addRoom(SmartPointer<Room>(new Room(4,600,FAMILY,customers)));
    hotel3->addRoom(SmartPointer<Room>(new Room(2,550,COUPLES,customers)));
    hotel3->addRoom(SmartPointer<Room>(new Room(2,550,COUPLES,customers)));
    hotels->append(hotel3);

    SmartPointer<Hotel> hotel4(new Hotel("DDD","444"));
    hotel4->addRoom(SmartPointer<Room>(new Room(3,400,DELUXE,customers)));
    hotel4->addRoom(SmartPointer<Room>(new Room(3,400,DELUXE,customers)));
    hotel4->addRoom(SmartPointer<Room>(new Room(1,500,EXECUTIVE,customers)));
    hotel4->addRoom(SmartPointer<Room>(new Room(1,500,EXECUTIVE,customers)));
    hotel4->addRoom(SmartPointer<Room>(new Room(4,600,FAMILY,customers)));
    hotel4->addRoom(SmartPointer<Room>(new Room(4,600,FAMILY,customers)));
    hotel4->addRoom(SmartPointer<Room>(new Room(2,550,COUPLES,customers)));
    hotel4->addRoom(SmartPointer<Room>(new Room(2,550,COUPLES,customers)));
    hotels->append(hotel4);

    SmartPointer<Hotel> hotel5(new Hotel("EEE","555"));
    hotel5->addRoom(SmartPointer<Room>(new Room(3,400,DELUXE,customers)));
    hotel5->addRoom(SmartPointer<Room>(new Room(3,400,DELUXE,customers)));
    hotel5->addRoom(SmartPointer<Room>(new Room(1,500,EXECUTIVE,customers)));
    hotel5->addRoom(SmartPointer<Room>(new Room(1,500,EXECUTIVE,customers)));
    hotel5->addRoom(SmartPointer<Room>(new Room(4,600,FAMILY,customers)));
    hotel5->addRoom(SmartPointer<Room>(new Room(4,600,FAMILY,customers)));
    hotel5->addRoom(SmartPointer<Room>(new Room(2,550,COUPLES,customers)));
    hotel5->addRoom(SmartPointer<Room>(new Room(2,550,COUPLES,customers)));
    hotels->append(hotel5);

}

//Predefined Data set for rooms
void loadCustomerData(SmartPointer<PositionalList<CustomerData> > customers){
    customers->addFirst(new CustomerData("Bob","Wen"));
    customers->addFirst(new CustomerData("Liu","Bon"));
    customers->addFirst(new CustomerData("Ver","Kim"));
    customers->addFirst(new CustomerData("Jin","Jan"));
    customers->addFirst(new CustomerData("Val","Ver"));
    customers->addFirst(new CustomerData("Zar","Wen"));
    customers->addFirst(new CustomerData("Alp","Cri"));
    customers->addFirst(new CustomerData("Zep","Muu"));
    customers->addFirst(new CustomerData("Car","Luk"));
    customers->addFirst(new CustomerData("Neo","Mac"));
    customers->addFirst(new CustomerData("Cam","Cas"));
    customers->addFirst(new CustomerData("Bam","Zam"));
    customers->addFirst(new CustomerData("Pre","Laz"));
    customers->addFirst(new CustomerData("Pam","Cen"));
    customers->addFirst(new CustomerData("Iqu","Poi"));
    customers->addFirst(new CustomerData("Zam","Ren"));
    customers->addFirst(new CustomerData("Pla","Zok"));
    customers->addFirst(new CustomerData("Qzo","Chi"));
}

//Interface for Task's
class Task{
public:
    //Show the menu for each task
    virtual void showMenu() = 0;
    //Perform the operations inside the task
    virtual Task* doTask(int n)=0;
    //Feed hotel data
    virtual void loadHotelData(SmartPointer<DynamicArray<Hotel > > hotels){
        std::cout<<"Sorry no data to load"<<std::endl;
    }
    //Feed customer data
    virtual void loadCustomerData(SmartPointer<PositionalList<CustomerData> > customers){
        std::cout<<"Sorry no data to load"<<std::endl;
    }
    //Print hotel data
    virtual void printHotelData(){
        std::cout<<"Sorry there are no hotel data"<<std::endl;
    }
    //Print customer data
    virtual void printCustomerData(){
        std::cout<<"Sorry there are no customer data"<<std::endl;
    }
    //Check if the task is the last
    virtual bool isLast(){
        return false;
    }
    //Clearscreen can be written corresponding to the operaing system
    virtual void clearScreen(){
    }
    //Virtual destructor, since this is an abstract class
    virtual ~Task(){

    }
};

//Task to exit the screen
class ExitTask: public Task{
public:
    void showMenu(){
        clearScreen();
        std::cout<<"Have a nice day!"<<std::endl;
    }
    Task* doTask(int n){
        return NULL;
    }
    bool isLast(){
        return true;
    }
};
class HotelTask;
//Task to handle customer information
class CustomerTask: public Task{
    SmartPointer<DynamicArray<Hotel > > mHotels;
    SmartPointer<PositionalList<CustomerData> > mCustomers;
public:
    CustomerTask(SmartPointer<DynamicArray<Hotel > > hotels,SmartPointer<PositionalList<CustomerData> > customers):mHotels(hotels),mCustomers(customers){
    }

    void showMenu(){
        clearScreen();
        //std::cout<<std::left<<std::setw(NUMW)<<"Index"<<std::left<<std::setw(NAMEW)<<"First Name"<<std::left<<std::setw(NAMEW)<<"Last Name"<<std::endl;
        //std::cout<<mCustomers;
        std::cout<<"1) Registration Form>"<<std::endl;
        std::cout<<"2) Update Your Details>"<<std::endl;//, currently there are ("<<mCustomers->getLength()<<") records"<<">"<<std::endl;
        std::cout<<"3) Cancel Registration Details>"<<std::endl;//, currently there are ("<<mCustomers->getLength()<<") records"<<">"<<std::endl;
        std::cout<<"4) Admin user, List all customer Details>"<<std::endl;//, currently there are ("<<mCustomers->getLength()<<") records"<<">"<<std::endl;
        std::cout<<"0) <Previous Screen"<<std::endl;
    }
    int validateCustomerInput(int rid){
        for(int i=0;i<mCustomers->getLength();i++){
            Position<CustomerData> data = mCustomers->getItemAtIndex(i);
            if(data.getElement()->getTag() == rid){
                return i;
            }
        }
        std::cout<<"Invalid customer id!"<<std::endl;
        return -1;
    }

    void showUpdateDetails(Position<CustomerData> customer){
        clearScreen();
        std::cout<<"Specify the details to update>"<<std::endl;
        std::cout<<"1) First Name("<<customer.getElement()->getFirstName()<<")>"<<std::endl;
        std::cout<<"2) Last Name("<<customer.getElement()->getLastName()<<")>"<<std::endl;
    }
    Task* doTask(int n);
};

//Task to handle room information
class RoomTask: public Task{
    SmartPointer<Hotel> mHotel;
    SmartPointer<PositionalList<CustomerData> > mCustomers;
    int mId;
public:
    RoomTask(SmartPointer<Hotel> hotel,SmartPointer<PositionalList<CustomerData> > customers):mHotel(hotel),mCustomers(customers){
        showHotelDetails();
    }
    void showHotelDetails() {
        std::cout<<std::left<<std::setw(NAMEW)<<"Hotel Name"<<std::left<<std::setw(NAMEW)<<"Location"<<std::endl;
        mHotel->setPrintRooms(true);
        std::cout<<mHotel;
        mHotel->setPrintRooms(false);
    }
    void showMenu(){
        clearScreen();
        std::cout<<"1) Book Room>"<<std::endl;
        std::cout<<"2) Show Booked Rooms(Admin)>"<<std::endl;
        std::cout<<"3) Show Free Rooms>"<<std::endl;
        std::cout<<"4) Cancel Booking>"<<std::endl;
        std::cout<<"5) Sales From Target Date>"<<std::endl;
        std::cout<<"6) Add Room>"<<std::endl;
        std::cout<<"7) Delete Room>"<<std::endl;
        std::cout<<"0) <Previous Screen"<<std::endl;
    }
    void setCustomerId(int id){
        mId = id;
    }
    bool validateRoomInput(int n){
        if(n>=1 && n <= mHotel->getNumberOfRooms()){
            return true;
        }
        std::cout<<"Index out of range!"<<std::endl;
        return false;
    }
    int validateCustomerInput(int rid){
        for(int i=0;i<mCustomers->getLength();i++){
            Position<CustomerData> data = mCustomers->getItemAtIndex(i);
            if(data.getElement()->getTag() == rid){
                return i;
            }
        }
        std::cout<<"Invalid customer id!"<<std::endl;
        return -1;
    }
    Task* doTask(int n){
        switch (n) {
            case 1:
                {
                    if(mHotel->getNumberOfRooms() > 0){
                        int index;
                        std::cout<<"(1-"<<mHotel->getNumberOfRooms()<<") Enter between range to book room:"<<std::endl;
                        std::cin>>index;
                        if(validateRoomInput(index)) {
                            SmartPointer<Room> room = mHotel->getRoomAtIndex(index-1);
                            int index = validateCustomerInput(mId);
                            if(index >= 0){
                                Position<CustomerData> customer = mCustomers->getItemAtIndex(index-1);
                                Date checkinDate,checkoutDate;
                                std::cout<<"Enter Checkin Date:"<<std::endl;
                                std::cin>>checkinDate;
                                std::cout<<"Enter Checkout Date:"<<std::endl;
                                std::cin>>checkoutDate;
                                if(!(checkoutDate >= checkinDate)){
                                    std::cout<<"Invalid Dates!"<<std::endl;
                                    return this;
                                }
                                if(!room->addBooking(checkinDate, checkoutDate, customer.getElement()->getTag(),mHotel->getTag())){
                                    std::cout<<"Room is already booked!"<<std::endl;
                                    return this;
                                }
                                std::cout<<"Room Booked!"<<std::endl;
                            } else {
                                std::cout<<"Unable to book a room!"<<std::endl;
                            }
                        }
                    }
                }
                return this;
                break;
            case 2:
                {
                    if(mHotel->getNumberOfRooms() > 0){
                        SmartPointer<Room> room;
                        std::cout<<"Booked Rooms:"<<std::endl;
                        for(int i=0;i<mHotel->getNumberOfRooms();i++){
                            room = mHotel->getRoomAtIndex(i);
                            if(room->isBooked()){
                                room->setPrint(true);
                                std::cout<<room;
                                room->setPrint(false);
                            }
                        }
                    }
                }
                return this;
                break;
            case 3:
                {
                    if(mHotel->getNumberOfRooms() > 0){
                        SmartPointer<Room> room;
                        std::cout<<"Free Rooms:"<<std::endl;
                        std::cout<<std::left<<std::setw(NUMW)<<"Capacity"
                        <<std::left<<std::setw(NUMW)<<"Cost/Day($)"<<std::left<<std::setw(NAMEW)<<"Room Type"<<std::left<<std::setw(NAMEW)<<"Bookings"<<std::endl;
                        for(int i=0;i<mHotel->getNumberOfRooms();i++){
                            room = mHotel->getRoomAtIndex(i);
                            if(!room->isBooked()){
                                std::cout<<room;
                            }
                        }
                    }
                }
                return this;
                break;
            case 4:
                {
                    if(mHotel->getNumberOfRooms() > 0){
                        int index;
                        std::cout<<"(1-"<<mHotel->getNumberOfRooms()<<") Enter between range to cancel booking:"<<std::endl;
                        std::cin>>index;
                        if(validateRoomInput(index)) {
                            SmartPointer<Room> room = mHotel->getRoomAtIndex(index-1);
                            if(!room->isBooked()){
                                std::cout<<"Room is already free!"<<std::endl;
                                break;
                            }
                            if(room->isBooked()){
                                std::cout<<"Booked Rooms:"<<std::endl;
                                room->setPrint(true);
                                std::cout<<room;
                                room->setPrint(false);
                            } else {
                                std::cout<<"There are no bookings for this room"<<std::endl;
                                return this;
                            }
                            int tag;

                            int index = validateCustomerInput(mId);
                            if(index >= 0){
                                Position<CustomerData> customer = mCustomers->getItemAtIndex(index-1);
                                if(room->cancelBooking(customer.getElement()->getTag(),mHotel->getTag()))
                                    std::cout<<"Room is cancelled!"<<std::endl;
                            }
                        }
                    }
                }
                return this;
                break;
            case 5:
                {
                    Date targetDate;
                    std::cout<<"Enter date after which to calculate sates target:"<<std::endl;
                    std::cin>>targetDate;
                    std::cout<<"Earned:"<<std::endl;
                    std::cout<<mHotel->calculatePrice(targetDate)<<std::endl;
                    std::cout<<"Total checkins from target date:"<<std::endl;
                    std::cout<<mHotel->calculateCheckins(targetDate)<<std::endl;
                }
                return this;
                break;
            case 6:
                {
                    SmartPointer<Room> room;
                    Room *newRoom = new Room();
                    std::cin>>*newRoom;
                    room = newRoom;
                    room->loadCustomerData(mCustomers);
                    mHotel->addRoom(room);
                    return this;
                }
            case 7:
                {
                    if(mHotel->getNumberOfRooms() > 0){
                        int index;
                        std::cout<<"(1-"<<mHotel->getNumberOfRooms()<<") Enter between range to cancel booking:"<<std::endl;
                        std::cin>>index;
                        if(validateRoomInput(index)) {
                            SmartPointer<Room> room = mHotel->getRoomAtIndex(index-1);
                            if(room->isBooked()) {
                                std::cout<<"Room is booked, clear bookings & try again."<<std::endl;
                            } else {
                                mHotel->deleteRoom(index-1);
                            }
                        }
                    }
                    return this;
                }
                break;
            default:
                break;
        }

        return NULL;
    }
};

//Task to handle hotel information
class HotelTask: public Task{
    SmartPointer<DynamicArray<Hotel > > mHotels;
    SmartPointer<PositionalList<CustomerData> > mCustomers;
    int mId;
public:
    HotelTask(SmartPointer<DynamicArray<Hotel > > hotels,SmartPointer<PositionalList<CustomerData> > customers):mHotels(hotels),mCustomers(customers){
    }
    void showMenu(){
        clearScreen();
        std::cout<<std::left<<std::setw(NUMW)<<"Index"<<std::left<<std::setw(NAMEW)<<"Hotel Name"<<std::left<<std::setw(NAMEW)<<"Location"<<std::setfill(SPACE)<<std::endl<<std::setfill(SPACE);
        std::cout<<mHotels;
        std::cout<<"1) List Hotel Details, currently there are ("<<mHotels->getLength()<<") hotels"<<">"<<std::endl;
        std::cout<<"2) Delete Hotel, currently there are ("<<mHotels->getLength()<<") hotels"<<">"<<std::endl;
        std::cout<<"3) Add Hotel>"<<std::endl;
        std::cout<<"0) Previous Screen>"<<std::endl;
    }
    bool validateHotelInput(int n){
        if(n>=1 && n <= mHotels->getLength()){
            return true;
        }
        std::cout<<"Index out of range!"<<std::endl;
        return false;
    }
    void setCustomerId(int id){
        mId = id;
    }
    Task* doTask(int n){
        switch (n) {
            case 1:
                if(mHotels->getLength() > 0){
                    int index;
                    std::cout<<"(1-"<<mHotels->getLength()<<") Enter between range to list details:"<<std::endl;
                    std::cin>>index;
                    if(validateHotelInput(index)){
                        RoomTask *task = new RoomTask(mHotels->getItem(index-1),mCustomers);
                        task->setCustomerId(mId);
                        return task;
                    }
                }
                return this;
                break;
            case 2:
                if(mHotels->getLength() > 0){
                    int index;
                    std::cout<<"(1-"<<mHotels->getLength()<<") Enter between range to delete hotel:"<<std::endl;
                    std::cin>>index;
                    if(validateHotelInput(index)) {
                        if(!mHotels->getItem(index-1)->hasBookings()) {
                        mHotels->deleteItem(index-1);
                        } else {
                            std::cout<<"The hotel has bookings. First clear the bookings and then delete."<<std::endl;
                        }
                    }
                }
                return this;
                break;
            case 3:
                {
                    SmartPointer<Hotel> hotel;
                    Hotel *newHotel = new Hotel();
                    std::cin>>*newHotel;
                    hotel = newHotel;
                    hotel.addReference();
                    mHotels->append(hotel);
                    return this;
                }
                break;
            default:
                return NULL;
        }
        return NULL;
    }
};

Task* CustomerTask::doTask(int n){
        switch (n) {
            case 1:
                {
                    SmartPointer<CustomerData> customer;
                    CustomerData *newCustomer = new CustomerData();
                    std::cin>>*newCustomer;
                    customer = newCustomer;
                    customer.addReference();
                    mCustomers->addLast(customer);
                    std::cout<<"Please save this booking id for future use:"<<customer->getTag()<<std::endl;
                    int n;
                    std::cout<<"Do you want to book a room now?(0-No/1-Yes)"<<std::endl;
                    std::cin>>n;
                    if(n == 0)
                        return this;
                    else
                    {
                        HotelTask *task = new HotelTask(mHotels,mCustomers);
                        task->setCustomerId(customer->getTag());
                        return task;
                    }
                }
                break;
            case 2:
                {
                    if(mCustomers->getLength() > 0){
                        int rid;
                        std::cout<<"Enter your registration id:"<<std::endl;
                        std::cin>>rid;
                        int index = validateCustomerInput(rid);
                        if(index >= 0){
                            Position<CustomerData> customer = mCustomers->getItemAtIndex(index);
                            showUpdateDetails(customer);
                            int choice;
                            std::cin>>choice;
                            switch (choice) {
                                case 1:
                                    {
                                        std::string fname;
                                        std::cout<<"Enter New First Name>"<<std::endl;
                                        std::cin>>fname;
                                        customer.getElement()->setFirstName(fname);
                                    }
                                    break;
                                case 2:
                                    {
                                        std::string lname;
                                        std::cout<<"Enter New Last Name>"<<std::endl;
                                        std::cin>>lname;
                                        customer.getElement()->setLastName(lname);
                                    }
                                    break;
                                default:
                                    std::cout<<"Invalid choice"<<std::endl;
                                    break;
                            }
                        }
                    }
                    return this;
                }
                break;
            case 3:
                {
                    if(mCustomers->getLength() > 0){
                        int rid;
                        std::cout<<"Enter your registration id>"<<std::endl;
                        std::cin>>rid;
                        int index = validateCustomerInput(rid);
                        if(index>=0){
                            Position<CustomerData> data = mCustomers->getItemAtIndex(index);
                            if(data.getElement()->hasBooked()){
                                std::cout<<"The customer has bookings>"<<std::endl;
                                data.getElement()->printBookedRooms(mHotels);
                                std::cout<<"Please cancel bookings and try again."<<std::endl;
                            } else {
                                mCustomers->deleteItem(data);
                                std::cout<<"Customer record deleted!"<<std::endl;
                            }
                        }
                    }
                    return this;
                }
                break;
            case 4:
                {
                    std::cout<<std::left<<std::setw(NUMW)<<"Index"<<std::left<<std::setw(NAMEW)<<"First Name"<<std::left<<std::setw(NAMEW)<<"Last Name"<<std::endl;
                    std::cout<<mCustomers;
                    return this;
                }
                break;
            default:
                break;
        }
        return NULL;
    }
//Main task serves as a navigation point for subtask's
class MainTask: public Task{
    SmartPointer<DynamicArray<Hotel > > mHotels;
    SmartPointer<PositionalList<CustomerData> > mCustomers;
public:
    MainTask(SmartPointer<DynamicArray<Hotel > > hotels,SmartPointer<PositionalList<CustomerData> > customers):mHotels(hotels),mCustomers(customers){
    }
    int validateCustomerInput(int rid){
        for(int i=0;i<mCustomers->getLength();i++){
            Position<CustomerData> data = mCustomers->getItemAtIndex(i);
            if(data.getElement()->getTag() == rid){
                return i;
            }
        }
        std::cout<<"Invalid customer id!"<<std::endl;
        return -1;
    }
    void showMenu(){
        clearScreen();
        std::cout<<"1) Register User>"<<std::endl;
        std::cout<<"2) View Hotels To Book>"<<std::endl;
        std::cout<<"0) <Exit System:"<<std::endl;
    }
    Task* doTask(int n){
        switch (n) {
            case 2:
                {
                    int rid;
                    std::cout<<"Enter your customer-id>"<<std::endl;
                    std::cin>>rid;
                    if(rid >= 0 && validateCustomerInput(rid)>=0) {
                        HotelTask *task = new HotelTask(mHotels,mCustomers);
                        task->setCustomerId(rid);
                        return task;
                    }
                    return this;
                }
                break;
            case 1:
                return new CustomerTask(mHotels,mCustomers);
            default:
                return NULL;
        }
    }
    void loadHotelData(SmartPointer<DynamicArray<Hotel > > hotels) {
        mHotels = hotels;
    }
    void loadCustomerData(SmartPointer<PositionalList<CustomerData> > customers) {
        mCustomers = customers;
    }

};

int main(int argc, const char * argv[]) {
    SmartPointer<DynamicArray<Hotel > > hotels(new DynamicArray<Hotel >());
    SmartPointer<PositionalList<CustomerData> > customers(new PositionalList<CustomerData>());
    SmartPointer<Stack<Task> > taskList(new Stack<Task>());
    loadCustomerData(customers);
    loadHotels(hotels,customers);
    taskList->push(new ExitTask());
    taskList->push(new MainTask(hotels,customers));
    int n;
    do{
        Position<Task> task = taskList->top();
        task.getElement()->showMenu();
        if(task.getElement()->isLast())
            break;
        std::cin>>n;
        Task *mainTask = task.getElement()->doTask(n);
        if(mainTask == NULL){
            taskList->pop();
        } else if(mainTask != task.getElement().getData()){
            taskList->push(mainTask);
        }
    }while (taskList->getLength() != 0);
    return 0;
}
