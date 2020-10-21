#include "GL/freeglut.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include "vecmath.h"
using namespace std;

// Globals

// This is the list of points (3D vectors)
vector<Vector3f> vecv;

// This is the list of normals (also 3D vectors)
vector<Vector3f> vecn;

// This is the list of faces (indices into vecv and vecn)
vector<vector<unsigned> > vecf;


// You will need more global variables to implement color and position changes
uint8_t currentColor = 0;
const uint8_t maxColors = 4;

Vector3f lightPos = { 1.f, 1.f, 5.f };
const float lightPosChangeValuePerKey = 0.5f;

// These are convenience functions which allow us to call OpenGL 
// methods on Vec3d objects
inline void glVertex(const Vector3f &a) 
{ glVertex3fv(a); }

inline void glNormal(const Vector3f &a) 
{ glNormal3fv(a); }

void draw3DObject()
{
    glPushMatrix();
    glRotatef(rotationAngle, 0.f, 1.f, 0.f);
    for (auto face : vecf)
    {
        glBegin(GL_TRIANGLES);
        glNormal3d(vecn[face[2] - 1][0], vecn[face[2] - 1][1], vecn[face[2] - 1][2]);
        glVertex3d(vecv[face[0] - 1][0], vecv[face[0] - 1][1], vecv[face[0] - 1][2]);
        glNormal3d(vecn[face[5] - 1][0], vecn[face[5] - 1][1], vecn[face[5] - 1][2]);
        glVertex3d(vecv[face[3] - 1][0], vecv[face[3] - 1][1], vecv[face[3] - 1][2]);
        glNormal3d(vecn[face[8] - 1][0], vecn[face[8] - 1][1], vecn[face[8] - 1][2]);
        glVertex3d(vecv[face[6] - 1][0], vecv[face[6] - 1][1], vecv[face[6] - 1][2]);
        glEnd();
    }
    glPopMatrix();
}

// This function is called whenever a "Normal" key press is received.
void keyboardFunc( unsigned char key, int x, int y )
{
    switch ( key )
    {
    case 27: // Escape key
        exit(0);
        break;
    case 'c':
        // add code to change color here
        currentColor = (currentColor + 1) % maxColors;
		cout << "Handled key press " << key << "." << endl; 
        break;
    default:
        cout << "Unhandled key press " << key << "." << endl;        
    }

	// this will refresh the screen so that the user sees the color change
    glutPostRedisplay();
}

// This function is called whenever a "Special" key press is received.
// Right now, it's handling the arrow keys.
void specialFunc( int key, int x, int y )
{
    auto val = lightPosChangeValuePerKey;

    switch ( key )
    {
    case GLUT_KEY_UP:
        lightPos[1] += val;
		cout << "Handled key press: up arrow." << endl;
		break;
    case GLUT_KEY_DOWN:
        lightPos[1] -= val;
		cout << "Handled key press: down arrow." << endl;
		break;
    case GLUT_KEY_LEFT:
        lightPos[0] -= val;
		cout << "Handled key press: left arrow." << endl;
		break;
    case GLUT_KEY_RIGHT:
        lightPos[0] += val;
		cout << "Handled key press: right arrow." << endl;
		break;
    }

	// this will refresh the screen so that the user sees the light position
    glutPostRedisplay();
}

// This function is responsible for displaying the object.
void drawScene(void)
{
    int i;

    // Clear the rendering window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rotate the image
    glMatrixMode( GL_MODELVIEW );  // Current matrix affects objects positions
    glLoadIdentity();              // Initialize to the identity

    // Position the camera at [0,0,5], looking at [0,0,0],
    // with [0,1,0] as the up direction.
    gluLookAt(0.0, 0.0, 5.0,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);

    // Set material properties of object

	// Here are some colors you might use - feel free to add more
    GLfloat diffColors[maxColors][maxColors] = { 
                                  {0.5, 0.5, 0.9, 1.0},
                                  {0.9, 0.5, 0.5, 1.0},
                                  {0.5, 0.9, 0.3, 1.0},
                                  {0.3, 0.8, 0.9, 1.0} 
    };
    
	// Here we use the first color entry as the diffuse color
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffColors[currentColor]);

	// Define specular color and shininess
    GLfloat specColor[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shininess[] = {100.0};

	// Note that the specular color and shininess can stay constant
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  
    // Set light properties

    // Light color (RGBA)
    GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
    // Light position
    GLfloat Lt0pos[] = { lightPos[0], lightPos[1], lightPos[2], 1.0f };

    glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

	// This GLUT method draws a teapot.  You should replace
	// it with code which draws the object you loaded.
	//glutSolidTeapot(1.0);
    draw3DObject();
    
    // Dump the image to the screen.
    glutSwapBuffers();

}


// Initialize OpenGL's rendering modes
void initRendering()
{
    glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
    glEnable(GL_LIGHTING);     // Enable lighting calculations
    glEnable(GL_LIGHT0);       // Turn on light #0.
}

// Called when the window is resized
// w, h - width and height of the window in pixels.
void reshapeFunc(int w, int h)
{
    // Always use the largest square viewport possible
    if (w > h) {
        glViewport((w - h) / 2, 0, h, h);
    } else {
        glViewport(0, (h - w) / 2, w, w);
    }

    // Set up a perspective view, with square aspect ratio
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 50 degree fov, uniform aspect ratio, near = 1, far = 100
    gluPerspective(50.0, 1.0, 1.0, 100.0);
}

void parseFace(std::vector<unsigned>& face, std::string s)
{
    string delimiter = "/";
    std::string token;

    size_t pos = 0;

    while ((pos = s.find(delimiter)) != std::string::npos)
    {
        token = s.substr(0, pos);
        std::cout << token << std::endl;
        face.push_back(std::stoi(token));
        s.erase(0, pos + delimiter.length());
    }

    face.push_back(std::stoi(s));

}

void loadInput()
{
    const int MAX_BUFFER_SIZE = 128;
    char buffer[MAX_BUFFER_SIZE];
    while (cin.getline(buffer, MAX_BUFFER_SIZE))
    {
        stringstream ss(buffer);
        string s;

        ss >> s;
        if (s == "v")
        {
            Vector3f v;
            ss >> v[0] >> v[1] >> v[2];

            vecv.push_back(v);
        }

        if (s == "vn")
        {
            Vector3f v;
            ss >> v[0] >> v[1] >> v[2];
            vecn.push_back(v);
        }

        if (s == "f")
        {
            vector<unsigned> face;
            string group;
            while (ss >> group)
            {
                parseFace(face, group);
            }

            vecf.push_back(face);
        }
    }
}


// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char** argv )
{
    loadInput();

    glutInit(&argc,argv);

    // We're going to animate it, so double buffer 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 360, 360 );
    glutCreateWindow("Assignment 0");

    // Initialize OpenGL parameters.
    initRendering();

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys

     // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop( );

    return 0;	// This line is never reached.
}
