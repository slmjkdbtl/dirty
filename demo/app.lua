local WIDTH = 480
local HEIGHT = 480
local SCALE = 4

local img = gfx.img_load("demo/res/wizad.png")
local pos = vec2(20, 20)

function init()
	gfx.init({
		scale = SCALE,
	})
end

function wave(a, b, t)
	return a + ((math.sin(t) + 1) / 2) * (b - a)
end

function frame()
	if app.key_pressed("esc") then
		app.quit()
	end
	if app.mouse_pressed() then
		pos = gfx.mouse_pos()
	end
	local t = app.time()
	gfx.blit_bg()
	gfx.push()
	gfx.pos(pos)
	gfx.rot(t)
	gfx.scale(vec2(wave(1, 2, t)))
	-- for i = 0, 1000 do
		gfx.draw_img(img)
	-- end
	gfx.pop()
	gfx.blit_poly({ vec2(10, 0), vec2(22), vec2(0, 22) }, color(255, 0, 255))
	gfx.present()
end

app.run({
	title = "test",
	width = WIDTH,
	height = HEIGHT,
	init = init,
	frame = frame,
})
