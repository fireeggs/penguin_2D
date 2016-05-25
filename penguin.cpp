/***********************************************************
             CSC418/2504, Fall 2009
  
                 penguin.cpp
                 
       Simple demo program using OpenGL and the glut/glui 
       libraries

  
    Instructions:
        Please read the assignment page to determine 
        exactly what needs to be implemented.  Then read 
        over this file and become acquainted with its 
        design.

        Add source code where it appears appropriate. In
        particular, see lines marked 'TODO'.

        You should not need to change the overall structure
        of the program. However it should be clear what
        your changes do, and you should use sufficient comments
        to explain your code.  While the point of the assignment
        is to draw and animate the character, you will
        also be marked based on your design.

***********************************************************/

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glui.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef _WIN32
#include <unistd.h>
#else
void usleep(unsigned int nanosec)
{
    Sleep(nanosec / 1000);
}
#endif


// *************** GLOBAL VARIABLES *************************


const float PI = 3.14159;

// --------------- USER INTERFACE VARIABLES -----------------

// Window settings
int windowID;               // Glut window ID (for display)
GLUI *glui;                 // Glui window (for controls)
int Win[2];                 // window (x,y) size


// ---------------- ANIMATION VARIABLES ---------------------

// Animation settings
int animate_mode = 0;       // 0 = no anim, 1 = animate
int animation_frame = 0;      // Specify current frame of animation

// Joint parameters
const float JOINT_MIN = -45.0f;
const float JOINT_MAX =  45.0f;
const float JOINT_HEAD_MIN = -20.0f;
const float JOINT_HEAD_MAX = 10.0f;
const float JOINT_FRONT_LEG_MIN = -25.0f;
const float JOINT_FRONT_LEG_MAX = 10.0f;
const float JOINT_BACK_LEG_MIN = -40.0f;
const float JOINT_BACK_LEG_MAX = 10.0f;
const float JOINT_FRONT_FOOT_MIN = -30.0f;
const float JOINT_FRONT_FOOT_MAX = 15.0f;
const float JOINT_BACK_FOOT_MIN = -70.0f;
const float JOINT_BACK_FOOT_MAX =10.0f;
const float UP_DOWN_LOWER_BEAK_MIN = 0.0f;
const float UP_DOWN_LOWER_BEAK_MAX = 10.0f;


//////////////////////////////////////////////////////
// TODO: Add additional joint parameters here
//////////////////////////////////////////////////////
float joint_rot = 0.0f;
float head_joint_rot = 0.0f;
float up_down_lower_beak = 0.0f;
float front_leg_joint = 0.0f;
float back_leg_joint = 0.0f;
float front_foot_joint = 0.0f;
float back_foot_joint = 0.0f;

// ***********  FUNCTION HEADER DECLARATIONS ****************


// Initialization functions
void initGlut(char* winName);
void initGlui();
void initGl();


// Callbacks for handling events in glut
void myReshape(int w, int h);
void animate();
void display(void);

// Callback for handling events in glui
void GLUI_Control(int id);


// Functions to help draw the object
void drawSquare(float size);
void drawCircle(float size);



// Return the current system clock (in seconds)
double getTime();


// ******************** FUNCTIONS ************************



// main() function
// Initializes the user interface (and any user variables)
// then hands over control to the event handler, which calls 
// display() whenever the GL window needs to be redrawn.
int main(int argc, char** argv)
{

    // Process program arguments
    if(argc != 3) {
        printf("Usage: demo [width] [height]\n");
        printf("Using 300x200 window by default...\n");
        Win[0] = 600;
        Win[1] = 500;
    } else {
        Win[0] = atoi(argv[1]);
        Win[1] = atoi(argv[2]);
    }


    // Initialize glut, glui, and opengl
    glutInit(&argc, argv);
    initGlut(argv[0]);
    initGlui();
    initGl();

    // Invoke the standard GLUT main event loop
    glutMainLoop();

    return 0;         // never reached
}


// Initialize glut and create a window with the specified caption 
void initGlut(char* winName)
{
    // Set video mode: double-buffered, color, depth-buffered
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Create window
    glutInitWindowPosition (0, 0);
    glutInitWindowSize(Win[0],Win[1]);
    windowID = glutCreateWindow(winName);

    // Setup callback functions to handle events
    glutReshapeFunc(myReshape); // Call myReshape whenever window resized
    glutDisplayFunc(display);   // Call display whenever new frame needed 
}


