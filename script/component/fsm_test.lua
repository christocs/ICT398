function begin() 
    print("hello world")
end
function shutdown()
    print("power off!")
end

local fsm = engine.fsm_builder()
    :state("init")
    :on("begin")
        :go("start")
        :call(begin)
    :state("start")
    :on("poweroff")
        :go("off")
    :state("off")
        :on_enter(shutdown) -- upon entering the 'off' state, shutdown is called
    :initial("init")
    :build()

fsm:fire("begin") -- moves from state 'init' to 'start' when 'begin' is received
fsm:fire("poweroff") -- moves from state 'start' to 'off' when 'poweroff' is received
