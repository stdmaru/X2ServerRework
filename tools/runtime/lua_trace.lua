-- Canonical trace of what a Lua script does to its environment (calls, arguments, globals it defines), so that
-- two versions of a script can be compared by behaviour instead of text (docs/ferramentas/runtime.md).
-- usage: luajit tools/runtime/lua_trace.lua <file>
local path = arg[1]
local f = assert(io.open(path, "rb")); local src = f:read("*a"); f:close()
if src:sub(1,3) == "\239\187\191" then src = src:sub(4) end
local out = {}
local function emit(s) out[#out+1] = s end
local R = {}           -- recorder metatable
local names = setmetatable({}, {__mode="k"})
local ser
local function rec(name) local o = setmetatable({}, R); names[o] = name; return o end
local function fmtnum(v) if v == math.floor(v) and math.abs(v) < 2^53 then return string.format("%d", v) end return string.format("%.17g", v) end
local seen
ser = function(v, depth)
  depth = depth or 0
  local t = type(v)
  if t == "number" then return fmtnum(v)
  elseif t == "string" then return string.format("%q", v)
  elseif t == "boolean" or t == "nil" then return tostring(v)
  elseif t == "function" then return "<function>"
  elseif t == "table" then
    if names[v] then return "@" .. names[v] end
    if depth > 30 then return "<deep>" end
    local keys = {}
    for k in pairs(v) do keys[#keys+1] = k end
    table.sort(keys, function(a, b) local ta, tb = type(a), type(b); if ta ~= tb then return ta < tb end; if ta == "number" or ta == "string" then return a < b end; return tostring(a) < tostring(b) end)
    local parts = {}
    for _, k in ipairs(keys) do parts[#parts+1] = "[" .. ser(k, depth+1) .. "]=" .. ser(v[k], depth+1) end
    return "{" .. table.concat(parts, ",") .. "}"
  end
  return "<" .. t .. ">"
end
R.__index = function(o, k) return rec(names[o] .. "." .. ser(k)) end
R.__newindex = function(o, k, v) emit("SET " .. names[o] .. "." .. ser(k) .. "=" .. ser(v)) end
R.__call = function(o, ...)
  local a = {...}; local n = select("#", ...); local p = {}
  for i = 1, n do p[i] = ser(a[i]) end
  local name = names[o] .. "(" .. table.concat(p, ",") .. ")"
  emit("CALL " .. name)
  return rec("ret:" .. name)
end
for _, op in ipairs{"add","sub","mul","div","mod","pow","concat"} do
  R["__" .. op] = function(a, b) return rec("(" .. ser(a) .. " " .. op .. " " .. ser(b) .. ")") end
end
R.__unm = function(a) return rec("(-" .. ser(a) .. ")") end
R.__len = function(a) return 0 end
R.__lt = function(a, b) return ser(a) < ser(b) end
R.__le = function(a, b) return ser(a) <= ser(b) end
R.__tostring = function(a) return "@" .. names[a] end
local real = {string=string, math=math, table=table, pairs=pairs, ipairs=ipairs, tostring=tostring, tonumber=tonumber,
  type=type, select=select, next=next, unpack=unpack, setmetatable=setmetatable, getmetatable=getmetatable,
  rawget=rawget, rawset=rawset, pcall=pcall, error=error, assert=assert, os={time=function() return 0 end, date=function() return "" end}}
local defined = {}
local env = setmetatable({}, {
  __index = function(_, k) if defined[k] ~= nil then return defined[k] end; if real[k] ~= nil then return real[k] end; return rec(tostring(k)) end,
  __newindex = function(_, k, v) defined[k] = v; emit("GSET " .. tostring(k)) end })
local fn, err = loadstring(src, "=" .. path)
if not fn then print("LOADERROR " .. err); os.exit(2) end
setfenv(fn, env)
local ok, e = pcall(fn)
if not ok then emit("RUNERROR " .. tostring(e):gsub("^[^:]*:%d+: ", "")) end
local ks = {}; for k in pairs(defined) do ks[#ks+1] = k end; table.sort(ks)
for _, k in ipairs(ks) do emit("G " .. k .. "=" .. ser(defined[k])) end
io.write(table.concat(out, "\n"), "\n")