// Quit button handler.  Called when the "quit" button is pressed.
void quitButton(int)
{
  exit(0);
}

// Animate button handler.  Called when the "animate" checkbox is pressed.
void animateButton(int)
{
  // synchronize variables that GLUT uses
  glui->sync_live();

  animation_frame = 0;
  if(animate_mode == 1) {
    // start animation
    GLUI_Master.set_glutIdleFunc(animate);
  } else {
    // stop animation
    GLUI_Master.set_glutIdleFunc(NULL);
  }
}

// Initialize GLUI and the user interface
void initGlui()
{
    GLUI_Master.set_glutIdleFunc(NULL);

    // Create GLUI window
    glui = GLUI_Master.create_glui("Glui Window", 0, Win[0]+10, 0);

    // Create a control to specify the rotation of the joint

    ///////////////////////////////////////////////////////////
    // TODO: 
    //   Add controls for additional joints here
    ///////////////////////////////////////////////////////////
    
    GLUI_Spinner *joint_spinner
        = glui->add_spinner("Arm Joint", GLUI_SPINNER_FLOAT, &joint_rot);
    joint_spinner->set_speed(0.2);
    joint_spinner->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);
    
	GLUI_Spinner *head_joint_rot_spinner
        = glui->add_spinner("Head Joint", GLUI_SPINNER_FLOAT, &head_joint_rot);
    head_joint_rot_spinner->set_speed(0.1);
    head_joint_rot_spinner->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);

	GLUI_Spinner *up_down_lower_beak_trans
        = glui->add_spinner("up down lower beak", GLUI_SPINNER_FLOAT, &up_down_lower_beak);
    up_down_lower_beak_trans->set_speed(0.1);
    up_down_lower_beak_trans->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);

	GLUI_Spinner *front_leg_joint_trans
        = glui->add_spinner("front leg joint", GLUI_SPINNER_FLOAT, &front_leg_joint);
    front_leg_joint_trans->set_speed(0.8);
    front_leg_joint_trans->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);
    
    GLUI_Spinner *back_leg_joint_trans
        = glui->add_spinner("back leg joint", GLUI_SPINNER_FLOAT, &back_leg_joint);
    back_leg_joint_trans->set_speed(0.8);
    back_leg_joint_trans->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);
    
    GLUI_Spinner *front_foot_joint_trans
        = glui->add_spinner("front foot joint", GLUI_SPINNER_FLOAT, &front_foot_joint);
    front_foot_joint_trans->set_speed(0.1);
    front_foot_joint_trans->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);
    
     GLUI_Spinner *back_foot_joint_trans
        = glui->add_spinner("back foot joint", GLUI_SPINNER_FLOAT, &back_foot_joint);
    back_foot_joint_trans->set_speed(0.1);
    back_foot_joint_trans->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);
        
    // Add button to specify animation mode 
    glui->add_separator();
    glui->add_checkbox("Animate", &animate_mode, 0, animateButton);

    // Add "Quit" button
    glui->add_separator();
    glui->add_button("Quit", 0, quitButton);

    // Set the main window to be the "active" window
    glui->set_main_gfx_window(windowID);
}


// Performs most of the OpenGL intialization
void initGl(void)
{
    // glClearColor (red, green, blue, alpha)
    // Ignore the meaning of the 'alpha' value for now
    glClearColor(0.7f,0.7f,0.9f,1.0f);
}




