#include "scene.hpp"


using namespace cgp;

void scene_structure::evolve_shape(int biome, bool is_creation)
{	
    size_t x = 101;
    size_t y = 1001;
    float cam = camera_control.camera_model.position().y;

    /* generate new perlin noise at beginning */
    for (size_t i = 0; i < x; i++)
    {
        for (size_t j = 0; j < y; j++)
        {
            vec3& p = shape[biome].position[i * y + j];

            if (is_creation || (p.y > cam - 10 && p.y < cam + 100)) {
                vec2 p0 = {(float) i - 50, j};
                float dz = 0;

                if (biome == 0) {
                    float persistency = 0.01f;
                    if (i < 46)
                        persistency = 2.0f + 0.4f * (100.0f - (i * 100.0f / 45.0f));
                    else if (i > 56)
                        persistency = 2.0f + 0.4f * (((i - 56) * 100.0f / 45.0f));
                    
                    dz = 0.2f * noise_perlin({ p0.x, p0.y, 0 }, 2, persistency, 0.3f) 
                        + 0.01f * noise_perlin({ 4 * p0.x, 4 * p0.y, timer.t }, 2);
                }
                if (biome == 1) {
                    dz = (noise_perlin({ std::abs(p0.x) / 5, 0 }, 2, 0.3, timer.t / 10) + noise_perlin({ p0.x / 10, p0.y / 10 }, 2, 0.3, 0)) * (-3 * cos(p0.x / 10) + 3);
                }
                p.z = dz * (-cos(p0.y / 53.0516) + 1);
            }
        }
    }

    /* infinite field with switch*/
    if (cam > 50 + 1000 * trigger && biome == (trigger + 1) % 2) {
        for(int k=0; k<x*y; ++k)
        {
            shape[biome].position[k].y += 2000;
        }
        trigger++;
    }
}

void scene_structure::evolve_sun()
{
    size_t const N = sun_position.size();
    vec3 pos = camera_control.camera_model.position();

	for(int k=0; k<N; ++k)
    {
        sun.position[k] = vec3(sun_position[k].x + pos.x, sun_position[k].y + pos.y, sun_position[k].z + pos.z);
    } 
}

void scene_structure::initialize()
{
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_z();
	camera_control.look_at({ 0.0f, -0.5f, 1.0f }, {0,0,1}, {0,0,1});

    environment.background_color = {0, 0, 0};

	shape = {mesh_primitive_grid({ -50,0,0 }, { 50,0,0 }, { 50,1000,0 }, { -50,1000,0 }, 101, 1001),
            mesh_primitive_grid({ -50,1000,0 }, { 50,1000,0 }, { 50,2000,0 }, { -50,2000,0 }, 101, 1001)};
    sun = mesh_primitive_disc(10, { 0,50,5 }, { 0,-1,0 }, 40);

	initial_position = {shape[0].position, shape[1].position};
    sun_position = sun.position;

    opengl_shader_structure shader;
    opengl_shader_structure single_color;

    shader.load("shaders/mesh/vert.glsl", "shaders/mesh/frag.glsl");
    single_color.load("shaders/single_color/vert.glsl", "shaders/single_color/frag.glsl");

    mesh_drawable a;
    mesh_drawable b;
    shape_visual = {a, b};

    for (int biome=0; biome < shape.size(); biome++)
    {
        evolve_shape(biome, true);
	    shape_visual[biome].initialize_data_on_gpu(shape[biome], shader);
	    shape_visual[biome].material.color = { 0.2f, 0.4f, 1.0f };
    }

	sun_visual.initialize_data_on_gpu(sun, single_color);
	sun_visual.material.color = { 1.0f, 0.7f, 0.0f };

	global_frame.initialize_data_on_gpu(mesh_primitive_frame());
}



void scene_structure::display_frame()
{

	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	if (gui.display_frame)
		draw(global_frame, environment);

	timer.update();


	for (int biome=0; biome < shape.size(); biome++)
    {
	    draw(shape_visual[biome], environment);
    }

    glDepthRange(0.999, 1.0);
    draw(sun_visual, environment);
    glDepthRange(0.0, 1.0);

	if (gui.display_wireframe) {
    }

    camera_control.camera_model.manipulator_translate_front(-0.2);

    for (int biome=0; biome < shape.size(); biome++)
    {
        evolve_shape(biome);
        shape_visual[biome].vbo_position.update(shape[biome].position);
        shape[biome].normal_update();
        shape_visual[biome].vbo_normal.update(shape[biome].normal);
    }

    evolve_sun();

    sun_visual.vbo_position.update(sun.position);
	// Recompute normals on the CPU (given the position and the connectivity currently in the mesh structure)
    sun.normal_update();
	// Send updated normals on the GPU
    sun_visual.vbo_normal.update(sun.normal);
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

