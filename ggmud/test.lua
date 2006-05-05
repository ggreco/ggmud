local function dump(name, v)
    print(name .. ": " .. v)
    show(name .. ": " .. v)
end

max_hp = 0
actual_hp = 0
max_move = 0
actual_move = 0
diamanti = 0
rubini = 0
zaffiri = 0
smeraldi = 0
punti_gemma = 0
punti_totali = 0
idle_loops = 0

is_afk = false
is_hide = false
logged = false
combat = false
in_group = false
standing = true
berserked = false
full_auto = false
use_autoassist = true
weapon = "UNDEF"
danno = "UNDEF"
bashtarget = nil
do_not_remove_afk = false

local function set_hide_off()
    if is_hide then
        is_hide = false
        show("$c0015HIDE mode $c0009OFF$c0007")    
    end
end

function set_nastrobash(target)
    if bashtarget == target then
        return
    elseif target == "" then
        bashtarget = nil
        return
    end

    bashtarget = target
    show("$C0015AVVIO $c0011NASTROBASH $c0009" .. bashtarget)
    send("bash " .. bashtarget)
end

function nastrobash()
    if bashtarget then
        send("bash " .. bashtarget)
    end
end

function autoassist(pg, stato)
    set_hide_off()
    
    if combat == true or use_autoassist == false then
        return
    end
    
    if string.lower(pg) == string.lower(tank) then
        if full_auto == false then
            send("assist " .. pg)
        else
            if stato == true then
                send("as " .. pg)
            else
                send("ass " .. pg .. ";bash")
            end
        end
    end
end

function autoassist_ko(pg) autoassist(pg, false) end
function autoassist_ok(pg) autoassist(pg, true) end


function update_status()
    clear("Status")
    
    if weapon == asl then
        danno = "SLASH"
    elseif weapon == abl then
        danno = "BLUNT"
    elseif weapon == api then
        danno = "PIERCE"
    end

    local combat_state = "$c0011IDLE$c0007"

    local combat_details = ""

    if combat == true then
        combat_state = "$c0011COMBAT$c0007"

        if standing then
            combat_details = "\n$c0010STANDING"
        else
            combat_details = "\n$C0009RESTING"
        end

        if berserked then
            combat_details = combat_details .. " $c0011BERSERKED"
        end
    end
    

  
    window("Status", "$c0007Arma: $c0014" .. danno .. " $c0007($c0015" .. weapon .. 
                       "$c0007) " .. combat_state ..
                     "\nGroup leader: $c0015" .. leader .. " $c0007Tank: $c0015" .. tank ..
                     "\n$c0007XP: $c0015" .. actualexp .. " $c0007Gold: $c0015" .. actualgold .. 
                       "$c0007 Align: $c0015".. align ..
                     "\n$c0007D: $c0015" .. diamanti .. " $c0007S: $c0010" .. smeraldi .. 
                       " $c0007 R: $c0009" .. rubini .. " $c0007Z: $c0005" .. zaffiri ..
                     "\n$c0007Punti gemma: $c0015" .. punti_gemma .. 
                       " $c0007totali: $c0015" .. punti_totali .. combat_details)
end

--  1164(1164) hit, 55(100) mana e 52(155) punti di

function score_line_health(line)
    _, _, actual_hp, max_hp, actual_move, max_move = 
                string.find(line, "(%d+)%((%d+)%) hit, %d+%(%d+%) mana e (%d+)%((%d+)%) punti")
end

function score_line_gemme(d, s, r, z)
    diamanti = d
    smeraldi = s
    rubini = r
    zaffiri = z
end

function score_line_stats(_exp, gold)
    actualexp = _exp
    actualgold = gold
end

function score_line_align(a)
    align = a
end

function score_line_punti(g, t)
    set_hide_off()

    punti_gemma = g
    punti_totali = t

    update_status()
end

function myidle()
   if logged == false then
       return
   end

   idle_loops = idle_loops + 1
   
-- se non sono in combattimento lancio uno score
   if idle_loops > 75 and 
      combat == false and 
      is_afk == false and 
      is_hide == false then
       idle_loops = 0
       send("score")
   end
end

-- ##HP:1164 MV:52 (TD) <nessuno>:* <nessuno>:*>
-- ##HP:1164 MV:60 (TD) Gregor:tagliato ecate:sanguinante>
function grab_prompt(p)
    _, _, actual_hp, actual_move = string.find(p, "HP:(%d+) MV:(%d+)")

    logged = true

    if string.find(p, "nessuno") == nil then
        if combat == false then
            bashtarget = nil
            set_hide_off() 
            combat = true
            update_status()
        end
    else
        if combat == true then
            combat = false
            update_status()
        end
    end
end

function changeweapon(w)
    local wt = string.lower(w)
   
    set_hide_off() 

