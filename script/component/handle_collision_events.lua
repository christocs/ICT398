local health_script = this:entity():script_data("script/component/health.lua")

local enemy_health_given = -10
local prey_health_given = 10

local enemy_score_given = -5
local prey_score_given = 10

function collision_event_handler(evt)
    local c_event = evt:to_collision()

    if c_event.action.contact_start then
        if c_event.type.prey then
            health_script.health = health_script.health + prey_health_given
            health_script.score = health_script.score + prey_score_given
        end

        if c_event.type.enemy then
            health_script.health = health_script.health + enemy_health_given
            health_script.score = health_script.score + enemy_score_given
        end

        if c_event.type.deathzone then
            health_script.health = health_script.min_health
        end

        if health_script.health > health_script.max_health then
            health_script.health = health_script.max_health
        elseif health_script.health < health_script.min_health then
            health_script.health = health_script.min_health
        end
    end
end

this:register_event(event.collision, collision_event_handler)
