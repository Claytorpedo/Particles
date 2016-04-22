# Particles Project

![TitleImage](https://cloud.githubusercontent.com/assets/11372001/14752743/ad3a96c8-0896-11e6-91fa-662e5d521a20.PNG)

A many-particle system using OpenGL-4 with SDL2. The particles are rendered and updated with the GPU, using framebuffers with textures.

Made for my Graphics 2 final project.

#Controls

The Essentials	
----

**Left Click** 	    - Change the position of the (currently active) gravity object. Drag to continuously move the (currently active) gravity object.

**Right Click** 		- Disable/enable the (currently active) gravity object.

**Ctrl + Left Drag** 	  - Rotate the camera.  
**Ctrl + Right Drag**	  - Pan the camera.  
**Ctrl + Mouse Wheel**	- Zoom the camera in or out.  
**Ctrl + R**		        - Reset camera.

**Spacebar**	- Pause/Resume the simulation.  
**F11**			  - Toggle fullscreen.

For Enthusiasts	
----

**NumPad Numbers** 	    - Change particle colours.

**Shift + Arrow Keys**	- Change the number of particles. Press Enter to confirm.  
**Enter/Return Key**	  - Confirm the number of particles, and re-initialize the system.

**Keyboard Numbers**	  - Set a gravity object as the active one. Default is 1.

**Backspace**		        - Turn off all gravity objects.  
**Ctrl + Backspace**	  - Turn off all gravity objects, and return their values to defaults.

Fine Details	
----
For the following, when arrow keys are used, up/down arrows increment by small amounts, and left/right arrows increment by large amounts. The mouse wheel can also be used.

**A + Arrow Keys**	- Change alpha value of the particles. Low alpha gives smoke-like effects.  
**C + Arrow Keys** 	- Change cohesiveness. Particle acceleration is calculated based on how far it is from gravity objects. If a particle gets very far away, it can take a very long time to come back, or might stop entirely. Cohesiveness sets a cutoff distance after which a constant acceleration is used. The cohesiveness value itself is this cutoff distance squared.  
**G + Arrow Keys**	- Change the gravity force. Setting this as negative pushes particles away.  
**P + Arrow Keys**	- Change point size. This is more interesting with smaller numbers of particles.  
**Z + Arrow Keys**	- Change camera zoom.

**C + R/G/B/Backspace** - Set background colour to red/green/blue/black.  

**Alt + Mouse Wheel** 	- Change the depth at which to place the gravity object.  
**Alt + R**	          	- Reset the gravity depth to default.

**L Key**		            - Write gravity object information to console.
