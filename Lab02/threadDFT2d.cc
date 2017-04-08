// Threaded two-dimensional Discrete FFT transform
// Ryan Gentry 
// ECE6122 Project 2


#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#include "Complex.h"
#include "InputImage.h"

using namespace std;

// You will likely need global variables indicating how
// many threads there are, and a Complex* that points to the
// 2d image being transformed.
Complex* ImageData;
int	 ImageWidth;
int	 ImageHeight;
// Global variables threads use to count elements
int*	 counts;
int	 nThreads = 16;
// Mutex and condition variables allow main thread to know when
// all helper threads are completed
pthread_mutex_t	startCountMutex;
pthread_mutex_t	exitMutex;
pthread_cond_t	exitCond;
int 		startCount;
int		N;
// Barrier global variables
pthread_mutex_t countMutex;
bool*		localSense;
bool		globalSense;
int		count;
int		P;
// Millisecond clock function
int GetMillisecondClock()
{
  timeval tv;
  gettimeofday(&tv,0);
  static bool first = true;
  static int startSec = 0;
  if (first)
  {
	startSec = tv.tv_sec;
	first = false;
  }
  // Time in milliseconds
  return (tv.tv_sec - startSec)*1000 + tv.tv_usec / 1000;
}

// Function to reverse bits in an unsigned integer
// This assumes there is a global variable N that is the
// number of points in the 1D transform.
unsigned ReverseBits(unsigned v,int N)
{ //  Provided to students
  unsigned n = N; // Size of array (which is even 2 power k value)
  unsigned r = 0; // Return value
   
  for (--n; n > 0; n >>= 1)
    {
      r <<= 1;        // Shift return value
      r |= (v & 0x1); // Merge in next bit
      v >>= 1;        // Shift reversal value
    }
  return r;
}

// GRAD Students implement the following 2 functions.
// Undergrads can use the built-in barriers in pthreads.

// Call MyBarrier_Init once in main
void MyBarrier_Init(int P0)// you will likely need some parameters)
{
  P = P0;
  count = P;
  // Initialize the mutex use for FetchAndDecrement
  pthread_mutex_init(&countMutex,0);
  // Create and initialize the localSense array, 1 entry per thread
  localSense = new bool[P0];
  for (int i = 0; i < P; ++i) localSense[i] = true;
  // Initialize global sense as true
  globalSense = true;
}

int FetchAndDecrementCount()
{
  pthread_mutex_lock(&countMutex);
  int myCount = count;
  count--;
  pthread_mutex_unlock(&countMutex);
  return myCount;
}

// Each thread calls MyBarrier after completing the row-wise DFT
void MyBarrier(int myId) // Again likely need parameters
{
  // Toggle private sense variable
  localSense[myId] = !localSense[myId];
  // If this is the last thread to check in, reset global P to nThreads
  // and set globalSense = whatever last localSense variable was set to
  if(FetchAndDecrementCount() == 1)
  {
	count = P;
	globalSense = localSense[myId];
  }
  else
  {
	while(globalSense != localSense[myId]) { } // let it spin
  }
}
                    
void Transform1D(Complex* h, int N)
{
  // Implement the efficient Danielson-Lanczos DFT here.
  // "h" is an input/output parameter
  // "N" is the size of the array (assume even power of 2)
  Complex W[N/2];
  unsigned r;
  for(int n = 0; n < N; ++n)
  {
	r = ReverseBits((unsigned)n,N);
	if ( r > n)
	{
		swap(h[n],h[r]);
	}
	if ( n < N/2)
	{
		W[n] = Complex(cos(2*M_PI*n/N),-sin(2*M_PI*n/N));
	}
  }
  // Now h is re-ordered and all W have been calculated
  // Initialize array H to hold old h values while in compute loop
  Complex H[N];
  // Loop through 2-pt transforms, to 4-pt, to 8-pt...1024-pt
  for (int size = 2; size <= 1024; size = size*2)
  {
	// Perform N/size transforms (512 2-pters, 256 4-pters,..., 1 1024-pters
	for (int n = 0; n < 1024; n = n+size)
	{
		// Store h[n] to h[n+size] in H for calculations
		for (int i = 0; i < size; ++i)
		{
			H[i] = h[n+i];
		}
		// Perform calculation at each point h[n] to h[n+size]
		for (int j = 0; j < size; ++j)
		{
			// 1st half of calcs vary with j as seen, with positive W
			if (j < size/2)
			{
				h[n+j] = H[j] + W[j*N/size]*H[j+size/2];
			}
			// 2nd half of calcs vary with j as seen, with negative W
			else
			{
				h[n+j] = H[j-size/2] - W[j*N/size-N/2]*H[j];
			}
		}
	}
  }
}

