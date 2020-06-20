local jetpack_script = this:entity():script_data("script/component/camera_keyboard_jetpack_control.lua")

start_health = 100
health = start_health
max_health = 200
min_health = 0

function health_display()
    local physics_component = this:entity():get_physics()
    local jetpack_percentage = round((jetpack_script.jetpack_current_duration / jetpack_script.jetpack_max_duration) * 100, 1)

    imgui.begin("Player Info")
    imgui.text("Health: " .. tostring(health))
    if health > min_health then
        imgui.text("Status: Alive")
    else
        imgui.text("Status: Dead")
    end
    imgui.text("Jetpack: " .. tostring(jetpack_percentage) .. "%")
    if imgui.button("Reset Level", vector2(80, 30)) then
        health = start_health
        physics_component:set_pos(vector3(0, 50, 0))
        physics_component:set_velocity(vector3(0, 0, 0))
    end


    imgui.end_()
end

function round(x, decimals)
    local n = 10^(decimals or 0)
    x = x * n
    if x >= 0 then x = math.floor(x + 0.5) else x = math.ceil(x - 0.5) end
    return x / n
end

this:register_event(event.render, health_display)
