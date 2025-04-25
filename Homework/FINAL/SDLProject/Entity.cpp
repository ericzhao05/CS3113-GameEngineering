#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"
#include "Scene.h"
#include <iostream>

void Entity::ai_activate(Entity *player)
{
    switch (m_ai_type)
    {
        case WALKER:
            ai_walk();
            break;
            
        case GUARD:
            ai_guard(player);
            break;

        case FLYER:
            switch (m_ai_state) {
                case DIAGONAL_FLY_RIGHT:
                    ai_flyer_diagonal_right();
                    break;
                    
                case DIAGONAL_FLY_LEFT:
                    ai_flyer_diagonal_left();
                    break;
                    
                case OVAL_FLY:
                   ai_flyer_oval();
                    break;
                    
                default:
                    break;
            }
            break;
            
        default:
            break;
    }
}

void Entity::ai_walk()
{
    if (m_position.x > 17.0f) {
        m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
    } else if (m_position.x < 14.0f) {
        m_movement = glm::vec3(1.0f, 0.0f, 0.0f);
    } else if (m_movement.x == 0.0f) {
        m_movement = glm::vec3(1.0f, 0.0f, 0.0f);
    }
}

void Entity::ai_guard(Entity *player)
{
    
    if (player == nullptr) return;
    
    
    switch (m_ai_state) {
        case IDLE:
            // Switch to walking state if player is within detection range
            if (glm::distance(m_position, player->get_position()) < 20.0f) {
                m_ai_state = WALKING;
            }
            break;
            
        case WALKING:
            // Move towards the player
            if (m_position.x > player->get_position().x) {
                m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);

            } else {
                m_movement = glm::vec3(1.0f, 0.0f, 0.0f);
   
            }
            
            if (glm::length(m_movement) > 1.0f) {
                m_movement = glm::normalize(m_movement);
            }
            break;
            
        case ATTACKING:
            break;
            
        default:
            break;
    }
}

void Entity::ai_flyer_diagonal_right() {
    // Define exact points
    glm::vec3 start_point = glm::vec3(25.00000000f, -14.00000000f, 0.00000000f);
    glm::vec3 mid_point = glm::vec3(17.00000000f, -4.00000000f, 0.00000000f);
    glm::vec3 end_point = glm::vec3(13.60000000f, 0.00000000f, 0.00000000f);
    
    static bool going_forward = true;
    float flight_speed = 10.0f;
     if (going_forward) {
        if (m_position.x <= end_point.x) {
            going_forward = false;
        } else {
            m_movement = glm::vec3(-0.60000000f, 0.80000000f, 0.00000000f) * flight_speed;
        }
    } else {
        if (m_position.x >= start_point.x || m_position.y <= -13.5f) {
            going_forward = true;
        } else {
            m_movement = glm::vec3(0.60000000f, -0.80000000f, 0.00000000f) * flight_speed;
        }
    }
}