void Transform1D_inv(Complex* h, int N)
{
  // Implement the efficient Danielson-Lanczos DFT here.
  // "h" is an input/output parameter
  // "N" is the size of the array (assume even power of 2)
  Complex W[N/2];
  unsigned r;
  for(int n = 0; n < N; ++n)
  {
	r = ReverseBits((unsigned)n,N);
	if ( r > n)
	{
		swap(h[n],h[r]);
	}
	if ( n < N/2)
	{
		W[n] = Complex(cos(2*M_PI*n/N),sin(2*M_PI*n/N));
	}
  }
  // Now h is re-ordered and all W have been calculated
  // Initialize array H to hold old h values while in compute loop
  Complex H[N];
  float Ni = (float)1/(float)N;
  // Loop through 2-pt transforms, to 4-pt, to 8-pt...1024-pt
  for (int size = 2; size <= 1024; size = size*2)
  {
	// Perform N/size transforms (512 2-pters, 256 4-pters,..., 1 1024-pters
	for (int n = 0; n < 1024; n = n+size)
	{
		// Store h[n] to h[n+size] in H for calculations
		for (int i = 0; i < size; ++i)
		{
			H[i] = h[n+i];
		}
		// Perform calculation at each point h[n] to h[n+size]
		for (int j = 0; j < size; ++j)
		{
			// 1st half of calcs vary with j as seen, with positive W
			if (j < size/2)
			{
				h[n+j] = H[j] + W[j*N/size]*H[j+size/2];
			}
			// 2nd half of calcs vary with j as seen, with negative W
			else
			{
				h[n+j] = H[j-size/2] - W[j*N/size-N/2]*H[j];
			}
			if (size == N)
			{
				h[n+j] = h[n+j]*Ni;
			}
		}
	}
  }
}

// Flip rows and columns of ImageData
void Transpose()
{
	// X keeps track of which row on ImageData program is on
	for (int x = 0; x < ImageWidth; ++x)
	{
		// Y keeps track of which column in row x program is on
		for (int y = x + 1; y < ImageHeight; ++y)
		{
			// Stay in upper diagonal of ImageData, and swap current item with its opposite
			// across diagonal
			swap(ImageData[y*ImageWidth+x],ImageData[x*ImageWidth+y]);			
		}
	}
}
void* Transform2DTHread(void* v)
{ // This is the thread startign point.  "v" is the thread number
  // Calculate 1d DFT for assigned rows
  // wait for all to complete
  // Calculate 1d DFT for assigned columns
  // Decrement active count and signal main if all complete
  // myId = Thread Number
  unsigned long myId = (unsigned long)v;
  // Evenly distribute rows to each thread
  int rowsPerThread = ImageHeight/nThreads;
  int startingRow = myId * rowsPerThread;
  for (int i = startingRow; i < (startingRow + rowsPerThread); ++i)
  {
	Transform1D(ImageData+i*ImageWidth, ImageWidth);
  }
  // After 1D transform is completed of assigned rows, enter Barrier
  MyBarrier(myId);
  // This thread has now finished, decrement active count and
  // see if all have finished
  pthread_mutex_lock(&startCountMutex);
  startCount--;
  if(startCount == 0)
  {
	// Last thread to exit notifies main
	pthread_mutex_unlock(&startCountMutex);
	pthread_mutex_lock(&exitMutex);
	pthread_cond_signal(&exitCond);
	pthread_mutex_unlock(&exitMutex);
  }
  else
  {
	pthread_mutex_unlock(&startCountMutex);
  }
  return 0;
}

void* Transform2D_inv_THread(void* v)
{ // This is the thread startign point.  "v" is the thread number
  // Calculate 1d DFT for assigned rows
  // wait for all to complete
  // Calculate 1d DFT for assigned columns
  // Decrement active count and signal main if all complete
  // myId = Thread Number
  unsigned long myId = (unsigned long)v;
  // Evenly distribute rows to each thread
  int rowsPerThread = ImageHeight/nThreads;
  int startingRow = myId * rowsPerThread;
  //cout << "Thread #" << myId << " started at row #" << startingRow << endl;
  for (int i = startingRow; i < (startingRow + rowsPerThread); ++i)
  {
	Transform1D_inv(ImageData+i*ImageWidth, ImageWidth);
  }
  // After 1D transform is completed of assigned rows, enter Barrier
  MyBarrier(myId);
  // This thread has now finished, decrement active count and
  // see if all have finished
  //cout << "Thread #" << myId << " shutting down..." << endl;
  pthread_mutex_lock(&startCountMutex);
  startCount--;
  if(startCount == 0)
  {
	// Last thread to exit notifies main
	pthread_mutex_unlock(&startCountMutex);
	pthread_mutex_lock(&exitMutex);
	pthread_cond_signal(&exitCond);
	pthread_mutex_unlock(&exitMutex);
  }
  else
  {
	pthread_mutex_unlock(&startCountMutex);
  }
  return 0;
}

