local Player = {}
Player.__index = Player

function Player.New()
    local self = {}
    self.time = 9.0
    self.speed = 3.0

    return setmetatable(self, Player)
end

setmetatable(Player, { __call = Player.New })

function Player:Finalize(entitySelf)
    -- Get required components.
    self.transform = ComponentSystem:GetTransform(entitySelf)

    return true
end

function Player:Update(entitySelf, timeDelta)
    -- Calculate movement direction.
    local direction = Vec2(0.0, 0.0)

    if InputState:IsKeyDown(Keys.D) then
        direction.x = direction.x + 1.0
    end

    if InputState:IsKeyDown(Keys.A) then
        direction.x = direction.x - 1.0
    end

    if InputState:IsKeyDown(Keys.W) then
        direction.y = direction.y + 1.0
    end

    if InputState:IsKeyDown(Keys.S) then
        direction.y = direction.y - 1.0
    end

    -- Set transform position.
    self.transform:SetPosition(direction)
end

return Player