void Entity::ai_flyer_oval() {
    // Define the oval center point and dimensions
    glm::vec3 center(15.0f, -4.0f, 0.0f);
    float width = 7.0f;  // Half-width of the oval
    float height = 4.0f; // Half-height of the oval
    
    // Use a time-based parameter to move around the oval
    static float t = 0.0f;
    float flight_speed = 7.0f;
    
    // Calculate the position on the oval using parametric equations
    float x = center.x + width * cos(t);
    float y = center.y + height * sin(t);
    
    // Calculate the target position
    glm::vec3 target_position(x, y, 0.0f);
    
    // Calculate the direction to move (tangent to the oval)
    float dx = -width * sin(t);
    float dy = height * cos(t);
    glm::vec3 direction(dx, dy, 0.0f);
    
    // Normalize the direction vector
    direction = glm::normalize(direction);
    
    // Set movement based on the direction
    m_movement = direction * flight_speed;
    
//    // Face the correct direction based on movement
//    if (m_movement.x < 0) {
//        face_left();
//    } else {
//        face_right();
//    }
    
    // Increment the parameter for the next frame
    t += 0.02f;
    if (t > 2.0f * M_PI) {
        t -= 2.0f * M_PI;  // Reset after a complete cycle
    }

    // Define the oval path points
    glm::vec3 bottom_point(15.0f, -7.0f, 0.0f);
    glm::vec3 left_point(7.0f, -4.0f, 0.0f);
    glm::vec3 right_point(22.0f, -4.0f, 0.0f);
    glm::vec3 top_point(15.0f, -1.0f, 0.0f);
    
    static int current_segment = 0;
    flight_speed = 5.0f; // Slightly slower for the oval path
    
    // Calculate distances to each point to determine which segment we're closest to
    float dist_to_bottom = glm::distance(m_position, bottom_point);
    float dist_to_left = glm::distance(m_position, left_point);
    float dist_to_right = glm::distance(m_position, right_point);
    float dist_to_top = glm::distance(m_position, top_point);
    
    // If we're very close to the current target point, move to the next segment
    const float POINT_REACH_THRESHOLD = 0.5f;
    
    // Points in sequence: bottom -> left -> top -> right -> bottom
    if ((current_segment == 0 && dist_to_left < POINT_REACH_THRESHOLD) ||
        (current_segment == 1 && dist_to_top < POINT_REACH_THRESHOLD) ||
        (current_segment == 2 && dist_to_right < POINT_REACH_THRESHOLD) ||
        (current_segment == 3 && dist_to_bottom < POINT_REACH_THRESHOLD)) {
        current_segment = (current_segment + 1) % 4;
    }
    
    // Set movement direction based on current segment
    switch (current_segment) {
        case 0: // Bottom to Left
            m_movement = glm::normalize(left_point - m_position) * flight_speed;
            face_left(); // Make the enemy face left
            break;
            
        case 1: // Left to Top
            m_movement = glm::normalize(top_point - m_position) * flight_speed;
            break;
            
        case 2: // Top to Right
            m_movement = glm::normalize(right_point - m_position) * flight_speed;
            face_right(); // Make the enemy face right
            break;
            
        case 3: // Right to Bottom
            m_movement = glm::normalize(bottom_point - m_position) * flight_speed;
            break;
    }
    
    // Safety check for NaN or infinite positions
    if (std::isinf(m_position.x) || std::isnan(m_position.x) ||
        std::isinf(m_position.y) || std::isnan(m_position.y) ||
        m_position.x < 0.0f || m_position.x > 30.0f ||
        m_position.y < -20.0f || m_position.y > 5.0f) {
        
        // Reset to the bottom point
        m_position = bottom_point;
        current_segment = 0;
        m_velocity = glm::vec3(0.0f);
    }
}

void Entity::ai_flyer_diagonal_left() {
    glm::vec3 start_point = glm::vec3(6.00000000f, -14.00000000f, 0.00000000f);
    glm::vec3 mid_point = glm::vec3(11.00000000f, -7.33000000f, 0.00000000f);
    glm::vec3 end_point = glm::vec3(16.54000000f, 0.00000000f, 0.00000000f);
    
    static bool going_forward = true;
    float flight_speed = 10.0f;
    
    if (going_forward) {
        if (m_position.x >= end_point.x) {
            going_forward = false;
        } else {
            m_movement = glm::vec3(0.60000000f, 0.80000000f, 0.00000000f) * flight_speed;
        }
    } else {
        if (m_position.x <= start_point.x || m_position.y <= -13.5f) {
            going_forward = true;
        } else {
            m_movement = glm::vec3(-0.60000000f, -0.80000000f, 0.00000000f) * flight_speed;
        }
    }
}

// Default constructor
Entity::Entity()
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(0.0f), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
    m_texture_id(0), m_velocity(0.0f), m_acceleration(0.0f), m_width(0.0f), m_height(0.0f)
{
    // Initialize m_walking with zeros or any default value
    for (int i = 0; i < SECONDS_PER_FRAME; ++i)
        for (int j = 0; j < SECONDS_PER_FRAME; ++j) m_walking[i][j] = 0;
}

