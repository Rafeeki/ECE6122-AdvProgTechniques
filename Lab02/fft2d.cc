// Distributed two-dimensional Discrete FFT transform
// Ryan Gentry
// ECE8893 Project 1


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <signal.h>
#include <math.h>
#include <mpi.h>
#include <stdlib.h>
#include "Complex.h"
#include "InputImage.h"

using namespace std;

void Transform1D(Complex* h, int w, Complex* H);
void iTransform1D(Complex* h, int w, Complex* H);

void Transform2D(const char* inputFN) 
{
  // Do the 2D transform here.
  // 1) Use the InputImage object to read in the Tower.txt file and
  //    find the width/height of the input image.
  // 2) Use MPI to find how many CPUs in total, and which one
  //    this process is
  // 3) Allocate an array of Complex object of sufficient size to
  //    hold the 2d DFT results (size is width * height)
  // 4) Obtain a pointer to the Complex 1d array of input data
  // 5) Do the individual 1D transforms on the rows assigned to your CPU
  // 6) Send the resultant transformed values to the appropriate
  //    other processors for the next phase.
  // 6a) To send and receive columns, you might need a separate
  //     Complex array of the correct size.
  // 7) Receive messages from other processes to collect your columns
  // 8) When all columns received, do the 1D transforms on the columns
  // 9) Send final answers to CPU 0 (unless you are CPU 0)
  //   9a) If you are CPU 0, collect all values from other processors
  //       and print out with SaveImageData().
  // Step (1) in the comments is the line above.
  // Your code here, steps 2-9

	// Intialize variables
  	int nCpus, myRank, w, nRows, sz, start, end, rc;
	
	// Create the helper object for reading the image
	InputImage image(inputFN);  			
	
	// Get initial dimensions for image
	w = image.GetWidth();
	nRows = image.GetHeight();
	sz = w*nRows;
	
	// Create Complex pointers for image (pimg) & end array (buff)
	Complex* pimg = image.GetImageData();
	Complex buff[sz];

	// Determine # of CPUs (numtasks) and which one this is (rank)
	MPI_Comm_size(MPI_COMM_WORLD,&nCpus);
	MPI_Comm_rank(MPI_COMM_WORLD,&myRank);
	start = (nRows/nCpus)*myRank;
	end = (nRows/nCpus)*(myRank+1);

	for(int f = 0; f < 4; ++f)
	{
		// Perform DTF row-by-row and store in buff

		{	
					}
		if (myRank != 0)
		{
			if (f >= 1)
			{
				MPI_Status status;
				rc = MPI_Recv(pimg+start*w,nCpus*w,MPI_DOUBLE_COMPLEX,0,f,MPI_COMM_WORLD,&status);
				if (rc != MPI_SUCCESS)
				{
					cout << "Rank " << myRank << " send failed, rc "<< rc << endl;
					MPI_Finalize();
					exit(1);
				}
				cout << "CPU # " << myRank << " received vals " << pimg[start*w] << "; " << pimg[(end-1)*w] << endl;
			}
			for(int i = start; i < end; ++i)
			{
				if (f <= 1)	
				{
					Transform1D(pimg+i*w, w, buff+i*w);
				}
				else
				{
					iTransform1D(pimg+i*w, w, buff+i*w);
				}
			}
			rc = MPI_Send(buff+start*w, nCpus*w, MPI_DOUBLE_COMPLEX, 0, f, MPI_COMM_WORLD);
			if (rc != MPI_SUCCESS)
			{
				cout << "Rank " << myRank << " send failed, rc "<< rc << endl;
				MPI_Finalize();
				exit(1);
			}
			cout << "CPU # " << myRank << " sending vals " << buff[start*w] << "; " << buff[(end-1)*w] << endl;
		}
		//Use CPU #0 as "traffic cop" 
		else
		{
			if (f <= 1)	
			{
				Transform1D(pimg, w, buff);
			}
			else
			{
				iTransform1D(pimg, w, buff);
			}
			for(int r = 1; r < nCpus; ++r)
			{
				if (f <= 1)	
				{
					Transform1D(pimg+r*w, w, buff+r*w);
				}
				else
				{
					iTransform1D(pimg+r*w, w, buff+r*w);
				}

				if (f >= 1)
				{
					rc = MPI_Send(pimg+r*nCpus*w,nCpus*w,MPI_DOUBLE_COMPLEX,r,f,MPI_COMM_WORLD);
					if( rc != MPI_SUCCESS)
					{
						cout << "Rank " << myRank << " recv failed, rc " << rc << endl;
						MPI_Finalize();
						exit(1);
					}
					cout << "CPU #0 sent to CPU #" << r << " vals " 
					     << pimg[r*nCpus*w] << "; " << pimg[r*(nCpus+1)*w-1] << endl;
				}
				MPI_Status status;
				rc = MPI_Recv(buff+r*nCpus*w,nCpus*w, MPI_DOUBLE_COMPLEX,r,f,MPI_COMM_WORLD,&status);
				if( rc != MPI_SUCCESS)
				{
					cout << "Rank " << myRank << " recv failed, rc " << rc << endl;
					MPI_Finalize();
					exit(1);
				}
				cout << "CPU #0 received from CPU #" << status.MPI_SOURCE << " vals " 
				     << buff[status.MPI_SOURCE*nCpus*w] << "; " << buff[status.MPI_SOURCE*(nCpus+1)*w-1] << endl;
			}
			if ( f <=1)
			{
				if ( f == 0)
				{
					string f0 = "MyAfter1D.txt";
					image.SaveImageData(f0.c_str(),buff,w,nRows);
					cout << f0 << " has been saved!" << endl;
				}
				for (int x = 0; x < w; ++x)
				{
					for (int y = 0; y < nRows; ++y)
					{
						pimg[y*w+x] = buff[x*w+y] ;		
					}	
				}
				if ( f == 1)
				{
					string f1 = "MyAfter2D.txt";
					image.SaveImageData(f1.c_str(),pimg,w,nRows);
					cout << f1 << " has been saved!" << endl;
				}
			}
			else
			{
				if ( f == 2)
				{
					string f2 = "MyAfter2Di.txt";
					image.SaveImageData(f2.c_str(),buff,w,nRows);
					cout << f2 << " has been saved!" << endl;
				}
				for (int x = 0; x < w; ++x)
				{
					for (int y = 0; y < nRows; ++y)
					{
						pimg[y*w+x] = buff[x*w+y] ;		
					}	
				}
				if ( f == 3)
				{
					string f3 = "MyAfterInverse.txt";
					image.SaveImageDataReal(f3.c_str(),pimg,w,nRows);
					cout << f3 << " has been saved!" << endl;
				}

			}
		}

	}
}

