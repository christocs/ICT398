function begin() 
    print("hello world")
end
function shutdown()
    print("power off!")
end
function leave_start()
    print("left start")
end

local fsm = engine.fsm_builder()
    :state("init")
    :on("begin")
        :go("start")
        :call(begin)
    :state("start")
    :on_exit(leave_start)
    :on("poweroff")
        :go("off")
    :state("off")
        :on_enter(shutdown) -- upon entering the 'off' state, shutdown is called
    :initial("init")
    :build()


function kp(evt)
    local key_pressed = evt:to_key().key
    if key_pressed == key.B then
        fsm:fire("begin")
    elseif key_pressed == key.P then
        fsm:fire("poweroff")
    end

end

this:register_event(event.key_down, kp)
