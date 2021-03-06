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
		for(int i = start; i < end; ++i)
		{	
			//Use CPU #0 as "traffic cop" 
			if(myRank == 0)
			{
				//Transform a row 0-15, store in buff 0-3839
	//			if(f == 1) {
	//				cout << "buff[0," << i+1 << "] = " << buff[i+1] << " and buff[" << i+1 << ",0] = " 
	//				     << buff[(i+1)*w] << endl; 
	//			}
				if (f <= 1)	
				{
					Transform1D(pimg+i*w, w, buff+i*w);
				}
				else
				{
					iTransform1D(pimg+i*w, w, buff+i*w);

				}

	//			cout << "CPU #0 stored buff row #" << i << " from CPU #0 (val = " 
	//			     << buff[i*w+15] << "; " << buff[i*w+17] << ")" << endl; 
				//MPI_Send next buff section to each CPU, and receive it back 
				for (int r = 1; r < nCpus; ++r)
				{
					//r*nCpus = start row for CPU #r
					//r*nCpus + i = current row for CPU #r
					//(r*nCpus +i)*w = starting memory location for CPU #r
				//	MPI_Request request;
	//				cout << "CPU #0 sent buff row #" << (r*nCpus+i) << " to CPU #" << r
	//				     << " (val = " << buff[(r*nCpus+i)*w+15] << "; "
	//				     << buff[(r*nCpus+i)*w+17] << ")" << endl;
					rc = MPI_Send(pimg+(r*nCpus+i)*w, w, MPI_DOUBLE_COMPLEX,r,f,MPI_COMM_WORLD);
					if (rc != MPI_SUCCESS)
					{
						cout << "Rank " << myRank << " send failed, rc "<< rc << endl;
						MPI_Finalize();
						exit(1);
					}
					MPI_Status status;
					rc = MPI_Recv(buff+(r*nCpus+i)*w, w,MPI_DOUBLE_COMPLEX,MPI_ANY_SOURCE,f,MPI_COMM_WORLD,&status);
					if( rc != MPI_SUCCESS)
					{
						cout << "Rank " << myRank << " recv failed, rc " << rc << endl;
						MPI_Finalize();
						exit(1);
					}
//					cout << "CPU #" << myRank << " received buff row #" << r*nCpus+i << " to CPU #" << r << "(val = " 
//					     << buff[(r*nCpus+i)*w+0] << "; " << buff[(r*nCpus+i)*w+255] << ")" << endl;
				}
			}
			//All CPUs 1-15 receive buffer from CPU 0, perform transform, and send back 
			else
			{
				MPI_Status status;
				//i = start which is already offset by rank, so no need to add r*nCpus to it like for CPU #0 
//				while(f == 1 && status.MPI_TAG != 1)
//				{
//					rc = MPI_Recv(buff+i*w, w,MPI_DOUBLE_COMPLEX,MPI_ANY_SOURCE,f,MPI_COMM_WORLD,&status);
//					cout << "Rank #" << myRank << " is waiting!!!" << endl;
//				}
				rc = MPI_Recv(pimg+i*w, w,MPI_DOUBLE_COMPLEX,MPI_ANY_SOURCE,f,MPI_COMM_WORLD,&status);
	//			cout << "CPU #" << myRank << " received buff row #" << i << " from CPU #0 (val = " 
	//			     << buff[i*w+15] << "; " << buff[i*w+17] << ")" << endl;
				if( rc != MPI_SUCCESS)
				{
					cout << "Rank " << myRank << " recv failed, rc " << rc << endl;
					MPI_Finalize();
					exit(1);
				}
				if (f <= 1)
				{	
					Transform1D(pimg+i*w, w,buff+i*w );
				}
				else
				{
					iTransform1D(pimg+i*w, w,buff+i*w );

				}
//				cout << "CPU #" << myRank << " sent buff row #" << i << " to CPU #0 (val = " 
//				     << buff[i*w+0] << "; " << buff[i*w+255] << ")" << endl;
				MPI_Send(buff+i*w,w,MPI_DOUBLE_COMPLEX,0,f,MPI_COMM_WORLD);
			}
		}
		//Transpose buff so that after 1st time through we can use same logic above to do column-wise transform
		//and after 2nd time through we can revert the matrix back to normal for saving 
		if(myRank == 0)
		{
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
        Complex W = Complex(cos(2*M_PI/w),-sin(2*M_PI/w));
        Complex W_n, W_k, summ;

        //Iterate through each position n in h/H
        for (int n = 0; n < w; ++n)
        {
                //W^0 = 1, so when n=0, W^n = 1
                W_n = 1;
                //When n>0, need to raise W to iterative power via self-multiplication
                for (int i = 0; i < n; ++i)
                {
                        W_n = W_n*W;
                }

                //When k=0, W^(nk) = 1, so H[n, k=0] = 1*h[0]   
                W_k = h[0];
                //Set summ = W^n, so summ^k = (W^n)^k via self-multiplication
                summ = W_n;
                //Iterate through each position k in h/H
                for (int k = 1; k < w; ++k)
                {
                        //When k = 1, H[n,k=1] = 1*h[0] + (W^n)*h[1]
                        W_k = W_k + summ*h[k];
                        //When k = 2, H[n,k=2] = H[n,k=1] + ((W^n)^2)*h[2] 
                        summ = W_n*summ;
                }
                H[n] = W_k;
        }


}

void iTransform1D(Complex* h, int w, Complex* H)
{
  // Implement a simple 1-d DFT using the double summation equation
  // given in the assignment handout.  h is the time-domain input
  // data, w is the width (N), and H is the output array.
	
        //Initialize variables and calculate W (only depends on W)
        Complex W = Complex(cos(2*M_PI/w),sin(2*M_PI/w));
        Complex W_n, W_k, summ;
	float N = (float)1/(float)w;

        //Iterate through each position n in h/H
        for (int n = 0; n < w; ++n)
        {
                //W^0 = 1, so when n=0, W^n = 1
                W_n = 1;
                //When n>0, need to raise W to iterative power via self-multiplication
                for (int i = 0; i < n; ++i)
                {
                        W_n = W_n*W;
                }

                //When k=0, W^(nk) = 1, so H[n, k=0] = 1*h[0]   
                W_k = h[0];
                //Set summ = W^n, so summ^k = (W^n)^k via self-multiplication
                summ = W_n;
                //Iterate through each position k in h/H
                for (int k = 1; k < w; ++k)
                {
                        //When k = 1, H[n,k=1] = 1*h[0] + (W^n)*h[1]
                        W_k = W_k + summ*h[k];
                        //When k = 2, H[n,k=2] = H[n,k=1] + ((W^n)^2)*h[2] 
                        summ = W_n*summ;
                }
                H[n] = W_k*N;
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
