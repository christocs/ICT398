local scr = this:entity():script_data("script/component/camera_keyboard_control.lua")

local function debug_stuff(evt)
    local key_evt = evt:to_key()
    if key_evt.key == key.Num1 then
        engine.toggle_wireframe()
    end
    if key_evt.key == key.Tilde then
        ui.toggle_menu()
    end
    if key_evt.key == key.Num2 then
        scr.speed = scr.speed + 1
    end
end
this:register_event(event.key_down, debug_stuff)
