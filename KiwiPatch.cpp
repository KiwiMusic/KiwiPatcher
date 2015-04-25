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

#include "KiwiPatch.h"
#include "KiwiInstance.h"
#include "KiwiConsole.h"
#include "KiwiPatcherController.h"
#include "KiwiPatcherWindow.h"

namespace Kiwi
{    
    // ================================================================================ //
    //                                      PAGE                                        //
    // ================================================================================ //
    
    Patcher::Patcher(sInstance instance) noexcept :
    GuiSketcher(instance),
    DspChain(instance),
    m_instance(instance)
    {
        setPosition(Point(30., 30.));
        setSize(Size(800., 600.));
        createAttr(Tags::unlocked_bgcolor,  "Unlocked Background Color",    "Appearance",   Color(0.88, 0.89, 0.88, 1.));
        createAttr(Tags::locked_bgcolor,    "Locked Background Color",      "Appearance",   Color(0.88, 0.89, 0.88, 1.));
        createAttr(Tags::gridsize,          "Grid Size",                    "Editing",      long(20ul));
    }
	
    Patcher::~Patcher()
    {
        m_links.clear();
        m_objects.clear();
        m_free_ids.clear();
    }
    
    sPatcher Patcher::create(sInstance instance, Dico& dico)
    {
        sPatcher patcher = make_shared<Patcher>(instance);
        if(patcher)
        {
            instance->DspContext::add(patcher);
			
            auto it = dico.find(Tags::patcher);
            if(it != dico.end())
            {
                patcher->add(it->second);
            }
        }
        return patcher;
    }
    
    void Patcher::createObject(Dico& dico)
    {
        sObject object;
        if(dico.count(Tags::name))
        {
            object = Factory::create(dico[Tags::name], Infos(getInstance(), getShared(), ulong(dico[Tags::id]), sTag(dico[Tags::name]), sTag(dico[Tags::text])->getName(), dico, dico[Tags::arguments]));
            if(object)
            {
                sDspNode dspnode = dynamic_pointer_cast<DspNode>(object);
                if(dspnode)
                {
                    DspChain::add(dspnode);
                }
                m_objects.push_back(object);
                GuiSketcher::add(object);
                object->loaded();
            }
        }
    }
    
    void Patcher::createLink(Dico const& dico)
    {
        if(dico.count(Tags::from) && dico.at(Tags::from).isVector() &&
           dico.count(Tags::to) && dico.at(Tags::to).isVector())
        {
            Vector const& vfrom  = dico.at(Tags::from);
            Vector const& vto    = dico.at(Tags::to);
            if(vfrom.size() > 1 && vto.size() > 1)
            {
                const sObject from = getObjectWithId(vfrom[0]);
                const sObject to   = getObjectWithId(vto[0]);
                if(from && to)
                {
                    const Object::sOutlet outlet  = from->getOutlet(vfrom[1]);
                    const Object::sInlet inlet    = to->getInlet(vto[1]);
                    if(outlet && inlet)
                    {
                        if(outlet->getType() >= Object::Io::Signal && inlet->getType() >= Object::Io::Signal)
                        {
                            sDspNode pfrom = dynamic_pointer_cast<DspNode>(from);
                            sDspNode pto   = dynamic_pointer_cast<DspNode>(to);
                            if(from && to)
                            {
                                ulong poutlet, pinlet;
                                try
                                {
                                    pinlet = from->getDspInletIndex(vfrom[1]);
                                }
                                catch(Error& e)
                                {
                                    Console::post(e.what());
                                    return;
                                }
                                
                                try
                                {
                                    poutlet = from->getDspOutletIndex(vto[1]);
                                }
                                catch(Error& e)
                                {
                                    Console::post(e.what());
                                    return;
                                }
                                
                                outlet->append(to, vfrom[1]);
                                inlet->append(from, vto[1]);
                                Object::Io::Type type = Object::Io::Signal;
                                if(outlet->getType() == Object::Io::Both && inlet->getType() == Object::Io::Both)
                                {
                                    type = Object::Io::Both;
                                }
                                shared_ptr<Link::SignalLink> link = make_shared<Link::SignalLink>(getShared(), from, vfrom[1], to, vto[1], type, pfrom, poutlet, pto, pinlet);
                                
                                DspChain::add(link);
                                m_links.push_back(static_pointer_cast<Link>(link));

                            }
                        }
                        else if(outlet->getType() == inlet->getType() || inlet->getType() == Object::Io::Both || outlet->getType() == Object::Io::Both)
                        {
                            
                            outlet->append(to, vfrom[1]);
                            inlet->append(from, vto[1]);
                            sLink link = make_shared<Link>(getShared(), from, vfrom[1], to, vto[1], Object::Io::Message);
                            m_links.push_back(link);
                        }
                    }
                }
                else
                {
                    throw Error("The dico isn't valid for a link creation.");
                }
            }
            else
            {
                throw Error("The dico isn't valid for a link creation.");
            }
        }
        else
        {
            throw Error("The dico isn't valid for a link creation.");
        }
    }
    
