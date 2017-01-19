local function dump(name, v)
    print(name .. ": " .. v)
    show(name .. ": " .. v)
end

bashok = 0
bashfail = 0
combat = false
in_group = false
standing = true

function tel(w,str)
       local x = string.len(w)
       local t = string.sub(str, x+2)
       local act = string.format('telep %s $c0008-$c0007=$c0015 %s $c0007=$c0008-$c0013', w, t)
       send (act)
end

function is_article(str)
    if str == nil then
        return false
    end

	local s = string.lower(str)

	if s == "il" or s == "lo" or s == "la" or s == "un" or s == "una" or 
	   s == "a" or s == "the" or s == "dei" or s == "le" or s == "uno" then
		return true
	end

	return false
end

function one_word_name(p)
   local i = string.find(p, " ")
   if i then
       local first = string.sub(p, 0, i - 1)
       if is_article(first) then
           local second = string.sub(p, i + 1)
           i = string.find(second, " ")
           if i then
               return string.sub(second, 0, i - 1)
           else
               return second
           end
       else
           return first
       end
   else
       return p
   end
end

function group_me(p)
   local i = string.find(p, " ")

-- Se e` polato tolgo l'estensione
	if i then
        local first = string.sub(p, 0, i - 1)

        if is_article(first) == false then
            p = first
        end
    end

    send("group " .. p)
end    

function handle_speaks(p, t, c)
    local namecol = "$c0015"

-- I say hanno il colore del nome diverso e non hanno lo strip del nome
    if c == namecol then
        namecol = "$c0006"
    else
        local i = string.find(p, " ")

-- Se e` polato tolgo l'estensione
	if i then
            local first = string.sub(p, 0, i - 1)

            if is_article(first) == false then
                p = first
            end
        end
    end

-- Gestisco l'afk
    if is_afk and c == "$c0013" then
        send("tele " .. p .. " [messaggio automatico] sono AFK ti rispondero` appena posso!;afk")
    end

-- Gestisco qui i cambi d'arma
    if p == leader then
        if t == "pierce'" then
            changeweapon("pierce")
        elseif t == "slash'" then
            changeweapon("slash")
        elseif t == "blunt'" then
            changeweapon("blunt")
        end
    end
    
    window("speaks", c .. "(" .. namecol .. p .. c .. ") '" .. t )
end

function handle_send(c, t) handle_speaks(c, t, "$c0013") end
function handle_shout(c, t) handle_speaks(c, t, "$c0009") end
function handle_goss(c, t) handle_speaks(c, t, "$c0011") end
function handle_think(c, t) handle_speaks(c, t, "$c0012") end
function handle_ot(c, t) handle_speaks(c, t, "$c0008") end
function handle_ctell(c, t) handle_speaks(c, t, "$c0014") end
function handle_ask(c, t) handle_speaks(c, t, "$c0006") end
function handle_say(c, t) handle_speaks(c, t, "$c0015") end
function handle_gt(c, t) handle_speaks(c, t, "$c0012") end

function bash_ok() bashok = bashok + 1 end
function bash_ko() bashfail = bashfail + 1 standing = false send("stand") end
function standing_ok()
	if standing == false then 
		standing = true 
	end 
end

function standing_ko()
	if standing == true then 
		standing = false 
        send("stand")
	end 
end

function disarm_ko()
	standing = false
	send("stand")
end

local function color(number)
    if number < 40 then
        return "$c0009"
    elseif number < 75 then
        return "$c0011"
    elseif number < 100 then
        return "$c0002" 
    else
        return "$c0010"
    end
end

function group_normal(name, hp, mana, move)
    if string.find(name, "(Capo)") or string.find(name, "(O)") then
        return
    end
   
    local h = tonumber(hp)
    local m = tonumber(mana)
    local v = tonumber(move)

    window("group", string.format("$c0009%16s $c0015H:%s%3d%% $c0015M:%s%3d%% $c0015V:%s%3d%%",
                        string.gsub(name, " ", ""), 
                        color(h), h,  color(m), m, color(v), v ) )
end

function group_leader(name, hp, mana, move)
    clear("group")
    leader = string.gsub(name, " ", "")

    local h = tonumber(hp)
    local m = tonumber(mana)
    local v = tonumber(move)

    window("group", string.format("$c0009%16s $c0015H:%s%3d%% $c0015M:%s%3d%% $c0015V:%s%3d%%",
                        leader, color(h), h, color(m), m, color(v), v ) )

end

show("$c0015Started script configuration...")

dump("Versione programma:", VERSION)

-- speaks
trigger("^[%1] dice al gruppo '%2", "handle_gt");
trigger("^[%1] ti manda il pensiero '%2", "handle_send");
trigger("^[%1] ti manda il messaggio '%2", "handle_send");
trigger("^[%1] ti dice '%2", "handle_send");
trigger("^[%1] dice '%2", "handle_say");
trigger("^[%1] ti chiede '%2", "handle_ask");
trigger("^[%1] vi dice '%2", "handle_goss");
trigger("^[%1] grida '%2", "handle_shout");
trigger("^::%1:: '%2", "handle_think");
trigger("^[%1] dice alle Lame '%2", "handle_ctell");
trigger("^[%1] '%2", "handle_ot");

-- Berserk/Bash counters
trigger("schiva il tuo urto", "bash_ko");
trigger("ai una forte spinta", "bash_ok");
trigger("gia` in piedi", "standing_ok");
trigger("i alzi.", "standing_ok");
trigger("Tenti di disarmare", "disarm_ko");
trigger("ti manda a gambe", "standing_ko");
trigger("Tenti di disarmare", "standing_ko");
trigger("ti schiaccia con violenza", "standing_ko");

-- GROUP window
trigger("%1 (Capo) HP: %2% MANA: %3% MV: %4%", "group_leader")
trigger("%1 (O) HP:%2% MANA:%3% MV:%4%", "group_normal")
trigger("%1 HP:%2% MANA:%3% MV:%4%", "group_normal")

-- autogroup
trigger("%1 inizia a seguirti", "group_me")

show("$c0015Configuration complete$c0007.")
