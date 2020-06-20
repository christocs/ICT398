local last_x = 0
local last_y = 0
local health_script = this:entity():script_data("script/component/health.lua")

function mouse_move(evt)
    if health_script.health > health_script.min_health then
        local mouse_evt = evt:to_mouse_move()
        local MAX_YAW = 89.0
        local sensitivity = 0.1
        local angles = camera:current().angle
        local dx = mouse_evt.x - last_x
        local dy = mouse_evt.y - last_y
        angles.x = angles.x + (dx * sensitivity)
        angles.y = angles.y + (-dy * sensitivity)
        angles.y = math.clamp(angles.y, -MAX_YAW, MAX_YAW)
        camera:current().angle = angles
        last_x = mouse_evt.x
        last_y = mouse_evt.y
    end
end

this:register_event(event.mouse_move, mouse_move)
