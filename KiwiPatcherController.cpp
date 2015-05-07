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
    m_patcher(patcher),
    m_selection(make_shared<Selection>(patcher)),
    m_lasso(make_shared<Lasso>(patcher, m_selection))
    {
        assert(m_patcher);
    }
    
    Patcher::sController Patcher::Controller::create(sPatcher patcher) noexcept
    {
        sController ctrl = make_shared<Controller>(patcher);
        if(ctrl)
        {
            ctrl->created();
        }
        return ctrl;
    }
    
    void Patcher::Controller::created() noexcept
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
            for(int x = bounds.x() - (int(bounds.x()) % grid_size); x < bounds.right(); x += grid_size)
            {
                for(int y = bounds.y() - (int(bounds.y()) % grid_size) ; y < bounds.bottom(); y += grid_size)
                {
                    sketch.drawPoint(x, y);
                }
            }
        }
    }
    
    // ================================================================================ //
    //                                      MOUSE                                       //
    // ================================================================================ //
    
    bool Patcher::Controller::receive(MouseEvent const& event)
    {
        switch (event.getType())
        {
            case MouseEvent::Enter:         return mouseEnter(event);
            case MouseEvent::Leave:         return mouseLeave(event);
            case MouseEvent::Move:          return mouseMove(event);
            case MouseEvent::Drag:          return mouseDrag(event);
            case MouseEvent::Down:          return mouseDown(event);
            case MouseEvent::Up:            return mouseUp(event);
            case MouseEvent::DoubleClick:   return mouseDoubleClick(event);
            case MouseEvent::Wheel:         return mouseWeel(event);
                
            default: break;
        }
        
        return false;
    }
    
    bool Patcher::Controller::mouseEnter(MouseEvent const& event)
    {
        return true;
    }
    
    bool Patcher::Controller::mouseLeave(MouseEvent const& event)
    {
        return true;
    }
    
    bool Patcher::Controller::mouseDrag(MouseEvent const& event)
    {
        m_lasso->drag(event.getPosition(), true, event.hasAlt(), event.hasShift());
        return true;
    }

    bool Patcher::Controller::mouseMove(MouseEvent const& event)
    {
        return true;
    }
    
    bool Patcher::Controller::mouseDown(MouseEvent const& event)
    {
        m_lasso->start(event.getPosition(), false);
        return true;
    }
    
    bool Patcher::Controller::mouseUp(MouseEvent const& event)
    {
        if (m_lasso->isDragging())
        {
            m_lasso->end();
            return true;
        }
        
        return false;
    }
    
    bool Patcher::Controller::mouseDoubleClick(MouseEvent const& event)
    {
        performAction(newObject);
        return true;
    }
    
    bool Patcher::Controller::mouseWeel(MouseEvent const& event)
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
    
    // ================================================================================ //
    //                                      ACTIONS                                     //
    // ================================================================================ //
    
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
            case newBang:
                return Action(KeyboardEvent(KeyboardEvent::Nothing, L'b'), "New Bang", "Add a new bang in the patcher", ActionCategories::editing);
            case newObject:
                return Action(KeyboardEvent(KeyboardEvent::Nothing, L'n'), "New Object", "Add a new object in the patcher", ActionCategories::editing);
                
            default: return Action();
        }
    }
    
    bool Patcher::Controller::performAction(const ulong code)
    {
        switch(code)
        {
            case editModeSwitch:
                setLockStatus(!getLockStatus());
                return true;
            case newBang:
                createObject("bang", getMouseRelativePosition());
                return true;
            case newObject:
                createObject("newobject", getMouseRelativePosition());
                return true;
                
            default: return false;
        }
    }
    
    void Patcher::Controller::createObject(string const& name, Point const& position)
    {
        const Dico dico({pair<sTag, Atom>(Tags::objects, Vector({Dico({pair<sTag, Atom>(Tags::name, Tag::create(name)), pair<sTag, Atom>(Tags::text, Tag::create(name)), pair<sTag, Atom>(Tags::position, {position.x(), position.y()})})}))});
        m_patcher->add(dico);
    }
    
    // ================================================================================ //
    //                                PATCHER SELECTION                                 //
    // ================================================================================ //
    
    bool Patcher::Selection::has(sObject object)
    {
        return object && (m_objects.find(object) != m_objects.end());
    }
    
    bool Patcher::Selection::has(sLink link)
    {
        return link && (m_links.find(link) != m_links.end());
    }
    
    bool Patcher::Selection::addAllObjects()
    {
        sPatcher patcher = getPatcher();
        if (patcher)
        {
            bool changed = false;
            lock_guard<mutex> guard(m_mutex);
            for(auto object : patcher->getObjects())
            {
                if(object && m_objects.insert(object).second)
                {
                    int todo_setSelectedStatus;
                    //object->setSelectedStatus(true);
                    changed = true;
                }
            }
            
            if(changed)
            {
                selectionChanged();
            }
            
            return true;
        }
        
        return false;
    }
    
    bool Patcher::Selection::addAllLinks()
    {
        sPatcher patcher = getPatcher();
        if (patcher)
        {
            bool changed = false;
            lock_guard<mutex> guard(m_mutex);
            
            for(auto link : patcher->getLinks())
            {
                if(link && m_links.insert(link).second)
                {
                    int todo_setSelectedStatus;
                    //link->setSelectedStatus(true);
                    changed = true;
                }
            }
            
            if(changed)
            {
                selectionChanged();
            }
            
            return true;
        }
        
        return false;
    }
    
    void Patcher::Selection::removeAll(const bool notify)
    {
        if(isAnythingSelected())
        {
            removeAllObjects(false);
            removeAllLinks(false);
            
            if(notify)
            {
                selectionChanged();
            }
        }
    }
    
    bool Patcher::Selection::removeAllObjects(const bool notify)
    {
        bool sendChange = false;
        if(!m_objects.empty())
        {
            for(auto elem : m_objects)
            {
                sObject object = elem.lock();
                if(object)
                {
                    int todo_setSelectedStatus;
                    //object->setSelectedStatus(false);
                    sendChange = true;
                }
            }
            
            {
                lock_guard<mutex> guard(m_mutex);
                m_objects.clear();
            }
            
            if(notify && sendChange)
            {
                selectionChanged();
            }
            
            return true;
        }
        return false;
    }
    
    bool Patcher::Selection::removeAllLinks(const bool notify)
    {
        bool sendChange = false;
        if(!m_links.empty())
        {
            for(auto elem : m_links)
            {
                sLink link = elem.lock();
                if(link)
                {
                    int todo_setSelectedStatus;
                    //link->setSelectedStatus(false);
                    sendChange = true;
                }
            }
            
            {
                lock_guard<mutex> guard(m_mutex);
                m_links.clear();
            }
            
            if(notify && sendChange)
            {
                selectionChanged();
            }
            
            return true;
        }
        return false;
    }
    
    void Patcher::Selection::add(vector<sObject>& objects)
    {
        bool notify = false;
        if(!objects.empty())
        {
            for(auto object : objects)
            {
                if(object && add(object, false))
                {
                    notify = true;
                }
            }
            
            if(notify)
            {
                selectionChanged();
            }
        }
    }
    
    void Patcher::Selection::add(vector<sLink>& links)
    {
        bool notify = false;
        if(!links.empty())
        {
            for(auto link : links)
            {
                if(link && add(link, false))
                {
                    notify = true;
                }
            }
            
            if(notify)
            {
                selectionChanged();
            }
        }
    }
    
    bool Patcher::Selection::add(sObject object, const bool notify)
    {
        if(object)
        {
            {
                lock_guard<mutex> guard(m_mutex);
                if(m_objects.insert(object).second)
                {
                    int todo_setSelectedStatus;
                    //object->setSelectedStatus(true);
                }
            }
            
            if(notify)
            {
                selectionChanged();
            }
            return true;
        }
        return false;
    }
    
    bool Patcher::Selection::add(sLink link, const bool notify)
    {
        if(link)
        {
            {
                lock_guard<mutex> guard(m_mutex);
                if(m_links.insert(link).second)
                {
                    int todo_setSelectedStatus;
                    //link->setSelectedStatus(true);
                }
            }
            
            if(notify)
            {
                selectionChanged();
            }
            return true;
        }
        return false;
    }
    
    bool Patcher::Selection::set(sObject object)
    {
        removeAll();
        return add(object, true);
    }
    
    bool Patcher::Selection::set(sLink link)
    {
        removeAll();
        return add(link, true);
    }
    
    void Patcher::Selection::remove(vector<sObject>& objects)
    {
        bool notify = false;
        if(isAnyObjectSelected() && !objects.empty())
        {
            for(auto object : objects)
            {
                if(object && remove(object, false))
                {
                    notify = true;
                }
            }
            
            if(notify)
            {
                selectionChanged();
            }
        }
    }
    
    void Patcher::Selection::remove(vector<sLink>& links)
    {
        bool notify = false;
        if(isAnyLinkSelected() && !links.empty())
        {
            for(auto link : links)
            {
                if(link && remove(link, false))
                {
                    notify = true;
                }
            }
            
            if(notify)
            {
                selectionChanged();
            }
        }
    }
    
    bool Patcher::Selection::remove(sObject object, const bool notify)
    {
        if(object)
        {
            {
                lock_guard<mutex> guard(m_mutex);
                if(m_objects.erase(object))
                {
                    int todo_setSelectedStatus;
                    //object->setSelectedStatus(false);
                }
            }

            if(notify)
            {
                selectionChanged();
            }
            return true;
        }
        return false;
    }
    
    bool Patcher::Selection::remove(sLink link, const bool notify)
    {
        if(link)
        {
            {
                lock_guard<mutex> guard(m_mutex);
                if(m_links.erase(link))
                {
                    int todo_setSelectedStatus;
                    //link->setSelectedStatus(false);
                }
            }
            
            if(notify)
            {
                selectionChanged();
            }
            return true;
        }
        return false;
    }
    
    // ================================================================================ //
    //                                  PATCHER LASSO                                   //
    // ================================================================================ //
    
    Patcher::Lasso::Lasso(sPatcher patcher, sSelection selection) noexcept :
    GuiSketcher(patcher->GuiSketcher::getContext()), m_patcher(patcher), m_selection(selection)
    {
        assert(m_patcher);
        assert(m_selection);
    }
    
    void Patcher::Lasso::addToPatcher() noexcept
    {
        if(!m_active)
        {
            m_patcher->addChild(static_pointer_cast<GuiSketcher>(shared_from_this()));
            m_active = true;
        }
    }
    
    void Patcher::Lasso::removeFromPatcher() noexcept
    {
        if(m_active)
        {
            m_patcher->removeChild(static_pointer_cast<GuiSketcher>(shared_from_this()));
            m_active = false;
        }
    }
    
    void Patcher::Lasso::start(Point const& point, const bool preserve) noexcept
    {
        // You need to call end() before to call start again to avoid this assertion.
        assert(!m_dragging);
        
        m_startpos = point;
        setBounds(Rectangle(point, Size()));
        addToPatcher();
        
        if(!preserve)
        {
            m_selection->removeAll();
        }
        else
        {
            lock_guard<mutex> guard(m_mutex);
            
            for(auto elem : m_selection->getObjects())
            {
                sObject object = elem.lock();
                if(object)
                {
                    m_objects.insert(object);
                }
            }
            
            for(auto elem : m_selection->getLinks())
            {
                sLink link = elem.lock();
                if(link)
                {
                    m_links.insert(link);
                }
            }
        }
        
        m_dragging = true;
    }
    
    void Patcher::Lasso::drag(Point const& point, const bool includeObjects, const bool includeLinks, const bool preserve) noexcept
    {
        setBounds(Rectangle::withCorners(m_startpos, point));
        bool changed = false;
        
        if(preserve)
        {
            if(includeObjects)
            {
                vector<sObject> lassoObjects;
                int todo_lassoHitTest;
                //knockObjects(getBounds(), lassoObjects);
                
                lock_guard<mutex> guard(m_mutex);
                for(auto object : m_patcher->getObjects())
                {
                    if(object)
                    {
                        const bool isSelected = m_selection->has(object);
                        const bool wasSelected = m_objects.find(object) != m_objects.end();
                        const bool inLasso = find(lassoObjects.begin(), lassoObjects.end(), object) != lassoObjects.end();
                        
                        if(!isSelected && (wasSelected != inLasso))
                        {
                            m_selection->add(object, false);
                            changed = true;
                        }
                        else if(isSelected && (wasSelected == inLasso))
                        {
                            m_selection->remove(object, false);
                            changed = true;
                        }
                    }
                }
            }
            if(includeLinks)
            {
                vector<sLink> lassoLinks;
                int todo_lassoHitTest;
                //knockObjects(getBounds(), lassoLinks);
                
                lock_guard<mutex> guard(m_mutex);
                for(auto link : m_patcher->getLinks())
                {
                    if(link)
                    {
                        const bool isSelected = m_selection->has(link);
                        const bool wasSelected = m_links.find(link) != m_links.end();
                        const bool inLasso = find(lassoLinks.begin(), lassoLinks.end(), link) != lassoLinks.end();
                        
                        if(!isSelected && (wasSelected != inLasso))
                        {
                            m_selection->add(link, false);
                            changed = true;
                        }
                        else if(isSelected && (wasSelected == inLasso))
                        {
                            m_selection->remove(link, false);
                            changed = true;
                        }
                    }
                }
            }
            
            if(changed)
            {
                int todo_notify_changes;
                //selectionChanged();
            }
        }
        else
        {
            m_selection->removeAll(false);
            if(includeObjects)
            {
                vector<sObject> lassoObjects;
                int todo_lassoHitTest;
                //knockObjects(getBounds(), lassoObjects);
                m_selection->add(lassoObjects);
            }
            
            if(includeLinks)
            {
                vector<sLink> lassoLinks;
                int todo_lassoHitTest;
                //knockLinks(getBounds(), lassoLinks);
                m_selection->add(lassoLinks);
            }
        }
    }
    
    void Patcher::Lasso::end() noexcept
    {
        m_objects.clear();
        m_links.clear();
        m_dragging = false;
        removeFromPatcher();
    }
    
    void Patcher::Lasso::draw(scGuiView view, Sketch& sketch) const
    {
        sketch.fillAll(Color(0.96, 0.96, 0.96, 0.4));
        sketch.setColor(Color(0.96, 0.96, 0.96, 1.));
        sketch.drawRectangle(getBounds().withZeroOrigin(), 1.);
    }
}




