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

-- local tweener = d.tween.new()

local t = nil

function frame()

	if d.app.key_pressed("esc") then
		d.app.quit()
	end

	if d.app.key_pressed("space") then
		d.audio.play(snd, { speed = rand(0.5, 1.5) })
	end

	if d.app.mouse_pressed() then
		t = d.tween.start(pos, d.gfx.mouse_pos(), 1, d.ease.out_elastic, function(p)
			pos = p
			-- for k, v in pairs(t) do
				-- print(k, v)
			-- end
		end)
	end

	if t then
		-- print(t)
		d.tween.update(t, d.app.dt())
	end
	-- tweener.update()

	local t = d.app.time()

	d.gfx.clear()
	d.gfx.blit_bg()

	d.gfx.push()
	d.gfx.pos(pos)
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
