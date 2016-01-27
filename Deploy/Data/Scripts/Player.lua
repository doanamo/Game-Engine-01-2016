local Player = {}
Player.__index = Player

function Player:New()
    local self = {}
    self.text = "Hello world!"
    
    return setmetatable(self, Player)
end

setmetatable(Player, { __call = Player.New })

function Player:Update(entitySelf, timeDelta)
    Log(self.text)
end

return Player