void Transform2D(const char* inputFN) 
{ // Do the 2D transform here.
  InputImage image(inputFN);  // Create the helper object for reading the image
  // Create the global pointer to the image array data
  // Create 16 threads
  // Wait for all threads complete
  // Write the transformed data
  ImageData = image.GetImageData();
  ImageWidth = image.GetWidth();
  ImageHeight = image.GetHeight();
  cout << "Image imported with width = " << ImageWidth << " & height = " << ImageHeight << endl;
  counts = new int[nThreads];
  //Initialize all mutex and condition variables
  pthread_mutex_init(&exitMutex,0);
  pthread_mutex_init(&startCountMutex,0);
  pthread_cond_init(&exitCond,0);
  // Hold the exit Mutex until waiting for exitCond condition
  pthread_mutex_lock(&exitMutex);
  // Create Barrier
  MyBarrier_Init(nThreads);
  // Get start time
  GetMillisecondClock();
  startCount = nThreads;
  // Now start the threads
  for (int i = 0; i < nThreads; ++i)
  {
	// pThread variable (output param from create)
	pthread_t pt;
	// 3rd param=thread starting fxn, 4th=passed to thread starting fxn
	pthread_create(&pt,0,Transform2DTHread,(void*)i);
  }
  // Main program waits until all child threads completed
  pthread_cond_wait(&exitCond,&exitMutex);
  // Save newly transformed data as MyAfter1D.txt
  string f1 = "MyAfter1D.txt";
  image.SaveImageData(f1.c_str(),ImageData,ImageWidth,ImageHeight);
  cout << f1 << " was saved!" << endl;
  // Transpose newly transformed data
  Transpose();
  // Create 16 new threads & have them Transform1D
  startCount = nThreads;
  for (int i = 0; i < nThreads; ++i)
  {
	// pThread variable (output param from create)
	pthread_t pt;
	// 3rd param=thread starting fxn, 4th=passed to thread starting fxn
	pthread_create(&pt,0,Transform2DTHread,(void*)i);
  }
  // Main program waits until all child threads completed
  pthread_cond_wait(&exitCond,&exitMutex);
  // Transpose newly transformed data
  Transpose();
  // Save transformed & transposed data as MyAfter2D.txt
  string f2 = "MyAfter2D.txt";
  image.SaveImageData(f2.c_str(),ImageData,ImageWidth,ImageHeight);
  cout << f2 << " was saved!" << endl;
  // Create 16 new threads & have them Transform1D_inv
  startCount = nThreads;
  for (int i = 0; i < nThreads; ++i)
  {
	// pThread variable (output param from create)
	pthread_t pt;
	// 3rd param=thread starting fxn, 4th=passed to thread starting fxn
	pthread_create(&pt,0,Transform2D_inv_THread,(void*)i);
  }
  // Main program waits until all child threads completed
  pthread_cond_wait(&exitCond,&exitMutex);
  // Transpose newly transformed data
  Transpose();
  // Create 16 new threads & have them Transform1D_inv
  startCount = nThreads;
  for (int i = 0; i < nThreads; ++i)
  {
	// pThread variable (output param from create)
	pthread_t pt;
	// 3rd param=thread starting fxn, 4th=passed to thread starting fxn
	pthread_create(&pt,0,Transform2D_inv_THread,(void*)i);
  }
  // Main program waits until all child threads completed
  pthread_cond_wait(&exitCond,&exitMutex);
  // Transpose newly transformed data
  Transpose();
  // Save transformed & transposed data as MyAfterInverse.txt
  string fi = "MyAfterInverse.txt";
  image.SaveImageDataReal(fi.c_str(),ImageData,ImageWidth,ImageHeight);
  cout << fi << " was saved!" << endl;

  cout << "All threads have completed! Elapsed time = " << (float)GetMillisecondClock()/1000 << " s" << endl;

}

int main(int argc, char** argv)
{
  string fn("Tower.txt"); // default file name
  if (argc > 1) fn = string(argv[1]);  // if name specified on cmd line
  // MPI initialization here
  Transform2D(fn.c_str()); // Perform the transform.
}  
  

  
