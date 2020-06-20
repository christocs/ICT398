local keyboard_script = this:entity():script_data("script/component/camera_keyboard_jetpack_control.lua")
local health_script = this:entity():script_data("script/component/health.lua")

function debug_stuff(evt)
    local key_evt = evt:to_key()

    if key_evt.key == key.Num1 then
        engine.toggle_wireframe()
        health_script.health = health_script.min_health
    end
    if key_evt.key == key.Tilde then
        ui.toggle_menu()
    end
end

this:register_event(event.key_down, debug_stuff)
