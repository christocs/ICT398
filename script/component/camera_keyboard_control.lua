speed = 10.0
function update(evt)
    local upd8 = evt:to_update()
    local dt = upd8.delta
    local vel = speed * dt
    local cur_cam = camera.current()
    local cur_cam_pos = cur_cam.pos
    if keyboard.is_pressed(key.W) then
        cur_cam.pos = cur_cam_pos:add(cur_cam:front():smul(vel))
    end
    if keyboard.is_pressed(key.S) then
        cur_cam.pos = cur_cam_pos:sub(cur_cam:front():smul(vel))
    end
    if keyboard.is_pressed(key.A) then
        cur_cam.pos = cur_cam_pos:sub(cur_cam:right():smul(vel))
    end
    if keyboard.is_pressed(key.D) then
        cur_cam.pos = cur_cam_pos:add(cur_cam:right():smul(vel))
    end
    local tf = this:entity():get_transform()
    -- tf.translation = cur_cam.pos
    tf.translation.x = cur_cam.pos.x
    tf.translation.y = cur_cam.pos.y
    tf.translation.z = cur_cam.pos.z
end

this:register_event(event.update, update)
