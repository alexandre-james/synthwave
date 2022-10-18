#pragma once


#include "cgp/cgp.hpp"
#include "environment.hpp"


using cgp::mesh_drawable;


struct gui_parameters {
	bool display_frame = false;
	bool display_wireframe = false;
};

// The structure of the custom scene
struct scene_structure : cgp::scene_inputs_generic {
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	camera_controller_orbit_euler camera_control;
	camera_projection_perspective camera_projection;
	window_structure window;

	mesh_drawable global_frame;          // The standard global frame
	environment_structure environment;   // Standard environment controler
	input_devices inputs;                // Storage for inputs status (mouse, keyboard, window dimension)
	gui_parameters gui;                  // Standard GUI element storage
	
	cgp::timer_basic timer;

    int trigger = 1;
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //

	std::vector<cgp::mesh> shape;
    cgp::mesh sun;
  	std::vector<cgp::numarray<cgp::vec3>> initial_position;
  	cgp::numarray<cgp::vec3> sun_position;
  	std::vector<cgp::mesh_drawable> shape_visual;
    cgp::mesh_drawable sun_visual;

	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();    // Standard initialization to be called before the animation loop
	void display_frame(); // The frame display to be called within the animation loop
	void display_gui();   // The display of the GUI, also called within the animation loop
	void create_shape(int biome); // Compute the deformation of the surface
    void teleport(int biome); // teleports the biome
    void evolve_sun(); // Compute the x pos of the sun

	void mouse_move_event();
	void mouse_click_event();
	void keyboard_event();
	void idle_frame();
};





