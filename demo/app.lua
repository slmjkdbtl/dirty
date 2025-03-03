local WIDTH = 480
local HEIGHT = 480
local SCALE = 4

local img = gfx.img_load("demo/res/wizard.png")
local model = gfx.model_load("demo/res/btfly.glb")
local snd = audio.sound_load("demo/res/pop.ogg")
local pos = vec2(20, 20)

function init()
	gfx.init({
		clear_color = color(0, 0, 0, 255),
		scale = SCALE,
	})
	audio.init()
end

function wave(a, b, t)
	return a + ((math.sin(t) + 1) / 2) * (b - a)
end

function frame()
	if app.key_pressed("esc") then
		app.quit()
	end
	if app.key_pressed("space") then
		audio.play(snd)
	end
	if app.mouse_pressed() then
		pos = gfx.mouse_pos()
	end
	local t = app.time()
	gfx.clear()
	gfx.blit_bg()

	gfx.push()
	gfx.pos3(vec3(gfx.width() / 2, gfx.height() / 2, 0))
	gfx.roty(t)
	gfx.rotz(t * -0.5)
	gfx.scale3(vec3(4, -4, 4))
	gfx.pos3(model.center * -1)
	gfx.draw_model(model)
	gfx.pop()

	gfx.present()
end

app.run({
	title = "test",
	width = WIDTH,
	height = HEIGHT,
	init = init,
	frame = frame,
})