void Transform1D(Complex* h, int w, Complex* H)
{
  // Implement a simple 1-d DFT using the double summation equation
  // given in the assignment handout.  h is the time-domain input
  // data, w is the width (N), and H is the output array.
	
        //Initialize variables and calculate W (only depends on W)
	Complex W;

        //Iterate through each position n in h/H
        for (int n = 0; n < w; ++n)
        {
		H[n] = h[0];
                for (int k = 1; k < w; ++k)
                {
        		W = Complex(cos(2*M_PI*n*k/w),-sin(2*M_PI*n*k/w));
			H[n] = H[n] + W*h[k]; 
                }
        }


}

void iTransform1D(Complex* h, int w, Complex* H)
{
  // Implement a simple 1-d DFT using the double summation equation
  // given in the assignment handout.  h is the time-domain input
  // data, w is the width (N), and H is the output array.
	
        //Initialize variables and calculate W (only depends on W)
	float N = (float)1/(float)w;
	Complex W;
        //Iterate through each position n in h/H
        for (int n = 0; n < w; ++n)
        {
                //When k=0, W^(nk) = 1, so H[n, k=0] = 1*h[0]   
                H[n] = h[0];
                //Iterate through each position k in h/H
                for (int k = 1; k < w; ++k)
                {
                        //When k = 1, H[n,k=1] = 1*h[0] + (W^n)*h[1]
        		W = Complex(cos(2*M_PI*n*k/w),sin(2*M_PI*n*k/w));
			H[n] = H[n] + W*h[k]; 
                }
                H[n] = H[n]*N;
        }



}


int main(int argc, char** argv)
{
	string fn("Tower.txt"); // default file name
	if (argc > 1) fn = string(argv[1]);  // if name specified on cmd line
  // MPI initialization here
	int rc;
	rc = MPI_Init(&argc,&argv);
	if (rc != MPI_SUCCESS) {
		printf("Error starting MPI program. Terminating.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}

	Transform2D(fn.c_str()); // Perform the transform.
  // Finalize MPI here
	MPI_Finalize();
}  