// Parameterized constructor
Entity::Entity(GLuint texture_id, float speed, glm::vec3 acceleration, float jump_power, int walking[4][4], float animation_time,
    int animation_frames, int animation_index, int animation_cols,
    int animation_rows, float width, float height, EntityType EntityType)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed),m_acceleration(acceleration), m_jumping_power(jump_power), m_animation_cols(animation_cols),
    m_animation_frames(animation_frames), m_animation_index(animation_index),
    m_animation_rows(animation_rows), m_animation_indices(nullptr),
    m_animation_time(animation_time), m_texture_id(texture_id), m_velocity(0.0f),
    m_width(width), m_height(height), m_entity_type(EntityType)
{
    face_right();
    set_walking(walking);
}

Entity::Entity(GLuint texture_id, float speed, glm::vec3 acceleration, float jump_power, int walking[5][1], float animation_time,
    int animation_frames, int animation_index, int animation_cols,
    int animation_rows, float width, float height, EntityType EntityType)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed),m_acceleration(acceleration), m_jumping_power(jump_power), m_animation_cols(animation_cols),
    m_animation_frames(animation_frames), m_animation_index(animation_index),
    m_animation_rows(animation_rows), m_animation_indices(nullptr),
    m_animation_time(animation_time), m_texture_id(texture_id), m_velocity(0.0f),
    m_width(width), m_height(height), m_entity_type(EntityType)
{
    four_health();
    set_walking_health(walking);
    
    if (EntityType == HEALTH_BAR) {
        m_movement = glm::vec3(0.0f);
    }
}

// Simpler constructor for partial initialization
Entity::Entity(GLuint texture_id, float speed,  float width, float height, EntityType EntityType)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
    m_texture_id(texture_id), m_velocity(0.0f), m_acceleration(0.0f), m_width(width), m_height(height),m_entity_type(EntityType)
{
    // Initialize m_walking with zeros or any default value
    for (int i = 0; i < SECONDS_PER_FRAME; ++i)
        for (int j = 0; j < SECONDS_PER_FRAME; ++j) m_walking[i][j] = 0;
}
Entity::Entity(GLuint texture_id, float speed, float width, float height, EntityType EntityType, AIType AIType, AIState AIState): m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
m_speed(speed), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
m_texture_id(texture_id), m_velocity(0.0f), m_acceleration(0.0f), m_width(width), m_height(height),m_entity_type(EntityType), m_ai_type(AIType), m_ai_state(AIState)
{
// Initialize m_walking with zeros or any default value
for (int i = 0; i < SECONDS_PER_FRAME; ++i)
    for (int j = 0; j < SECONDS_PER_FRAME; ++j) m_walking[i][j] = 0;
}

Entity::~Entity() { }

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{
    // Step 1: Calculate the UV location of the indexed frame
    float u_coord = (float)(index % m_animation_cols) / (float)m_animation_cols;
    float v_coord = (float)(index / m_animation_cols) / (float)m_animation_rows;

    // Step 2: Calculate its UV size
    float width = 1.0f / (float)m_animation_cols;
    float height = 1.0f / (float)m_animation_rows;

    // Step 3: Just as we have done before, match the texture coordinates to the vertices
    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width, v_coord,
        u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };

    // Step 4: And render
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

bool const Entity::check_collision(Entity* other) const
{
    // If either entity is inactive, no collision should occur
    if (!other->m_is_active) {
        return false;
    }
    
    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}

void const Entity::check_collision_y(Entity *collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity *collidable_entity = &collidable_entities[i];
        
        if (check_collision(collidable_entity))
        {
            float y_distance = fabs(m_position.y - collidable_entity->m_position.y);
            float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->m_height / 2.0f));
            if (m_velocity.y > 0)
            {
                m_position.y   -= y_overlap;
                m_velocity.y    = 0;

                // Collision!
                m_collided_top  = true;
            } else if (m_velocity.y < 0)
            {
                m_position.y      += y_overlap;
                m_velocity.y       = 0;

                // Collision!
                m_collided_bottom  = true;
            }
        }
    }
}

