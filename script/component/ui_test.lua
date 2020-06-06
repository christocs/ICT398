local checked = false
function draw_ui()
    
    imgui.begin("Lua Textboxes")
    _, checked = imgui.checkbox("Asdf", checked)
    if checked then
        imgui.text("checked:" .. tostring(checked))
    end
    imgui.end_()
end

this:register_event(event.render, draw_ui)
