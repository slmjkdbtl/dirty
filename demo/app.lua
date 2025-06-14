v2 = d.vec2
rgb = d.rgb
rand = d.rand

local WIDTH = 480
local HEIGHT = 480
local SCALE = 2

print(rgb(255, 100, 24):hsl())

local tween = d.tween.manager()
local timer = d.timer.manager()
local task = d.task.manager()
local assets = d.task.assetloader({
	img = {
		["wizard"] = "demo/res/wizard.png",
	},
	sound = {
		["pop"] = "demo/res/pop.ogg",
	},
	model = {
		["btfly"] = "demo/res/btfly.glb",
	},
})

local pos = v2(20, 20)

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
		clear_color = rgb(0, 0, 0),
		scale = SCALE,
		depth_test = true,
		backface_cull = true,
	})
	d.audio.init()
end

task:run(function()
	assets.loader:load()
	print("loaded")
	timer:wait(2)
	print("2 sec later")
	tween:add_async(pos, d.gfx.mouse_pos(), 1, d.ease.out_elastic, function(t)
		pos = t.val
	end)
	print("tween done")
end)

function frame()

	task:update()
	timer:update(d.app.dt())
	tween:update(d.app.dt())

	if d.app.key_pressed("esc") then
		d.app.quit()
	end

	if not assets.loader:finished() then
		d.gfx.clear()
		d.gfx.blit_bg()
		local x1 = 20
		local x2 = 120
		local y1 = 20
		local y2 = 30
		local xl = math.map(assets.loader:progress(), 0, 1, x1, x2)
		d.gfx.blit_rect(v2(x1, y1), v2(x2, y2), rgb(0, 0, 0))
		d.gfx.blit_rect(v2(x1, y1), v2(xl, y2), rgb(255, 255, 255))
		d.gfx.present()
		return
	end

	if d.app.key_pressed("space") then
		d.audio.play(assets.sound["pop"], { speed = rand(0.5, 1.5) })
	end

	if d.app.mouse_pressed() then
		print(d.app.mouse_pos())
		tween:add(pos, d.gfx.mouse_pos(), 1, d.ease.out_elastic, function(t)
			pos = t.val
		end)
	end

	print(d.gfx.mouse_pos())

	d.gfx.clear()
	d.gfx.blit_bg()

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
