local health_script = this:entity():script_data("script/component/health.lua")

local speed = 600000.0
local jetpack_speed = speed * 2.0
local jetpack_max_duration = 0.2
local jetpack_current_duration = 0.0

function update(evt)
    local upd8 = evt:to_update()
    local dt = upd8.delta
    local vel = speed * dt
    local jetpack_vel = jetpack_speed * dt
    local cur_cam = camera.current()
    local physics_component = this:entity():get_physics()

    if health_script.health > health_script.min_health then
        if keyboard.is_pressed(key.W) then
            physics_component:apply_force(cur_cam:front():smul(vel))
        end
        if keyboard.is_pressed(key.S) then
            physics_component:apply_force(cur_cam:front():smul(vel):inverse())
        end
        if keyboard.is_pressed(key.A) then
            physics_component:apply_force(cur_cam:right():smul(vel):inverse())
        end
        if keyboard.is_pressed(key.D) then
            physics_component:apply_force(cur_cam:right():smul(vel))
        end
        if keyboard.is_pressed(key.Space) then
            if jetpack_current_duration < jetpack_max_duration then
                jetpack_current_duration = jetpack_current_duration + dt
                physics_component:apply_force(cur_cam:up():smul(jetpack_vel))
            end
        elseif (jetpack_current_duration - dt) >= 0 then
            jetpack_current_duration = jetpack_current_duration - dt
        else
            jetpack_current_duration = 0
        end
    end

    cur_cam.pos = this:entity():get_transform().translation
end

this:register_event(event.update, update)
