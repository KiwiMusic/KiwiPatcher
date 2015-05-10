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

#include "KiwiObjectController.h"
#include "KiwiInstance.h"
#include "KiwiConsole.h"

namespace Kiwi
{
    // ================================================================================ //
    //                              OBJECT  CONTROLLER                                  //
    // ================================================================================ //
    
    Object::Controller::Controller(sObject object) noexcept :
    GuiController(object),
    m_object(object)
    {
        ;
    }
    
    Object::Controller::~Controller() noexcept
    {
        ;
    }
    
    // ================================================================================ //
    //										PRESENTATION                                //
    // ================================================================================ //
    
    void Object::Controller::setZoom(ulong zoom)
    {
        m_zoom = clip(zoom, 1ul, 1000ul);
    }
    
    void Object::Controller::setLockStatus(bool locked)
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
             listener->objectViewLockStatusChanged();
             ++it;
             }
             }
             m_listeners_mutex.unlock();
             */
            //unselectAll();
            //lockStatusChanged();
            //redraw();
        }
    }
    
    void Object::Controller::draw(sGuiView view, Sketch& sketch)
    {
        //m_object->draw(view, sketch);
        if(!m_locked)
        {
            Size size = getSize();
            const ulong ninlets = max(m_object->getNumberOfInlets(), 2ul) - 1ul;
            sketch.setColor(Colors::black);
            for(ulong i = 0; i < m_object->getNumberOfInlets(); i++)
            {
                sketch.fillRectangle(i / ninlets * size.width(), 0., 6., 3.);
            }
        }
        
    }
}
















