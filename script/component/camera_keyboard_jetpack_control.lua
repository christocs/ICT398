speed = 1000000.0
function update(evt)
    local upd8 = evt:to_update()
    local dt = upd8.delta
    local vel = speed * dt
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

    local tf = this:entity():get_transform()
--    local cur_cam_pos = cur_cam.pos
--    cur_cam_pos.x = tf.translation.x
--    cur_cam_pos.y = tf.translation.y
--    cur_cam_pos.z = tf.translation.z
    -- tf.translation = cur_cam.pos
--    tf.translation.x = cur_cam.pos.x
--    tf.translation.y = cur_cam.pos.y
--    tf.translation.z = cur_cam.pos.z

--    camera.current().pos.x = tf.translation.x
--    camera.current().pos.y = tf.translation.y
--    camera.current().pos.z = tf.translation.z
end

this:register_event(event.update, update)
