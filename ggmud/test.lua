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

logged = false
combat = false
in_group = false
berserked = false
full_auto = false
use_autoassist = true
weapon = "UNDEF"

function autoassist(pg, stato)
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

    local danno = "UNDEF"
    
    if weapon == asl then
        danno = "SLASH"
    elseif weapon == abl then
        danno = "BLUNT"
    elseif weapon == api then
        danno = "PIERCE"
    end

    local combat_state = "$c0011IDLE$c0007";
    if combat == true then
        combat_state = "$c0011COMBAT$c0007";
    end
    
    window("Status", "$c0007Arma: $c0014" .. danno .. " $c0007($c0015" .. weapon .. "$c0007) " .. combat_state);
    window("Status", "Group leader: $c0015" .. leader .. " $c0007Tank: $c0015" .. tank)
    window("Status", "XP: $c0015" .. actualexp .. " $c0007Gold: $c0015" .. actualgold .. "$c0007 Align: $c0015".. align);
    window("Status", "D: $c0015" .. diamanti .. " $c0007S: $c0010" .. smeraldi .. " $c0007 R: $c0009" .. rubini .. " $c0007Z: $c0005" .. zaffiri)
    window("Status", "Punti gemma: $c0015" .. punti_gemma .. " $c0007totali: $c0015" .. punti_totali)
end

--  1164(1164) hit, 55(100) mana e 52(155) punti di

function score_line_health(line)
    _, _, actual_hp, max_hp, actual_move, max_move = string.find(line, "(%d+)%((%d+)%) hit, %d+%(%d+%) mana e (%d+)%((%d+)%) punti")
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
    punti_gemma = g
    punti_totali = t

    update_status()
end

function myidle()
   if logged == false then
       return
   end

   if max_hp == 0 then
       send("score;group")
       return
   end

   idle_loops++
   
-- se non sono in combattimento lancio uno score
   if idle_loops > 60 and combat == false then
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
    
    if leader == "mongo" then
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
        oldweapon = asl
        send("rem " .. api .. ";rem " .. abl)
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

    if c == namecol then
        namecol = "$c0006"
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

show("$c0015Started script configuration for $c0011Mongo$c0007...")

dump("tank", tank)
dump("Guida:", leader)
dump("Versione programma:", VERSION)


trigger("^##%1>", "grab_prompt")

-- autoassist
trigger("violenta spinta di %1 fa perdere", "autoassist_ok");
trigger("travolto da %1 che perde", "autoassist_ko");

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

-- idle
idle_function("myidle");

show("$c0015Configuration complete$c0007.")
