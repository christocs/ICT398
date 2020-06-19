start_health = 100
health = start_health
max_health = 200
min_health = 0

function health_display()
    local physics_component = this:entity():get_physics()

    imgui.begin("Player Info")
    imgui.text("Health: " .. tostring(health))
    if health > min_health then
        imgui.text("Status: Alive")
    else
        imgui.text("Status: Dead")
    end
    if imgui.button("Reset Level", vector2(100, 50)) then
        health = start_health
--        physics_component.apply_force(vector3(0, 50, 0))
--        physics_component.set_velocity(vector3(0, 0, 0))
    end


    imgui.end_()
end

this:register_event(event.render, health_display)
