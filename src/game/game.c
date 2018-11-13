#include "game.h"

#include <stdlib.h>

#include "../component_defs.h"
#include "../helpers/world_collision_info.h"

struct Game
{
    float start_pos;
};

Game *game_new( ECS *ecs )
{
    Game *game = malloc( sizeof( Game ) );

    Entity player_entity = 0;
    ECS_FIND_FIRST_ENTITY_WITH_COMPONENT( Player, ecs, &player_entity );
    ECS_GET_COMPONENT_DECL( Transform, player_transform, ecs, player_entity );

    game->start_pos = player_transform->position[1];

    return game;
}

void game_update( Game *game, ECS *ecs )
{
    ECS_GET_SINGLETON_DECL( ClockInfo, ecs, clock_info );

    Entity player_entity = 0;
    ECS_FIND_FIRST_ENTITY_WITH_COMPONENT( Player, ecs, &player_entity );
    ECS_GET_COMPONENT_DECL( Transform, player_transform, ecs, player_entity );

    player_transform->position[1] = game->start_pos + sinf(clock_info->millis_since_start / 1000.f);

    ECS_GET_SINGLETON_DECL( WorldCollisionInfo, ecs, collision );
    float intersect = world_collision_info_raycast( collision, player_transform->position, (vec3){ 0, -1, 0 } );
    if( intersect > 0.f )
    {
        vec3 intersect_pt;
        glm_vec_copy( player_transform->position, intersect_pt );
        // TODO math
        player_transform->position[1] = 0.f;
    }

    Entity game_camera_entity = 0;
    ECS_FIND_FIRST_ENTITY_WITH_COMPONENT( GameCamera, ecs, &game_camera_entity );
    ECS_GET_COMPONENT_DECL( GameCamera, game_camera, ecs, game_camera_entity );
    ECS_GET_COMPONENT_DECL( Transform, camera_target_transform, ecs, game_camera->target );
    ECS_GET_COMPONENT_DECL( Transform, camera_transform, ecs, game_camera_entity );

    mat4 m;
    versor q;
    glm_lookat( camera_target_transform->position, camera_transform->position, (vec3){0,1,0}, m );
    glm_mat4_quat( m, q ); 
    glm_quat_inv( q, camera_transform->rotation );
}

void game_delete( Game *game )
{
    if( !game ) return;

    free( game );
}
