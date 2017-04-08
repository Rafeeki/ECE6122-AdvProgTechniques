// Implementation of the templated Vector class
// ECE4122/6122 lab 3
// Ryan Gentry

#include <iostream> // debugging
#include "Vector.h"
#include <string.h> // for strcmp()

// Your implementation here
// Fill in all the necessary functions below
using namespace std;

// Default constructor
template <typename T>
Vector<T>::Vector()
{
  // Count = number of items in elements, Reserved = total room in elements
  count = 0;
  reserved = 0;
  // Allocate memory for elements by the size of the type T that will be stored
  elements = (T*)malloc((count)*sizeof(T));
}

// Copy constructor
template <typename T>
Vector<T>::Vector(const Vector& rhs)
{
  // Number of items in rhs = number of items to be stored in elements
  count = rhs.Size();
  if(count != 0)
  {
	elements = (T*)malloc(count*sizeof(T));
	// In place copy constructor from each element of rhs to elements
	for(size_t i = 0; i < count; ++i)
	{ 
		new(&elements[i]) T(rhs[i]);
	}
	// Currently allocated 'count' spaces in elements
	reserved = count;
  }
}

// Assignment operator
template <typename T>
Vector<T>& Vector<T>::operator=(const Vector& rhs)
{
  // Unnecessary
}

#ifdef GRAD_STUDENT
// Other constructors
template <typename T>
Vector<T>::Vector(size_t nReserved)
{ // Initialize with reserved memory
  // Just like default constructor, but with a non-zero size
  count = nReserved;
  elements = (T*)malloc(count*sizeof(T));
  reserved = count;
}

template <typename T>
Vector<T>::Vector(size_t n, const T& t)
{ // Initialize with "n" copies of "t"
  count = n;
  elements = (T*)malloc(count*sizeof(T));
  // In place copy constructor of same t into each spot in elements
  for (size_t i = 0; i < count; ++i)
  {
	new(&elements[i]) T(t);
  }
  // Currently allocated 'count' spaces in elements
  reserved = count;
}

template <typename T>
void Vector<T>::Reserve(size_t n)
{ // Reserve extra memory
  reserved = reserved+n;
  // Create new placeholder vector with 'reserved' amount of memory allocated
  T* newElements = (T*)malloc(reserved*sizeof(T));
  // However, only copy construct the 'count' elements from old vector into new
  for(size_t i = 0; i < count; ++i)
  {
	new(&newElements[i]) T(elements[i]);
	// Destruct the old elements before freeing them
	elements[i].~T();
  }
  // Free the old elements and point elements to the new Vector
  free(elements);
  elements = newElements;
}

#endif

// Destructor
template <typename T>
Vector<T>::~Vector()
{ // Destruct the old elements before freeing them and setting count/reserved to 0
  for (size_t i = 0; i < count; ++i)
  {
	elements[i].~T();
  }
  count = 0;
  free(elements);
  reserved = count;

}

// Add and access front and back
template <typename T>
void Vector<T>::Push_Back(const T& rhs)
{ // If extra space in Vector already, store element at end until run out of room
  if(reserved > count)
  {
	new(&elements[count]) T(rhs);
	count++;
  }
  // If no extra space, create a placeholder Vector with one extra spot
  else
  {
	T* newElements = (T*)malloc((count+1)*sizeof(T));
	for(size_t i = 0; i < count; ++i)
	{  // Copy construct in newElements and destruct in elements
		new(&newElements[i]) T(elements[i]);
		elements[i].~T();
	}
	// Store new element in the final spot, iterate count/reserved
	new(&newElements[count]) T(rhs);
	count++;
	reserved++;
	// Free old elements and point it at newElements
	free(elements);
	elements = newElements;
  }
}

template <typename T>
void Vector<T>::Push_Front(const T& rhs)
{ // If extra space in Vector already, move every element one space backwards
  if (reserved > count)
  {  // Start from the back and move to front to avoid overwriting data
	for (size_t i = count; i > 0; --i)
	{ // Destruct old elements to allow writing new data
		new(&elements[i]) T(elements[i-1]);
		elements[i-1].~T();
	}
	count++;
  }
  // If no extra space, create placeholder Vector with count+1 spots
  else
  {
	T* newElements = (T*)malloc((count+1)*sizeof(T));
	for (size_t i = 0; i < count; ++i)
	{ // Copy construct old elements into +1 spot in placeholder, leave 0 open
	  // Destruct old elements before freeing
		new(&newElements[i+1]) T(elements[i]);
		elements[i].~T();
	}
	// Iterate count & reserved, free old memory from elements and point to newElements
	count++;
	reserved = count;
	free(elements);
	elements = newElements;
  }
  // Regardless of how much space was left in elements, copy construct new data into spot 0
  new(&elements[0]) T(rhs);
}

template <typename T>
void Vector<T>::Pop_Back()
{ // Remove last element and decrement count/reserved
  if (count != 0) elements[count-1].~T();
  count--;
  reserved = count;
}

template <typename T>
void Vector<T>::Pop_Front()
{ // Remove first element by copy constructing elements to placeholder in spot-1
  T* newElements = (T*)malloc((count-1)*sizeof(T));
  for (size_t i = 1; i < count; ++i)
  {
	new(&newElements[i-1]) T(elements[i]);
	// Destruct old elements
	elements[i].~T();
  }
  // Destruct first element in elements, decrement count/reserved
  elements[0].~T();
  count--;
  reserved = count;
  // Free old memory from elements and point to newElements
  free(elements);
  elements = newElements;
}

