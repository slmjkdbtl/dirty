function clone(val)
	local ty = type(val)
	if ty == "userdata" then
		local name = getmetatable(val)["__name"]
		if name == "vec2" or name == "vec3" or name == "color" then
			return val:clone()
		else
			return val
		end
	elseif ty == "table" then
		local copy = {}
		for k, v in next, val, nil do
			copy[clone(k)] = clone(v)
		end
		setmetatable(copy, clone(getmetatable(val)))
	end
	return val
end

function table.hashset(t)
	local t2 = {}
	for _, v in ipairs(t) do
		t2[v] = true
	end
	return t2
end

function table.keys(t)
	local keys = {}
	for k in pairs(t) do
		keys[#keys + 1] = k
	end
	return keys
end

function table.values(t)
	local values = {}
	for _, v in pairs(t) do
		values[#values + 1] = v
	end
	return values
end

function table.map(t, func)
	local res = {}
	for k, v in pairs(t) do
		res[k] = func(v, k)
	end
	return res
end

function table.filter(t, pred)
	local res = {}
	for k, v in pairs(t) do
		if pred(v, k) then
			res[k] = v
		end
	end
	return res
end

function table.find(t, val)
	for i, v in ipairs(t) do
		if v == val then
			return i
		end
	end
	return nil
end

function table.clone(t)
	return clone(t)
end

function math.round(n)
	return math.floor(n + 0.5)
end

function math.clamp(x, min, max)
	return x < min and min or (x > max and max or x)
end

function math.chance(c)
	return math.random() <= c
end

function math.wave(a, b, t)
	return a + (math.sin(t) + 1) / 2 * (b - a)
end

function string.split(s, sep)
	local l = {}
	for part in s:gmatch("([^" .. sep .. "]+)") do
		l[#l + 1] = part
	end
	return l
end

function string.trim(s)
	return s:match("^%s*(.-)%s*$")
end

d.tween = {}

local function lerp(a, b, t)
	return a + (b - a) * t
end

function d.tween.update(t, dt)
	if t.done or t.paused then
		return
	end
	t.elapsed = t.elapsed + dt
	local tt = t.elapsed / t.duration
	if tt >= 1.0 then
		t.done = true
		t.elapsed = t.duration
		t.val = t.to
		t.action(t)
		return
	end
	local t2 = t.func(tt)
	local ty = type(t.val)
	if ty == "number" then
		t.val = lerp(t.from, t.to, t2)
	elseif ty == "userdata" then
		local name = getmetatable(t.val)["__name"]
		if name == "vec2" then
			t.val.x = lerp(t.from.x, t.to.x, t2)
			t.val.y = lerp(t.from.y, t.to.y, t2)
		elseif name == "vec3" then
			t.val.x = lerp(t.from.x, t.to.x, t2)
			t.val.y = lerp(t.from.y, t.to.y, t2)
			t.val.z = lerp(t.from.z, t.to.z, t2)
		elseif name == "color" then
			t.val.r = lerp(t.from.r, t.to.r, t2)
			t.val.g = lerp(t.from.g, t.to.g, t2)
			t.val.b = lerp(t.from.b, t.to.b, t2)
			t.val.a = lerp(t.from.a, t.to.a, t2)
		else
			error("unsupported tween type: " .. name)
		end
	else
		error("unsupported tween type: " .. ty)
	end
	t.action(t)
end

function d.tween.new(from, to, duration, func, action, name)
	return {
		from = from,
		to = to,
		val = clone(from),
		duration = duration,
		func = func,
		action = action,
		name = name,
		elapsed = 0,
		done = false,
		paused = false,
		update = d.tween.update,
	}
end

function d.tween.manager()
	local m = {
		paused = false,
	}
	local tweens = {}
	function m:add(...)
		local t = d.tween.new(...)
		if t.name then
			for i = #tweens, 1, -1 do
				if tweens[i].name == t.name then
					table.remove(tweens, i)
				end
			end
		end
		tweens[#tweens + 1] = t
		return t
	end
	function m:add_async(...)
		local t = m:add(...)
		while not t.done do
			coroutine.yield()
		end
	end
	function m:update(dt)
		if m.paused then return end
		for i = #tweens, 1, -1 do
			local t = tweens[i]
			t:update(dt)
			if t.done then
				table.remove(tweens, i)
			end
		end
	end
	function m:clear()
		tweens = {}
	end
	return m
end

d.data = {}

local function serialize_inner(value, fmt, depth, seen)
	seen = seen or {}
	depth = depth or 0
	local nl = fmt and "\n" or ""
	local sp = fmt and " " or ""
	local indent = string.rep(sp, 2)
	local ty = type(value)
	local supported = table.hashset({ "table", "number", "string", "boolean", "nil" })
	if ty == "table" then
		if seen[value] then
			error("serialization cannot contain circular reference")
		end
		seen[value] = true
		local result = "{" .. nl
		local keys = {}
		for k in pairs(value) do
			table.insert(keys, k)
		end
		table.sort(keys, function(a, b)
			return tostring(a) < tostring(b)
		end)
		for _, k in ipairs(keys) do
			local v = value[k]
			if supported[type(v)] then
				result = result
					.. string.rep(indent, depth + 1)
					.. "["
					.. serialize_inner(k)
					.. "]" .. sp .. "=" .. sp
					.. serialize_inner(v, fmt, depth + 1, seen)
					.. ","
					.. nl
			end
		end
		result = result .. string.rep(indent, depth) .. "}"
		return result
	elseif ty == "string" then
		return string.format("%q", value)
	elseif ty == "boolean" or t == "nil" then
		return tostring(value)
	elseif ty == "number" then
		if      value ~=  value then return  "0/0"      --  nan
		elseif  value ==  1 / 0 then return  "1/0"      --  inf
		elseif  value == -1 / 0 then return "-1/0" end  -- -inf
		return tostring(value)
	else
		error("unsupported serialization type: " .. ty)
	end
end

function d.data.serialize(value, fmt)
	return serialize_inner(value, fmt, 0, {})
end

function d.data.deserialize(str)
	return load("return " .. str)()
end

function d.data.load(name, def)
	local dir = d.fs.data_dir()
	-- TODO
end

function d.data.save(name, data)
	local dir = d.fs.data_dir()
	-- TODO
end

function runner()
	local tasks = {}
	local r = {}
	function r:update()
		for i = 1, #tasks do
			tasks[i]()
		end
	end
	function r:run(task)
		local co = coroutine.create(task)
		tasks[#tasks + 1] = function()
			if coroutine.status(co) ~= "dead" then
				local success, err = coroutine.resume(co)
				if not success then
					error(err)
				end
			end
		end
	end
	return r
end

function wait(seconds)
	local start = d.app.time()
	while d.app.time() - start < seconds do
		coroutine.yield()
	end
end
