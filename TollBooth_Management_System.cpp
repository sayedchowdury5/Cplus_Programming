/*
*************** Toll Booth Management System *************
          TMF 1434- DATA STRUCTURE AND ALGORITHM
                           PROJECT
======================= Group Members ====================
1. ABU SAYED                                       - 59395
2. MUHAMMAD HASSIF IMRAN BIN MUHAMMAD HARRIS IRWAN - 58818
3. MUHAMMAD FAREZ BIN SAID                         - 59727
4. MUHAMMAD ZULFIQAR BIN RAZALI                    - 59745

****************** DUE DATE - 24 MAY, 2017 ***************
*/

// Preprocessor directives or header files
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <limits>
#include <sstream>
#include <map>
#include <ctime>
#include <vector>
#include <algorithm>

// Constant variables declaration
#define RTYPES 4
#define SPACE ' '
#define UNDERLINE '-'
#define NAMEW 20
#define ADJUST 11
#define NUMW NAMEW
#define BOOLW  10
#define BSTATUS 2
#define SUCCESS 0
#define ERROR 1
#define MAX 10000
#define MIN 1000


/* This is an error recovery routine which recovers when you have typed an illegal character
 where it is expecting a number.
 */
void inputNumber(int &data) {
	std::cin >> data;
	while (std::cin.fail()) {
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Invalid Input Expected Number" << std::endl;
		std::cin >> data;
	}
}

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
		if (mReference->releaseReference() == 0)
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
			if (mReference->releaseReference() == 0)
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
	T* getData() {
		return mData;
	}
	//Overloaded ostream for use with cout
	friend std::ostream &operator<<(std::ostream &output, const SmartPointer &sp) {
		return output << *(sp.mData);
	}
};

//Array that dynamically resizes it's capacity
template<class T>
class DynamicArray {
private:
	int mCount;
	int mCapacity;
	T **mArray;

	//To calclulate the new size of the array
	void resize(int capacity) {
		T **mTempArray = new T*[capacity];
		mCapacity = capacity;
		for (int i = 0; i<mCount; i++) {
			mTempArray[i] = mArray[i];
		}
		mArray = mTempArray;
	}

public:
	//Constructor
	DynamicArray() {
		mCount = 0;
		mCapacity = 2;
		mArray = new T*[mCapacity];
	}
	//Destructor
	~DynamicArray() {
	}
	//Get the number of items in this array
	int getLength() const {
		return mCount;
	}
	void setItem(SmartPointer<T> data, int index) {
		mArray[index] = data.getData();
	}
	//Get item at index
	SmartPointer<T> getItem(int index) {
		if (!(index >= 0 && index<mCount)) {
			//error
		}
		SmartPointer<T> item(mArray[index]);
		item.addReference();
		return item;
	}
	//Delete item at index
	void deleteItem(int index) {
		SmartPointer<T> temp = mArray[index];
		for (int i = index + 1; i<mCount; i++) {
			mArray[i - 1] = mArray[i];
		}
		mCount--;
		if (mCount == mCapacity / 4) {
			resize(mCapacity / 2);
		}
	}
	//Append an item at the last
	void append(SmartPointer<T> item) {
		item.addReference();
		if (mCount == mCapacity) {
			resize(mCapacity * 2);
		}
		mArray[mCount] = item.getData();
		mCount++;
	}
	//Overloaded ostream for use with cout
	friend std::ostream &operator<<(std::ostream &output, const DynamicArray &dArray) {
		for (int i = 0; i<dArray.mCount; i++) {
			output << std::left << std::setw(NUMW) << std::setfill(SPACE) << i + 1 << *(dArray.mArray[i]);
		}
		return output;
	}



};

template <class T, class V>
SmartPointer<T> binarySearch(SmartPointer<DynamicArray<T > > data, V &search, int(*compare)(SmartPointer<T> a, V &b)) {
	int first = 0;
	int last = data->getLength() - 1;
	int middle = (first + last) / 2;
	while (first <= last)
	{
		if (compare(data->getItem(middle), search)<0)
			first = middle + 1;
		else if (compare(data->getItem(middle), search)>0)
			last = middle - 1;
		else
			return data->getItem(middle);
		middle = (first + last) / 2;
	}
	return NULL;
}


template <class T>
void sort(SmartPointer<DynamicArray<T > > data, int(*compare)(SmartPointer<T> a, SmartPointer<T> b)) {
	for (int i = 1, j = 0; i<data->getLength(); i++)
	{
		SmartPointer<T> tempA = data->getItem(i);
		tempA.addReference();
		j = i - 1;
		while (j >= 0)
		{
			SmartPointer<T> tempB = data->getItem(j);
			tempB.addReference();
			if (!(compare(tempA, tempB)<0)) {
				break;
			}
			data->setItem(tempB, j + 1);
			tempB.releaseReference();
			j = j - 1;
		}
		data->setItem(tempA, j + 1);
		tempA.releaseReference();
	}
}

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
class DoubleLinkedBase {
public:
	//Data structure for double linked list
	class Node {
	public:
		SmartPointer<T> mElement;
		Node *mPrevious;
		Node *mNext;
		Node(SmartPointer<T> element, Node *previous, Node *next) {
			mElement = element;
			mPrevious = previous;
			mNext = next;
		}