// Element Access
template <typename T>
T& Vector<T>::Front() const
{ // Return first element
  return elements[0];
}

// Element Access
template <typename T>
T& Vector<T>::Back() const
{ // Return last element
  return elements[count-1];
}

template <typename T>
const T& Vector<T>::operator[](size_t i) const
{ // const element access at spot i
  return elements[i];
}

template <typename T>
T& Vector<T>::operator[](size_t i)
{ // nonconst element access at spot i
  return elements[i];
}

template <typename T>
size_t Vector<T>::Size() const
{ // Return # of elements
  return count;
}

template <typename T>
bool Vector<T>::Empty() const
{
  if (count==0) return true;
  else return false;
}

// Implement clear
template <typename T>
void Vector<T>::Clear()
{ // Call in-place destructor on each existing element
  for (size_t i = 0; i < count; ++i)
  {
	elements[count - i - 1].~T();
  }
  // Reset count but keep memory
  count = 0;
}

// Iterator access functions
template <typename T>
VectorIterator<T> Vector<T>::Begin() const
{ // Return a VectorIterator<T> (really just a pointer) starting at the front of elements
  return VectorIterator<T>(elements);
}

template <typename T>
VectorIterator<T> Vector<T>::End() const
{ // Return a VectorIterator<T> (really just a pointer) starting at the end of elements
  return VectorIterator<T>(elements+reserved-1);
}

#ifdef GRAD_STUDENT
// Erase and insert
template <typename T>
void Vector<T>::Erase(const VectorIterator<T>& it)
{ // Find spot in elements where it exists
  size_t spot = 0;
  // Use strcmp to compare strings... need to use c_str function to turn each into char*
  while(strcmp((elements[spot]).c_str(),(*it).c_str()))
  {
	spot++;
  }
  // With spot known, create placeholder to copy construct all old elements but it into
  T* newElements = (T*)malloc((count-1)*sizeof(T)); 
  for (size_t i = 0; i < count; ++i)
  { // Before spot: copy construct into same location, and destruct old element
	if (i < spot)
	{
		new(&newElements[i]) T(elements[i]);
		elements[i].~T();
	}
    // At spot: only destruct old element
	else if (i == spot)
	{
		elements[i].~T();
	}
    // After spot: copy construct old element into i-1 location of placeholder
	else
	{
		new(&newElements[i-1]) T(elements[i]);
		elements[i].~T();
	}
  }
  // Decrement count/reserved, free old elements and point at newElements
  count--;
  reserved=count;
  free(elements);
  elements = newElements;
}

template <typename T>
void Vector<T>::Insert(const T& rhs, const VectorIterator<T>& it)
{ // Find spot in elements where it exists
  size_t spot = 0;
  // Use strcmp to compare strings... need to use c_str function to turn each into char*
  while(strcmp((elements[spot]).c_str(),(*it).c_str()))
  {
	spot++;
  }
  // With spot known, create placeholder to copy construct all old elements but it into
  // Increment count by 1 first, since elements will be growing by 1
  count++;
  T* newElements = (T*)malloc(count*sizeof(T));
  for (size_t i = 0; i < count; ++i)
  {	// Before spot: copy construct into same location, destruct old element
	if (i < spot)
	{
		new(&newElements[i]) T(elements[i]);
		elements[i].~T();
	}
	// At spot: copy construct it into current location, no destruct
	else if (i == spot)
	{
		new(&newElements[i]) T(rhs);
	}
	// After spot: copy construct i-1 old elements into newElements, destruct old
	else
	{
		new(&newElements[i]) T(elements[i-1]);
		elements[i-1].~T();
	}
  } 
  // Set reserved equal to count, free old memory from elements and point to new
  reserved = count;
  free(elements);
  elements = newElements;
}
#endif

// Implement the iterators

// Constructors
template <typename T>
VectorIterator<T>::VectorIterator()
{
  // Unnecessary, current initialized in Vector.h 
}

template <typename T>
VectorIterator<T>::VectorIterator(T* c)
{ // Point current to c
  current = c;
}

// Copy constructor
template <typename T>
VectorIterator<T>::VectorIterator(const VectorIterator<T>& rhs)
{ // Because Vector is a friendly class, VectorIterator can share its private current value
  current = rhs.current;
}

// Iterator defeferencing operator
template <typename T>
T& VectorIterator<T>::operator*() const
{
  return *current;
}

// Prefix increment
template <typename T>
VectorIterator<T>  VectorIterator<T>::operator++()
{ // Iterate current to next position and return it
  current++;
  return current;
} 

// Postfix increment
template <typename T>
VectorIterator<T> VectorIterator<T>::operator++(int)
{ // Create pointer to old current, iterate current, and return old current
  VectorIterator<T> i = *this;
  current++;
  return i;
}

// Comparison operators
template <typename T>
bool VectorIterator<T>::operator !=(const VectorIterator<T>& rhs) const
{ // Result = 0 if the two match, so return False for matching, and True for not
  int result = strcmp((*current).c_str(),(*rhs).c_str());
  return result;
}

template <typename T>
bool VectorIterator<T>::operator ==(const VectorIterator<T>& rhs) const
{ // Result = 0 if the two match, so return True for matching, and False for not
  int result = strcmp((*current).c_str(),(*rhs).c_str());
  return !result;
}




