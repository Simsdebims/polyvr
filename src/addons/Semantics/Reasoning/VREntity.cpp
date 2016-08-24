#include "VREntity.h"
#include "VRProperty.h"
#include "VROntology.h"
#include "core/utils/VRStorage_template.h"

#include <iostream>

using namespace OSG;

VREntity::VREntity(string name, VRConceptPtr c) {
    concept = c;

    setStorageType("Entity");
    storeObjName("concept", &concept, &conceptName);

    setNameSpace("entity");
    setName(name);
}

VREntityPtr VREntity::create(string name, VRConceptPtr c) { return VREntityPtr( new VREntity(name, c) ); }

void VREntity::setConcept(VRConceptPtr c) { concept = c; }
VRConceptPtr VREntity::getConcept() { return concept.lock(); }

void VREntity::set(string name, string value) {
    if (!properties.count(name)) { add(name, value); return; }
    auto cpts = concept.lock();
    auto prop = cpts->getProperty(name);
    if (!prop) { cout << "Warning (set): Entity " << this->name << " has no property " << name << endl; return; }
    properties[name][0]->value = value;
    // TODO: warn if vector size bigger 1
}

void VREntity::add(string name, string value) {
    auto cpts = concept.lock();
    if (!cpts) { cout << "Warning (add): Entity " << this->name << " has no valid concept" << endl; return; }
    //string pconcept; if (auto p = cpts->parent.lock()) pconcept = p->getName();
    //cout << "VREntity::add " << name << " " << value << " " << concept->name << " " << pconcept << endl;
    auto prop = cpts->getProperty(name);
    if (!prop) { cout << "Warning (add): Entity " << this->name << " has no property " << name << endl; return; }
    prop = prop->copy();
    prop->value = value;
    properties[name].push_back( prop );
}

void VREntity::rem(VRPropertyPtr p) {
    string name = p->getName();
    if (properties.count(name)) {
        auto& v = properties[name];
        v.erase( remove(v.begin(), v.end(), p), v.end() );
    }
}

void VREntity::setVector(string name, vector<string> v, string type) {
    string v_name = this->name+"_"+name;
    set(name, v_name);
    auto cpts = concept.lock();
    if (cpts) cpts->ontology.lock()->addVectorInstance(v_name, type, v);
}

vector<VRPropertyPtr> VREntity::getProperties(string name) {
    if (name != "" && properties.count(name)) return properties[name];
    vector<VRPropertyPtr> res;
    if (name == "") for (auto pv : properties) for (auto p : pv.second) res.push_back(p);
    return res;
}

vector<string> VREntity::getAtPath(vector<string> path) {
    /*cout << "  get value at path ";
    for (auto p : path) cout << "/" << p;
    cout << endl;*/

    vector<string> res;
    auto cpts = concept.lock();

    if (path.size() == 2) {
        string m = path[1];
        auto prop = cpts->getProperty(m);
        //cout << "  get value of member " << m << " with id " << id << endl;
        if (!prop) return res;
        if (!properties.count(prop->getName())) return res;
        for (auto p : properties[prop->getName()]) res.push_back(p->value);
        return res;
    }

    return res;
}

string VREntity::toString() {
    string data = "Entity " + name;
    auto cpts = concept.lock();
    if (cpts) data += " of type " + cpts->getName();
    else data += " unknown type";
    data += " with properties:";
    for (auto p : properties) {
        for (auto sp : p.second) {
            data += " "+sp->getName()+"("+sp->type+")="+sp->value;
        }
    }
    return data;
}

void VREntity::save(xmlpp::Element* e, int p) {
    VRStorage::save(e,p);
    e = e->add_child("properties");
    for (auto p : properties) {
        auto e2 = e->add_child(p.first);
        for (auto sp : p.second) {
            auto e3 = e2->add_child(sp->getName());
            e3->set_attribute("value", sp->value);
            e3->set_attribute("type", sp->type);
        }
    }
}

void VREntity::load(xmlpp::Element* e) {
    VRStorage::load(e);
    e = getChild(e, "properties");
    for (auto el : getChildren(e)) {
        for (auto el2 : getChildren(el)) {
            string n = el2->get_name();
            auto p = VRProperty::create(n,"");
            if (el2->get_attribute("value")) p->value = el2->get_attribute("value")->get_value();
            if (el2->get_attribute("type")) p->type = el2->get_attribute("type")->get_value();
            properties[n].push_back(p);
        }
    }
}
