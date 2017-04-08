// Implementation of the templated Vector class
// ECE6122 lab 3
// Ryan Gentry

#include <iostream> // debugging
#include <string.h>
#include <stdlib.h>
#include "Vector.h"

// Your implementation here
// Fill in all the necessary functions below
using namespace std;

// Static counts
//T* Vector::elements;
//size_t Vector::count;
//size_t  Vector::reserved;
//T* VectorIterator::current;

// Default constructor
template <typename T>
Vector<T>::Vector()
{ // Default to empty vector
  count = 1;
  elements = (T*)malloc((count+1)*sizeof(T));
  //elements = (T*)malloc((count+1)*sizeof(T));
  cout << "Vector of count = " << count << " of size = " << (count+1)*sizeof(T) << " created!" << endl;
  reserved = count+1;
}

// Default constructor
//template <typename T>
//Vector<T>::Vector(const T* vec)
//{ // Allocate enough memory for new array "vec" of type T, length given by sizeof(T),
//  // if it's a string +1 b/c C-style strings terminated by 0-byte terminator
//  count = 1;
//  T* elements = (T*)malloc((count+1)*sizeof(T));
//}

// Copy constructor
template <typename T>
Vector<T>::Vector(const Vector& rhs)
{
}

// Assignment operator
template <typename T>
Vector<T>& Vector<T>::operator=(const Vector& rhs)
{
}

#ifdef GRAD_STUDENT
// Other constructors
template <typename T>
Vector<T>::Vector(size_t nReserved)
{ // Initialize with reserved memory
}

template <typename T>
Vector<T>::Vector(size_t n, const T& t)
{ // Initialize with "n" copies of "t"
}

template <typename T>
void Vector<T>::Reserve(size_t n)
{ // Reserve extra memory
  if( reserved >= n) return; // Already enough room
  if( count > 0)
  {
	T* newElements = (T*)malloc(sizeof(T) * n);
	for(size_t i = 0; i < count; ++i)
	{ // Copy from existing to new
		new(&elements[i]) T(elements[i]);
		elements[i].~T();
	}
  	free(elements);
  	elements = newElements;
  }
  else
  {
	free(elements);
	elements = (T*)malloc(sizeof(T) * n);
  }
  reserved = n;
}

#endif

// Destructor
template <typename T>
Vector<T>::~Vector()
{
  cout << "Destructor called on " << &elements << endl;
  delete[] elements;
}

// Add and access front and back
template <typename T>
void Vector<T>::Push_Back(const T& rhs)
{
  cout << "Push_Back count = " << count << " reserved = " << reserved << endl;
  if(reserved > count)
  { // Already have the memory, just need to add the element
	cout << "Push_back already had memory" << endl;
	new(&elements[count]) T(rhs);
	count++;
  }
  else
  { // Not enough memory, need to reallocate
	// Create new pointer of type T with enough memory for count+1 elements
	cout << "Reallocating count to " << count + 1 << " with sizeof(T) = " << sizeof(T) << endl;
	T* newElements = (T*)malloc((count+1) * sizeof(T)); //typecast malloc bc void pointer
	cout << "newElements = " << &newElements << endl;
	// Reserved now has size of newElements as # of allocated elements
	count++;
	reserved = count+1; // Reserved says size of last malloc
	// And copy elements to new memory
	//cout << "Reserved increased to " << reserved << endl;
	for(size_t i = 0; i < count; ++i)
	{
		// Is &newElements[0] properly initialized? If String, it'll seg fault
		// because need +1 entries.
		// Normally can't assume copy constructor exists or is called, but can here
		new(&newElements[i]) T(elements[i]); // Should call copy constructor for [0]
		// ^ a copy constructor b/c uses T's copy constructors
		// And call T's destructor on old element
		cout << "newElements[" << i << "] = " << &newElements[i] << endl;
		cout << "elements[" << i << "] = " << &elements[i] << " being freed" << endl;
		elements[i].~T();
		cout << "elements[" << i << "] is free" << endl;
	}
	cout << "oldElements = " << &elements << endl;
	free(elements);
	elements = newElements;
	cout << "newElements = " << &elements << endl;
  }
  //cout << "Ending Push_Back with count = " << count << ", reserved = " << reserved << endl;

}

template <typename T>
void Vector<T>::Push_Front(const T& rhs)
{
  if(reserved > count)
  { // Have enough memory, just copy existing and add to zeroth element
	for(int i = 0; i<count; ++i)
	{
		new(&elements[i+1]) T(elements[i]);
		// And delete original
		elements[i].~T();
	}
  }
  else
  { // Reallocate and move
	T* newElements = (T*)malloc((count+1)*sizeof(T));
	count++;
	reserved = count+1;
	for(size_t i = 0; i < count; ++i)
	{
		new(&newElements[i + 1]) T(elements[i]);
		// And delete original
		elements[i].~T();
	}
	free(elements);
	elements = newElements;
  }
  // And add new front
  new(&elements[0]) T(rhs);
}

template <typename T>
void Vector<T>::Pop_Back()
{ // Remove last element
}

template <typename T>
void Vector<T>::Pop_Front()
{ // Remove first element
}

// Element Access
template <typename T>
T& Vector<T>::Front() const
{
}

// Element Access
template <typename T>
T& Vector<T>::Back() const
{
}

template <typename T>
const T& Vector<T>::operator[](size_t i) const
{ // const element access
  return elements[i];
}

template <typename T>
T& Vector<T>::operator[](size_t i)
{//nonconst element access
  return elements[i];
}

template <typename T>
size_t Vector<T>::Size() const
{
  return count;
}

template <typename T>
bool Vector<T>::Empty() const
{
  if (count > 0) return true;
  else return false;
}

// Implement clear
template <typename T>
void Vector<T>::Clear()
{
  reserved = count;
  for(size_t i = 0; i < count; ++i)
  { // Call in-place destructor on each existing elements
	elements[count - i - 1].~T();
  }
  // Reset count, but keep memory
  count = 0;
}

// Iterator access functions
template <typename T>
VectorIterator<T> Vector<T>::Begin() const
{
  return VectorIterator<T>(elements);
}

template <typename T>
VectorIterator<T> Vector<T>::End() const
{
  return VectorIterator<T>(elements+count);
}

#ifdef GRAD_STUDENT
// Erase and insert
template <typename T>
void Vector<T>::Erase(const VectorIterator<T>& it)
{
}

template <typename T>
void Vector<T>::Insert(const T& rhs, const VectorIterator<T>& it)
{
}
#endif

// Implement the iterators

// Constructors
template <typename T>
VectorIterator<T>::VectorIterator()
{
}

template <typename T>
VectorIterator<T>::VectorIterator(T* c)
{
}

// Copy constructor
template <typename T>
VectorIterator<T>::VectorIterator(const VectorIterator<T>& rhs)
{
}

// Iterator defeferencing operator
template <typename T>
T& VectorIterator<T>::operator*() const
{
}

// Prefix increment
template <typename T>
VectorIterator<T>  VectorIterator<T>::operator++()
{
}

// Postfix increment
template <typename T>
VectorIterator<T> VectorIterator<T>::operator++(int)
{;
}

// Comparison operators
template <typename T>
bool VectorIterator<T>::operator !=(const VectorIterator<T>& rhs) const
{
}

template <typename T>
bool VectorIterator<T>::operator ==(const VectorIterator<T>& rhs) const
{
}




