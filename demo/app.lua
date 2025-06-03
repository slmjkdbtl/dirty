local WIDTH = 480
local HEIGHT = 480
local SCALE = 2

local img = d.gfx.img_load("demo/res/wizard.png")
local model = d.gfx.model_load("demo/res/btfly.glb")
local snd = d.audio.sound_load("demo/res/pop.ogg")
local pos = vec2(20, 20)

function init()
	d.gfx.init({
		clear_color = color(0, 0, 0, 255),
		scale = SCALE,
		depth_test = true,
		backface_cull = true,
	})
	d.audio.init()
end

function wave(a, b, t)
	return a + ((math.sin(t) + 1) / 2) * (b - a)
end

local tweener = d.tween.manager()

local co_tasks = {}

function co(task)
	local co = coroutine.create(task)
	co_tasks[#co_tasks + 1] = function()
		if coroutine.status(co) ~= "dead" then
			local success, err = coroutine.resume(co)
			if not success then
				error(err)
			end
		end
	end
end

function wait(seconds)
	local start = d.app.time()
	while d.app.time() - start < seconds do
		coroutine.yield()
	end
end

function tween(...)
	local t = tweener:add(...)
	while not t.done do
		coroutine.yield()
	end
end

co(function()
	wait(2)
	print("2 sec later")
	tween(pos, d.gfx.mouse_pos(), 1, d.ease.out_elastic, function(t)
		pos = t.val
	end, "move")
	print("tween done")
end)

function frame()

	for i = 1, #co_tasks do
		co_tasks[i]()
	end

	if d.app.key_pressed("esc") then
		d.app.quit()
	end

	if d.app.key_pressed("space") then
		d.audio.play(snd, { speed = rand(0.5, 1.5) })
	end

	if d.app.mouse_pressed() then
		tweener:add(pos, d.gfx.mouse_pos(), 1, d.ease.out_elastic, function(t)
			pos = t.val
		end, "move")
	end

	tweener:update(d.app.dt())

	d.gfx.clear()
	d.gfx.blit_bg()

	d.gfx.push()
	d.gfx.pos(pos)
	d.gfx.pos(img:dimension() * -0.5)
	d.gfx.draw_img(img)
	d.gfx.pop()

	d.gfx.present()

end

d.app.run({
	title = "test",
	width = WIDTH,
	height = HEIGHT,
	init = init,
	frame = frame,
})
