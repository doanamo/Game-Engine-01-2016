local Player = {}
Player.__index = Player

function Player.New()
    local self = {}
    self.time = 9.0

    return setmetatable(self, Player)
end

setmetatable(Player, { __call = Player.New })

function Player:Update(entitySelf, timeDelta)
    self.time = self.time + timeDelta
    
    if self.time >= 10.0 then
        Log("Entity Handle: " .. entitySelf.identifier .. ":" .. entitySelf.version)
        Log("Time Delta: " .. timeDelta)
        self.time = 0.0
    end
end

return Player
