local WIDTH = 480
local HEIGHT = 480
local SCALE = 2

local img = d.gfx.img_load("demo/res/wizard.png")
local model = d.gfx.model_load("demo/res/btfly.glb")
local snd = d.audio.sound_load("demo/res/pop.ogg")
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

local tweener = d.tween.manager()

local co = runner()

co:run(function()
	wait(2)
	print("2 sec later")
	tweener:add_async(pos, d.gfx.mouse_pos(), 1, d.ease.out_elastic, function(t)
		pos = t.val
	end, "move")
	print("tween done")
end)

function frame()

	co:update()

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
