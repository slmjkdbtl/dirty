local WIDTH = 480
local HEIGHT = 480
local SCALE = 2

local toload = {
	img = {
		["wizard"] = "demo/res/wizard.png",
	},
	snd = {
		["pop"] = "demo/res/pop.ogg",
	},
	model = {
		["btfly"] = "demo/res/btfly.glb",
	},
}

local assets = {
	img = {},
	snd = {},
	model = {},
}

local tweener = d.tween.manager()
local timer = d.timer.manager()
local task = d.task.manager()
local loader = d.task.loader()

for name, path in pairs(toload.img) do
	loader:add(function()
		assets.img[name] = d.gfx.img_load(path)
	end)
end

for name, path in pairs(toload.model) do
	loader:add(function()
		assets.model[name] = d.gfx.model_load(path)
	end)
end

for name, path in pairs(toload.snd) do
	loader:add(function()
		assets.snd[name] = d.audio.sound_load(path)
	end)
end

local pos = vec2(20, 20)

local data = {
	x = 42,
	list = { 5, 3, 9 },
	name = "space55",
}

print(d.data.serialize(data, true))
local a = d.data.deserialize(d.data.serialize(data))
print(d.data.serialize(a, true))

function init()
	d.gfx.init({
		clear_color = color(0, 0, 0, 255),
		scale = SCALE,
		depth_test = true,
		backface_cull = true,
	})
	d.audio.init()
end

task:run(function()
	loader:start()
	print("loaded")
	timer:wait(2)
	print("2 sec later")
	tweener:add_async(pos, d.gfx.mouse_pos(), 1, d.ease.out_elastic, function(t)
		pos = t.val
	end, "move")
	print("tween done")
end)

function frame()

	if d.app.key_pressed("esc") then
		d.app.quit()
	end

	if d.app.key_pressed("space") then
		d.audio.play(assets.snd["pop"], { speed = rand(0.5, 1.5) })
	end

	if d.app.mouse_pressed() then
		tweener:add(pos, d.gfx.mouse_pos(), 1, d.ease.out_elastic, function(t)
			pos = t.val
		end, "move")
	end

	task:update()
	timer:update(d.app.dt())
	tweener:update(d.app.dt())

	d.gfx.clear()
	d.gfx.blit_bg()

	if not loader:finished() then
		local x1 = 20
		local x2 = 120
		local y1 = 20
		local y2 = 30
		d.gfx.blit_rect(vec2(x1, y1), vec2(x2, y2), color(0, 0, 0))
		d.gfx.blit_rect(vec2(x1, y1), vec2(math.map(loader:progress(), 0, 1, x1, x2), y2), color(255, 255, 255))
		d.gfx.present()
		return
	end

	d.gfx.push()
	d.gfx.pos(pos)
	d.gfx.pos(assets.img["wizard"]:dimension() * -0.5)
	d.gfx.draw_img(assets.img["wizard"])
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
