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

#include "KiwiInstance.h"
#include "KiwiConsole.h"
#include "../KiwiObjects/KiwiObjects.h"

namespace Kiwi
{
    bool libraries_loaded = false;
    
    // ================================================================================ //
    //                                      INSTANCE                                    //
    // ================================================================================ //
    
    Instance::Instance(sGuiDeviceManager guiDevice, sDspDeviceManager dspDevice, sTag name) noexcept :
    GuiContext(guiDevice),
    DspContext(dspDevice),
    m_name(name)
    {
		;
    }
    
    Instance::~Instance()
    {
        lock_guard<mutex> guard(m_patchers_mutex);
        m_patchers.clear();
        m_listeners.clear();
    }
    
    sInstance Instance::create(sGuiDeviceManager guiDevice, sDspDeviceManager dspDevice, string const& name)
    {
        return make_shared<Instance>(guiDevice, dspDevice, Tag::create(name));
    }
    
    sPatcher Instance::createPatcher()
    {
        Dico dico;
        return createPatcher(dico);
    }

    sPatcher Instance::createPatcher(Dico& dico)
    {
        sPatcher patcher;
        bool state(false);
        {
            patcher = Patcher::create(getShared(), dico);
            lock_guard<mutex> guard(m_patchers_mutex);
            if(patcher && m_patchers.insert(patcher).second)
            {
                DspContext::add(patcher);
                state = true;
            }
        }
        if(state)
        {
            m_listeners.call(&Listener::patcherCreated, getShared(), patcher);
        }
        return patcher;
    }
    
    void Instance::removePatcher(sPatcher patcher)
    {
        bool state(false);
        {
            lock_guard<mutex> guard(m_patchers_mutex);
            if(m_patchers.erase(patcher))
            {
                DspContext::remove(patcher);
                state = true;
            }
        }
        if(state)
        {
            m_listeners.call(&Listener::patcherRemoved, getShared(), patcher);
        }
    }
    
    void Instance::getPatchers(vector<sPatcher>& patchers)
    {
        lock_guard<mutex> guard(m_patchers_mutex);
        patchers.assign(m_patchers.begin(), m_patchers.end());
    }
    
    void Instance::addListener(sListener listener)
    {
        m_listeners.add(listener);
    }
    
    void Instance::removeListener(sListener listener)
    {
        m_listeners.remove(listener);
    }
    
    sGuiWindow Instance::createWindow()
    {
        sWindow window(make_shared<Window>(getShared()));
        if(window)
        {
            window->addToDesktop();
        }
        return window;
    }
    
    // ================================================================================ //
    //                                  INSTANCE WINDOW                                 //
    // ================================================================================ //
    
    Instance::Window::Window(sInstance instance) : GuiWindow(instance),
    m_instance(instance)
    {
        setHeader(make_shared<GuiWindow::Header>(instance, "Kiwi Studio"));
        setContent(make_shared<GuiViewPort>(instance));
    }
    
    void Instance::Window::viewCreated(sGuiView view) noexcept
    {
        sGuiContext ctxt(getContext());
        if(view && ctxt)
        {
            sGuiController ctrl(view->getController());
            if(ctrl)
            {
                ctrl->setBounds(ctxt->getScreenBounds(ctrl->getBounds().centre()));
            }
        }
    }
    
    void Instance::Window::patcherCreated(sInstance instance, sPatcher patcher) {}
    
    void Instance::Window::patcherRemoved(sInstance instance, sPatcher patcher) {}
    
    void Instance::Window::dspStarted(sInstance instance) {}
    
    void Instance::Window::dspStopped(sInstance instance) {}
    
    // ================================================================================ //
    //                                      OBJECT FACTORY                              //
    // ================================================================================ //
    
    map<sTag, shared_ptr<Factory::Creator>> Factory::m_creators;
    mutex Factory::m_mutex;
    
    sObject Factory::create(sTag name, Infos const& detail)
    {
        auto it = m_creators.find(name);
        if(it != m_creators.end())
        {
            sObject obj = it->second->create(detail);
            if(obj)
            {
                obj->read(detail.dico);
            }
            return obj;
        }
        else
        {
            Console::error("The factory doesn't know the object " + name->getName());
            return nullptr;
        }
    }
    
    bool Factory::has(sTag name)
    {
        lock_guard<mutex> guard(m_mutex);
        return m_creators.find(name) != m_creators.end();
    }
    
    vector<sTag> Factory::names()
    {
        lock_guard<mutex> guard(m_mutex);
        vector<sTag> names;
        for(auto it = m_creators.begin(); it !=  m_creators.end(); ++it)
        {
            names.push_back(it->first);
        }
        return names;
    }
}
















