-- stdlib for dirty lua binding

local function clone_inner(val, seen)
	seen = seen or {}
	local ty = type(val)
	if ty == "userdata" then
		local name = getmetatable(val)["__name"]
		if name == "vec2" or name == "vec3" or name == "color" then
			return val:clone()
		else
			return val
		end
	elseif ty == "table" then
		if seen[val] then
			return seen[val]
		end
		local copy = {}
		seen[val] = copy
		for k, v in next, val, nil do
			copy[clone_inner(k, seen)] = clone_inner(v, seen)
		end
		setmetatable(copy, clone_inner(getmetatable(val), seen))
	end
	return val
end

function clone(val)
	return clone_inner(val, {})
end

function table.hashset(t)
	local t2 = {}
	for _, v in pairs(t) do
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
	if table.isarray(t) then
		for i, v in ipairs(t) do
			if pred(v, i) then
				res[#res + 1] = v
			end
		end
	else
		for k, v in pairs(t) do
			if pred(v, k) then
				res[k] = v
			end
		end
	end
	return res
end

function table.find(t, val)
	local iter = table.isarray(t) and ipairs or pairs
	for k, v in iter(t) do
		if v == val then
			return k
		end
	end
	return nil
end

function table.clone(t)
	return clone(t)
end

function table.random(t)
	return t[math.random(#t)]
end

function table.weighedchoice(t)
	local sum = 0
	for _, v in pairs(t) do
		assert(v >= 0, "weight value less than zero")
		sum = sum + v
	end
	assert(sum ~= 0, "all weights are zero")
	local rnd = rand(sum)
	for k, v in pairs(t) do
		if rnd < v then
			return k
		end
		rnd = rnd - v
	end
end

function table.shuffle(t)
	local rtn = {}
	for i = 1, #t do
		local r = math.random(i)
		if r ~= i then
			rtn[i] = rtn[r]
		end
		rtn[r] = t[i]
	end
	return rtn
end

function table.push(t, v)
	t[#t + 1] = v
	return v
end

function table.delete(t, val)
	local i = table.find(t, val)
	if i then
		table.remove(t, i)
	end
end

function table.slice(t, a, b)
	b = b or #t
	a = a or 1
	local t2 = {}
	for i = a, b do
		t2[#t2 + 1] = t[i]
	end
	return t2
end

function table.isarray(t)
	return type(t) == "table" and t[1] ~= nil
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

function math.sign(x)
	return x < 0 and -1 or 1
end

function math.lerp(a, b, t)
	return a + (b - a) * t
end

function math.smooth(a, b, t)
	local m = t * t * (3 - 2 * t)
	return a + (b - a) * m
end

function math.map(v, l1, h1, l2, h2)
	return l2 + (v - l1) * (h2 - l2) / (h1 - l1)
end

function string.split(s, sep)
	local l = {}
	if not sep or sep == "" then
		for i = 1, #s do
			l[i] = s:sub(i, i)
		end
	else
		for part in s:gmatch("([^" .. sep .. "]+)") do
			l[#l + 1] = part
		end
	end
	return l
end

function string.trim(s)
	return s:match("^%s*(.-)%s*$")
end

function string.startswith(s, start)
	return s:sub(1, #start) == start
end

function string.endswith(s, ending)
	return ending == "" or s:sub(-#ending) == ending
end

d.tween = {}

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
		t.val = math.lerp(t.from, t.to, t2)
	elseif ty == "userdata" then
		local name = getmetatable(t.val)["__name"]
		if name == "vec2" then
			t.val.x = math.lerp(t.from.x, t.to.x, t2)
			t.val.y = math.lerp(t.from.y, t.to.y, t2)
		elseif name == "vec3" then
			t.val.x = math.lerp(t.from.x, t.to.x, t2)
			t.val.y = math.lerp(t.from.y, t.to.y, t2)
			t.val.z = math.lerp(t.from.z, t.to.z, t2)
		elseif name == "color" then
			t.val.r = math.lerp(t.from.r, t.to.r, t2)
			t.val.g = math.lerp(t.from.g, t.to.g, t2)
			t.val.b = math.lerp(t.from.b, t.to.b, t2)
			t.val.a = math.lerp(t.from.a, t.to.a, t2)
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
	-- TODO: name
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

d.timer = {}

function d.timer.new(secs, action, loop)
	local t = {
		time = 0,
		paused = false,
		done = false,
	}
	function t:update(dt)
		if self.paused or self.done then return end
		self.time = self.time + dt
		if self.time >= secs then
			if action then
				action()
			end
			if loop then
				self.time = 0
			else
				self.done = true
			end
		end
	end
	return t
end

function d.timer.manager()
	local timers = {}
	local m = {
		paused = false,
	}
	function m:add(...)
		local t = d.timer.new(...)
		timers[#timers + 1] = t
		return t
	end
	function m:wait(secs)
		local t = self:add(secs)
		while not t.done do
			coroutine.yield()
		end
	end
	function m:update(dt)
		for i = #timers, 1, -1 do
			local t = timers[i]
			t:update(dt)
			if t.done then
				table.remove(timers, i)
			end
		end
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

d.task = {}

function d.task.isrunning()
	local co, ismain = coroutine.running()
	return co and not ismain
end

function d.task.manager()
	local tasks = {}
	local m = {}
	function m:update()
		for i = #tasks, 1, -1 do
			local task = tasks[i]
			task.run()
		end
		tasks = table.filter(tasks, function(t)
			return coroutine.status(t.co) ~= "dead"
		end)
	end
	function m:run(task)
		local co = coroutine.create(task)
		local task = {
			done = function()
				return coroutine.status(co) == "dead"
			end,
			co = co,
			run = function()
				if coroutine.status(co) == "dead" then
					return
				end
				local success, err = coroutine.resume(co)
				if not success then
					error(err)
				end
			end,
		}
		tasks[#tasks + 1] = task
		return task
	end
	return m
end

function d.task.loader()
	local loader = {}
	local actions = {}
	local loaded = 0
	function loader:add(action)
		actions[#actions + 1] = action
	end
	function loader:start()
		loaded = 0
		for _, action in ipairs(actions) do
			action()
			loaded = loaded + 1
			if d.task.isrunning() then
				coroutine.yield()
			end
		end
	end
	function loader:progress()
		return loaded / #actions
	end
	function loader:finished()
		return loaded >= #actions
	end
	function loader:loaded()
		return loaded
	end
	function loader:count()
		return #actions
	end
	return loader
end

function wait(secs)
	local start = d.app.time()
	while d.app.time() - start < secs do
		coroutine.yield()
	end
end