		~Node() {
		}
		friend class Position<T>;
	};
	Node *mHeader;
	Node *mTrailer;
	int mSize;
	//Constructor
	DoubleLinkedBase() {
		mHeader = new Node(NULL, NULL, NULL);
		mTrailer = new Node(NULL, NULL, NULL);
		mHeader->mNext = mTrailer;
		mTrailer->mPrevious = mHeader;
		mSize = 0;
	}
	//Get the size of the list
	int getLength() {
		return mSize;
	}
	//Check if list is empty
	bool isEmpty() {
		return mSize == 0;
	}
	//Insert data between a predecessor and successor
	Node* insertBetween(SmartPointer<T> element, Node *predecessor, Node *successor) {
		Node *newest = new Node(element, predecessor, successor);
		predecessor->mNext = newest;
		successor->mPrevious = newest;
		mSize += 1;
		return newest;
	}
	//Delete the given node
	void deleteNode(Node *node) {
		Node *predecessor = node->mPrevious;
		Node *successor = node->mNext;
		predecessor->mNext = successor;
		successor->mPrevious = predecessor;
		//delete node;
		mSize -= 1;
	}
	//Destructor
	~DoubleLinkedBase() {
		Node *header = mHeader->mNext;
		while (mHeader != mTrailer->mPrevious) {
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
class Position {
private:
	PositionalList<T> *mContainer;
	typename DoubleLinkedBase<T>::Node *mNode;
public:
	friend class PositionalList<T>;
	//Constructor
	Position() {
		mContainer = NULL;
		mNode = NULL;
	}
	Position(const Position &p) {
		mContainer = p.mContainer;
		mNode = p.mNode;
	}
	Position(PositionalList<T> *list, typename DoubleLinkedBase<T>::Node *node) {
		mContainer = list;
		mNode = node;
	}
	//Overloaded equals operator
	void operator=(const Position &p) {
		mContainer = p.mContainer;
		mNode = p.mNode;
	}
	//Get the element corresponding to this Position
	SmartPointer<T> getElement() {
		return mNode->mElement;
	}
	//Overloaded ostream for use with cout
	friend std::ostream &operator<<(std::ostream &output, Position &pList) {
		output << pList.mNode->mElement;
		return output;
	}
	friend class PositionalListIteraor<T>;
};


template<class T>
class PositionalList : DoubleLinkedBase<T> {
private:
	//Get the header node
	typename DoubleLinkedBase<T>::Node * getHeader() {
		return DoubleLinkedBase<T>::mHeader;
	}
	//Get the trailer node
	typename DoubleLinkedBase<T>::Node * getTrailer() {
		return DoubleLinkedBase<T>::mTrailer;
	}
public:
	//Get the length of the positional list
	int getLength() {
		return DoubleLinkedBase<T>::getLength();
	}
	//Get the first element corresponding to the list
	Position<T> getFirst() {
		return Position<T>(this, DoubleLinkedBase<T>::mHeader->mNext);
	}
	//Get the last element corresponding to the list
	Position<T> getLast() {
		return Position<T>(this, DoubleLinkedBase<T>::mTrailer->mPrevious);
	}
	//Get the element after given position
	Position<T> getAfter(const Position<T> &p) {
		return Position<T>(this, p.mNode->mNext);
	}
	//Add the element at first position
	Position<T> addFirst(SmartPointer<T> element) {
		return Position<T>(this, DoubleLinkedBase<T>::insertBetween(element, DoubleLinkedBase<T>::mHeader, DoubleLinkedBase<T>::mHeader->mNext));
	}
	//Add the element at last position
	Position<T> addLast(SmartPointer<T> element) {
		return Position<T>(this, DoubleLinkedBase<T>::insertBetween(element, DoubleLinkedBase<T>::mTrailer->mPrevious, DoubleLinkedBase<T>::mTrailer));
	}
	//Add the element before a given position
	Position<T> addBefore(Position<T> &p, SmartPointer<T> element) {
		return Position<T>(this, DoubleLinkedBase<T>::insertBetween(element, p.mNode->mPrevious, p.mNode));
	}
	//Add the element after a given position
	Position<T> addAfter(Position<T> &p, SmartPointer<T> element) {
		return Position<T>(this, DoubleLinkedBase<T>::insertBetween(element, p.mNode, p.mNode->mNext));
	}
	//Delete an item at the given position
	void deleteItem(Position<T> &p) {
		DoubleLinkedBase<T>::deleteNode(p.mNode);
	}
	//Delete an item at the given index
	void deleteAtIndex(int index) {
		SmartPointer<PositionalListIteraor<T> > list(new PositionalListIteraor<T>(this));
		int i = 1;
		do {
			Position<T> search = list->next();
			if (i == index) {
				deleteItem(search);
				break;
			}
			i++;
		} while (!list->isLast());
	}
	//Get an item at the given index
	Position<T> getItemAtIndex(int index) {
		Position<T> search;
		SmartPointer<PositionalListIteraor<T> > list(new PositionalListIteraor<T>(this));
		int i = 1;
		do {
			search = list->next();
			if (i == index)
				break;
			i++;
		} while (!list->isLast());
		return search;
	}

	//Overloaded ostream for use with cout
	friend std::ostream &operator<<(std::ostream &output, PositionalList &pList)
	{
		if (!(pList.getLength() > 0))
			return output;

		SmartPointer<PositionalListIteraor<T> > list(new PositionalListIteraor<T>(&pList));
		int index = 1;
		do {
			Position<T> search = list->next();
			output << std::left << std::setw(NUMW) << std::setfill(SPACE) << index << search.getElement();
			index++;
		} while (!list->isLast());
		return output;
	}
	friend class PositionalListIteraor<T>;
};

//Stack used in this case for task handling
template<class T>
class Stack : public PositionalList<T> {
public:
	//Push an element to front of the stack
	void push(SmartPointer<T> element) {
		PositionalList<T>::addFirst(element);
	}
	//Get the topmost element
	Position<T> top() {
		return PositionalList<T>::getFirst();
	}
	//Pop the recently inserted element
	void pop() {
		Position<T> topElement = top();
		PositionalList<T>::deleteItem(topElement);
	}
};

//Stack used in this case for task handling
template<class T>
class Queue : public PositionalList<T> {
public:
	//Push an element to front of the stack
	void enqueue(SmartPointer<T> element) {
		PositionalList<T>::addLast(element);
	}
	//Get the topmost element
	Position<T> front() {
		return PositionalList<T>::getFirst();
	}
	//Pop the recently inserted element
	void dequeue() {
		Position<T> topElement = front();
		PositionalList<T>::deleteItem(topElement);
	}

	//Overloaded ostream for use with cout
	friend std::ostream &operator<<(std::ostream &output, Queue &pList) {
		if (!(pList.getLength() > 0))
			return output;
		SmartPointer<PositionalListIteraor<T> > list(new PositionalListIteraor<T>(&pList));
		int index = 1;
		do {
			Position<T> search = list->next();
			output << std::left << std::setw(NUMW) << std::setfill(SPACE) << index << search.getElement();
			index++;
		} while (!list->isLast());
		return output;
	}

};

//Iteraor for looping through the list
template<class T>
class PositionalListIteraor {
	PositionalList<T> *mPlist;
	Position<T> mCurrentPosition;
public:
	//Constructor
	PositionalListIteraor() {

	}
	PositionalListIteraor(PositionalList<T> *list) {
		mPlist = list;
		mCurrentPosition = (Position<T>(list, mPlist->getHeader()));
	}
	//Get the first element of the list
	Position<T> first() {
		return mPlist->getFirst();
	}
	//Get the last element of the list
	Position<T> last() {
		return mPlist->getLast();
	}
	//Get the next element
	Position<T> next() {
		mCurrentPosition = mPlist->getAfter(Position<T>(mCurrentPosition.mContainer, (mCurrentPosition.mNode)));
		return mCurrentPosition;
	}
	//Check if we have reached end of the list
	bool isLast() {
		return mPlist->getTrailer() == mCurrentPosition.mNode->mNext;
	}
	//Destructor
	~PositionalListIteraor() {
	}
};

//Class for holding the date information
class Date {
private:
	int mDay;
	int mMonth;
	int mYear;
public:
	Date() {
		mDay = mMonth = mYear = 0;
	}
	Date(int day, int month, int year) {
		mDay = day; mMonth = month; mYear = year;
	}
	//Set the day
	void setDay(int day) {
		mDay = day;
	}
	//Set the month
	void setMonth(int month) {
		mMonth = month;
	}
	//Set the year
	void setYear(int year) {
		mYear = year;
	}
	//Get the day
	int getDay() {
		return mDay;
	}
	//Get the month
	int getMonth() {
		return mMonth;
	}
	//Get the year
	int getYear() {
		return mYear;
	}
	//Check if it is a leap year
	bool isLeap(int year) {
		return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
	}
	//Get the number of days passed in the given year
	int getDaysPassedInYear() {
		int days[] = { 31,(isLeap(mYear)) ? 29 : 28,31,30,31,30,31,31,30,31,30,31 };
		int sum = mDay;
		for (int i = 0; i<mMonth; i++) {
			sum += days[i];
		}
		return sum;
	}
	//Compute max of two value
	int max(int a, int b) {
		return (a>b) ? a : b;
	}
	//Compute min of two value
	int min(int a, int b) {
		return (a<b) ? a : b;
	}
	//Compare two dates
	int operator>=(Date &other) {
		if (mYear > other.mYear)
			return true;
		if (mYear < other.mYear)
			return false;
		if (mMonth > other.mMonth)
			return true;
		if (mMonth < other.mMonth)
			return false;
		return mDay >= other.mDay;
	}

	bool operator==(Date &other) {
		if (mYear == other.mYear && mMonth == other.mMonth && mDay == other.mDay)
			return true;
		return false;
	}
	//Get the difference between two dates in days
	int operator-(Date &other)
	{
		int sum = 0;
		for (int i = min(mYear, other.mYear); i<max(mYear, other.mYear); i++) {
			sum += isLeap(i) ? 366 : 365;
		}
		int daysThis = getDaysPassedInYear();
		int daysThat = other.getDaysPassedInYear();
		sum += max(daysThat, daysThis) - min(daysThis, daysThat) + 1;
		return sum;
	}
	//Overloaded istream for used with cin
	friend std::istream& operator>>(std::istream &input, Date &date) {
		std::cout << "Enter The Date(YYYY/MM/DD): ";
		int days[] = { 31,(date.isLeap(date.mYear)) ? 29 : 28,31,30,31,30,31,31,30,31,30,31 };
		while (true) {
			input >> date.mYear;
			if (std::cin.get() != '/')
			{
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Invalid Date" << std::endl;
				std::cout << "Please Re-enter the Date:" << std::endl;
				continue;
			}
			input >> date.mMonth;
			if (date.mMonth < 1 || date.mMonth > 12) {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				date.mMonth = 12;
				std::cout << "Invalid Month" << std::endl;
				std::cout << "Please Re-enter the Date:" << std::endl;
				continue;
			}
			if (std::cin.get() != '/')
			{
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Invalid Date" << std::endl;
				std::cout << "Please Re-enter the Date:" << std::endl;
				continue;
			}
			input >> date.mDay;
			if (input.fail()) {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Invalid Date" << std::endl;
				std::cout << "Please Re-enter the Date:" << std::endl;
				continue;
			}
			if (days[date.mMonth - 1] < date.mDay || date.mDay < 0) {
				date.mDay = days[date.mMonth - 1];
				std::cout << "Invalid Day" << date.mDay << std::endl;
				std::cout << "Please Re-enter the Date:" << std::endl;
				continue;
			}
			break;
		}
		return input;
	}

	//Overloaded ostream for use with cout
	friend std::ostream &operator<<(std::ostream &output, const Date &date) {
		std::string months[] = { "JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC" };
		output << std::right << std::setw(2) << std::setfill('0') << date.mDay << "/" << std::right << std::setw(3) << std::setfill(SPACE) << months[date.mMonth - 1] << "/" << std::right << std::setw(4) << std::setfill('0') << date.mYear;
		return output;
	}

	operator std::string() const {
		std::string months[] = { "JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC" };
		std::stringstream output;
		output << std::right << std::setw(2) << std::setfill('0') << mDay << "/" << std::right << std::setw(3) << std::setfill(SPACE) << months[mMonth - 1] << "/" << std::right << std::setw(4) << std::setfill('0') << mYear;
		return output.str();
	}
};

std::vector<int> uid;

//Generate a random uid for passcards

int getRandom() {
	srand(time(NULL));
	int no = 0;
	while ((no = rand() % (MAX - MIN + 1) + MIN) && std::find(uid.begin(), uid.end(), no) != uid.end()) {
	}
	uid.push_back(no);
	return no;
}


class PassCard
{

public:
	enum VehicleType { TWO_AXL, THREE_MORE_AXL, TAXI, BUS, UNKNOWN };
	enum WheelerType { THREE_WHLR, FOUR_WHLR, FIVE_WHLR, SIX_WHLR, UNK_WHLR };

private:
	static int uid;         //generate a sequential id
	int mTag;               //store the sequential id to objects
	long mPassNumber;       //pass number for each pass card
	float mRechargeAmount;  //current balance
	Date mDate;

public:

    //overloaded constructors
	PassCard() :mTag(uid++) {
	}
	PassCard(long regNo, Date date, float amount) :mTag(uid++) {
		mPassNumber = regNo;
		mRechargeAmount = amount;
		mDate = date;
	}
	//get the current tag, associated with an transaction
	int getTag() {
		return mTag;
	}

	//get the current date

	Date &getDate() {
		return mDate;
	}

	//set the current date

	void setDate(Date &date) {
		mDate = date;
	}

	//get pass card number
	long getPassNumber() {
		return mPassNumber;
	}

	//set pass card number
	void setPassNumber(long regNo) {
		mPassNumber = regNo;
	}

	//set the balance
	void setRechargeAmount(float rechargeAmt) {
		mRechargeAmount = rechargeAmt;
	}

	//get the current balance
	float getRechargeAmount() {
		return mRechargeAmount;
	}

	//Print a header
	static void printHeader() {
		std::cout << std::left << std::setw(NAMEW) << "INDEX" << std::left << std::setw(NAMEW) << "PASS CARD NO" << std::left << std::setw(NAMEW) << "DATE" << std::setw(NAMEW) << "RECHARGE AMT(RM)" << std::endl;
	}

	//Overloaded cout
	friend std::ostream &operator<<(std::ostream &output, PassCard &passcard) {
		std::cout << std::left << std::setw(NAMEW) << passcard.mPassNumber << std::left << std::setw(NAMEW) << passcard.mDate << std::setfill(SPACE) << std::setw(NAMEW) << passcard.mRechargeAmount << std::endl;
		return output;
	}


	//Overloaded ostream for use with cin
	friend std::istream& operator>>(std::istream &input, PassCard &passcard) {
		input >> passcard.mDate;
		passcard.mPassNumber = getRandom();
		std::cout << "Your Pass Card Number: " << passcard.mPassNumber << std::endl;
		std::cout << "\nPlease Enter Recharge Amount: RM ";

       // Check the Entered Amount and display the correct direction
		while (std::cin >> passcard.mRechargeAmount && passcard.mRechargeAmount < 10.0) {
			std::cout << "Sorry! You can not top-up less than RM 10.00" << std::endl;
			std::cout << "\nPlease Re-enter Recharge Amount: RM \n";
		}
		return input;
	}

};

class TollRate
{
	static int uid;

	int mTag;                       //A tag for toll Rate
	PassCard::VehicleType mVType;   //Vehicle type
	PassCard::WheelerType mWType;   //Wheeler type
	float mToll;

public:
	TollRate() :mTag(uid++) {}
	TollRate(PassCard::VehicleType vType, PassCard::WheelerType wType, float toll) :mTag(uid++) {
		mVType = vType;
		mWType = wType;
		mToll = toll;
	}

	//print the header for the toll rate
	static void printHeader() {
		std::cout << std::left << std::setw(NAMEW) << "INDEX" << std::setw(80) << "VEHICLE TYPE" << std::setw(NAMEW) <<"TOLL RATE" << std::endl;
	}

	//get the vehicle type
	PassCard::VehicleType getVehicleType() {
		return mVType;
	}

	//get the wheeler type
	PassCard::WheelerType getWheelerType() {
		return mWType;
	}

	//get the toll
	float getToll() {
		return mToll;
	}

	//set the toll
	void setToll(float toll) {
		mToll = toll;
	}

	//overloaded cout
	friend std::ostream &operator<<(std::ostream &output, TollRate &rate) {
		std::string description;
		if (rate.mVType == PassCard::TAXI) {
			description = "Taxis";
		}
		else if (rate.mVType == PassCard::BUS) {
			description = "Buses";
		}
		else if (rate.mVType == PassCard::TWO_AXL && (rate.mWType == PassCard::THREE_WHLR || rate.mWType == PassCard::FOUR_WHLR)) {
			description = "Vehicles with 2 axles and 3 or 4 wheels excluding taxis";
		}
		else if (rate.mVType == PassCard::TWO_AXL && (rate.mWType == PassCard::FIVE_WHLR || rate.mWType == PassCard::SIX_WHLR)) {
			description = "Vehicles with 2 axles and 5 or 6 wheels excluding buses";
		}
		else if (rate.mVType == PassCard::THREE_MORE_AXL) {
			description = "Vehicles with 3 or more axles";

		}

		output << std::left << std::setw(80) << description << std::setw(NAMEW) << rate.mToll << std::endl;

		return output;

	}
};

class TollTransaction
{
	static int uid;

	int mTag;                    //A tag for transaction
	Date mDate;                  //Date of the transaction
	int mPassCardTag;            // Associated with a pass card
	float mAmount;               //Amount of the transaction
	PassCard::VehicleType mVType; //Vehicle type associated with the transaction
	PassCard::WheelerType mWType; //Wheeler type associated with the transaction

public:

	TollTransaction(Date date, int tag, PassCard::VehicleType vType, PassCard::WheelerType wType, float amount) :mTag(uid++) {
		mDate = date;
		mPassCardTag = tag;
		mAmount = amount;
		mVType = vType;
		mWType = wType;
	}

	// getters and setters for the associated data members
	PassCard::VehicleType getVehicleType()
	{
	    return mVType;
	}

	int getPassCardTag()
	{
		return mPassCardTag;
	}

	PassCard::WheelerType getWheelerType()
	{
		return mWType;
	}

	Date &getDate()
	{
		return mDate;
	}

	float getAmount()
	{
		return mAmount;
	}

	int getTag()
	{
		return mTag;
	}

	std::string getDescription()
	{
		std::string description;

		if (mVType == PassCard::TAXI) {
			description = "Taxis";
		}
		else if (mVType == PassCard::BUS) {
			description = "Buses";
		}
		else if (mVType == PassCard::TWO_AXL && (mWType == PassCard::THREE_WHLR || mWType == PassCard::FOUR_WHLR)) {
			description = "Vehicles with 2 axles and 3 or 4 wheels excluding taxis";
		}
		else if (mVType == PassCard::TWO_AXL && (mWType == PassCard::FIVE_WHLR || mWType == PassCard::SIX_WHLR)) {
			description = "Vehicles with 2 axles and 5 or 6 wheels excluding buses";
		}
		else if (mVType == PassCard::THREE_MORE_AXL) {
			description = "Vehicles with 3 or more axles";

		}
		return description;
	}
};

/*static data initialization*/
int PassCard::uid = 0;
int TollRate::uid = 0;
int TollTransaction::uid = 0;

//Callback function for binary search by pass number
int searchByPassIdB(SmartPointer<PassCard> value, long &passId) {
	if (value->getPassNumber() < passId) return -1;
	if (value->getPassNumber() > passId) return 1;
	return 0;
}

//Callback function for sorting by pass number
int searchByPassIdS(SmartPointer<PassCard> valueA, SmartPointer<PassCard> valueB) {
	if (valueA->getPassNumber() < valueB->getPassNumber()) return -1;
	if (valueA->getPassNumber() > valueB->getPassNumber()) return 1;
	return 0;
}

//Callback function for sorting by balance
int searchByBalanceS(SmartPointer<PassCard> valueA, SmartPointer<PassCard> valueB)
{
	if (valueA->getRechargeAmount() < valueB->getRechargeAmount()) return -1;
	if (valueA->getRechargeAmount() > valueB->getRechargeAmount()) return 1;
	return 0;
}

class Task
{
    public:
	//Show the menu for each task
	virtual void showMenu() = 0;
	//Perform the operations inside the task
	virtual Task* doTask(int n) = 0;
	//Check if the task is the last
	virtual bool isLast() {
		return false;
	}
	virtual void pauseTask() {
		system("pause");
	}

	//Virtual destructor, since this is an abstract class
	virtual ~Task() {

	}
};

//Task to exit the screen
class ExitTask : public Task
{
    public:

	void showMenu()
	{
		std::cout << "\n**** Thanks for your Transaction ********\n\n ******** Have a nice day! ********\n" << std::endl;
	}

	Task* doTask(int n)
	{
		return NULL;
	}

	bool isLast()
	{
		return true;
	}
};

//Task associated with creation and deletion of pass card

class PassCardTask : public Task

{
	SmartPointer<Queue<PassCard> > mPassCards;
	SmartPointer<DynamicArray<TollTransaction> > mTransactions;
	SmartPointer<DynamicArray<TollRate> > mTollRates;

public:
	PassCardTask(SmartPointer<Queue<PassCard> > passCard, SmartPointer<DynamicArray<TollTransaction> > transactions, SmartPointer<DynamicArray<TollRate> > tollrate) {
		mPassCards = passCard;
		mTransactions = transactions;
		mTollRates = tollrate;
	}
	bool isValidTollRateIndex(int index)
	{
		return (index >= 1 && index <= mTollRates->getLength());
	}

	bool isValidPassCardIndex(int index)
	{
		return (index >= 1 && index <= mPassCards->getLength());
	}

	PassCardTask(SmartPointer<Queue<PassCard> > passCard)
	{
		mPassCards = passCard;
	}

	//display the main menu
	void showMenu()
	{
		system("cls");  //display new screen

		std::cout << "\n\t\t..........Welcome to The Toll Booth Management System.......\n";
		std::cout << "\t\t\t  ===========================================\n\n\n\n";
		std::cout << "\t\t...........Please Choose any operation.............\n\n\n";
		std::cout << "\t\t--------------------------------------------------\n";
		std::cout << "\t\t|\t1. Display Pass Cards(Sorted) \t\t |\n";
		std::cout << "\t\t--------------------------------------------------\n";
		std::cout << "\t\t|\t2. Add Pass Card \t\t\t |\n";
		std::cout << "\t\t--------------------------------------------------\n";
		std::cout << "\t\t|\t3. Exit \t\t\t\t |\n";
		std::cout << "\t\t--------------------------------------------------\n\n";
		std::cout << "Enter choice: ";
	}

	Task* doTask(int n)
	{
		switch (n) {
		case 1:
		{
			PassCard::printHeader();
			SmartPointer<DynamicArray<PassCard> > passes(new DynamicArray<PassCard>());
			for (int i = 0; i < mPassCards->getLength(); i++) {
				SmartPointer<PassCard> passCard = mPassCards->getItemAtIndex(i).getElement();
				passes->append(passCard);
			}
			sort(passes, searchByBalanceS);
			std::cout << passes;
			pauseTask();
			return this;
		}
		case 2:
		{
			PassCard *newPassCard = new PassCard();
			std::cin >> *newPassCard;
			mPassCards->enqueue(newPassCard);
			pauseTask();
			return this;
		}
		break;
		default:
			break;
		}


		return NULL;
	}
};

//Task associated with the main screen
class MainTask : public Task {
	SmartPointer<Queue<PassCard> > mPassCards;
	SmartPointer<DynamicArray<TollTransaction> > mTransactions;
	SmartPointer<DynamicArray<TollRate> > mTollRates;

	class Category {
		std::string mDescription;
		float mAmount;

	public:
		Category() {}
		Category(std::string description, float amount) {
			mDescription = description;
			mAmount = amount;
		}
		std::string getDescription() {
			return mDescription;
		}
		float getAmount() {
			return mAmount;
		}
		void setAmount(float amount) {
			mAmount = amount;
		}
	};

	void initWithData(SmartPointer<DynamicArray<Category> > passes)
	{
		passes->append(new Category("Taxis", -1.0f));
		passes->append(new Category("Buses", -1.0f));
		passes->append(new Category("Vehicles with 2 axles and 3 or 4 wheels excluding taxis", -1.0f));
		passes->append(new Category("Vehicles with 2 axles and 5 or 6 wheels excluding buses", -1.0f));
		passes->append(new Category("Vehicles with 3 or more axles", -1.0f));
	}

	SmartPointer<Category> getACategory(SmartPointer<DynamicArray<Category> > category, std::string descripton)
	{
		for (int i = 0; i<category->getLength(); i++)
        {
			if (category->getItem(i)->getDescription() == descripton)
				return category->getItem(i);
		}
		return NULL;
	}

public:
	MainTask(SmartPointer<Queue<PassCard> > passCard, SmartPointer<DynamicArray<TollTransaction> > transactions, SmartPointer<DynamicArray<TollRate> > tollrate)
	{
		mPassCards = passCard;
		mTransactions = transactions;
		mTollRates = tollrate;
	}

	void setNewRechargeAmount(SmartPointer<PassCard> passCard)
	{
		float amount;
		std::cout << "Enter recharge amount (RM):" << std::endl;
		std::cin >> amount;

		passCard->setRechargeAmount(passCard->getRechargeAmount() + amount);
		std::cout << "Now RM " << passCard->getRechargeAmount() << " remaining in your card\n";
	}

	float getTollAmount(PassCard::VehicleType vType, PassCard::WheelerType wType)
	{
		for (int i = 0; i<mTollRates->getLength(); i++)
        {
			SmartPointer<TollRate> rate = mTollRates->getItem(i);
			if (rate->getVehicleType() == vType && rate->getWheelerType() == wType)
			{
				return rate->getToll();
			}
		}
		return -1;
	}

	bool isValidTollRateIndex(int index)
	{
		return (index >= 1 && index <= mTollRates->getLength());
	}

	bool isValidPassCardIndex(int index)
	{
		return (index >= 1 && index <= mPassCards->getLength());
	}

	SmartPointer<PassCard> getPassCard(int tag)
	{
		SmartPointer<PositionalListIteraor<PassCard > > list(new PositionalListIteraor<PassCard >(mPassCards.getData()));
		int i = 1;
		do {
			Position<PassCard> search = list->next();

			if (search.getElement()->getTag() == tag)
            {
				return search.getElement();
			}
			i++;
		} while (!list->isLast());
		return NULL;
	}

	//Display main menu when user execute the program
	void showMenu() {
		system("cls");
		std::cout << "\n\t\t..........Welcome to The Toll Booth Management System.......\n";
		std::cout << "\t\t\t  ===========================================\n\n\n\n";
		std::cout << "\t\t...........Please Choose any operation.............\n\n\n";
		std::cout << "\t\t--------------------------------------------------\n";
		std::cout << "\t\t|\t1. Entry Pass Card \t\t\t |\n";
		std::cout << "\t\t--------------------------------------------------\n";
		std::cout << "\t\t|\t2. Add Vehicle \t\t\t\t |\n";
		std::cout << "\t\t--------------------------------------------------\n";
		std::cout << "\t\t|\t3. View Vehicle information \t\t |\n";
		std::cout << "\t\t--------------------------------------------------\n";
		std::cout << "\t\t|\t4. Delete information \t\t\t |\n";
		std::cout << "\t\t--------------------------------------------------\n";
		std::cout << "\t\t|\t5. Modify Toll Rate \t\t\t |\n";
		std::cout << "\t\t--------------------------------------------------\n";
		std::cout << "\t\t|\t6. Daily Summary \t\t\t |\n";
		std::cout << "\t\t--------------------------------------------------\n";
		std::cout << "\t\t|\t7. Recharge      \t\t\t |\n";
		std::cout << "\t\t--------------------------------------------------\n";
		std::cout << "\t\t|\t8. Exit \t\t\t\t |\n";
		std::cout << "\t\t--------------------------------------------------\n\n";
		std::cout << "Enter choice: ";
	}

	Task* doTask(int n)
	{
		switch (n) {
		case 1:               //Case for Entry Pass Card Function
		{
			return new PassCardTask(mPassCards, mTransactions, mTollRates);
		}
		break;

		case 2:               //Case for Add Vehicle Function
		{
			long passNo;
			int tollIndex = 0;
			SmartPointer<DynamicArray<PassCard> > passes(new DynamicArray<PassCard>());
			for (int i = 0; i<mPassCards->getLength(); i++)
				passes->append(mPassCards->getItemAtIndex(i).getElement());
			sort(passes, searchByPassIdS);


			PassCard::printHeader();
			std::cout << passes;                                                 //display vehicle information for adding
			std::cout << "Please Enter your Pass card number : " << std::endl;
			std::cin >> passNo;                                                  //get input from the user

			TollRate::printHeader();
			std::cout << mTollRates;
			std::cout << "\nPlease Choose any Vehicle Classes: " << std::endl;
			std::cin >> tollIndex;                                               //user will choose the vehicle classes


			SmartPointer<PassCard> item = binarySearch(passes, passNo, searchByPassIdB);

			if (item.getData() == NULL)
                {
                    std::cout << "You Entered invalid card number" << std::endl;
                }
			else if (!isValidTollRateIndex(tollIndex))
                {
                    std::cout << "Wrong input" << std::endl;
                }

			else
            {
			    std::cout << "\n" << std::endl;
                    Date date;
					std::cin >> date;
					item->setDate(date);

				SmartPointer<TollRate> rate = mTollRates->getItem(tollIndex - 1);
				std::cout << "\nYou have RM " << item->getRechargeAmount() << " in your card" << std::endl;;
				float balance = item->getRechargeAmount() - rate->getToll();

				if (balance >= 0.0)
                {
					std::cout << "Just now RM " << rate->getToll() << " was debited from your card" << std::endl;
					item->setRechargeAmount(balance);

					std::cout << "Now RM " << balance << " remaining in your card\n\n";
					mTransactions->append(new TollTransaction(date, item->getTag(), rate->getVehicleType(), rate->getWheelerType(), rate->getToll()));
				}
				else
                {
					std::cout << "Sorry, You have not enough balance to pass this vehicle" << std::endl;
					std::cout << "Please top-up into your card!" << std::endl;
				}
			}
			pauseTask();
			return this;

		}
		break;


    //Case for displaying vehicle information

		case 3:
		{
			TollRate::printHeader();
			std::cout << mTollRates;
			pauseTask();
			return this;
		}
		break;

     //Case for deleting information

		case 4:
		{
			SmartPointer<DynamicArray<PassCard> > passes(new DynamicArray<PassCard>());
			for (int i = 0; i<mPassCards->getLength(); i++)

				passes->append(mPassCards->getItemAtIndex(i).getElement());

			sort(passes, searchByPassIdS);         //Sorting information
			long passNo;
			PassCard::printHeader();
			std::cout << passes;

			std::cout << "Enter the Pass card number to delete:" << std::endl;
			std::cin >> passNo;
			SmartPointer<PassCard> item = binarySearch(passes, passNo, searchByPassIdB);

			if (item.getData() == NULL)
                {
                    std::cout << "Sorry, This pass card " << passNo << " is not listed" << std::endl;
                }

			else
            {
				SmartPointer<PositionalListIteraor<PassCard > > list(new PositionalListIteraor<PassCard >(mPassCards.getData()));
				int i = 1;

				do {
					Position<PassCard> search = list->next();
					if (search.getElement()->getTag() == item->getTag())
                    {
						std::cout << "The Pass card " << passNo << " is deleted." << std::endl;
						mPassCards->deleteItem(search);
						break;
					}
					i++;
				} while (!list->isLast());
			}
			pauseTask();
			return this;
		}
		break;

      //Case for update toll rate information
		case 5:
		{
			TollRate::printHeader();
			std::cout << mTollRates;

			int index;

			std::cout << "which vehicle class toll rate you want to modify?" << std::endl;
			inputNumber(index);

			if (isValidTollRateIndex(index)) {
				SmartPointer<TollRate> tollRate = mTollRates->getItem(index - 1);

				float tollFare;

				if (index == 1)
				{
					std::cout << "Now the price of Vehicles with 2 axles and 3 or 4 wheels is RM " << tollRate->getToll() << std::endl;;
				}

				else if (index == 2)
				{
					std::cout << "Now the price of Vehicles with 2 axles and 5 or 6 wheels excluding buses is RM " << tollRate->getToll() << std::endl;
				}

				else if (index == 3)
				{
					std::cout << "Now the price of Vehicles with 3 or more axles is RM " << tollRate->getToll() << std::endl;
				}

				else if (index == 4)
				{
					std::cout << "Now the price of Taxis is RM " << tollRate->getToll() << std::endl;;
				}

				else if (index == 5)
				{
					std::cout << "Now the price of Buses is RM " << tollRate->getToll() << std::endl;
				}
				std::cout << "Input your New price (RM): ";
				std::cin >> tollFare;
				tollRate->setToll(tollFare);
				std::cout << "Price has been updated" << std::endl;
				pauseTask();
			}
			return this;

		}
		break;

    //Case for view the summary by daily basis
		case 6:
		{
			system("cls");

			Date date;
			std::cin >> date;
			std::cout << "\n\n\n\t\t\t\tDaily Transaction Summary of The Toll Booth Management System\n";
			std::cout << "\t\t\t\t____________________________________________________________\n\n";

			float collection = 0.0f;

			for (int i = 0; i < mPassCards->getLength(); i++)
            {
				SmartPointer<PassCard> passCard = mPassCards->getItemAtIndex(i).getElement();

				float cashBox = 0.0f;

				SmartPointer<DynamicArray<Category> > category(new DynamicArray<Category>());
				initWithData(category);

				bool flag = false;

				for (int j = 0; j < mTransactions->getLength(); j++)
                {
					SmartPointer<TollTransaction> transaction = mTransactions->getItem(j);

					if (passCard->getTag() == transaction->getPassCardTag() && date == transaction->getDate())
                    {
						cashBox += transaction->getAmount();

						SmartPointer<Category> item = getACategory(category, transaction->getDescription());
						flag = true;

						if (item.getData() != NULL)
                        {
							if (item->getAmount()<0.0f)
								item->setAmount(0.0f);
							item->setAmount(item->getAmount() + transaction->getAmount());
						}
					}
				}
				if (flag) {
					std::cout << "\n\n\t\tDate: " << date << std::setfill(SPACE) << std::endl;
					std::cout << "\n\n";
					std::cout << "\t\tPasscard Number: " << passCard->getPassNumber();
					std::cout << "\n\n\t\tPasscard Amount:RM " << passCard->getRechargeAmount();
					std::cout << "\n\n\t\tTotal Cashbox Amount:RM " << cashBox;
					std::cout << "\n\n\t\tvehicle class: " << std::endl;

					for (int i = 0; i<category->getLength(); i++)
                    {
						SmartPointer<Category> item = category->getItem(i);

						if (item->getAmount()> 0)
							std::cout << std::right << std::setw(60) << item->getDescription() << std::right << std::setw(NAMEW) << item->getAmount() << std::endl;
					}
					std::cout << "\n\n";
					collection += cashBox;
					cashBox = 0.0f;
				}
			}
			std::cout << "\tTotal collection for this day is: RM " << collection << std::endl;
			pauseTask();
			return this;
        }
		break;

		//Case for Recharge balance
		case 7:
		{
			long passNo;

			SmartPointer<DynamicArray<PassCard> > passes(new DynamicArray<PassCard>());

			for (int i = 0; i<mPassCards->getLength(); i++)
				passes->append(mPassCards->getItemAtIndex(i).getElement());
			sort(passes, searchByPassIdS);
			PassCard::printHeader();
			std::cout << passes;

			std::cout << "Enter the Pass card number to recharge:" << std::endl;
			std::cin >> passNo;
			SmartPointer<PassCard> item = binarySearch(passes, passNo, searchByPassIdB);

			if (item.getData() == NULL)
            {
				std::cout << "Sorry, This pass card " << passNo << " is not listed" << std::endl;
			}
			else
			{
				setNewRechargeAmount(item);
			}
			pauseTask();
			return this;

		}
		break;

		default:
			break;
		}
		return NULL;
	}
};

//Pre-set data for transactions

void loadTransaction(SmartPointer<DynamicArray<TollTransaction> > transactions) {
	transactions->append(new TollTransaction(Date(1, 1, 2017), 0, PassCard::TWO_AXL, PassCard::THREE_WHLR, 1.50f));
	transactions->append(new TollTransaction(Date(1, 1, 2017), 0, PassCard::TWO_AXL, PassCard::THREE_WHLR, 1.50f));
	transactions->append(new TollTransaction(Date(1, 1, 2017), 0, PassCard::TWO_AXL, PassCard::THREE_WHLR, 1.50f));
	transactions->append(new TollTransaction(Date(1, 1, 2017), 0, PassCard::TWO_AXL, PassCard::THREE_WHLR, 1.50f));
	transactions->append(new TollTransaction(Date(1, 1, 2017), 0, PassCard::TWO_AXL, PassCard::THREE_WHLR, 1.50f));
	transactions->append(new TollTransaction(Date(1, 1, 2017), 0, PassCard::TWO_AXL, PassCard::THREE_WHLR, 1.50f));
	transactions->append(new TollTransaction(Date(1, 1, 2017), 0, PassCard::TWO_AXL, PassCard::THREE_WHLR, 1.50f));
	//////////////////////////////////////////////////////////////////
	transactions->append(new TollTransaction(Date(1, 2, 2017), 1, PassCard::TWO_AXL, PassCard::THREE_WHLR, 1.50f));
	transactions->append(new TollTransaction(Date(1, 2, 2017), 1, PassCard::TWO_AXL, PassCard::THREE_WHLR, 1.50f));
	transactions->append(new TollTransaction(Date(1, 2, 2017), 1, PassCard::TWO_AXL, PassCard::THREE_WHLR, 1.50f));
	transactions->append(new TollTransaction(Date(1, 2, 2017), 1, PassCard::TWO_AXL, PassCard::THREE_WHLR, 1.50f));
	transactions->append(new TollTransaction(Date(1, 2, 2017), 1, PassCard::TWO_AXL, PassCard::THREE_WHLR, 1.50f));
	transactions->append(new TollTransaction(Date(1, 2, 2017), 1, PassCard::TWO_AXL, PassCard::THREE_WHLR, 1.50f));
	transactions->append(new TollTransaction(Date(1, 2, 2017), 1, PassCard::TWO_AXL, PassCard::THREE_WHLR, 1.50f));
	//////////////////////////////////////////////////////////////////
	transactions->append(new TollTransaction(Date(1, 3, 2017), 2, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 1.50f));
	transactions->append(new TollTransaction(Date(1, 3, 2017), 2, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 1.50f));
	transactions->append(new TollTransaction(Date(1, 3, 2017), 2, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 1.50f));
	transactions->append(new TollTransaction(Date(1, 3, 2017), 2, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 1.50f));
	transactions->append(new TollTransaction(Date(1, 3, 2017), 2, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 1.50f));
	transactions->append(new TollTransaction(Date(1, 3, 2017), 2, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 1.50f));
	transactions->append(new TollTransaction(Date(1, 3, 2017), 2, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 1.50f));
	//////////////////////////////////////////////////////////////////
	transactions->append(new TollTransaction(Date(1, 4, 2017), 3, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 1.50f));
	transactions->append(new TollTransaction(Date(1, 4, 2017), 3, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 1.50f));
	transactions->append(new TollTransaction(Date(1, 4, 2017), 3, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 1.50f));
	transactions->append(new TollTransaction(Date(1, 4, 2017), 3, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 1.50f));
	transactions->append(new TollTransaction(Date(1, 4, 2017), 3, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 1.50f));
	transactions->append(new TollTransaction(Date(1, 4, 2017), 3, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 1.50f));
	transactions->append(new TollTransaction(Date(1, 4, 2017), 3, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 1.50f));
	//////////////////////////////////////////////////////////////////
	transactions->append(new TollTransaction(Date(1, 1, 2017), 4, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 2.30f));
	transactions->append(new TollTransaction(Date(1, 1, 2017), 4, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 2.30f));
	transactions->append(new TollTransaction(Date(1, 1, 2017), 4, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 2.30f));
	transactions->append(new TollTransaction(Date(1, 1, 2017), 4, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 2.30f));
	transactions->append(new TollTransaction(Date(1, 1, 2017), 4, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 2.30f));
	transactions->append(new TollTransaction(Date(1, 1, 2017), 4, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 2.30f));
	transactions->append(new TollTransaction(Date(1, 1, 2017), 4, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 2.30f));
	//////////////////////////////////////////////////////////////////
	transactions->append(new TollTransaction(Date(1, 2, 2017), 5, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 2.30f));
	transactions->append(new TollTransaction(Date(1, 2, 2017), 5, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 2.30f));
	transactions->append(new TollTransaction(Date(1, 2, 2017), 5, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 2.30f));
	transactions->append(new TollTransaction(Date(1, 2, 2017), 5, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 2.30f));
	transactions->append(new TollTransaction(Date(1, 2, 2017), 5, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 2.30f));
	transactions->append(new TollTransaction(Date(1, 2, 2017), 5, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 2.30f));
	transactions->append(new TollTransaction(Date(1, 2, 2017), 5, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 2.30f));
	//////////////////////////////////////////////////////////////////
	transactions->append(new TollTransaction(Date(1, 3, 2017), 6, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 2.30f));
	transactions->append(new TollTransaction(Date(1, 3, 2017), 6, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 2.30f));
	transactions->append(new TollTransaction(Date(1, 3, 2017), 6, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 2.30f));
	transactions->append(new TollTransaction(Date(1, 3, 2017), 6, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 2.30f));
	transactions->append(new TollTransaction(Date(1, 3, 2017), 6, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 2.30f));
	transactions->append(new TollTransaction(Date(1, 3, 2017), 6, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 2.30f));
	transactions->append(new TollTransaction(Date(1, 3, 2017), 6, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 2.30f));
	//////////////////////////////////////////////////////////////////
	transactions->append(new TollTransaction(Date(1, 4, 2017), 7, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 2.30f));
	transactions->append(new TollTransaction(Date(1, 4, 2017), 7, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 2.30f));
	transactions->append(new TollTransaction(Date(1, 4, 2017), 7, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 2.30f));
	transactions->append(new TollTransaction(Date(1, 4, 2017), 7, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 2.30f));
	transactions->append(new TollTransaction(Date(1, 4, 2017), 7, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 2.30f));
	transactions->append(new TollTransaction(Date(1, 4, 2017), 7, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 2.30f));
	transactions->append(new TollTransaction(Date(1, 4, 2017), 7, PassCard::TWO_AXL, PassCard::FIVE_WHLR, 2.30f));
	//////////////////////////////////////////////////////////////////
	transactions->append(new TollTransaction(Date(1, 1, 2017), 8, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 3.00f));
	transactions->append(new TollTransaction(Date(1, 1, 2017), 8, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 3.00f));
	transactions->append(new TollTransaction(Date(1, 1, 2017), 8, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 3.00f));
	transactions->append(new TollTransaction(Date(1, 1, 2017), 8, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 3.00f));
	transactions->append(new TollTransaction(Date(1, 1, 2017), 8, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 3.00f));
	transactions->append(new TollTransaction(Date(1, 1, 2017), 8, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 3.00f));
	transactions->append(new TollTransaction(Date(1, 1, 2017), 8, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 3.00f));
	//////////////////////////////////////////////////////////////////
	transactions->append(new TollTransaction(Date(1, 2, 2017), 9, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 3.00f));
	transactions->append(new TollTransaction(Date(1, 2, 2017), 9, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 3.00f));
	transactions->append(new TollTransaction(Date(1, 2, 2017), 9, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 3.00f));
	transactions->append(new TollTransaction(Date(1, 2, 2017), 9, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 3.00f));
	transactions->append(new TollTransaction(Date(1, 2, 2017), 9, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 3.00f));
	transactions->append(new TollTransaction(Date(1, 2, 2017), 9, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 3.00f));
	transactions->append(new TollTransaction(Date(1, 2, 2017), 9, PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 3.00f));
	//////////////////////////////////////////////////////////////////
	transactions->append(new TollTransaction(Date(1, 1, 2017), 10, PassCard::BUS, PassCard::SIX_WHLR, 0.80f));
	transactions->append(new TollTransaction(Date(1, 1, 2017), 10, PassCard::BUS, PassCard::SIX_WHLR, 0.80f));
	transactions->append(new TollTransaction(Date(1, 1, 2017), 10, PassCard::BUS, PassCard::SIX_WHLR, 0.80f));
	transactions->append(new TollTransaction(Date(1, 1, 2017), 10, PassCard::BUS, PassCard::SIX_WHLR, 0.80f));
	transactions->append(new TollTransaction(Date(1, 1, 2017), 10, PassCard::BUS, PassCard::SIX_WHLR, 0.80f));
	transactions->append(new TollTransaction(Date(1, 1, 2017), 10, PassCard::BUS, PassCard::SIX_WHLR, 0.80f));
	transactions->append(new TollTransaction(Date(1, 1, 2017), 10, PassCard::BUS, PassCard::SIX_WHLR, 0.80f));
	//////////////////////////////////////////////////////////////////
	transactions->append(new TollTransaction(Date(1, 3, 2017), 11, PassCard::BUS, PassCard::SIX_WHLR, 0.80f));
	transactions->append(new TollTransaction(Date(1, 3, 2017), 11, PassCard::BUS, PassCard::SIX_WHLR, 0.80f));
	transactions->append(new TollTransaction(Date(1, 3, 2017), 11, PassCard::BUS, PassCard::SIX_WHLR, 0.80f));
	transactions->append(new TollTransaction(Date(1, 3, 2017), 11, PassCard::BUS, PassCard::SIX_WHLR, 0.80f));
	transactions->append(new TollTransaction(Date(1, 3, 2017), 11, PassCard::BUS, PassCard::SIX_WHLR, 0.80f));
	transactions->append(new TollTransaction(Date(1, 3, 2017), 11, PassCard::BUS, PassCard::SIX_WHLR, 0.80f));
	transactions->append(new TollTransaction(Date(1, 3, 2017), 11, PassCard::BUS, PassCard::SIX_WHLR, 0.80f));
	//////////////////////////////////////////////////////////////////
	transactions->append(new TollTransaction(Date(1, 2, 2017), 12, PassCard::TAXI, PassCard::FOUR_WHLR, 1.10f));
	transactions->append(new TollTransaction(Date(1, 2, 2017), 12, PassCard::TAXI, PassCard::FOUR_WHLR, 1.10f));
	transactions->append(new TollTransaction(Date(1, 2, 2017), 12, PassCard::TAXI, PassCard::FOUR_WHLR, 1.10f));
	transactions->append(new TollTransaction(Date(1, 2, 2017), 12, PassCard::TAXI, PassCard::FOUR_WHLR, 1.10f));
	transactions->append(new TollTransaction(Date(1, 2, 2017), 12, PassCard::TAXI, PassCard::FOUR_WHLR, 1.10f));
	transactions->append(new TollTransaction(Date(1, 2, 2017), 12, PassCard::TAXI, PassCard::FOUR_WHLR, 1.10f));
	transactions->append(new TollTransaction(Date(1, 2, 2017), 12, PassCard::TAXI, PassCard::FOUR_WHLR, 1.10f));
	//////////////////////////////////////////////////////////////////
	transactions->append(new TollTransaction(Date(1, 3, 2017), 13, PassCard::TAXI, PassCard::FOUR_WHLR, 1.10f));
	transactions->append(new TollTransaction(Date(1, 3, 2017), 13, PassCard::TAXI, PassCard::FOUR_WHLR, 1.10f));
	transactions->append(new TollTransaction(Date(1, 3, 2017), 13, PassCard::TAXI, PassCard::FOUR_WHLR, 1.10f));
	transactions->append(new TollTransaction(Date(1, 3, 2017), 13, PassCard::TAXI, PassCard::FOUR_WHLR, 1.10f));
	transactions->append(new TollTransaction(Date(1, 3, 2017), 13, PassCard::TAXI, PassCard::FOUR_WHLR, 1.10f));
	transactions->append(new TollTransaction(Date(1, 3, 2017), 13, PassCard::TAXI, PassCard::FOUR_WHLR, 1.10f));
	transactions->append(new TollTransaction(Date(1, 3, 2017), 13, PassCard::TAXI, PassCard::FOUR_WHLR, 1.10f));
	//////////////////////////////////////////////////////////////////
}


//Pre-set data for pass card

void loadPassCard(SmartPointer<Queue<PassCard> > passCards) {
	int uids[] = { 1134,4132,6131,5136,3139,2036,7132,2132,5733,2920,5063,2974,6524,7798 };
	uid.insert(uid.end(), uids, uids + 14);
	passCards->enqueue(new PassCard(1134, Date(1, 1, 2017), 10.0f));
	passCards->enqueue(new PassCard(4132, Date(1, 2, 2017), 33.0f));
	passCards->enqueue(new PassCard(6131, Date(1, 3, 2017), 24.0f));
	passCards->enqueue(new PassCard(5136, Date(1, 4, 2017), 31.0f));
	passCards->enqueue(new PassCard(3139, Date(1, 1, 2017), 10.0f));
	passCards->enqueue(new PassCard(2036, Date(1, 2, 2017), 17.0f));
	passCards->enqueue(new PassCard(7132, Date(1, 3, 2017), 21.0f));
	passCards->enqueue(new PassCard(2132, Date(1, 4, 2017), 23.0f));
	passCards->enqueue(new PassCard(5733, Date(1, 1, 2017), 90.0f));
	passCards->enqueue(new PassCard(2920, Date(1, 2, 2017), 86.0f));
	passCards->enqueue(new PassCard(5063, Date(1, 1, 2017), 45.0f));
	passCards->enqueue(new PassCard(2974, Date(1, 3, 2017), 36.0f));
	passCards->enqueue(new PassCard(6524, Date(1, 2, 2017), 98.0f));
	passCards->enqueue(new PassCard(7798, Date(1, 3, 2017), 76.0f));
}

//Pre set data for toll -rate

void loadTollRate(SmartPointer<DynamicArray<TollRate> > tollRates) {
	tollRates->append(new TollRate(PassCard::TWO_AXL, PassCard::THREE_WHLR, 1.50f));
	tollRates->append(new TollRate(PassCard::TWO_AXL, PassCard::FIVE_WHLR, 2.30f));
	tollRates->append(new TollRate(PassCard::THREE_MORE_AXL, PassCard::UNK_WHLR, 3.00f));
	tollRates->append(new TollRate(PassCard::TAXI, PassCard::FOUR_WHLR, 0.80f));
	tollRates->append(new TollRate(PassCard::BUS, PassCard::SIX_WHLR, 1.10f));
}

// main function for this program

int main()
 {
	system("COLOR 1F");    //display the color
	system("cls");         //display new screen

	SmartPointer<Queue<PassCard> > passCards(new Queue<PassCard>());    //Queue for maintaining passcard info
	SmartPointer<Stack<Task> > taskList(new Stack<Task>());             //Stack for maintianing tasks on the screen
	SmartPointer<DynamicArray<TollTransaction> > transactions(new DynamicArray<TollTransaction>()); //Dynamic array to hold transactions
	SmartPointer<DynamicArray<TollRate> > tollRates(new DynamicArray<TollRate>()); // Dynamic array to hold toll rates

	loadPassCard(passCards);
	loadTollRate(tollRates);
	loadTransaction(transactions);

	taskList->push(new ExitTask());
	taskList->push(new MainTask(passCards, transactions, tollRates));

	int n;
	do {
		Position<Task> task = taskList->top();
		task.getElement()->showMenu();

		if (task.getElement()->isLast())
			break;

		inputNumber(n);
		Task *mainTask = task.getElement()->doTask(n);

		if (mainTask == NULL) {
			taskList->pop();
		}
		else if (mainTask != task.getElement().getData()) {
			taskList->push(mainTask);
		}
	} while (taskList->getLength() != 0);

	return 0;
}

///////////////////////////////////////////////END OF THIS PROGRAM/////////////////////////////////////////////////////////
