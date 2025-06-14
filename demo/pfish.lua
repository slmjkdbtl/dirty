v2 = d.vec2
rgb = d.rgb
rand = d.rand
randi = d.randi

local WIDTH = 640
local HEIGHT = 480
local NUM_FISH = 160
local PFISH_POS = 3
local MAX_SQUEEZE_INTERVAL = 30
local SQUEEZE_INTENSITY = 4

local assets = d.task.assetloader({
	img = {
		["fish"] = "demo/res/fish.png",
		["pfish"] = "demo/res/pfish.png",
	},
	sound = {
		["pop"] = "demo/res/pop.ogg",
	},
})

assets.loader:load()

function make_fish(pos, is_pink)

	local fish = {
		is_pink = is_pink,
	}
	local pos = pos:clone()
	local rot = randi(0, 360)
	local timer = d.timer.manager()
	local tween = d.tween.manager()
	local img = assets.img[is_pink and "pfish" or "fish"]
	local origin = img:dimension() * 0.5

	local rect = {
		v2(16, 4),
		v2(img.width - 32, img.height - 4),
	}

	local poly = d.geo.transform_rect(rect[1], rect[2], d.mat4())

	function calc_transform()
		return d.mat4()
			:translate(pos)
			:rotate(rot)
			:translate(-origin)
	end

	-- local t = mat4()

	-- function get_bbox()
		-- return table.map(rect, function(pt)
			-- return t * pt
		-- end)
	-- end

	-- local bbox = get_bbox()

	-- function fish:has_pt(pt)
		-- return d.col.pt_poly(pt, bbox)
	-- end

	function fish:draw()
		d.gfx.push()
		d.gfx.pos(pos)
		d.gfx.rot(rot)
		d.gfx.pos(-origin)
		d.gfx.draw_img(img)
		d.gfx.pop()
	end

	function fish:update()
		local dt = d.app.dt()
		timer:update(dt)
		tween:update(dt)
	end

	function fish:squeeze()
		d.audio.play(assets.sound["pop"], {
			speed = rand(0.1, 1.2),
			volume = rand(0.5, 1.0),
		})
		local orig_rot = rot
		rot = rot + (rand() > 0.5 and -SQUEEZE_INTENSITY or SQUEEZE_INTENSITY)
		tween:add(rot, orig_rot, 1, d.ease.out_elastic, function(t)
			rot = t.val
		end)
	end

	timer:add(rand(1, MAX_SQUEEZE_INTERVAL), function(t)
		fish:squeeze()
		t:reset(rand(1, MAX_SQUEEZE_INTERVAL))
	end)

	return fish

end

local pool = {}

function init()
	d.gfx.init({
		clear_color = rgb(0, 0, 0),
	})
	d.audio.init()
	for i = 1, NUM_FISH do
		local is_pink = i == PFISH_POS
		local pad = is_pink and 200 or 100
		local x = randi(pad, d.gfx.width() - pad)
		local y = randi(pad, d.gfx.height() - pad)
		local fish = make_fish(v2(x, y), is_pink)
		pool[i] = fish
	end
end

function frame()

	local mpos = d.gfx.mouse_pos()

	if d.app.key_pressed("esc") then
		d.app.quit()
	end

	if d.app.mouse_pressed() then
		for i = #pool, 1, -1 do
			local fish = pool[i]
			-- TODO
		end
	end

	d.gfx.clear()

	for _, fish in ipairs(pool) do
		fish:update()
	end

	for _, fish in ipairs(pool) do
		fish:draw()
	end

	d.gfx.blit_text(d.app.fps(), v2(10, 10))
	d.gfx.present()

end

d.app.run({
	title = "pfish",
	width = WIDTH,
	height = HEIGHT,
	init = init,
	frame = frame,
})
