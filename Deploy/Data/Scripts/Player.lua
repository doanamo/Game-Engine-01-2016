local Player = {}
Player.__index = Player

function Player.New()
    local self = {}
    self.time = 9.0

    return setmetatable(self, Player)
end

setmetatable(Player, { __call = Player.New })

function Player:Finalize(entitySelf)
    -- Get required components.
    self.transform = ComponentSystem:GetTransform(entitySelf)

    return true
end

function Player:Update(entitySelf, timeDelta)
    -- Update character position.
    local position = self.transform:GetPosition()
    local speed = 3.0

    if InputState:IsKeyDown(Keys.D) then
        position.x = position.x + speed * timeDelta
    end

    if InputState:IsKeyDown(Keys.A) then
        position.x = position.x - speed * timeDelta
    end

    if InputState:IsKeyDown(Keys.W) then
        position.y = position.y + speed * timeDelta
    end

    if InputState:IsKeyDown(Keys.S) then
        position.y = position.y - speed * timeDelta
    end

    self.transform:SetPosition(position)
end

return Player