void const Entity::check_collision_x(Entity *collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity *collidable_entity = &collidable_entities[i];
        
        if (check_collision(collidable_entity))
        {
            float x_distance = fabs(m_position.x - collidable_entity->m_position.x);
            float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->m_width / 2.0f));
            if (m_velocity.x > 0)
            {
                m_position.x     -= x_overlap;
                m_velocity.x      = 0;

                // Collision!
                m_collided_right  = true;
                
            } else if (m_velocity.x < 0)
            {
                m_position.x    += x_overlap;
                m_velocity.x     = 0;
 
                // Collision!
                m_collided_left  = true;
            }
        }
    }
}

void const Entity::check_collision_y(Map *map)
{
    // Probes for tiles above
    glm::vec3 top = glm::vec3(m_position.x, m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_left = glm::vec3(m_position.x - (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_right = glm::vec3(m_position.x + (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    
    // Probes for tiles below
    glm::vec3 bottom = glm::vec3(m_position.x, m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_left = glm::vec3(m_position.x - (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_right = glm::vec3(m_position.x + (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    // If the map is solid, check the top three points
    if (map->is_solid(top, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_left, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_right, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    
    // And the bottom three points
    if (map->is_solid(bottom, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_left, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
            m_position.y += penetration_y;
            m_velocity.y = 0;
            m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_right, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
        
    }
}

void const Entity::check_collision_x(Map *map)
{
    // Probes for tiles; the x-checking is much simpler
    glm::vec3 left  = glm::vec3(m_position.x - (m_width / 2), m_position.y, m_position.z);
    glm::vec3 right = glm::vec3(m_position.x + (m_width / 2), m_position.y, m_position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if (map->is_solid(left, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
    }
    if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x;
        m_velocity.x = 0;
        m_collided_right = true;
    }
}
void Entity::update(float delta_time, Entity *player, Entity *collidable_entities, int collidable_entity_count, Map *map)
{
 
    m_collided_top    = false;
    m_collided_bottom = false;
    m_collided_left   = false;
    m_collided_right  = false;
    
    if (m_entity_type == ENEMY) ai_activate(player);
    
    if (m_entity_type == HEALTH_BAR) {
        m_movement = glm::vec3(0.0f);
    }
    
    if (m_animation_indices != NULL)
    {
        if (glm::length(m_movement) != 0)
        {
            m_animation_time += delta_time;
            float frames_per_second = (float) 1 / SECONDS_PER_FRAME;
            
            if (m_animation_time >= frames_per_second)
            {
                m_animation_time = 0.0f;
                m_animation_index++;
                
                if (m_animation_index >= m_animation_frames)
                {
                    m_animation_index = 0;
                }
            }
        }
    }
    
    m_velocity.x = m_movement.x * m_speed;
    
    // For flyers, set the y velocity directly from movement
    if (m_ai_type == FLYER) {
        m_velocity.y = m_movement.y * m_speed;
        m_velocity.x = m_movement.x * m_speed;
    } else {
        m_velocity += m_acceleration * delta_time;
    }
    
   
    
    if (m_is_jumping)
    {
        m_is_jumping = false;
        m_velocity.y += m_jumping_power;
    }
    
    m_position.y += m_velocity.y * delta_time;
    
    check_collision_y(collidable_entities, collidable_entity_count);
    check_collision_y(map);
    
    m_position.x += m_velocity.x * delta_time;
    check_collision_x(collidable_entities, collidable_entity_count);
    check_collision_x(map);
    
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    // Apply scaling based on width and height
    m_model_matrix = glm::scale(m_model_matrix, glm::vec3(m_width, m_height, 1.0f));
}


void Entity::render(ShaderProgram* program)
{
    program->set_model_matrix(m_model_matrix);

    if (m_animation_indices != NULL)
    {
        draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

void Entity::shoot_projectile()
{    
    m_scene->shoot_projectile(this);
}
