/*
 *  HW4: Projections
 *  CSCI-5229 Fall 2016 Computer Graphics
 *  Instructor: Willem Schreuder
 *  Student: Bo Cao
 *  Email: boca7588@colorado.edu or bo.cao-1@colorado.edu
 *  
 *  Lighting
 *
 *  Demonstrates basic lighting using a sphere and a cube.
 *
 *  For first person navigation, please switch 
 * 
 *  Key bindings:
 *  First Person Navigation Tips: Using arrows & PgDn/PgUp
 *  arrows
 *      Up		  Move forward
 *      Down              Move backward
 *      Left              Rotate view point to left
 *      Right             Rotate view point to right
 *  PgDn/PgUp             Decrease/Increase horizontal level of view point
 *
 *  Orthogonal / Perspective Tips:
 *  w/W s/S            　　 Change view point up/down
 *  a/A d/D               Change view point to left/right
 *  1                     Change to Orthogonal Projection
 *  2                     Change to Perspective Projection
 *  3                     Change to First Person Project (Same as Perspective Project)
 *  p/P                   Toggles ortogonal/perspective projection
 *  +/-                   Change field of view of perspective
 *  x/X                   Toggle axes
 *  m/M                   Toggle Megaman
 *  c/C                   Toggle Cutman
 *  .			  Decrease world size
 *  ,			  Increase world size
 *  0                     Reset view angle(for all modes)
 *  ESC                   Exit
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int th=-55;         //  Azimuth of view angle
int ph=10;         //  Elevation of view angle
double zh=0;      //  Rotation of teapot
int toggleAxes=1;       //  Display axes
int toggleMegaman = 1; // 　Display megaman
int toggleCutman = 1; // 　Display cutman

int axes=1;       //  Display axes
int mode=1;       //  Projection mode
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double worldSize=3.0;   //  Size of world
double Ex = 0.0;
double Ey = 0.0;
double Ez = 0.0;
double eye_x = -5.0;
double eye_y = 1.0;
double eye_z = -2.0;
double reference_x = 6;
double reference_y = 0.5;
double reference_z = 3;
int eye_th = 0;

//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.1415927/180))
#define Sin(x) (sin((x)*3.1415927/180))

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  //  Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

void Project(int mode, double fov,double asp,double worldSize)
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();

   //  Perspective transformation
   if (mode && fov)
      gluPerspective(fov,asp,worldSize/16,16*worldSize);
   //  Orthogonal transformation
   else
      glOrtho(-asp*worldSize,asp*worldSize,-worldSize,+worldSize,-worldSize,+worldSize);

   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

void ErrCheck(const char* where)
{
   int err = glGetError();
   if (err) fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
}

/*
 *  Draw vertex in polar coordinates
 */
static void Vertex(double th,double ph)
{
   glColor3f(Cos(th)*Cos(th) , Sin(ph)*Sin(ph) , Sin(th)*Sin(th));
   glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
}

/*
 *  Draw a cube with color space 0 ~ 255 from (r, g, b)
 *     at (x,y,z)
 *     worldSizeentions (dx,dy,dz)
 *     rotated phi about the x axis
 *     rotated theta about the y axis
 *     rotated psi about the z axis
 */
static void cube_color(double x,double y,double z,
                 double dx,double dy,double dz,
                 double r, double g, double b,
                 double phi, double theta, double psi)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(phi,1,0,0);
   glRotated(theta,0,1,0);
   glRotated(psi,0,0,1);
   glScaled(dx,dy,dz);
   //scale color into 255 color space
   double sr = r / 255, sg = g / 255, sb = b / 255;
   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glColor3d(sr, sg, sb);
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   //  Back
   glColor3d(sr, sg, sb);
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glColor3d(sr, sg, sb);
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   //  Left
   glColor3d(sr, sg, sb);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,+1);
   glVertex3f(-1,+1,+1);
   glVertex3f(-1,+1,-1);
   //  Top
   glColor3d(sr, sg, sb);
   glVertex3f(-1,+1,+1);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(-1,+1,-1);
   //  Bottom
   glColor3d(sr, sg, sb);
   glVertex3f(-1,-1,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,-1,+1);
   glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Undo transformations
   glPopMatrix();
}

/*
 *  Draw a sphere
 *     at (x,y,z)
 *     radius (r)
 */