    void Patcher::add(Dico const& dico)
    {
        Vector objects, links;
        auto it = dico.find(Tags::objects);
        if(it != dico.end())
        {
            objects = it->second;
        }
        it = dico.find(Tags::links);
        if(it != dico.end())
        {
            links = it->second;
        }

        lock_guard<mutex> guard(m_mutex);
        for(vector<sObject>::size_type i = 0; i < objects.size(); i++)
        {
            Dico objdico(objects[i]);
            if(!objdico.empty())
            {
                const ulong r_id = objdico[Tags::id];
                const ulong n_id = m_free_ids.empty() ? m_objects.size() + 1 : m_free_ids[0];
                if(!m_free_ids.empty())
                {
                    m_free_ids.erase(m_free_ids.begin());
                }
                objdico[Tags::id] = (long)n_id;
                Vector atoms;
                for(vector<sLink>::size_type i = 0; i < links.size(); i++)
                {
                    atoms = objdico[Tags::from];
                    if(atoms.size() > 1 && r_id == (ulong)atoms[0])
                    {
                        objdico[Tags::from] = {(long)n_id, atoms[1]};
                    }
            
                    atoms = objdico[Tags::to];
                    if(atoms.size() > 1 && r_id == (ulong)atoms[0])
                    {
                        objdico[Tags::to] = {(long)n_id, atoms[1]};
                    }
                }
                createObject(objdico);
            }
        }
        
        for(vector<sLink>::size_type i = 0; i < links.size(); i++)
        {
            Dico linkdico(links[i]);
            if(!linkdico.empty())
            {
                createLink(linkdico);
            }
        }
    }
    
    void Patcher::remove(sObject object)
    {
        if(object)
        {
            lock_guard<mutex> guard(m_mutex);
            auto it = find(m_objects.begin(), m_objects.end(), object);
            if(it != m_objects.end())
            {
                for(auto li = m_links.begin(); li != m_links.end();)
                {
                    if((*li)->getObjectFrom() == object || (*li)->getObjectTo() == object)
                    {
                        sDspLink dsplink = dynamic_pointer_cast<DspLink>((*li));
                        if(dsplink)
                        {
                            DspChain::remove(dsplink);
                        }
                    }
                    else
                    {
                        ++li;
                    }
                }
                sDspNode dspnode = dynamic_pointer_cast<DspNode>(object);
                if(dspnode)
                {
                    DspChain::remove(dspnode);
                }
                m_objects.erase(it);
                GuiSketcher::remove(object);
                m_free_ids.push_back(object->getId());
            }
        }
    }
    
    void Patcher::remove(sLink link)
    {
        if(link)
        {
            lock_guard<mutex> guard(m_mutex);
            auto it = find(m_links.begin(), m_links.end(), link);
            if(it != m_links.end())
            {
                sDspLink dsplink = dynamic_pointer_cast<DspLink>(link);
                if(dsplink)
                {
                    DspChain::remove(dsplink);
                }
                m_links.erase(it);
            }
        }
    }
    
    void Patcher::toFront(sObject object)
    {
        if(object)
        {
            lock_guard<mutex> guard(m_mutex);
            auto it = find(m_objects.begin(), m_objects.end(), object);
            if(it != m_objects.end())
            {
                m_objects.erase(it);
                m_objects.push_back(object);
            }
        }
    }
    
    void Patcher::toBack(sObject object)
    {
        if(object)
        {
            lock_guard<mutex> guard(m_mutex);
            auto it = find(m_objects.begin(), m_objects.end(), object);
            if(it != m_objects.end())
            {
                m_objects.erase(it);
                m_objects.insert(m_objects.begin(), object);
            }
        }
    }
	
    void Patcher::write(Dico& dico) const
    {
        if(1)
        {
            /*
			Dico& subpatcher = Dico::create();
			if(subpatcher)
			{
                Vector atoms;
				lock_guard<mutex> guard(m_mutex);
                
				for(vector<sObject>::size_type i = 0; i < m_objects.size(); i++)
				{
					Dico& object = Dico::create();
					if(object)
					{
						m_objects[i]->write(object);
                        atoms.push_back(object);
					}
				}
				subpatcher->set(Tags::objects, atoms);
				
				atoms.clear();
				
				for(vector<sLink>::size_type i = 0; i < m_links.size(); i++)
				{
                    Dico& link = Dico::create();
					if(link)
					{
						m_links[i]->write(link);
						atoms.push_back(link);
					}
				}
				subpatcher->set(Tags::links, atoms);
				dico->set(Tags::patcher, subpatcher);
			}*/
        }
    }
     
    void Patcher:: draw(scGuiView ctrl, Sketch& sketch) const
    {
        ;
    }
    
    sGuiWindow Patcher::createWindow()
    {
        sInstance instance = getInstance();
        if(instance)
        {
            shared_ptr<Window> window = make_shared<Window>(getShared());
            if(window)
            {
                window->initialize();
                window->display();
            }
            return window;
        }
        return sGuiWindow();
    }
    
    sGuiController Patcher::createController()
    {
        return make_shared<Patcher::Controller>(getShared());
    }
    
    Patcher::sLasso Patcher::createLasso()
    {
        sLasso lasso = make_shared<Lasso>(GuiSketcher::getContext());
        if(lasso)
        {
            GuiSketcher::add(lasso);
        }
        return lasso;
    }
    
    void Patcher::removeLasso(sLasso lasso)
    {
        GuiSketcher::remove(lasso);
    }
    
    void Patcher::Lasso::draw(scGuiView view, Sketch& sketch) const
    {
        sketch.fillAll(Color(0.96, 0.96, 0.96, 0.4));
        sketch.setColor(Color(0.96, 0.96, 0.96, 1.));
        sketch.drawRectangle(getBounds().withZeroOrigin(), 1.);
    }
}




