#include "scene.hpp"


using namespace cgp;


void scene_structure::evolve_shape()
{
    size_t const N = initial_position.size();
    for(size_t k=0; k<N; ++k)
    {
        vec3 const& p0 = initial_position[k];
        vec3& p        = shape.position[k];
		float const dz = 0.3f * noise_perlin({ p0.x + 0.2f * timer.t, p0.y, 0 }, 2)
						+ 0.015f * noise_perlin({ 4 * p0.x, 4 * p0.y, timer.t }, 2);
        p.z = dz;
    }
}

void scene_structure::initialize()
{
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_y();
	camera_control.look_at({ 3.0f, 2.0f, 2.0f }, {0,0,0}, {0,0,1});

	int N = 25;
	shape = mesh_primitive_grid({ 0,0,0 }, { 1,0,0 }, { 1,1,0 }, { 0,1,0 }, N, N);
	initial_position = shape.position;
	shape_visual.initialize_data_on_gpu(shape);
	shape_visual.material.color = { 0.6f, 0.6f, 0.9f };
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());


}



void scene_structure::display_frame()
{

	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	if (gui.display_frame)
		draw(global_frame, environment);

	timer.update();
	
	draw(shape_visual, environment);
	if (gui.display_frame)
    	draw_wireframe(shape_visual, environment, { 0,0,0 });

	evolve_shape();
	shape_visual.vbo_position.update(shape.position);
	// Recompute normals on the CPU (given the position and the connectivity currently in the mesh structure)
	shape.normal_update();
	// Send updated normals on the GPU
	shape_visual.vbo_normal.update(shape.normal);
}

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);
}

void scene_structure::mouse_move_event()
{
	if (!inputs.keyboard.shift)
		camera_control.action_mouse_move(environment.camera_view);
}
void scene_structure::mouse_click_event()
{
	camera_control.action_mouse_click(environment.camera_view);
}
void scene_structure::keyboard_event()
{
	camera_control.action_keyboard(environment.camera_view);
}
void scene_structure::idle_frame()
{
	camera_control.idle_frame(environment.camera_view);
}