// Callback idle function for animating the scene
void animate()
{
    // Update geometry
    const double joint_rot_speed = 0.1;
    double joint_rot_t = (sin(animation_frame*joint_rot_speed) + 1.0) / 2.0;
    joint_rot = joint_rot_t * JOINT_MIN + (1 - joint_rot_t) * JOINT_MAX;
    
    ///////////////////////////////////////////////////////////
    // TODO:
    //   Modify this function animate the character's joints
    //   Note: Nothing should be drawn in this function!  OpenGL drawing
    //   should only happen in the display() callback.
    ///////////////////////////////////////////////////////////
	head_joint_rot = -(joint_rot_t * JOINT_HEAD_MIN + (1 - joint_rot_t) * JOINT_HEAD_MAX);

	up_down_lower_beak = joint_rot_t * UP_DOWN_LOWER_BEAK_MIN + (1 - joint_rot_t) * UP_DOWN_LOWER_BEAK_MAX;

	front_leg_joint = joint_rot_t * JOINT_FRONT_LEG_MIN + (1 - joint_rot_t) * JOINT_FRONT_LEG_MAX;
	
	back_leg_joint = -(joint_rot_t * JOINT_BACK_LEG_MIN + (1 - joint_rot_t) * JOINT_BACK_LEG_MAX);
	
	front_foot_joint = joint_rot_t * JOINT_FRONT_FOOT_MIN + (1 - joint_rot_t) * JOINT_FRONT_FOOT_MAX;
	
	back_foot_joint = -(joint_rot_t * JOINT_BACK_FOOT_MIN + (1 - joint_rot_t) * JOINT_BACK_FOOT_MAX);
	
    // Update user interface
    glui->sync_live();

    // Tell glut window to update itself.  This will cause the display()
    // callback to be called, which renders the object (once you've written
    // the callback).
    glutSetWindow(windowID);
    glutPostRedisplay();

    // increment the frame number.
    animation_frame++;

    // Wait 50 ms between frames (20 frames per second)
    usleep(50000);
}


// Handles the window being resized by updating the viewport
// and projection matrices
void myReshape(int w, int h)
{
    // Setup projection matrix for new window
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-w/2, w/2, -h/2, h/2);

    // Update OpenGL viewport and internal variables
    glViewport(0,0, w,h);
    Win[0] = w;
    Win[1] = h;
}



