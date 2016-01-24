#include "Precompiled.hpp"
#include "Script.hpp"
using namespace Game;

Script::Script(System::ResourceManager* resourceManager) :
    System::Resource(resourceManager),
    m_initialized(false)
{
}

Script::~Script()
{
    if(m_initialized)
        this->Cleanup();
}

void Script::Cleanup()
{
    // Reset initialization state.
    m_initialized = false;
}

bool Script::Load(std::string filename)
{
    // Setup initialization routine.
    if(m_initialized)
        this->Cleanup();

    SCOPE_GUARD
    (
        if(!m_initialized)
            this->Cleanup();
    );

    // Success!
    return m_initialized = true;
}