-- dico in gossip l'arma se sono capogruppo
    if string.lower(leader) == "mongo" then
        send("gt " .. w)
    end

    local oldweapon = weapon

    if wt == "pierce" then
        weapon = api
    elseif wt == "slash" then
        weapon = asl
    elseif wt == "blunt" then
        weapon = abl
    end
 
    if oldweapon == "UNDEF" then
        send("rem " .. asl .. ";rem " .. api .. 
             ";rem " .. abl .. ";wield " .. weapon)

        update_status()   
        return 
    end

    if oldweapon == weapon then
        show("$c0009*** $c0015usi gia` " .. string.upper(w) .. " $c0009***")
    else
        send("rem " .. oldweapon .. ";wield " .. weapon)
        update_status()
    end
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
        	p = string.sub(p, 0, i - 1)
	    end
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

function set_afk_on() is_afk = true show("$c0015AFK mode $c0009ON$c0007") end
function set_afk_off() if do_not_remove_afk == false then is_afk = false show("$c0015AFK mode $c0009OFF$c0007") else do_not_remove_afk = false end end
function set_hide_on() is_hide = true show("$c0015HIDE mode $c0009ON$c0007") end
function berserk_ok() bersok = bersok + 1 berserked = true update_status() end
function berserk_ko() bersfail = bersfail + 1 berserked = false end
function bash_ok() bashok = bashok + 1 bashtarget = nil end
function bash_ko() bashfail = bashfail + 1 standing = false update_status() send("stand") bashtarget = nil end
function standing_ok()
	if standing == false then 
		standing = true 
		update_status() 
	end 
end

function standing_ko()
	if standing == true then 
		standing = false 
		update_status() 
        send("stand")
	end 
end

function disarm_ko()
	standing = false
	update_status()
	send("stand")
end

function set_berserk_off()
    if berserked then 
        berserked = false 
        update_status() 
    end 
end
    
function bersrep()
    local total = bersok + bersfail
    local perc = bersok * 100 / total     
    
    send(string.format("gt Su un totale di $c0014%d$c0012 BERSERK ne ho azzeccati $c0015%d$c0012 pari al $c0011%d%%", 
                       total, bersok, perc) )
end

function bashrep()
    local total = bashok + bashfail
    local perc = bashok * 100 / total
    
    send(string.format("gt Su un totale di $c0014%d$c0012 BASH ne ho azzeccati $c0015%d$c0012 pari al $c0011%d%%", 
                       total, bashok, perc) )
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

function check_afk()
    if is_afk then
	do_not_remove_afk = true
	send("afk")
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
    set_hide_off()
    clear("group")
    leader = string.gsub(name, " ", "")

    local h = tonumber(hp)
    local m = tonumber(mana)
    local v = tonumber(move)

    window("group", string.format("$c0009%16s $c0015H:%s%3d%% $c0015M:%s%3d%% $c0015V:%s%3d%%",
                        leader, color(h), h, color(m), m, color(v), v ) )

end

show("$c0015Started script configuration for $c0011Mongo$c0007...")

-- dump("tank", tank)
-- dump("Guida:", leader)
dump("Versione programma:", VERSION)

trigger("^##%1>", "grab_prompt")

-- autoassist
trigger("violenta spinta di %1 fa perdere", "autoassist_ok");
trigger("travolto da %1 che perde", "autoassist_ko");
trigger("^Chi vuoi colpire?", "nastrobash");
-- status
trigger("Tu hai %1 movimento", "score_line_health")
trigger("i %1 diamanti e %2 smeraldi, %3 rubini e %4 zaff", "score_line_gemme")
trigger("effettuato %1 punti gemma su %2 punti", "score_line_punti");
trigger("effettuato %1 exp, ed hai %2 monete", "score_line_stats");
trigger("allineamento e`: %1", "score_line_align");

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
trigger("^[%1] dice alla gilda '%2", "handle_ctell");
trigger("^[%1] '%2", "handle_ot");

-- Berserk/Bash counters
trigger("rabbia ti pervade", "berserk_ok")
trigger("sei calmato", "set_berserk_off")
trigger("raggiungere la furia che", "berserk_ko")
trigger("schiva il tuo urto", "bash_ko")
trigger("ai una forte spinta", "bash_ok")
trigger("gia` in piedi", "standing_ok")
trigger("i alzi.", "standing_ok")
trigger("Tenti di disarmare", "disarm_ko")
trigger("ti manda a gambe", "standing_ko")
trigger("Tenti di disarmare", "standing_ko")

-- AFK mode
trigger("^Cerchi di conf", "set_hide_on");
trigger("^Ti allontani", "set_afk_on");
trigger("^Ritorni alla", "set_afk_off");
trigger("ti ordina '", "check_afk");

-- GROUP window
trigger("%1 (Capo) HP: %2% MANA: %3% MV: %4%", "group_leader")
trigger("%1 (O) HP:%2% MANA:%3% MV:%4%", "group_normal")
trigger("%1 HP:%2% MANA:%3% MV:%4%", "group_normal")

-- idle
idle_function("myidle");

show("$c0015Configuration complete$c0007.")
