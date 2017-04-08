// Draw an Icosahedron
// ECE4893/8893 Project 4
// YOUR NAME HERE

#include <iostream>
#include <math.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include <GL/glu.h>

using namespace std;

#define NFACE 20
#define NVERTEX 12 // Not 60 b/c they share, 1 vertex is used multiple times

// X & Z are constants, Y is what changes
#define X .525731112119133606
#define Z .850650808352039932

// These are the 12 vertices for the icosahedron
// 3 Vertices make a face that's a triangle
static GLfloat vdata[NVERTEX][3] = {    
   {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},    
   {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},    
   {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0} 
};

// These are the 20 faces.  Each of the three entries for each 
// vertex gives the 3 vertices that make the face.
// Values in each {} are indices into vdata
static GLint tindices[NFACE][3] = { 
   {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},    
   {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},    
   {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
   {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };

int testNumber; // Global variable indicating which test number is desired

// Initialize rotation, dRotation,  updateRate and maxDepth values
static GLfloat rotX = 0.0;
static GLfloat rotY = 0.0;
static GLfloat rotZ = 0.0;
static GLfloat dX = 1.0;
static GLfloat dY = 0.8;
static GLfloat dZ = 0.6;
static int updateRate = 40;
static GLuint maxDepth = 4;

// Helpers
void normalize(GLfloat v[3])
{ // Calculate the Euclidean norm of v, and divide all dimensions of v by it if non-zero
  GLfloat d = sqrt(v[0] * v[0] +
		   v[1] * v[1] +
		   v[2] * v[2]);
  if (d == 0.0) return;
  v[0] /= d;
  v[1] /= d;
  v[2] /= d; 
}

void debugExtend(GLfloat* v, GLfloat extend)
{ // Calculate the Euclidean norm of v, and divide all dimensions of v by it times extend
  return; // Why return here??
  GLfloat mag = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  v[0] /= mag * extend;
  v[1] /= mag * extend;
  v[2] /= mag * extend;
}

void drawTriangle(GLfloat* v1, GLfloat* v2, GLfloat* v3)
{ // Inputs point to an array of 3 vertices (3 coordinates each), which makes a face
  glBegin(GL_TRIANGLES); // The next 3 vertices define a triangle
  glColor3f(1.0, 1.0, 1.0); // White triangles
  glColor3f(0.0, 0.0, 1.0); // Blue triangles
  // glNormal gives the normal vector to the face of the triangle, used for lighting & reflection
  // Need 3fv b/c v's point to 3 coordinates each, different from 2f from circle
  // Program would probably work without this section
  glNormal3fv(v1); glVertex3fv(v1); 
  glNormal3fv(v2); glVertex3fv(v2);
  glNormal3fv(v3); glVertex3fv(v3);
  glEnd();
  // Now draw the three lines connecting the vertices to create the triangle face
  glBegin(GL_LINE_LOOP); // Endpoint of last line should automatically connect at start of next line
  // glColor3f(0.0, 0.0, 0.0); // Black lines
  glColor3f(1.0, 1.0, 1.0); // White lines 
  glNormal3fv(v1); glVertex3fv(v1);
  glNormal3fv(v2); glVertex3fv(v2);
  glNormal3fv(v3); glVertex3fv(v3);
  glEnd();
}

void subDivide(GLfloat* v1, GLfloat* v2, GLfloat* v3, int depth)
{
  if (depth == 0)
  {
	drawTriangle(v1, v2, v3);
	return;
  }
  // Find the midpoint of each triangle side
  GLfloat v12[3];
  GLfloat v23[3];
  GLfloat v31[3];
  for (int i = 0; i < 3; ++i)
  { // Should divide by 2, but ok b/c handed in normalize
	v12[i] = v1[i] + v2[i];
	v23[i] = v2[i] + v3[i];
	v31[i] = v3[i] + v1[i];
  }
  normalize(v12);
  normalize(v23);
  normalize(v31);
  // Now draw the smaller triangles
  subDivide(v1, v12, v31, depth-1); // X coordinate, and midpoint between X&Y and X&Z
  subDivide(v2, v23, v12, depth-1); // Y coordinate, and midpoint between X&Y and Y&Z
  subDivide(v3, v31, v23, depth-1); // Z coordinate, and midpoint between X&Z and Y&Z
  subDivide(v12, v23, v31, depth-1); // subdividing each original triangle creates 4 new ones
}


void drawIcosahedronOld()
{
  // glBegin(GL_TRIANGLES);
  glBegin(GL_LINE_LOOP);
  for (int = 0; i < NFACE; i++)
  { // Color transformation here if needed
	glNormal3fv(&vdata[tindices[i][0]][0]);
	glVertex3fv(&vdata[tindices[i][0]][0]);
	glNormal3fv(&vdata[tindices[i][1]][0]);
	glVertex3fv(&vdata[tindices[i][1]][0]);
	glNormal3fv(&vdata[tindices[i][2]][0]);
	glVertex3fv(&vdata[tindices[i][2]][0]);

  }
  glEnd();
}

void drawIcosahedron(int maxDepth)
{ // Doesn't draw anything since we're using recursion
  for (int i = 0; i < NFACE; i++)
  { // Set different color transformation here if needed
    subDivide(&vdata[tindices[i][0]][0],
    		 &vdata[tindices[i][1]][0],
  		 &vdata[tindices[i][2]][0],
    		 maxDepth); 
  }
}

void display(void)
{
  glEnable(GL_LINE_SMOOTH); //enable anti-aliasing
  // clear all
  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);
  // Enable depth buffer
  glEnable(GL_DEPTH_TEST);
  // Draw white icosahedron
  glcolor3f(1.0, 1.0, 1.0);
  // Clear the matrix
  glLoadIdentity();
  // Set the viewing transformation
  // gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); 
  gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); // Y value is "up" 
  glPushMatrix();
  // glRotatef(rotX, rotY, rotZ, 0.0);
  glTranslate(winW/Z, winH/Z, 0); // Where do winW & winH come from?
  glScalef(winW, winW, winW);
  glRotatef(rotX, rotY, rotZ, 1.0);
  drawIcosahedron(maxDepth); // doesn't draw anything, just makes subdivisions and recurses
  // glut SolidIcosahedron();
  // glColor3f(0.0, 0.0, 0.0); // change color to black
  // glutWireIcosahedron();
  glPopMatrix();
  rotX += dX;
  rotY += dY;
  rotZ += dZ;
  // Flush buffer
  glFlush();
}

void init()
{ // select clearing (background) color
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glEnable(GL_LINE_SMOOTH);
}



// Test cases.  Fill in your code for each test case
void Test1()
{ 
}

void Test2()
{
}

void Test3()
{
}

void Test4()
{
}

void Test5(int depth)
{
}

void Test6(int depth)
{
}

void reshape(int w, int h)
{
  winW = w;
  winH = h;
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // glFrustum(-1.0, 1.0, -1.0, 1.0, 2.0, 20.0);
  glOrtho(0, w, 0, h, -w, w);
  glMatrixMode(GL_MODELVIEW);
}

void timer(int)
{
  glutPostRedisplay();
  glutTimerFunc(1000.0/updateRate, timer,0);
}

int main(int argc, char** argv)
{
  if (argc < 2)
    {
      std::cout << "Usage: icosahedron testnumber" << endl;
      exit(1);
    }
  // Set the global test number
  testNumber = atol(argv[1]);
  // Initialize glut  and create your window here
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_Double | GLUT_RGB);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Icosahedron");
  // Set your glut callbacks here
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutTimerFunc(1000.0 / updateRate, timer, 0);
  // Enter the glut main loop here
  glutMainLoop();
  return 0;
}

