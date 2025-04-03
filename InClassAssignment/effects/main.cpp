void process_input()
{
    // ... existing code ...
    
    const Uint8 *key_state = SDL_GetKeyboardState(nullptr);
    
    if (key_state[SDL_SCANCODE_C])
    {
        g_curr_scene->get_state().player->set_player_state(CHARGING);
        
        // Only start the shake effect if not already running
        if (g_effects->get_current_effect() != SHAKE) {
            g_effects->start(SHAKE, 1.0f);
        }
    }
    else
    {
        g_curr_scene->get_state().player->set_player_state(REST);
        
        // Stop the shake effect if it was active
        if (g_effects->get_current_effect() == SHAKE) {
            g_effects->start(NONE);
        }
    }
         
    // ... rest of the function ...
}

void update()
{
    // ... existing code ...
    
    // Base view matrix
    g_view_matrix = glm::mat4(1.0f);
    
    // Apply camera follow
    g_view_matrix = glm::translate(g_view_matrix,
                                 glm::vec3(-g_curr_scene->get_state().player->get_position().x,
                                         3.75,
                                         0));
    
    // Apply shake effect from Effects class
    g_view_matrix = g_effects->apply_shake_effect(g_view_matrix);
} 