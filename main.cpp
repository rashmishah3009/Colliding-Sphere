#include <string.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <ctime>
using namespace std;

bool WireFrame = false;


const GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

float sphere1_x = -1.5;             //Initial position of sphere 1
float sphere2_x = 0.5;              //Initial position of sphere 2
float sphere3_z = -1.5;             //Initial position of sphere 3
float sphere4_z = 0.5;              //Initial position of sphere 4

float sphere1_speed = 0.00001;      //Speed of sphere 1 movement
float sphere2_speed = 0.00001;      //Speed of sphere 2 movement
float sphere3_speed = 0.00001;      //Speed of sphere 3 movement
float sphere4_speed = 0.00001;      //Speed of sphere 4 movement

float zoom = -5.0f;                 //Initial zoom value
float rotate_y = 0;                 //Initial position of cube




float distance(float x1, float y1, float z1, float x2, float y2, float z2)    //Calculating distance between 2 spheres
{
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + (z1 - z2) * (z1 - z2));
}

struct Sphere {                                       //Initial specifications of cube
    double x, y, z;
    double vx, vy, vz;
    double r, g, b;                                   //RGB color values for the sphere

    Sphere() : r(0.5), g(1.0), b(1.5) {}              //Default color for all spheres
};

bool checkSphereCollision(const Sphere& sphere1, const Sphere& sphere2, double sphere_radius) {     //Check if spheres collide
    float dist = distance(sphere1.x, sphere1.y, sphere1.z, sphere2.x, sphere2.y, sphere2.z);
    return dist < 2 * sphere_radius;
}

Sphere spheres[4];                                    //Array for storing spheres information

void randomColor(double& r, double& g, double& b)     //Selecting a random color for spheres
{
    r = static_cast<double>(rand()) / RAND_MAX;
    g = static_cast<double>(rand()) / RAND_MAX;
    b = static_cast<double>(rand()) / RAND_MAX;
}

/* GLUT callback Handlers */

static void resize(int width, int height)
{
    if (width <= height)
        glViewport(0, (GLsizei)(height - width) / 2, (GLsizei)width, (GLsizei)width);
    else
        glViewport((GLsizei)(width - height) / 2, 0, (GLsizei)height, (GLsizei)height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0f, 1, 0.1f, 100.0f);
}


void Collisions() {                                    //Reverting spheres when collided
    float radius = 0.5;                                //Radius of Spheres

    for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 4; j++) {
            if (checkSphereCollision(spheres[i], spheres[j], radius)) {      //Collision detection between 2 spheres
                double dx = spheres[i].x - spheres[j].x;
                double dy = spheres[i].y - spheres[j].y;
                double dz = spheres[i].z - spheres[j].z;
                double distance = sqrt(dx * dx + dy * dy + dz * dz);

                double mtd = (2 * radius - distance) / distance;             //Calculate distance for collision

                spheres[i].vx *= -1;                   //Reversing first sphere's motion
                spheres[i].vy *= -1;
                spheres[i].vz *= -1;

                spheres[j].vx *= -1;                   //Reversing second sphere's motion
                spheres[j].vy *= -1;
                spheres[j].vz *= -1;
            }
        }
    }
}

static void display(void)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0, 5, 20 + zoom, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    if (WireFrame)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);     //Draw Our Mesh In Wireframe Mesh
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);     //Toggle WIRE FRAME


    glPushMatrix();                                    //Drawing the cube
    glRotatef(rotate_y, 0.0f, 1.0f, 0.0f);
    glColor3f(1, 1, 1);
    glScalef(1.0, 1.0, 1.0);
    glutWireCube(4.0);
    glPopMatrix();

    float radius = 0.5;                                //Radius of Spheres

    Collisions();

    for (int i = 0; i < 4; i++) {
        spheres[i].x += spheres[i].vx;                 //Update sphere positions
        spheres[i].y += spheres[i].vy;
        spheres[i].z += spheres[i].vz;


        if (spheres[i].x + radius >= 2.0 || spheres[i].x - radius <= -2.0)   //Check collision with cube wall
            spheres[i].vx *= -1;                                             //If collided reverse the sphere's motion
        if (spheres[i].y + radius >= 2.0 || spheres[i].y - radius <= -2.0)
            spheres[i].vy *= -1;
        if (spheres[i].z + radius >= 2.0 || spheres[i].z - radius <= -2.0)
            spheres[i].vz *= -1;

        for (int j = 0; j < 4; j++) {
            if (i != j) {
                if (checkSphereCollision(spheres[i], spheres[j], radius)) {  //Check sphere's collision with other sphere
                    randomColor(spheres[i].r, spheres[i].g, spheres[i].b);   //If collided change color
                    randomColor(spheres[j].r, spheres[j].g, spheres[j].b);
                }
            }
        }

        glPushMatrix();                                //Drawing spheres
        glColor3f(spheres[i].r, spheres[i].g, spheres[i].b);
        glTranslatef(spheres[i].x, spheres[i].y, spheres[i].z);
        glScalef(1.0, 1.0, 1.0);
        glutSolidSphere(radius, 16, 16);
        glPopMatrix();
    }

    glutSwapBuffers();
}

static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:
    case 'q':
        exit(0);
        break;
    }
}

void Specialkeys(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP:         //If up arrow key if pressed
        zoom -= 0.5;          // Zoom in
        break;
    case GLUT_KEY_DOWN:       //If down arrow key if pressed
        zoom += 0.5;          //Zoom out
        break;
    case GLUT_KEY_LEFT:       //If left arrow key if pressed
        rotate_y -= 5;        //Rotate left along y-axis
        break;
    case GLUT_KEY_RIGHT:      //If right arrow key if pressed
        rotate_y += 5;        //Rotate right along y-axis
        break;
    }
    glutPostRedisplay();
}

static void idle(void)
{
    glutPostRedisplay();
}

static void init(void)
{
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_DEPTH_TEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glShadeModel(GL_SMOOTH);

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);


    for (int i = 0; i < 4; i++) {            //Initial position and motion of sphere
        spheres[i].x = (rand() / static_cast<double>(RAND_MAX) - 0.5) * 3.0;
        spheres[i].y = (rand() / static_cast<double>(RAND_MAX) - 0.5) * 3.0;
        spheres[i].z = (rand() / static_cast<double>(RAND_MAX) - 0.5) * 3.0;
        spheres[i].vx = (rand() / static_cast<double>(RAND_MAX) - 0.5) * 0.01;
        spheres[i].vy = (rand() / static_cast<double>(RAND_MAX) - 0.5) * 0.01;
        spheres[i].vz = (rand() / static_cast<double>(RAND_MAX) - 0.5) * 0.01;
    }
}

/* Program entry point */

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);

    glutInitWindowSize(800, 600);
    glutInitWindowPosition(10, 10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Colliding Spheres");
    init();
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(Specialkeys);

    glutIdleFunc(idle);
    glutMainLoop();

    return EXIT_SUCCESS;
}
