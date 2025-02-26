local WIDTH = 240
local HEIGHT = 240
local SCALE = 2

gfx.init({
	width = WIDTH,
	height = HEIGHT,
})

local img = gfx.img_load("demo/res/wizard.png")
print(img.width, img.height)
print(fs.read_text("Makefile"))

function frame()
	if app.key_pressed("esc") then
		app.quit()
	end
	gfx.blit_bg()
	gfx.present()
end

app.run({
	title = "test",
	width = WIDTH * SCALE,
	height = HEIGHT * SCALE,
	init = init,
	frame = frame,
})
