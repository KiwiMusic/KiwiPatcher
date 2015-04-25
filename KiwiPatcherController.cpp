/*
 ==============================================================================
 
 This file is part of the KIWI library.
 Copyright (c) 2014 Pierre Guillot & Eliott Paris.
 
 Permission is granted to use this software under the terms of either:
 a) the GPL v2 (or any later version)
 b) the Affero GPL v3
 
 Details of these licenses can be found at: www.gnu.org/licenses
 
 KIWI is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 
 ------------------------------------------------------------------------------
 
 To release a closed-source product which uses KIWI, contact : guillotpierre6@gmail.com
 
 ==============================================================================
*/

#include "KiwiPatcherController.h"
#include "KiwiInstance.h"
#include "KiwiConsole.h"

namespace Kiwi
{    
    // ================================================================================ //
    //                              PATCHER CONTROLLER                                  //    
    // ================================================================================ //
    
    Patcher::Controller::Controller(sPatcher patcher) noexcept :
    GuiController(patcher),
    m_patcher(patcher)
    {
        ;
    }
    
    Patcher::Controller::~Controller() noexcept
    {
        ;
    }
    
    // ================================================================================ //
    //										PRESENTATION                                //
    // ================================================================================ //
    
    void Patcher::Controller::setZoom(ulong zoom)
    {
        m_zoom = clip(zoom, 1ul, 1000ul);
    }
    
    void Patcher::Controller::setLockStatus(bool locked)
    {
        if(m_locked != locked)
        {
            m_locked = locked;
            /*
            m_listeners_mutex.lock();
            auto it = m_listeners.begin();
            while(it != m_listeners.end())
            {
                if((*it).expired())
                {
                    it = m_listeners.erase(it);
                }
                else
                {
                    sListener listener = (*it).lock();
                    listener->patcherViewLockStatusChanged();
                    ++it;
                }
            }
            m_listeners_mutex.unlock();
            */
            //unselectAll();
            //lockStatusChanged();
            redraw();
        }
    }
    
    //! The paint method that can be override.
    /** The function shoulds draw some stuff in the sketch.
     @param sketch  A sketch to draw.
     */
    void Patcher::Controller::draw(Sketch& sketch)
    {
        const bool locked = getLockStatus();
        const Color bgcolor = locked ? m_patcher->getLockedBackgroundColor() : m_patcher->getUnlockedBackgroundColor();
        sketch.fillAll(bgcolor);
        if(!locked)
        {
            const int grid_size = m_patcher->getGridSize();
            const Rectangle bounds = sketch.getBounds();
            sketch.setColor((bgcolor.contrasted(0.5)).withAlpha(0.7));
            for(int x = bounds.x() - (int(bounds.x()) % grid_size); x < bounds.width(); x += grid_size)
            {
                for(int y = bounds.y() - (int(bounds.y()) % grid_size) ; y < bounds.height(); y += grid_size)
                {
                    sketch.fillRectangle(x, y, 1, 1);
                }
            }
        }
    }
    
    bool Patcher::Controller::receive(MouseEvent const& event)
    {
        if(event.isDoubleClick())
        {
            performAction(newObject);
        }
        return true;
    }
    
    bool Patcher::Controller::mouseDown(MouseEvent const& event)
    {
        return true;
    }
    
    bool Patcher::Controller::receive(KeyboardEvent const& event)
    {
        return true;
    }
    
    bool Patcher::Controller::receive(KeyboardFocus const focus)
    {
        return true;
    }
    
    vector<Action::Code> Patcher::Controller::getActionCodes()
    {
        return vector<Action::Code>({newBang, newObject, editModeSwitch});
    }
    
    Action Patcher::Controller::getAction(const ulong code)
    {
        switch(code)
        {
            case editModeSwitch:
                return Action(KeyboardEvent(KeyboardEvent::Cmd, L'e'), "Edit", "Switch between edit and play mode", ActionCategories::editing);
                break;
            case newBang:
                return Action(KeyboardEvent(KeyboardEvent::Nothing, L'b'), "New Bang", "Add a new bang in the patcher", ActionCategories::editing);
                break;
            case newObject:
                return Action(KeyboardEvent(KeyboardEvent::Nothing, L'n'), "New Object", "Add a new object in the patcher", ActionCategories::editing);
                break;
                
            default:
                return Action();
                break;
        }
    }
    
    bool Patcher::Controller::performAction(const ulong code)
    {
        switch(code)
        {
            case editModeSwitch:
                setLockStatus(!getLockStatus());
                return true;
                break;
            case newBang:
                createObject("bang", getMouseRelativePosition());
                return true;
                break;
            case newObject:
                createObject("newobject", getMouseRelativePosition());
                return true;
                break;
                
            default:
                return false;
                break;
        }
    }
    
    void Patcher::Controller::createObject(string const& name, Point const& position)
    {
        const Dico dico({pair<sTag, Atom>(Tags::objects, Vector({Dico({pair<sTag, Atom>(Tags::name, Tag::create(name)), pair<sTag, Atom>(Tags::text, Tag::create(name)), pair<sTag, Atom>(Tags::position, {position.x(), position.y()})})}))});
        m_patcher->add(dico);
    }
}




