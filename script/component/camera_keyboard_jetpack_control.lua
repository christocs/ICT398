speed = 600000.0
jetpack_speed = speed * 0.7
jetpack_max_duration = 3.0
jetpack_current_duration = 0.0

function update(evt)
    local upd8 = evt:to_update()
    local dt = upd8.delta
    local vel = speed * dt
    local jetpack_vel = jetpack_speed * dt
    local cur_cam = camera.current()
    local physics_component = this:entity():get_physics()

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
        physics_component:apply_force(cur_cam:up():smul(jetpack_vel))
    end

    cur_cam.pos = this:entity():get_transform().translation
end

this:register_event(event.update, update)
