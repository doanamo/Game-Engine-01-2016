local Player = {}
Player.__index = Player

function Player.New()
    local self = {}
    self.time = 9.0

    return setmetatable(self, Player)
end

setmetatable(Player, { __call = Player.New })

function Player:Finalize(entitySelf)
    Log("Finalize called!")
    return true
end

function Player:Update(entitySelf, timeDelta)
    self.time = self.time + timeDelta

    local position = Vec2()
    position.x = 101.1
    position.y = 42.43

    if InputState:IsKeyDown(Keys.F1, false) then
        Log("Key pressed!")
    end
    
    if self.time >= 10.0 then
        Log("Entity Handle: " .. entitySelf.identifier .. ":" .. entitySelf.version)
        Log("Time Delta: " .. timeDelta)
        self.time = 0.0
    end
end

return Player