static void sphere(double x,double y,double z,double r)
{
   const int d=5;
   int th,ph;

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);

   //  South pole cap
   glBegin(GL_TRIANGLE_FAN);
   Vertex(0,-90);
   for (th=0;th<=360;th+=d)
   {
      Vertex(th,d-90);
   }
   glEnd();

   //  Latitude bands
   for (ph=d-90;ph<=90-2*d;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=d)
      {
         Vertex(th,ph);
         Vertex(th,ph+d);
      }
      glEnd();
   }

   //  North pole cap
   glBegin(GL_TRIANGLE_FAN);
   Vertex(0,90);
   for (th=0;th<=360;th+=d)
   {
      Vertex(th,90-d);
   }
   glEnd();

   //  Undo transformations
   glPopMatrix();
}

/*
 *  Draw a Megaman
 *     at (x, y, z)
 *     scale (ds)
 *     rotated phi about the x axis
 *     rotated theta about the y axis
 *     rotated psi about the z axis
 */ 
static void megaman(double x, double y, double z, double ds, double phi, double theta, double psi)
{
    // Save transformation
    glPushMatrix();

    // Offset and scale
    glTranslated(x, y, z);
    glRotated(phi, 1, 0, 0);
    glRotated(theta, 0, 1, 0);
    glRotated(psi, 0, 0, 1);
    glScaled(ds, ds, ds);

    // Head
    cube_color(0, 0.58, 0, 0.45, 0.45, 0.45, // x, y, z & dx, dy, dz
		256, 256, 256, 0, 0, 0); // r, g, b & phi, theta, psi 

    // Helmet
    // Top
    cube_color(0, 1.1, -0.03, 0.47, 0.13, 0.47, // x, y, z & dx, dy, dz
		77, 148, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0, 1.3, -0.06, 0.36, 0.1, 0.36, // x, y, z & dx, dy, dz
		77, 148, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0, 1, 0.45, 0.08, 0.1, 0.01, // x, y, z & dx, dy, dz
		77, 148, 255, 0, 0, 0); // r, g, b & phi, theta, psi  
    cube_color(0, 1.22, 0.4, 0.1, 0.1, 0.1, // x, y, z & dx, dy, dz
		102, 217, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0, 1.45, -0.1, 0.1, 0.1, 0.3, // x, y, z & dx, dy, dz
		102, 217, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0, 1.32, -0.46, 0.1, 0.1, 0.1, // x, y, z & dx, dy, dz
		102, 217, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0, 1.16, -0.56, 0.1, 0.1, 0.1, // x, y, z & dx, dy, dz
		102, 217, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0, 1, -0.68, 0.1, 0.1, 0.1, // x, y, z & dx, dy, dz
		102, 217, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    // Left
    cube_color(0.5, 0.6, 0, 0.07, 0.45, 0.45, // x, y, z & dx, dy, dz
		77, 148, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0.6, 0.6, -0.1, 0.07, 0.2, 0.2, // x, y, z & dx, dy, dz
		102, 217, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    // Right
    cube_color(-0.5, 0.6, 0, 0.07, 0.45, 0.45, // x, y, z & dx, dy, dz
		77, 148, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(-0.6, 0.6, -0.1, 0.07, 0.2, 0.2, // x, y, z & dx, dy, dz
		102, 217, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    // Back
    cube_color(0, 0.6, -0.5, 0.45, 0.45, 0.07, // x, y, z & dx, dy, dz
		77, 148, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0, 0.6, -0.6, 0.4, 0.4, 0.07, // x, y, z & dx, dy, dz
		77, 148, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0, 0.6, -0.7, 0.3, 0.3, 0.07, // x, y, z & dx, dy, dz
		77, 148, 255, 0, 0, 0); // r, g, b & phi, theta, psi
 
    // Eyes
    cube_color(0.2, 0.62, 0.45, 0.05, 0.2, 0.01, // x, y, z & dx, dy, dz
		0, 0, 0, 0, 0, 0); // r, g, b & phi, theta, psi  
    cube_color(-0.2, 0.62, 0.45, 0.05, 0.2, 0.01, // x, y, z & dx, dy, dz
		0, 0, 0, 0, 0, 0); // r, g, b & phi, theta, psi 

    // Mouth
    cube_color(0, 0.25, 0.48, 0.2, 0.05, 0.01, // x, y, z & dx, dy, dz
		0, 0, 0, 0, 0, 0); // r, g, b & phi, theta, psi     

    // Draw Neck
    cube_color(0, 0.2, 0, 0.1, 0.05, 0.1, // x, y, z & dx, dy, dz
		102, 217, 255, 0, 0, 0); // r, g, b & phi, theta, psi    

    // Draw Body
    cube_color(0, 0, 0, 0.25, 0.2, 0.15, // x, y, z & dx, dy, dz
		102, 217, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0, -0.27, 0, 0.19, 0.07, 0.1, // x, y, z & dx, dy, dz
		102, 217, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    
    // Draw Arms
    // Draw Left Arm
    cube_color(0.54, 0.02, 0, 0.3, 0.08, 0.08, // x, y, z & dx, dy, dz
		102, 217, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0.85, 0.02, 0, 0.05, 0.15, 0.15, // x, y, z & dx, dy, dz
		77, 148, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(1.23, 0.02, 0, 0.33, 0.23, 0.23, // x, y, z & dx, dy, dz
		77, 148, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(1.6, 0.02, 0, 0.05, 0.15, 0.15, // x, y, z & dx, dy, dz
		77, 148, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    sphere(1.6, 0.02, 0, 0.15);

    // Draw Right Arm
    cube_color(-0.32, 0.02, 0.2, 0.08, 0.08, 0.3, // x, y, z & dx, dy, dz
		102, 217, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(-0.32, 0.02, 0.55, 0.15, 0.15, 0.05, // x, y, z & dx, dy, dz
		77, 148, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(-0.32, 0.02, 0.93, 0.23, 0.23, 0.33, // x, y, z & dx, dy, dz
		77, 148, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(-0.32, 0.02, 1.3, 0.15, 0.15, 0.05, // x, y, z & dx, dy, dz
		77, 148, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    sphere(-0.32, 0.02, 1.3, 0.15);
    //Shoot
    sphere(-0.32, 0.02, 1.5 + zh / 15, 0.5);

    // Draw Underwear
    cube_color(0, -0.4, 0, 0.15, 0.08, 0.12, // x, y, z & dx, dy, dz
		77, 148, 255, 0, 0, 0); // r, g, b & phi, theta, psi

    // Draw Legs
    // Draw Left Leg
    cube_color(0.15, -0.6, 0, 0.08, 0.17, 0.08, // x, y, z & dx, dy, dz
		77, 195, 255, 0, 0, 25); // r, g, b & phi, theta, psi
    cube_color(0.25, -0.8, 0, 0.14, 0.08, 0.14, // x, y, z & dx, dy, dz
		26, 117, 255, 0, 0, 25); // r, g, b & phi, theta, psi
    cube_color(0.3, -0.9, 0, 0.2, 0.1, 0.2, // x, y, z & dx, dy, dz
		26, 117, 255, 0, 0, 25); // r, g, b & phi, theta, psi
    cube_color(0.41, -1.1, 0, 0.3, 0.15, 0.3, // x, y, z & dx, dy, dz
		26, 117, 255, 0, 0, 25); // r, g, b & phi, theta, psi
    cube_color(0.55, -1.4, 0, 0.4, 0.3, 0.4, // x, y, z & dx, dy, dz
		26, 117, 255, 0, 0, 25); // r, g, b & phi, theta, psi
    cube_color(0.65, -1.6, 0.1, 0.5, 0.1, 0.5, // x, y, z & dx, dy, dz
		26, 117, 255, 0, 0, 25); // r, g, b & phi, theta, psi

    // Draw Right Leg
    cube_color(-0.15, -0.6, 0, 0.08, 0.17, 0.08, // x, y, z & dx, dy, dz
		77, 195, 255, 0, 0, -25); // r, g, b & phi, theta, psi
    cube_color(-0.25, -0.8, 0, 0.14, 0.08, 0.14, // x, y, z & dx, dy, dz
		26, 117, 255, 0, 0, -25); // r, g, b & phi, theta, psi
    cube_color(-0.3, -0.9, 0, 0.2, 0.1, 0.2, // x, y, z & dx, dy, dz
		26, 117, 255, 0, 0, -25); // r, g, b & phi, theta, psi
    cube_color(-0.41, -1.1, 0, 0.3, 0.15, 0.3, // x, y, z & dx, dy, dz
		26, 117, 255, 0, 0, -25); // r, g, b & phi, theta, psi
    cube_color(-0.55, -1.4, 0, 0.4, 0.3, 0.4, // x, y, z & dx, dy, dz
		26, 117, 255, 0, 0, -25); // r, g, b & phi, theta, psi
    cube_color(-0.65, -1.6, 0.1, 0.5, 0.1, 0.5, // x, y, z & dx, dy, dz
		26, 117, 255, 0, 0, -25); // r, g, b & phi, theta, psi

    glPopMatrix();
}

/*
 *  Draw a cutter
 *     at (x, y, z)
 *     scale (ds)
 *     rotated phi about the x axis
 *     rotated theta about the y axis
 *     rotated psi about the z axis
 */ 
static void cutter(double x, double y, double z, double ds, double phi, double theta, double psi)
{
    // Save transformation
    glPushMatrix();

    // Offset and scale
    glTranslated(x, y, z);
    glRotated(phi, 1, 0, 0);
    glRotated(theta, 0, 1, 0);
    glRotated(psi, 0, 0, 1);
    glScaled(ds, ds, ds);

    // Right Blade
    cube_color(0.3, -0.5, 0, 0.1, 0.8, 0.02, // x, y, z & dx, dy, dz
		255, 255, 255, 0, 0, -20); // r, g, b & phi, theta, psi
    cube_color(0.47, -0.55, 0, 0.1, 0.6, 0.02, // x, y, z & dx, dy, dz
		255, 255, 255, 0, 0, -20); // r, g, b & phi, theta, psi
    cube_color(0.6, -0.6, 0, 0.1, 0.4, 0.02, // x, y, z & dx, dy, dz
		255, 255, 255, 0, 0, -20); // r, g, b & phi, theta, psi
    cube_color(0.72, -0.61, 0, 0.1, 0.2, 0.02, // x, y, z & dx, dy, dz
		255, 255, 255, 0, 0, -20); // r, g, b & phi, theta, psi

    // Left Blade
    cube_color(-0.3, -0.5, 0, 0.1, 0.8, 0.02, // x, y, z & dx, dy, dz
		255, 255, 255, 0, 0, 20); // r, g, b & phi, theta, psi
    cube_color(-0.47, -0.55, 0, 0.1, 0.6, 0.02, // x, y, z & dx, dy, dz
		255, 255, 255, 0, 0, 20); // r, g, b & phi, theta, psi
    cube_color(-0.6, -0.6, 0, 0.1, 0.4, 0.02, // x, y, z & dx, dy, dz
		255, 255, 255, 0, 0, 20); // r, g, b & phi, theta, psi
    cube_color(-0.72, -0.61, 0, 0.1, 0.2, 0.02, // x, y, z & dx, dy, dz
		255, 255, 255, 0, 0, 20); // r, g, b & phi, theta, psi
    glPopMatrix();
}

/*
 *  Draw a Cutman
 *     at (x, y, z)
 *     scale (ds)
 *     rotated phi about the x axis
 *     rotated theta about the y axis
 *     rotated psi about the z axis
 */ 
static void cutman(double x, double y, double z, double ds, double phi, double theta, double psi)
{
    // Save transformation
    glPushMatrix();

    // Offset and scale
    glTranslated(x, y, z);
    glRotated(phi, 1, 0, 0);
    glRotated(theta, 0, 1, 0);
    glRotated(psi, 0, 0, 1);
    glScaled(ds, ds, ds);

    // Head
    cube_color(0, 0.58, 0, 0.45, 0.45, 0.45, // x, y, z & dx, dy, dz
		255, 255, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0, 1.45, -0.1, 0.1, 0.1, 0.3, // x, y, z & dx, dy, dz
		255, 255, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0, 1.32, -0.46, 0.1, 0.1, 0.1, // x, y, z & dx, dy, dz
		255, 255, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0, 1.16, -0.56, 0.1, 0.1, 0.1, // x, y, z & dx, dy, dz
		255, 255, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0, 1, -0.68, 0.1, 0.1, 0.1, // x, y, z & dx, dy, dz
		255, 255, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    // Draw A Cutter On Top of The Head
    cutter(0, 2.1, 0, 0.5, 0, 3 * zh, 0);

    // Helmet
    // Top
    cube_color(0, 1.1, -0.03, 0.47, 0.13, 0.47, // x, y, z & dx, dy, dz
		255, 51, 51, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0, 1.3, -0.06, 0.36, 0.1, 0.36, // x, y, z & dx, dy, dz
		255, 51, 51, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0, 1, 0.45, 0.08, 0.1, 0.01, // x, y, z & dx, dy, dz
		255, 51, 51, 0, 0, 0); // r, g, b & phi, theta, psi  
    // Left
    cube_color(0.5, 0.6, 0, 0.07, 0.45, 0.45, // x, y, z & dx, dy, dz
		255, 51, 51, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0.6, 0.6, -0.1, 0.07, 0.2, 0.2, // x, y, z & dx, dy, dz
		191, 191, 191, 0, 0, 0); // r, g, b & phi, theta, psi
    // Right
    cube_color(-0.5, 0.6, 0, 0.07, 0.45, 0.45, // x, y, z & dx, dy, dz
		255, 51, 51, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(-0.6, 0.6, -0.1, 0.07, 0.2, 0.2, // x, y, z & dx, dy, dz
		191, 191, 191, 0, 0, 0); // r, g, b & phi, theta, psi
    // Back
    cube_color(0, 0.6, -0.5, 0.45, 0.45, 0.07, // x, y, z & dx, dy, dz
		255, 51, 51, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0, 0.6, -0.6, 0.4, 0.4, 0.07, // x, y, z & dx, dy, dz
		255, 51, 51, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0, 0.6, -0.7, 0.3, 0.3, 0.07, // x, y, z & dx, dy, dz
		255, 51, 51, 0, 0, 0); // r, g, b & phi, theta, psi
 
    // Eyes
    sphere(0.2, 0.62, 0.45, 0.18); 
    sphere(-0.2, 0.62, 0.45, 0.18); 

    // Mouth
    cube_color(0, 0.25, 0.48, 0.2, 0.05, 0.01, // x, y, z & dx, dy, dz
		0, 0, 0, 0, 0, 0); // r, g, b & phi, theta, psi     

    // Draw Neck
    cube_color(0, 0.2, 0, 0.1, 0.05, 0.1, // x, y, z & dx, dy, dz
		191, 191, 191, 0, 0, 0); // r, g, b & phi, theta, psi    

    // Draw Body
    cube_color(0, 0, 0, 0.25, 0.2, 0.15, // x, y, z & dx, dy, dz
		255, 255, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0, -0.27, 0, 0.19, 0.07, 0.1, // x, y, z & dx, dy, dz
		255, 255, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    
    // Draw Arms
    // Draw Left Arm
    cube_color(0.54, 0.02, 0, 0.3, 0.08, 0.08, // x, y, z & dx, dy, dz
		255, 255, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0.85, 0.02, 0, 0.05, 0.15, 0.15, // x, y, z & dx, dy, dz
		255, 51, 51, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(1.23, 0.02, 0, 0.33, 0.23, 0.23, // x, y, z & dx, dy, dz
		255, 51, 51, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(1.6, 0.02, 0, 0.05, 0.15, 0.15, // x, y, z & dx, dy, dz
		255, 51, 51, 0, 0, 0); // r, g, b & phi, theta, psi
    //cutter(-0.32, 0.05, 1.9, 0.5, 90, 3 * zh, 0);
    sphere(1.6, 0.02, 0, 0.15);
    // Shoot a Cutter
    cutter(-0.32 , 0.05 + Cos(zh * 2), 3 + Sin(zh * 2), 0.5, 90, 3 * zh, 0);

    // Draw Right Arm
    cube_color(-0.32, 0.02, 0.2, 0.08, 0.08, 0.3, // x, y, z & dx, dy, dz
		255, 255, 255, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(-0.32, 0.02, 0.55, 0.15, 0.15, 0.05, // x, y, z & dx, dy, dz
		255, 51, 51, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(-0.32, 0.02, 0.93, 0.23, 0.23, 0.33, // x, y, z & dx, dy, dz
		255, 51, 51, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(-0.32, 0.02, 1.3, 0.15, 0.15, 0.05, // x, y, z & dx, dy, dz
		255, 51, 51, 0, 0, 0); // r, g, b & phi, theta, psi
    cutter(2.25, 0.02, 0, 0.5, 3 * zh, 0, 270);
    sphere(-0.32, 0.02, 1.3, 0.15);

    // Draw Underwear
    cube_color(0, -0.4, 0, 0.15, 0.08, 0.12, // x, y, z & dx, dy, dz
		255, 51, 51, 0, 0, 0); // r, g, b & phi, theta, psi

    // Draw Legs
    // Draw Left Leg
    cube_color(0.15, -0.6, 0, 0.08, 0.17, 0.08, // x, y, z & dx, dy, dz
		255, 255, 255, 0, 0, 25); // r, g, b & phi, theta, psi
    cube_color(0.25, -0.8, 0, 0.14, 0.08, 0.14, // x, y, z & dx, dy, dz
		255, 51, 51, 0, 0, 25); // r, g, b & phi, theta, psi
    cube_color(0.3, -0.9, 0, 0.2, 0.1, 0.2, // x, y, z & dx, dy, dz
		255, 51, 51, 0, 0, 25); // r, g, b & phi, theta, psi
    cube_color(0.41, -1.1, 0, 0.3, 0.15, 0.3, // x, y, z & dx, dy, dz
		255, 51, 51, 0, 0, 25); // r, g, b & phi, theta, psi
    cube_color(0.55, -1.4, 0, 0.4, 0.3, 0.4, // x, y, z & dx, dy, dz
		255, 51, 51, 0, 0, 25); // r, g, b & phi, theta, psi
    cube_color(0.65, -1.6, 0.1, 0.5, 0.1, 0.5, // x, y, z & dx, dy, dz
		255, 51, 51, 0, 0, 25); // r, g, b & phi, theta, psi

    // Draw Right Leg
    cube_color(-0.15, -0.6, 0, 0.08, 0.17, 0.08, // x, y, z & dx, dy, dz
		255, 255, 255, 0, 0, -25); // r, g, b & phi, theta, psi
    cube_color(-0.25, -0.8, 0, 0.14, 0.08, 0.14, // x, y, z & dx, dy, dz
		255, 51, 51, 0, 0, -25); // r, g, b & phi, theta, psi
    cube_color(-0.3, -0.9, 0, 0.2, 0.1, 0.2, // x, y, z & dx, dy, dz
		255, 51, 51, 0, 0, -25); // r, g, b & phi, theta, psi
    cube_color(-0.41, -1.1, 0, 0.3, 0.15, 0.3, // x, y, z & dx, dy, dz
		255, 51, 51, 0, 0, -25); // r, g, b & phi, theta, psi
    cube_color(-0.55, -1.4, 0, 0.4, 0.3, 0.4, // x, y, z & dx, dy, dz
		255, 51, 51, 0, 0, -25); // r, g, b & phi, theta, psi
    cube_color(-0.65, -1.6, 0.1, 0.5, 0.1, 0.5, // x, y, z & dx, dy, dz
		255, 51, 51, 0, 0, -25); // r, g, b & phi, theta, psi

    glPopMatrix();
}

/*
 * Draw a ladder
 *	at (x, y, z)
 *      with length (dy)
 *      rotated phi about the x axis
 *      rotated theta about the y axis
 *      rotated psi about the z axis
 */
static void ladder(double x, double y, double z, double dy, double phi, double theta, double psi)
{
    // Save transformation
    glPushMatrix();
    
    // Offset and scale
    glTranslated(x, y, z);
    glRotated(phi, 1, 0, 0);
    glRotated(theta, 0, 1, 0);
    glRotated(psi, 0, 0, 1);
    glScaled(1, dy, 1);

    // Draw left stick
    cube_color(0.15, 0.15, 0, 0.02, 0.15, 0.02, // x, y, z & dx, dy, dz
		179, 179, 179, 0, 0, 0); // r, g, b & phi, theta, psi
    // Draw right stick
    cube_color(-0.15, 0.15, 0, 0.02, 0.15, 0.02, // x, y, z & dx, dy, dz
		179, 179, 179, 0, 0, 0); // r, g, b & phi, theta, psi

    cube_color(0, 0.05, 0, 0.15, 0.005, 0.02, // x, y, z & dx, dy, dz
		204, 204, 204, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0, 0.1, 0, 0.15, 0.005, 0.02, // x, y, z & dx, dy, dz
		204, 204, 204, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0, 0.15, 0, 0.15, 0.005, 0.02, // x, y, z & dx, dy, dz
		204, 204, 204, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0, 0.2, 0, 0.15, 0.005, 0.02, // x, y, z & dx, dy, dz
		204, 204, 204, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(0, 0.25, 0, 0.15, 0.005, 0.02, // x, y, z & dx, dy, dz
		204, 204, 204, 0, 0, 0); // r, g, b & phi, theta, psi

    glPopMatrix();
}

/*
 * Draw a background
 *	at (0, -0.2, 0)
 *
 */
static void background()
{
    // Save transformation
    glPushMatrix();

    // Draw a ground
    cube_color(0, -0.2, 0, 1.8, 0.2, 1.8, // x, y, z & dx, dy, dz
		26, 255, 140, 0, 0, 0); // r, g, b & phi, theta, psi

    // Draw cubes
    cube_color(-1, 0.2, 1, 0.2, 0.2, 0.2, // x, y, z & dx, dy, dz
		46, 184, 46, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(-1, 0.2, 1.5, 0.2, 0.2, 0.2, // x, y, z & dx, dy, dz
		46, 184, 46, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(-1, 0.5, 1.5, 0.2, 0.2, 0.2, // x, y, z & dx, dy, dz
		46, 184, 46, 0, 0, 0); // r, g, b & phi, theta, psi
    cube_color(1, 0.2, 1.5, 0.2, 0.2, 0.2, // x, y, z & dx, dy, dz
		46, 184, 46, 0, 0, 0); // r, g, b & phi, theta, psi

    // Draw level 2 floor
    cube_color(-1.3, 0.5, -0.6, 0.4, 0.05, 1, // x, y, z & dx, dy, dz
		71, 209, 71, 0, 0, 0); // r, g, b & phi, theta, psi
    ladder(-0.9, -0.05, -0.22, 2, 0, 90, 0);

    glPopMatrix();
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   const double len=1.5;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective - set eye position

   //  Orthogonal - set world orientation
   if (mode == 0) {
       glRotatef(ph,1,0,0);
       glRotatef(th,0,1,0);
   } else if (mode == 1) {
      Ex = -2*worldSize*Sin(th)*Cos(ph);
      Ey = +2*worldSize        *Sin(ph);
      Ez = +2*worldSize*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
	
   } else {
      //gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
      gluLookAt(eye_x, eye_y, eye_z, 
	reference_x * Cos(eye_th),reference_y,reference_z * Sin(eye_th),	
	0,1,0);
   }
   
   //  Draw a background
   background();

   // Draw Megaman
   if (toggleMegaman) {
      megaman(-1.3, 0.98, -0.5, 0.25, 0, 45, 0);
      megaman(0.3, 1, -1.2, 0.6, 0, 0, 0);
   }
   
   // Draw Cutman
   if (toggleCutman) {
      cutman(1.5, 0.45, 0.4, 0.25, 0, 225, 0);
      cutman(1, 0.56, 1.5, 0.1, 0, 180, 0);
   }

   //  White
   glColor3f(1,1,1);
   //  Draw axes
   if (toggleAxes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }

   //  Display parameters
   glWindowPos2i(5,5);
   if (mode == 0)
   	Print("Angle=%d,%d  worldSize=%.1f FOV=%d Projection=%s",th,ph,worldSize,fov,"Orthogonal");
   else if (mode == 1)
   	Print("Angle=%d,%d  worldSize=%.1f FOV=%d Projection=%s",th,ph,worldSize,fov,"Perpective");
   else if (mode == 2)
   	Print("Angle=%d,%d  worldSize=%.1f FOV=%d Projection=%s",th,ph,worldSize,fov,"First Person");

   //  Render the scene and make it visible
   ErrCheck("display");
   //  Render the scene
   glFlush();
   //  Make the rendered scene visible
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  First person navigation using arrows in perspective mode
   //  Right arrow key - rotate right view
   if (key == GLUT_KEY_RIGHT)
      eye_th += 10;
   //  Left arrow key - rotate left view
   else if (key == GLUT_KEY_LEFT)
      eye_th -= 10;
   //  Up arrow key - move forward
   else if (key == GLUT_KEY_UP) {
      //eye_x += 0.1;
      double delta_eye_x = reference_x * Cos(eye_th) - eye_x,
		delta_eye_z = reference_z * Sin(eye_th) - eye_z;
      double ratio = delta_eye_x / delta_eye_z;
      if (ratio > 0) {
      	eye_z += 0.1 * Sin(eye_th);
      	eye_x += 0.1 * ratio * Cos(eye_th);
	reference_z += 0.1 * Sin(eye_th);
      	reference_x += 0.1 * ratio * Cos(eye_th);
      } else {
        eye_z -= 0.1 * Sin(eye_th);
      	eye_x -= 0.1 * ratio * Cos(eye_th);
	reference_z -= 0.1 * Sin(eye_th);
      	reference_x -= 0.1 * ratio * Cos(eye_th);
      }
   }
   //  Down arrow key - move back
   else if (key == GLUT_KEY_DOWN) {
      double delta_eye_x = reference_x * Cos(eye_th) - eye_x,
		delta_eye_z = reference_z * Sin(eye_th) - eye_z;
      double ratio = delta_eye_x / delta_eye_z;
      if (ratio > 0) {
      	eye_z -= 0.1 * Sin(eye_th);
      	eye_x -= 0.1 * ratio * Cos(eye_th);
	reference_z -= 0.1 * Sin(eye_th);
      	reference_x -= 0.1 * ratio * Cos(eye_th);
      } else {
        eye_z += 0.1 * Sin(eye_th);
      	eye_x += 0.1 * ratio * Cos(eye_th);
	reference_z += 0.1 * Sin(eye_th);
      	reference_x += 0.1 * ratio * Cos(eye_th);
      }
   }
   //  PageUp key - increase eye horizontal level
   else if (key == GLUT_KEY_PAGE_DOWN)
      eye_y -= 0.1;
   //  PageDown key - decrease eye horizontal level
   else if (key == GLUT_KEY_PAGE_UP && worldSize>1)
      eye_y += 0.1;

   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   eye_th %= 360;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this toutine when there is nothing else to do
 */
void idle()
{
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360);
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0') {
      th = ph = 0;
      eye_x = -5.0;
      eye_y = 1.0;
      eye_z = -2.0;
      reference_x = 6;
      reference_y = 0.5;
      reference_z = 3;
      eye_th = 0;
   }
   //  Reset all parameters
   else if (ch == ' ') {
      th = ph = 0;
      Ex = Ey = Ez = 0;
   //  Toggle axes
   } else if (ch == 'x' || ch == 'X')
      toggleAxes = 1-toggleAxes;
   //  Toggle Megaman
   else if (ch == 'm' || ch == 'M')
      toggleMegaman = 1-toggleMegaman;
   //  Toggle Cutman
   else if (ch == 'c' || ch == 'C')
      toggleCutman = 1-toggleCutman;
   //  Switch projection mode
   else if (ch == 'p' || ch == 'P') {
      mode = mode + 1;
      mode %= 3;
   //  Change field of view angle
   } else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
   else if (ch == '1') mode = 0;
   else if (ch == '2') mode = 1;
   else if (ch == '3') mode = 2;
   //  d/D - increase angle by 5 degrees
   else if (ch == 'd' || ch == 'D')
      th += 5;
   //  a/A - decrease angle by 5 degrees
   else if (ch == 'a' || ch == 'A')
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (ch == 'w' || ch == 'W')
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (ch == 's' || ch == 'S')
      ph -= 5;
   //  ',' - increase worldSize
   else if (ch == ',')
      worldSize += 0.1;
   //  '.' - decrease worldSize
   else if (ch == '.' && worldSize>1)
      worldSize -= 0.1;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;

   //  Reproject   
   Project(mode,fov,asp,worldSize);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   const double worldSize=2.5;
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Ratio of the width to the height of the window
   double w2h = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal projection
   glOrtho(-w2h*worldSize,+w2h*worldSize, -worldSize,+worldSize, -worldSize,+worldSize);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   //  Set projection
   Project(mode,fov,asp,worldSize);
   glLoadIdentity();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitWindowSize(1000,800);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   //  Create the window
   glutCreateWindow("Bo Cao CSCI-5229 Computer Graphics Assignment 4");
   //  Tell GLUT to call "idle" when there is nothing else to do
   glutIdleFunc(idle);
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
   //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}