// display callback
//
// This gets called by the event handler to draw
// the scene, so this is where you need to build
// your scene -- make your changes and additions here.
// All rendering happens in this function.  For Assignment 1,
// updates to geometry should happen in the "animate" function.
void display(void)
{
    // glClearColor (red, green, blue, alpha)
    // Ignore the meaning of the 'alpha' value for now
    glClearColor(0.7f,0.7f,0.9f,1.0f);

    // OK, now clear the screen with the background colour
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

    // Setup the model-view transformation matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    ///////////////////////////////////////////////////////////
    // TODO:
    //   Modify this function draw the scene
    //   This should include function calls to pieces that
    //   apply the appropriate transformation matrice and
    //   render the individual body parts.
    ///////////////////////////////////////////////////////////

    // Draw our hinged object
    const float BODY_WIDTH = 30.0f;
    const float BODY_LENGTH = 40.0f;
    const float ARM_LENGTH = 40.0f;
    const float ARM_WIDTH = 20.0f;
    const float HEAD_WIDTH = 30.0f;
    const float HEAD_LENGTH = 30.0f;
    const float UPPER_BEAK_WIDTH = 30.0f;
    const float UPPER_BEAK_LENGTH = 10.0f;
    const float LOWER_BEAK_WIDTH = 30.0f;
    const float LOWER_BEAK_LENGTH = 15.0f;
    const float LEG_WIDTH = 20.0f;
    const float LEG_LENGTH = 40.0f;
    const float FOOT_WIDTH = 20.0f;
    const float FOOT_LENGTH = 40.0f;

    // Push the current transformation matrix on the stack
    glPushMatrix();
        
        // Draw the 'body'
 		glPushMatrix();
            // Scale the size of 'body'
            glScalef(BODY_WIDTH, BODY_LENGTH, 1.0);
            
            // Set the Colour of 'body'
			glColor3f(1.0, 0, 1.0);
			
			// Begin drawing of polygon for 'body'
			glBegin(GL_POLYGON);
				glVertex3f(-0.7f,0.5f,0.0f);//first vertex
				glVertex3f(0.7f,0.5f,0.0f);//second vertex
				glVertex3f(2.0f,-2.5f,0.0f);//third vertex
				glVertex3f(1.0f,-3.5f,0.0f);//fourth vertex
				glVertex3f(-0.7f,-3.5f,0.0f);//fifth vertex
				glVertex3f(-1.7f,-2.5f,0.0f);//sixth vertex
			glEnd();
			// End drawing of polygon for 'body'

        glPopMatrix();
        
        glPushMatrix();
			// Draw the 'head'
			// Fixed and Rotate along the hinge
			glTranslatef(0.0, BODY_LENGTH/2 - 0.2, 0.0);
			glRotatef(head_joint_rot, 0.0, 0.0, 1.0);
				
				glPushMatrix();
			
					// Scale size of 'head'
					glScalef(HEAD_LENGTH, HEAD_WIDTH, 1.0);
					
					// Set the colour of 'head'
					glColor3f(0, 0, 1.0);
					
					// Begin drawing of polygon for 'head'
					glBegin(GL_POLYGON);
						glVertex3f(-0.7f,1.0f,0.0f);//first vertex
						glVertex3f(-0.3f,1.2f,0.0f);//second vertex
						glVertex3f(0.4f,1.0f,0.0f);//third vertex
						glVertex3f(0.9f,-0.2f,0.0f);//fourth vertex
						glVertex3f(-0.9f,-0.2f,0.0f);//fifth vertex
					glEnd();
					// End drawing of polygon for 'head'
				glPopMatrix();				
				
				glPushMatrix();
					// Draw the 'joint'
					glColor3f(0.0, 0.0, 0.0);
					drawCircle(3.0);
				glPopMatrix();
				
				glPushMatrix();
					// Draw the 'eye'
					glTranslatef(-14, 23, 0.0);
					glColor3f(0.0, 0.0, 0.0);
					drawCircle(4.0);
				glPopMatrix();
				
				glPushMatrix();
					// Draw the 'upper beak'
					glTranslatef(0, HEAD_LENGTH/2, 0.0);
					
					// Scale the 'upper beak'
					glScalef(UPPER_BEAK_WIDTH, UPPER_BEAK_LENGTH, 1.0);
					
					// Set the colour of the 'upper beak'
					glColor3f(1.0, 1.0, 0);
					
						// Begin drawing of polygon for 'upper beak'
						glBegin(GL_POLYGON);
							glVertex3f(-0.7f,1.0f,0.0f);//first vertex
							glVertex3f(-0.7f,0.1f,0.0f);//second vertex
							glVertex3f(-2.0f,0.1f,0.0f);//third vertex
							glVertex3f(-2.0f,0.7f,0.0f);//fourth vertex
						glEnd();
						// End drawing of polygon for 'upper beak'
				glPopMatrix();
					
				glPushMatrix();
					// Draw the 'lower beak'
					glTranslatef(0, up_down_lower_beak, 0);
					
					glPushMatrix();
						// Draw the 'lower beak'
						glTranslatef(0, HEAD_LENGTH/2, 0.0);
						
						// Scale the 'lower beak'
						glScalef(LOWER_BEAK_WIDTH, LOWER_BEAK_LENGTH, 0.0);
						
						// Set the colour of the 'lower beak'
						glColor3f(0, 1.0, 0);
						
							// Begin drawing of polygon for 'upper beak'
							glBegin(GL_POLYGON);
								glVertex3f(-0.7f,-0.5f,0.0f);//first vertex
								glVertex3f(-0.7f,-0.9f,0.0f);//second vertex
								glVertex3f(-2.0f,-0.9f,0.0f);//third vertex
								glVertex3f(-2.0f,-0.5f,0.0f);//fourth vertex
							glEnd();
							// End drawing of polygon,upper beak
					glPopMatrix();
					
				glPopMatrix();
				
        glPopMatrix();

        glPushMatrix();
	 
			// Draw the 'arm'

			// Move the arm to the joint hinge
			glTranslatef(BODY_WIDTH/3 -5, -BODY_LENGTH/2, 0.0);

			// Rotate along the hinge
			glRotatef(joint_rot -10, 0.0, 0.0, 1.0);
			
			// Set the color of the arm
			glColor3f(1.0, 0.0, 0.0);
			
			// Scale the size of the arm
			glScalef(ARM_WIDTH, ARM_LENGTH, 1.0);
			
			glBegin(GL_POLYGON);
				glVertex3f(0.5f,0.3f,0.0f);//first vertex
				glVertex3f(0.2f,-1.2f,0.0f);//second vertex
				glVertex3f(-0.2f,-1.2f,0.0f);//third vertex
				glVertex3f(-0.5f,0.3f,0.0f);//fourth vertex
			glEnd();

			glPushMatrix();
					// Draw the 'joint'
					glColor3f(0.0, 0.0, 0.0);
					drawCircle(0.1);
			glPopMatrix();
		glPopMatrix();	
		
		glPushMatrix();
			
			// Draw the front leg
			glTranslatef(-BODY_WIDTH/3 -10, -BODY_LENGTH-80, 0.0);

			// Rotate along the hinge
			glRotatef(front_leg_joint - 10, 0.0, 0.0, 1.0);
				
			glPushMatrix();
				glScalef(LEG_WIDTH, LEG_LENGTH, 1.0);
				glTranslatef(0.0, -0.3, 0.0);
				glColor3f(1.0,1.0,0.0);
				drawSquare(1.0);
				
				glPushMatrix();
					// Draw the 'joint'
					glTranslatef(0.0, 0.3, 0.0);
					glColor3f(0.0, 0.0, 0.0);
					drawCircle(0.1);
				glPopMatrix();
			glPopMatrix();
			
			glPushMatrix();
				
				// Draw the front foot
				glTranslatef(0.0, -FOOT_LENGTH+3, 0.0);
				
				// Rotate along the hinge
				glRotatef(front_foot_joint-70, 0.0, 0.0, 1.0);
				
				glPushMatrix();
					// Set the color of the front foot
					glColor3f(1.0,0.0,0.0);
					
					// Set the scale of the front foot
					glScalef(FOOT_WIDTH, FOOT_LENGTH, 1.0);
					
					glBegin(GL_POLYGON);
						glVertex3f(0.5f,0.3f,0.0f);//first vertex
						glVertex3f(0.5f,-1.0f,0.0f);//second vertex
						glVertex3f(-0.5f,-1.0f,0.0f);//third vertex
						glVertex3f(-0.5f,0.3f,0.0f);//fourth vertex
					glEnd();
					
					glPushMatrix();
						// Draw the 'joint'
						glColor3f(0.0, 0.0, 0.0);
						drawCircle(0.1);
					glPopMatrix();
					
				glPopMatrix();
				
			glPopMatrix();
			
		glPopMatrix();
		
		glPushMatrix();
			
			// Draw the back leg
			glTranslatef(BODY_WIDTH/2+10, -BODY_LENGTH-80, 0.0);

			// Rotate along the hinge
			glRotatef(back_leg_joint+10, 0.0, 0.0, 1.0);
	
			glPushMatrix();
				glScalef(LEG_WIDTH, LEG_LENGTH, 1.0);
				glTranslatef(0.0, -0.3, 0.0);
				glColor3f(1.0,1.0,0.0);
				drawSquare(1.0);
				
				glPushMatrix();
					// Draw the 'joint'
					glTranslatef(0.0, 0.3, 0.0);
					glColor3f(0.0, 0.0, 0.0);
					drawCircle(0.1);
				glPopMatrix();
				
			glPopMatrix();
			
			glPushMatrix();
				// Draw the 'back foot'
				glTranslatef(-1.5, -LEG_LENGTH+8, 1.0);
			
				
				// Rotate along the hinge
				glRotatef(back_foot_joint-60, 0.0, 0.0, 1.0);
				
				glPushMatrix();
					// Set the color of the 'back foot'
					glColor3f(1.0,0.0,0.0);
					
					// Set the scale of the 'back foot'
					glScalef(FOOT_WIDTH, FOOT_LENGTH, 1.0);
					
					glBegin(GL_POLYGON);
						glVertex3f(0.5f,0.3f,0.0f);//first vertex
						glVertex3f(0.5f,-1.0f,0.0f);//second vertex
						glVertex3f(-0.5f,-1.0f,0.0f);//third vertex
						glVertex3f(-0.5f,0.3f,0.0f);//fourth vertex
					glEnd();
					
					glPushMatrix();
						// Draw the 'joint'
						glColor3f(0.0, 0.0, 0.0);
						drawCircle(0.1);
					glPopMatrix();
					
				glPopMatrix();
				
			glPopMatrix();
			
			
		glPopMatrix();
			

    // Retrieve the previous state of the transformation stack
    glPopMatrix();


    // Execute any GL functions that are in the queue just to be safe
    glFlush();

    // Now, show the frame buffer that we just drew into.
    // (this prevents flickering).
    glutSwapBuffers();
}


// Draw a square of the specified size, centered at the current location
void drawSquare(float width)
{
    // Draw the square
    glBegin(GL_POLYGON);
    glVertex2d(-width/2, -width/2);
    glVertex2d(width/2, -width/2);
    glVertex2d(width/2, width/2);
    glVertex2d(-width/2, width/2);
    glEnd();
}


// Darw a circle of the specified size, centered at the current location
void drawCircle(float width)
{
	const float change2Rad = 3.14/180;
	glBegin(GL_LINE_LOOP);
	for (int i=0; i<360; i++){
		float rad = i*change2Rad;
		glVertex2f(cos(rad)*width, sin(rad) *width);
	}
	glEnd();
}
